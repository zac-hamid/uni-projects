#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/seek.h>
#include <lib.h>
#include <uio.h>
#include <thread.h>
#include <current.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <proc.h>
#include <copyinout.h>
#include <synch.h>
#include <endian.h>

// Checks if a given file descriptor is valid
// ie if it points to an open file or is within legal bounds (0 - (OPEN_MAX-1))
bool is_fd_valid(int f_descriptor) {
    if ((f_descriptor < __OPEN_MAX) && (f_descriptor >= 0)) {
		if (curproc->fd_table[f_descriptor] == NULL) {
			return false;
		}
	} else return false;

    return true;
}

// Helper function for setting up the UIO structure for read/write operations
void uio_uinit(struct iovec *iov, struct uio *u, userptr_t buf, size_t len, off_t offset, enum uio_rw rw) {
    iov->iov_ubase = buf;
    iov->iov_len = len;
    u->uio_iov = iov;
    u->uio_iovcnt = 1;
    u->uio_offset = offset;
    u->uio_resid = len;
    u->uio_segflg = UIO_USERSPACE;
    u->uio_rw = rw;
    u->uio_space = proc_getas();
}

// Opens a new file, adds entry to the open file table, adds reference to this entry in file
// descriptor table, and stores index of FD table entry in f_descriptor
int file_open(char *file_path, int openflags, mode_t mode, int* f_descriptor) {
	struct vnode *vn;

	// Call vfs_open
	int open_result = vfs_open(file_path, openflags, mode, &vn);

	// Do vfs_open error stuff here
	if (open_result != 0) {
		(*f_descriptor) = -1;
		return open_result;
	}

	// Add entry to OF Table
	struct oftable_entry entry;
	entry.f_ptr = 0;
	// If O_APPEND is used, f_ptr must become file size
	if ((openflags & O_APPEND) > 0) {
		struct stat *statbuf = kmalloc(sizeof(struct stat));
		int error = 0;
		// Get file size
		error = VOP_STAT(vn, statbuf);
		if (error != 0) {
			(*f_descriptor) = -1;
			return error;
		}
		entry.f_ptr = (int)statbuf->st_size;
		kfree(statbuf);
	}
	
	// Set up new entry for open file table
	entry.vnode_ptr = vn;
	entry.ref_cnt = 1;
	entry.accmode = (openflags & O_ACCMODE);
	entry.l = lock_create("oftentry");

	if (entry.l == NULL) panic("Lock for oftable_entry couldn't be created!\n");

	// Search for next free space in OF Table and insert new entry

	lock_acquire(oftable_lock);
	bool found_slot = false;
	int oftable_index = 0;
	for (int i = 0; i < __OPEN_MAX; i++) {
		if (of_table[i].vnode_ptr == NULL) {
			// Found a free slot
			of_table[i] = entry;
			oftable_index = i;
			found_slot = true;
			break;
		}
	}
	lock_release(oftable_lock);

	if (!found_slot) {
		// No more space to open files, set fd to -1 and return error
		(*f_descriptor) = -1;
		return ENFILE;
	}

	// Add entry to FD Table for current process
	found_slot = false;
	int file_descriptor = -1;
	for (int i = 0; i < __OPEN_MAX; i++) {
		if (curproc->fd_table[i] == NULL) {
			curproc->fd_table[i] = (struct oftable_entry *)&of_table[oftable_index];
			file_descriptor = i;
			found_slot = true;
			break;
		}
	}
	// If found slot, file_descriptor stores
	if (!found_slot) {
		// No more space for file descriptors, return error
		(*f_descriptor) = -1;
		return EMFILE;
	}

	(*f_descriptor) = file_descriptor;
	return 0;
}

