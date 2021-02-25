/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <spinlock.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>
#include <proc.h>

/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 *
 * UNSW: If you use ASST3 config as required, then this file forms
 * part of the VM subsystem.
 *
 */

/*
 * Create a new address space
 */
struct addrspace * as_create(void) {
	struct addrspace *as;
	// Allocate memory for address space
	if ((as = (struct addrspace *)kmalloc(sizeof(struct addrspace))) == NULL) return NULL;

	// Need space allocated in memory for root page table itself, check if there's space
	if ((as->ptable = (uint32_t **)kmalloc(PTABLE_SIZE * sizeof(uint32_t *))) == NULL) {
		// No space for root page table
		// Free address space and return
		kfree(as);
		return NULL;
	}

	// Regions not loaded yet, just set to NULL
	as->as_regions = NULL;

	// No allocated pages yet so just set all pointers to NULL
	for (int i = 0; i < PTABLE_SIZE; i++) {
		as->ptable[i] = NULL;
	}

	return as;
}

/*
 * Deep copy one address space to another
 */
int as_copy(struct addrspace *old, struct addrspace **ret) {
	struct addrspace *newas;

	// Create new address space
	if ((newas = as_create()) == NULL) return ENOMEM;
	// Preliminary error checks
	if (old == NULL) return EFAULT;
	if (old->ptable == NULL) return EFAULT;

	// Copy regions
	struct region *curr = old->as_regions;

	// Define regions in new address space with same parameters as those in the old address space
	while (curr != NULL) {
		int result = as_define_region(newas, curr->v_address, curr->num_pages * PAGE_SIZE, curr->readable, curr->writeable, curr->executable);
		if (result != 0) {
			as_destroy(newas);
			return ENOMEM;
		}
		curr = curr->next;
	}

	// Loop through list and copy prev_writeable value to new regions
	// Probably not necessary but just to be safe
	curr = old->as_regions;
	struct region *new_curr = newas->as_regions;
	while(curr != NULL) {
		new_curr->prev_writeable = curr->prev_writeable;
		new_curr = new_curr->next;
		curr = curr->next;
	}

	// Copy page table (deep copy)
	for (int i = 0; i < PTABLE_SIZE; i++) {
		// If there is a leaf table for index i, loop through it
		if (old->ptable[i] != NULL) {
			// Create leaf table at this index for newas
			int result = vm_insert_pt_root_entry(newas->ptable, i);
			if (result != 0) {
				as_destroy(newas);
				return result;
			}
			// Loop through leaf table
			for (int j = 0; j < PTABLE_SIZE; j++) {
				// vm_insert_pt_root_entry already initialises all entries to 0 in leaf page table
				// So only need to update non-zero entries
				if (old->ptable[i][j] != 0) {
					// Need to setup new frame and memcpy data to it from old frame
					vaddr_t vkaddr = alloc_kpages(1);
    				if (vkaddr == 0) {
						as_destroy(newas);
						return ENOMEM;
					}
    				bzero((void *)vkaddr, PAGE_SIZE); // Probably not needed but included just in case
					// Can only access kseg0 address directly (not physical address) for memcpy
					vaddr_t old_vkaddr = PADDR_TO_KVADDR(old->ptable[i][j] & PAGE_FRAME);
					memcpy((void *)vkaddr, (const void *)old_vkaddr, PAGE_SIZE);
					// now add this as a PTE to new page table
					newas->ptable[i][j] = (uint32_t)(KVADDR_TO_PADDR(vkaddr) & PAGE_FRAME);
    				newas->ptable[i][j] |= (old->ptable[i][j] & TLBLO_DIRTY);
    				newas->ptable[i][j] |= (old->ptable[i][j] & TLBLO_VALID);
				}
			}
		}
	}

	*ret = newas;
	return 0;
}

/*
 * Clean up all allocated memory before finally freeing address space
 */
