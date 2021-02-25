#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/tlb.h>
#include <mips/vm.h>
#include <current.h>
#include <proc.h>
#include <spl.h>


/*
 * Allocates memory for an entry in the root page table and assign all PTE's in this entry to 0
 */
int vm_insert_pt_root_entry(uint32_t **ptable, uint32_t index){
    if (ptable == NULL) return EFAULT; // Shouldn't happen

    // Allocate memory for entry in root page table
    if ((ptable[index] = (uint32_t *)kmalloc(PTABLE_SIZE * sizeof(uint32_t))) == NULL) return ENOMEM;
    
    // Must initialise all the PTE's at index of root table to 0
    for (int i = 0; i < PTABLE_SIZE; i++) {
        ptable[index][i] = 0;
    }
    
    return 0;
}

/*
 * Allocates physical frame and updates PTE at ptable[index_root][index_leaf] with it's information
 */
int vm_insert_pt_leaf_entry(uint32_t **ptable, uint32_t index_root, uint32_t index_leaf, bool dirty_bit, bool valid_bit) {
    if (ptable == NULL) return EFAULT; // Shouldn't happen
    // Check that leaf page table exists
    if (ptable[index_root] == NULL) return EINVAL;

    // index bounds checking
    if ((index_root >= PTABLE_SIZE) || (index_leaf >= PTABLE_SIZE)) return EINVAL;

    // Allocate page
    vaddr_t vkaddr = alloc_kpages(1);
    if (vkaddr == 0) return ENOMEM;

    // zero out the allocated frame
    bzero((void *)vkaddr, PAGE_SIZE);

    // PTE is made up of:
    // physical address of frame (Convert virtual kseg0 address to physical memory address)
    // Dirty bit: writeable or not
    // Valid bit: valid or not
    // This is to make it consistent with TLB entry
    ptable[index_root][index_leaf] = (uint32_t)(KVADDR_TO_PADDR(vkaddr) & PAGE_FRAME);
    ptable[index_root][index_leaf] |= (dirty_bit << 10);
    ptable[index_root][index_leaf] |= (valid_bit << 9);

    return 0;
}

void vm_bootstrap(void) {
    /* Initialise VM sub-system.  You probably want to initialise your 
       frame table here as well.
    */
}

int vm_fault(int faulttype, vaddr_t faultaddress) {

    struct addrspace *as;
    // Check what type of fault occurred
    switch(faulttype) {
        case VM_FAULT_READ:
        case VM_FAULT_WRITE:
            break;
        case VM_FAULT_READONLY:
            // if trying to write to a read only page
            return EFAULT;
        default:
            // Not a legitimate faulttype
            return EINVAL;
    }

	/*
     * From dumbvm.c
	 * No process. This is probably a kernel fault early
	 * in boot. Return EFAULT so as to panic instead of
	 * getting into an infinite faulting loop.
	 */
	if (curproc == NULL) return EFAULT;

	/*
     * From dumbvm.c
	 * No address space set up. This is probably also a
	 * kernel fault early in boot.
	 */
	as = proc_getas();
	if (as == NULL) return EFAULT;

    // No page table set up, this should not happen
    if (as->ptable == NULL) return EFAULT;
    
    // From assignment intro lecture, just extracting page table indexing bits
    // top 10 bits for root table index, bits 12 - 21 for leaf table index
    uint32_t index_root = faultaddress >> 22;
    uint32_t index_leaf = (faultaddress << 10) >> 22;
    
    // index bounds checking
    if ((index_root >= PTABLE_SIZE) || (index_leaf >= PTABLE_SIZE)) return EINVAL;

    bool dirty_bit = 0;
    bool valid_bit = 0;
    bool found_region = false;

    int result;
    bool leaf_table_created = false;

    // Need to check if 2nd level page table exists for index_root, otherwise needs to be created
    if (as->ptable[index_root] == NULL) {
        if ((result = vm_insert_pt_root_entry(as->ptable, index_root)) != 0) return result;
        leaf_table_created = true;
    }

    // Following flowchart from Assignment 3 intro lecture slides

    // Check PTE, make sure faultaddress within a region

    // Valid translation? 0 means no valid translation present, otherwise skip past this and just load TLB
    if (as->ptable[index_root][index_leaf] == 0) {
        // Loop through regions to find which region faultaddress is in
        struct region *current = as->as_regions;
        while(current != NULL) {
            uint32_t region_base = current->v_address;
            uint32_t region_end = region_base + (PAGE_SIZE * current->num_pages) - 1;
            // Check faultaddress sits in region
            if (((uint32_t)faultaddress >= region_base) && ((uint32_t)faultaddress <= region_end)) {
                // faultaddress sits in current region
                // get dirty and valid bits based on regions permissions
                dirty_bit = current->writeable;
                // based on wiki, valid if region is readable, writeable or executable
                if (current->readable || current->writeable || current->executable) valid_bit = 1;
                found_region = true;
                // If PTE sits in valid region, allocate frame, zero-fill it and insert into the page table
                result = vm_insert_pt_leaf_entry(as->ptable, index_root, index_leaf, dirty_bit, valid_bit);
                if (result != 0) {
                    // Free 2nd level page table if it was newly assigned and thus not used
                    if (leaf_table_created) {
                        as->ptable[index_root] = NULL;
                        kfree(as->ptable[index_root]);
                    }
                    return result;
                }
                break; // Found the region so stop looping
            }
            current = current->next;
        }

        if (!found_region) {
            // If faultaddress doesn't sit in any of the regions in address space, return EFAULT
            // Free 2nd level page table if it was newly assigned and thus not used
            if (leaf_table_created) {
                as->ptable[index_root] = NULL;
                kfree(as->ptable[index_root]);
            }
            return EFAULT;
        }
    }

    // Now write to tlb, this will write to tlb even if page table entry exists already (means it was moved out of memory)
    int spl = splhigh();
    tlb_random((faultaddress & PAGE_FRAME), as->ptable[index_root][index_leaf]);
    splx(spl);

    return 0;
}

/*
 *
 * SMP-specific functions.  Unused in our configuration.
 */

void vm_tlbshootdown(const struct tlbshootdown *ts) {
	(void)ts;
	panic("vm tried to do tlb shootdown?!\n");
}