// Decrements reference count for a given entry and removes entry in FD table
// if that reference count drops to 0, calls vfs_close() on the vnode in the entry
// and clears the entry from the OF table
int file_close(int f_descriptor) {

	// Obtain lock for both OF table and individual entry of interest
	lock_acquire(oftable_lock);
	lock_acquire(curproc->fd_table[f_descriptor]->l);

	// Decrement reference count of vnode
	curproc->fd_table[f_descriptor]->ref_cnt--;

	// If the OF table entry needs to be cleared, need to release the lock still
	struct lock *old_lock = curproc->fd_table[f_descriptor]->l;
	bool destroy_lock = false;
	// If reference count of vnode is 0, close vnode and deallocate entry from OF table
	if (curproc->fd_table[f_descriptor]->ref_cnt <= 0) {
		// Deallocate in OF table and close vnode
		vfs_close(curproc->fd_table[f_descriptor]->vnode_ptr);
		struct oftable_entry e = {-1, NULL, -1, -1, NULL};
		(*curproc->fd_table[f_descriptor]) = e;
		destroy_lock = true;
	}
	
	lock_release(oftable_lock);
	lock_release(old_lock);

	// If lock needs to be destroyed, no files are referencing it anyway
	// so we are safe to destroy it
	if (destroy_lock) lock_destroy(old_lock);

	// Clear entry from FD Table for this process
	curproc->fd_table[f_descriptor] = NULL;
	
	return 0;
}

// Finds entry referenced by f_descriptor index in FD table, and calls a read on the vnode
// of this entry, for a given number of bytes (buflen), returns the number of bytes that could
// actually be read (num_bytes_read), and stores these in given buffer (buffer) and updates 
// the file pointer offset of the OF table entry based on the number of bytes that were read
int file_read(int f_descriptor, void *buffer, size_t buflen, int *num_bytes_read) {
    struct uio user_uio;
	struct iovec iov;
	struct oftable_entry *oft_e;
	int error; 					// Used to store result of calls

	oft_e = curproc->fd_table[f_descriptor];

	// Make sure to acquire lock on entry before accessing
	lock_acquire(oft_e->l);

	// If it wasn't opened for reading, must return EBADF too
	if ((oft_e->accmode != O_RDONLY) && (oft_e->accmode != O_RDWR)) {
		lock_release(oft_e->l);
		(*num_bytes_read) = -1;
		return EBADF;
	}

	// initialise uio for reading requested data
	uio_uinit(&iov, &user_uio, buffer, buflen, oft_e->f_ptr, UIO_READ);
	
	// do the read
	error = VOP_READ(oft_e->vnode_ptr, &user_uio);

	// If we encountered an error during the read, release lock and return error
	if (error != 0) {
		lock_release(oft_e->l);
		(*num_bytes_read) = -1;
		return error;
	}

	// uio updates its own offset variable to reflect how many bytes were read
	// so we can use this to update our own f_ptr variable
	oft_e->f_ptr = user_uio.uio_offset;

	// Leave critical section
	lock_release(oft_e->l);

	// read() call returns the number of bytes that were successfully read
	(*num_bytes_read) = buflen - user_uio.uio_resid;

	return 0;
}

// Write functions the same way as read but instead of reading from the file and 
// storing the bytes in buffer, it takes the bytes in buffer and writes them to the file
// (see comments on file_read for more information)
int file_write(int f_descriptor, void *buffer, size_t buflen, int *num_bytes_written) {
    struct uio user_uio;
	struct iovec iov;
	struct oftable_entry *oft_e;
	int error; 					// Used to store result of calls

	oft_e = curproc->fd_table[f_descriptor];

	// Make sure to acquire lock on entry before accessing
	lock_acquire(oft_e->l);

	// If it wasn't opened for reading, must return EBADF too
	if ((oft_e->accmode != O_WRONLY) && (oft_e->accmode != O_RDWR)) {
		lock_release(oft_e->l);
		(*num_bytes_written) = -1;
		return EBADF;
	}

	// initialise uio for writing requested data
	uio_uinit(&iov, &user_uio, buffer, buflen, oft_e->f_ptr, UIO_WRITE);
	
	// do the write
	error = VOP_WRITE(oft_e->vnode_ptr, &user_uio);

	// If we encountered an error during the write, release lock and return error
	if (error != 0) {
		lock_release(oft_e->l);
		(*num_bytes_written) = -1;
		return error;
	}

	// uio updates its own offset variable to reflect how many bytes were read
	// so we can use this to update our own f_ptr variable
	oft_e->f_ptr = user_uio.uio_offset;

	// Leave critical section
	lock_release(oft_e->l);

	// read() call returns the number of bytes that were successfully read
	(*num_bytes_written) = buflen - user_uio.uio_resid;

	return 0;
}

