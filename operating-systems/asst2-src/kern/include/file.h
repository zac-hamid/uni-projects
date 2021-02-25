/*
 * Declarations for file handle and file table management.
 */

#ifndef _FILE_H_
#define _FILE_H_

/*
 * Contains some file-related maximum length constants
 */
#include <limits.h>
#include <kern/iovec.h>
#include <uio.h>

// Structure of an entry in the open file table
// Contains information about the file as well as a lock for concurrency control
struct oftable_entry {
    off_t f_ptr;                  // file pointer
    struct vnode *vnode_ptr;    // pointer to vnode
    int ref_cnt;                // number of fd references to this entry
    int accmode;                // access mode for this entry
    struct lock *l;             // lock for concurrency control (for changes made to a single oft_entry)
};

// The table is initialised in main.c at the bottom of boot()
// The FD table is specified in proc.h, inside the proc struct
// And initialised in proc.c
struct oftable_entry of_table[__OPEN_MAX];

// lock for entire OF table (for concurrency with adding/deleting oft_entry blocks)
// Only need this lock for open and close operations
struct lock* oftable_lock;

bool is_fd_valid(int f_descriptor);

void uio_uinit(struct iovec *iov, struct uio *u, userptr_t buf, size_t len, off_t offset, enum uio_rw rw);

// These functions do the file side of their respective syscall
// the sys_<blank> functions in syscall.c do some simple checks 
// and then pass the work on to these functions
int file_open(char *file_path, int openflags, mode_t mode, int* f_descriptor);
int file_close(int f_descriptor);
int file_read(int f_descriptor, void *buffer, size_t buflen, int *num_bytes_read);
int file_write(int f_descriptor, void *buffer, size_t buflen, int *num_bytes_written);
int file_dup2(int old_fd, int new_fd, int *ret_fd);
int file_lseek(int fd, uint64_t offset, int whence, off_t *retval64);

#endif /* _FILE_H_ */