void as_destroy(struct addrspace *as) {
	// Preliminary error checks
	if (as == NULL) return;
	if (as->ptable == NULL) return;

	// Clean up in reverse to allocation
	// First, the pages associated with each used PTE in the page table
	for (int i = 0; i < PTABLE_SIZE; i++) {
		if (as->ptable[i] != NULL) {
			for (int j = 0; j < PTABLE_SIZE; j++) {
				if (as->ptable[i][j] != 0) free_kpages(PADDR_TO_KVADDR(as->ptable[i][j] & PAGE_FRAME));
			}
			// Next, the leaf tables themselves
			kfree(as->ptable[i]);
		}
	}
	// Next, free the whole root page table
	kfree(as->ptable);

	// Then, free the regions associated with the address space
    struct region *current = as->as_regions;
	struct region *prev = current;
    while (current != NULL) {
        current = current->next;
        kfree(prev);
        prev = current;
    }

	// Finally, free address space
	kfree(as);
}

/*
 * From dumbvm.c
 */
void as_activate(void) {
	int i, spl;
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

/*
 * From dumbvm.c
 */
void as_deactivate(void) {
	int i, spl;
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

/*
 * Set up a segment at virtual address VADDR of size MEMSIZE. The
 * segment in memory extends from VADDR up to (but not including)
 * VADDR+MEMSIZE.
 *
 * The READABLE, WRITEABLE, and EXECUTABLE flags are set if read,
 * write, or execute permission should be set on the segment. At the
 * moment, these are ignored. When you write the VM system, you may
 * want to implement them.
 */
int as_define_region(struct addrspace *as, vaddr_t vaddr, size_t memsize,
		 int readable, int writeable, int executable) {

	// Allocate memory for new region
	struct region *new_region = (struct region *)kmalloc(sizeof(struct region));
	if (new_region == NULL) return ENOMEM; // Run out of memory, can't allocate new region

	// As per Piazza, if memsize is 0 just ignore it
	if (memsize == 0) return 0;

	// Some code from dumbvm as_define_region()

	size_t npages;

	/* Align the region. First, the base... */
	memsize += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	memsize = (memsize + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = memsize / PAGE_SIZE;

	// End of dumbvm.c as_define_region() code

	// Overflow of address space check
	if ((vaddr + memsize) < vaddr) return EINVAL;

	// Set parameters of new region
	new_region->v_address = vaddr;
	new_region->num_pages = npages;
	new_region->prev_writeable = false;
	new_region->readable = readable;
	new_region->writeable = writeable;
	new_region->executable = executable;
	new_region->next = NULL;
	
	// Need to find where to put region in linked list
	if (as->as_regions != NULL) {
		struct region *current = as->as_regions;
		struct region *prev = current;
		vaddr_t new_reg_top = new_region->v_address + (new_region->num_pages * PAGE_SIZE) - 1;
		// Check if new region goes at the start of the list (edge case)
		if (new_reg_top < as->as_regions->v_address) {
			new_region->next = as->as_regions;
			as->as_regions = new_region;
		} else {
			// Otherwise find where in the list it goes (in order of base address)
			while(current != NULL) {
				current = current->next;
				// If we get to the end of the list, assign new region at end of list and break loop
				if (current == NULL) {
					prev->next = new_region;
					break;
				}
				// Check if new region collides with any other regions
				if (isCollide(prev, new_region)) return EADDRINUSE;
				if (isCollide(new_region, current)) return EADDRINUSE;
				// Check to see where new region goes (regions are ordered by their address)
				vaddr_t prev_top = prev->v_address + (prev->num_pages * PAGE_SIZE) - 1;
				// Does region fit between previous region and current region?
				if ( (prev_top < new_region->v_address) && (new_reg_top < current->v_address)){
					// Add to linked list here
					prev->next = new_region;
					new_region->next = current;
					break;
				}
				prev = prev->next;
			}
		}
	} else {
		// Could be the first entry (in which case as->as_regions would be NULL)
		as->as_regions = new_region;
	}
	return 0;
}

/*
 * Sets all regions to be temporarily writeable
 */
int as_prepare_load(struct addrspace *as) {

	// Preliminary error check
	if (as == NULL) return EFAULT;

	struct region *current = as->as_regions;

	// Loop through regions in address space
	while(current != NULL) {
		current->prev_writeable = current->writeable;
		current->writeable = true;

		current = current->next;
	}

	return 0;
}

/*
 * Returns region permissions back to what they were before as_prepare_load() was called and flushes TLB
 */
int as_complete_load(struct addrspace *as) {

	// Preliminary error checks
	if (as == NULL) return EFAULT;
	if (as->ptable == NULL) return EFAULT;

	struct region *current = as->as_regions;
	int i = 0;
	int j = 0;
	bool move_to_next_region;

	// Loop through all regions in address space
	while(current != NULL) {
		move_to_next_region = false;
		vaddr_t reg_start = current->v_address;
		vaddr_t reg_end = current->v_address + (current->num_pages * PAGE_SIZE) - 1;
		current->writeable = current->prev_writeable;
		// Loop through root page table (and allocated leaf page tables) at the same time
		for (int k = i; k < PTABLE_SIZE; k++) {
			if (as->ptable[k] != NULL) {
				for (int l = j; l < PTABLE_SIZE; l++) {
					// If entry is 0 we can skip it
					if (as->ptable[k][l] == 0) {
						j++; continue;
					}
					// Convert PTE index into a virtual address
					vaddr_t pte_vaddr = (k << 22) | (l << 12);
					// Is this page table entry within the current region?
					if (pte_vaddr >= reg_start && pte_vaddr <= reg_end) {
						// If it is, update it's dirty bit based on region permissions
						// Retain frame address and valid bit, set dirty bit to 0
						as->ptable[k][l] &= (PAGE_FRAME | TLBLO_VALID);
						// Update dirty bit based on region permissions
						as->ptable[k][l] |= (current->writeable << 10);
					} else {
						// Need to move to next region (because current PTE is not within current region)
						// Keep current PTE that we're looking at the same
						move_to_next_region = true;
						break;
					}
					j++;
				}
			}
			// Check is before i++ to keep current PTE being looked at the same when changing to next region
			if (move_to_next_region) break;
			i++;
		}
		current = current->next;
	}
	// Flush TLB after all PTE permissions are fixed
	tlb_flush();
	return 0;
}

/*
 * Defines region for the stack
 */
int as_define_stack(struct addrspace *as, vaddr_t *stackptr) {
	// as_define_stack just calls as_define_region with specific parameters
	// USERSTACK defines the start of KSEG0, since stack can be fixed-size we can just subtract the size from USERSTACK
	// to get the base address
	int ret_result = as_define_region(as, USERSTACK - (STACK_NUM_PAGES * PAGE_SIZE), STACK_NUM_PAGES * PAGE_SIZE, 1, 1, 0);
	// If as_define_region fails, return the error
	if (ret_result != 0) return ret_result;

	/* Initial user-level stack pointer */
	*stackptr = USERSTACK;

	return 0;
}

/*
 * Helper function to flush the TLB through a function call (from dumbvm.c)
 */
void tlb_flush(void) {
	struct addrspace *as;

	as = proc_getas();
	if (as == NULL) {
		return;
	}

	/* Disable interrupts on this CPU while frobbing the TLB. */
	int spl = splhigh();

	for (int i = 0; i < NUM_TLB; i++) {
		tlb_write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

/*
 * Helper function to determine if two regions overlap or collider (for error checking purposes)
 */
bool isCollide(struct region *reg1, struct region *reg2) {
	if (reg1 == NULL || reg2 == NULL) return false;
	uint32_t reg1_start = reg1->v_address;
	uint32_t reg1_end = reg1->v_address + (reg1->num_pages * PAGE_SIZE) - 1;

	uint32_t reg2_start = reg2->v_address;
	uint32_t reg2_end = reg2->v_address + (reg2->num_pages * PAGE_SIZE) - 1;

	if (reg2_start >= reg1_start && reg2_start <= reg1_end) return true;
	if (reg2_end >= reg1_start && reg2_end <= reg1_end) return true;

	if (reg1_start >= reg2_start && reg1_start <= reg2_end) return true;
	if (reg1_end >= reg2_start && reg1_end <= reg2_end) return true;

	return false;
}