// Clones a file descriptor (old_fd) into a new file descriptor (new_fd) and returns (new_fd)
// If new_fd currently references an open file, that file is closed
// After dup2 is called, any call on new_fd will act on the same file with the same file pointer
// offset as old_fd
int file_dup2(int old_fd, int new_fd, int *ret_fd) {
	// Check if new_fd is open already, if it is -> close it
    // can call file_close directly since new_fd is already checked in sys_dup2
    // for range validity
	if (curproc->fd_table[new_fd] != NULL) {
		int result = file_close(new_fd);
		if (result != 0) {
			(*ret_fd) = -1;
			return result;
		}
	}

	// Check open file limit for FD table, otherwise EMFILE
	bool found_slot = false;
	for (int i = 0; i < __OPEN_MAX; i++) {
		if (curproc->fd_table[i] == NULL) {
			found_slot = true;
			break;
		}
	}
	
	if (!found_slot) {
		// No more space for file descriptors, return error
		(*ret_fd) = -1;
		return EMFILE;
	}

	// Perform dup by copying reference to entry into new fd
	// and incrementing reference counter of entry in open file table
	curproc->fd_table[new_fd] = curproc->fd_table[old_fd];

	lock_acquire(curproc->fd_table[new_fd]->l);
	curproc->fd_table[new_fd]->ref_cnt++;
	lock_release(curproc->fd_table[new_fd]->l);

	return 0;
}

// If the file referenced by the given file descriptor (fd) is seekable, moves the file pointer
// offset of this file entry (opened by fd) by offset, starting from whence.
// whence can be:
// SEEK_SET - move to given offset
// SEEK_CUR - add given offset to current file pointer offset
// SEEK_END - add given offset to end of file
int file_lseek(int fd, uint64_t offset, int whence, off_t *retval64) {

	// Make sure object pointed to by file descriptor is seekable
	lock_acquire(curproc->fd_table[fd]->l);
	if (!VOP_ISSEEKABLE(curproc->fd_table[fd]->vnode_ptr)) {
		lock_release(curproc->fd_table[fd]->l);
		return ESPIPE;
	}

	off_t new_pos = 0;

	// whence can be SEEK_SET, SEEK_CUR, or SEEK_END
	// Any other values should fail
	// SEEK_SET -> f_ptr becomes the specifed offset
	// SEEK_CUR -> f_ptr becomes current f_ptr + offset
	// SEEK_END -> f_ptr becomes end of file + offset
	switch(whence) {
		case SEEK_SET:
			new_pos = offset;
			break;
		case SEEK_CUR:
			new_pos = curproc->fd_table[fd]->f_ptr + offset;
			break;
		case SEEK_END:
			; struct stat *statbuf = kmalloc(sizeof(struct stat));
			// Get file size
			int error = 0;
			error = VOP_STAT(curproc->fd_table[fd]->vnode_ptr, statbuf);
			if (error != 0) {
				lock_release(curproc->fd_table[fd]->l);
				return error;
			}
			new_pos = (int)statbuf->st_size + offset;
			kfree(statbuf);
			break;
		default:
			lock_release(curproc->fd_table[fd]->l);
			return EINVAL;
	}
	
    // If new_pos is not a legal value, release lock and return error
	if (new_pos < 0) {
		lock_release(curproc->fd_table[fd]->l);
		return EINVAL;
	}

	// new_pos is a legal value (non-negative) so we can update the f_ptr of the entry
	curproc->fd_table[fd]->f_ptr = new_pos;
	lock_release(curproc->fd_table[fd]->l);
	
	(*retval64) = new_pos;
	return 0;
}