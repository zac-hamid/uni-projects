//
//  COMP1927 Assignment 1 - Vlad: the memory allocator
//  allocator.c ... implementation
//
//  Created by Liam O'Connor on 18/07/12.
//  Modified by John Shepherd in August 2014, August 2015
//  Copyright (c) 2012-2015 UNSW. All rights reserved.
//

#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define HEADER_SIZE    sizeof(struct free_list_header)  
#define MAGIC_FREE     0xDEADBEEF
#define MAGIC_ALLOC    0xBEEFDEAD

#define TRUE 1
#define FALSE 0

typedef unsigned char byte;

//DONT FORGET TO CHANGE BACK AND REMOVE #INCLUDE!!!!

typedef uint32_t vlink_t;
typedef uint32_t vsize_t;
typedef uint32_t vaddr_t;

//DONT FORGET TO CHANGE BACK AND REMOVE #INCLUDE!!!!

typedef struct free_list_header {
   uint32_t magic;  // ought to contain MAGIC_FREE
   vsize_t size;     // # bytes in this block (including header)
   vlink_t next;     // memory[] index of next free block
   vlink_t prev;     // memory[] index of previous free block
} free_header_t;

// Global data

free_header_t* freePointer(vaddr_t index);
free_header_t* halfCurr(free_header_t* curr);
vlink_t freeIndex(free_header_t* pointer);

static byte *memory = NULL;   // pointer to start of allocator memory
static vaddr_t free_list_ptr; // index in memory[] of first block in free list
static vsize_t memory_size;   // number of bytes malloc'd in memory[]


// Input: size - number of bytes to make available to the allocator
// Output: none              
// Precondition: Size is a power of two.
// Postcondition: `size` bytes are now available to the allocator
// 
// (If the allocator is already initialised, this function does nothing,
//  even if it was initialised with different size)

void vlad_init(uint32_t size)
{
   int found = FALSE;
   int counter = 2;
   //Loop through and find most suitable 2^n byte-size for memory block
   while(!found){
      if (size < HEADER_SIZE){
         fprintf(stderr, "vlad_init: memory too small to allocate\n");
         abort();
      }
      if (size > counter){ //if size is greater than power of 2, increment counter
         counter *= 2;
	  } else found = TRUE; //size is now less than counter and can be malloc()'d
   }
   memory = malloc(counter);
   if (memory == NULL){
      fprintf(stderr, "vlad_init: insufficient memory\n");
      abort();
   }
   //Set default header values
   free_header_t* header;
   memory_size = counter;
   header = (void*)memory;
   header->magic = MAGIC_FREE;
   header->size = counter;
   header->next = (vlink_t)0;
   header->prev = (vlink_t)0;
   free_list_ptr = (vaddr_t)0;
}


// Input: n - number of bytes requested
// Output: p - a pointer, or NULL
// Precondition: n is < size of memory available to the allocator
// Postcondition: If a region of size n or greater cannot be found, p = NULL 
//                Else, p points to a location immediately after a header block
//                      for a newly-allocated region of some size >= 
//                      n + header size.

void *vlad_malloc(uint32_t n)
{
   free_header_t* curr = freePointer(free_list_ptr);
   free_header_t* newHeader = curr;
   free_header_t* prev = curr;
   free_header_t* checker = freePointer(curr->next);
   int check_size = freePointer(free_list_ptr)->size;
   void *p = NULL;
   //Traverse free list to find the smallest available block to fit n into
   while((freeIndex(checker) != free_list_ptr) && (freeIndex(checker) != checker->next)){
      if ((n <= (checker->size - HEADER_SIZE)) && (checker->size <= check_size)){
         check_size = checker->size;
         curr = checker;
      }
      checker = freePointer(checker->next);
   }
   //There's only so many bytes to go around.
   if (n > memory_size){
      fprintf(stderr, "vlad_malloc: insufficient memory available\n");
      p = NULL;
      return p;
   //can't assign 0 bytes... That's silly
   } else if (n == 0){
      p = NULL;
      return p;
   } else {
      if(freePointer(free_list_ptr)->magic == MAGIC_FREE){
         while(TRUE){
            p = curr;
            prev = curr;
            //Check if n can be fit into a smaller block, if so, divide the block
            if (n <= (curr->size/2) - HEADER_SIZE){
               curr = halfCurr(curr);
               prev->size /= 2;
               newHeader = curr;
               newHeader->magic = MAGIC_FREE;
               newHeader->size = prev->size;
               newHeader->prev = freeIndex(prev);
               newHeader->next = prev->next;
               freePointer(newHeader->next)->prev = freeIndex(newHeader);
               prev->next = freeIndex(newHeader);
            } else {
               break;
            }
         }
      } else {
         fprintf(stderr,"Memory corrupted.\n");
         abort();
      }
   }
   //If there is only one thing in the list (after splitting)
   //NULL is returned (otherwise free list gets lost)
   if (freeIndex(curr) == curr->next){
      return NULL;
   }

   curr->magic = MAGIC_ALLOC;
   
   //If the allocated block is at the start of the free list, shift free_list_ptr across
   if(freeIndex(curr) == free_list_ptr){
      free_list_ptr = curr->next;
   }
   
   //Remove allocated block from free list
   freePointer(curr->prev)->next = curr->next;
   freePointer(curr->next)->prev = curr->prev;
   return (void*)(p + HEADER_SIZE);
}

free_header_t* halfCurr(free_header_t* curr){
	curr = freePointer((vaddr_t)(((byte*)curr-memory)+(curr->size/2)));
	return curr;
}

free_header_t* freePointer(vaddr_t index){
   free_header_t* pointer;
   pointer = (free_header_t*)(&memory[index]);
   return pointer;
}

vlink_t freeIndex(free_header_t* pointer){
   vlink_t index;
   index = (vlink_t)(((byte*)pointer - memory));
   return index;
}

void mergeFront(free_header_t** p, free_header_t* next){
   (*p)->next = next->next;
   freePointer(next->next)->prev = freeIndex(*p);
   (*p)->size *= 2;
   next->size = 0;
}

void mergeBack(free_header_t** p, free_header_t* prev){
   prev->next = (*p)->next;
   prev->size *= 2;
   freePointer((*p)->next)->prev = freeIndex(prev);
   (*p)->size = 0;
   *p = freePointer((*p)->prev);
}

// Input: object, a pointer.
// Output: none
// Precondition: object points to a location immediately after a header block
//               within the allocator's memory.
// Postcondition: The region pointed to by object can be re-allocated by 
//                vlad_malloc

void vlad_free(void *object)
{
   free_header_t* checker = freePointer(freePointer(free_list_ptr)->next);
   free_header_t* prev = freePointer(free_list_ptr);
   free_header_t* curr = (free_header_t*)(object - HEADER_SIZE);
   //Only want to free blocks that are already allocated
   if(curr->magic == MAGIC_ALLOC){
      //If there's only one thing in the list (special case)
      if(freeIndex(checker) == checker->next){
            curr->next = freeIndex(checker);
            curr->prev = freeIndex(checker);
            checker->next = freeIndex(curr);
            checker->prev = freeIndex(curr);
            curr->magic = MAGIC_FREE;
         //If the block being free'd is before free_list_ptr, assign free_list_ptr to this
         if (freeIndex(curr) < free_list_ptr){
            free_list_ptr = freeIndex(curr);
         }
         //Checks to see if block can be merged forwards or backwards
         if ((freePointer(curr->prev)->size == curr->size) && (curr->prev == (freeIndex(curr)-(curr->size)))){
            mergeBack(&curr, freePointer(curr->prev));
         } else if ((freePointer(curr->next)->size == curr->size) && (curr->next == (freeIndex(curr)+(curr->size)))){
            mergeFront(&curr, freePointer(curr->next));
         }
      }
      //Continue this while loop until the free list has been searched through once completely
      //There must also be more than one thing in the list
      while((freeIndex(checker) != free_list_ptr) && (freeIndex(checker) != checker->next)){
         //If the block to be free'd is at the back of the list (special case)
         if(checker->next == free_list_ptr){
            //If block is between prev and checker (checker is free_list_ptr->next)
            if((freeIndex(curr)) > freeIndex(prev)){
               if (freeIndex(curr) < freeIndex(checker)){
                  curr->next = freeIndex(checker);
                  curr->prev = checker->prev;
                  freePointer(checker->prev)->next = freeIndex(curr);
                  checker->prev = freeIndex(curr);
               } else {
                  curr->next = checker->next;
                  checker->next = freeIndex(curr);
                  curr->prev = freeIndex(checker);
               }
               curr->magic = MAGIC_FREE;
               while(TRUE){
                  //Same check as before while loop
                  if ((freePointer(curr->prev)->size == curr->size) && (curr->prev == (freeIndex(curr)-(curr->size)))){
                     mergeBack(&curr, freePointer(curr->prev));
                  } else if ((freePointer(curr->next)->size == curr->size) && (curr->next == (freeIndex(curr)+(curr->size)))){
                     mergeFront(&curr, freePointer(curr->next));
                  } else break;
               }
               break;
            }
         //Is block before free_list_ptr?
         } else if (freeIndex(curr) < free_list_ptr){
            curr->next = free_list_ptr;
            curr->prev = freePointer(free_list_ptr)->prev;
            freePointer(free_list_ptr)->prev = freeIndex(curr);
            freePointer(curr->prev)->next = freeIndex(curr);
            free_list_ptr = freeIndex(curr);
            curr->magic = MAGIC_FREE;
            //If it's not at the start of the memory block (special case)
            if(curr != 0){
               while(TRUE){
                  //Same check as above
                  if((freePointer(curr->prev)->size == curr->size) && (curr->prev == (freeIndex(curr)-(curr->size)))){
                        mergeBack(&curr, freePointer(curr->prev));
                  } else if ((freePointer(curr->next)->size == curr->size) && (curr->next == (freeIndex(curr)+(curr->size)))){
                        mergeFront(&curr, freePointer(curr->next));
                  } else break;
               }
            } else {
               while(TRUE){
                  //Obviously if it's at the start of the block, it can only be merged forwards
                  if ((freePointer(curr->next)->size == curr->size) && (curr->next == (freeIndex(curr)+(curr->size)))){
                     mergeFront(&curr, freePointer(curr->next));
                  } else break;
               }
            }
            break;
         } else {
            //Special case, somewhere in the middle of the memory block
            if ((freeIndex(curr) < checker->next) && (freeIndex(curr) > freeIndex(prev))){
               //Traverse checker so free'd block is inserted correctly into the list
               while (checker->size > curr->size){
                  checker = freePointer(checker->next);
               }
               curr->next = checker->next;
               checker->next = freeIndex(curr);
               curr->prev = freeIndex(checker);
               curr->magic = MAGIC_FREE;
               while(TRUE){
                  //Same check as above
                  if((freePointer(curr->prev)->size == curr->size) && (curr->prev == (freeIndex(curr)-(curr->size)))){
                     mergeBack(&curr, freePointer(curr->prev));
                  } else if ((freePointer(curr->next)->size == curr->size) && (curr->next == (freeIndex(curr)+(curr->size)))){
                     mergeFront(&curr, freePointer(curr->next));
                  } else break;
               }
               break;
            }
         }
         //Don't want an infinite loop... Right?
         prev = checker;
         checker = freePointer(checker->next);
      }
   } else {
      fprintf(stderr, "Attempt to free non-allocated memory");
      abort();
   }
}

// Stop the allocator, so that it can be init'ed again:
// Precondition: allocator memory was once allocated by vlad_init()
// Postcondition: allocator is unusable until vlad_int() executed again

void vlad_end(void)
{
   free(memory);
}


// Precondition: allocator has been vlad_init()'d
// Postcondition: allocator stats displayed on stdout

void vlad_stats(void)
{
   // TODO
   // put whatever code you think will help you
   // understand Vlad's current state in this function
   // REMOVE all pfthese statements when your vlad_malloc() is done
   free_header_t* curr = freePointer(free_list_ptr);
   int i=0;
   while(TRUE){
      printf("====== Header %2d ========\n", i);
      printf("Index: %u\n", (void*)curr - (void*)memory);
      printf("Next: %u\n", curr->next);
      printf("Prev: %u\n", curr->prev);
      printf("Size: %u\n", curr->size);
      printf("Magic: %x\n", curr->magic);
      printf("==========================\n");
      if(curr->next == free_list_ptr){
         break;
      } else {
         curr = freePointer(curr->next);
         i++;
      }
   }
   return;
}


//
// All of the code below here was written by Alen Bou-Haidar, COMP1927 14s2
//

//
// Fancy allocator stats
// 2D diagram for your allocator.c ... implementation
// 
// Copyright (C) 2014 Alen Bou-Haidar <alencool@gmail.com>
// 
// FancyStat is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or 
// (at your option) any later version.
// 
// FancyStat is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>


#include <string.h>

#define STAT_WIDTH  32
#define STAT_HEIGHT 16
#define BG_FREE      "\x1b[48;5;35m" 
#define BG_ALLOC     "\x1b[48;5;39m"
#define FG_FREE      "\x1b[38;5;35m" 
#define FG_ALLOC     "\x1b[38;5;39m"
#define CL_RESET     "\x1b[0m"


typedef struct point {int x, y;} point;

static point offset_to_point(int offset,  int size, int is_end);
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20], 
   int offset, char * label);



// Print fancy 2D view of memory
// Note, This is limited to memory_sizes of under 16MB
void vlad_reveal(void *alpha[26])
{
  int i, j;
  vlink_t offset;
  char graph[STAT_HEIGHT][STAT_WIDTH][20];
  char free_sizes[26][32];
  char alloc_sizes[26][32];
    char label[3]; // letters for used memory, numbers for free memory
    int free_count, alloc_count, max_count;
    free_header_t * block;

	// TODO
	// REMOVE these statements when your vlad_malloc() is done
    //printf("vlad_reveal() won't work until vlad_malloc() works\n");
    //return;

    // initilise size lists
    for (i=0; i<26; i++) {
       free_sizes[i][0]= '\0';
       alloc_sizes[i][0]= '\0';
    }

    // Fill graph with free memory
    offset = 0;
    i = 1;
    free_count = 0;
    while (offset < memory_size){
       block = (free_header_t *)(memory + offset);
       if (block->magic == MAGIC_FREE) {
         snprintf(free_sizes[free_count++], 32, 
           "%d) %d bytes", i, block->size);
         snprintf(label, 3, "%d", i++);
         fill_block(graph, offset,label);
      }
      offset += block->size;
   }

    // Fill graph with allocated memory
   alloc_count = 0;
   for (i=0; i<26; i++) {
    if (alpha[i] != NULL) {
      offset = ((byte *) alpha[i] - (byte *) memory) - HEADER_SIZE;
      block = (free_header_t *)(memory + offset);
      snprintf(alloc_sizes[alloc_count++], 32, 
        "%c) %d bytes", 'a' + i, block->size);
      snprintf(label, 3, "%c", 'a' + i);
      fill_block(graph, offset,label);
   }
}

    // Print all the memory!
for (i=0; i<STAT_HEIGHT; i++) {
 for (j=0; j<STAT_WIDTH; j++) {
   printf("%s", graph[i][j]);
}
printf("\n");
}

    //Print table of sizes
max_count = (free_count > alloc_count)? free_count: alloc_count;
printf(FG_FREE"%-32s"CL_RESET, "Free");
if (alloc_count > 0){
 printf(FG_ALLOC"%s\n"CL_RESET, "Allocated");
} else {
 printf("\n");
}
for (i=0; i<max_count;i++) {
 printf("%-32s%s\n", free_sizes[i], alloc_sizes[i]);
}

}

// Fill block area
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20], 
   int offset, char * label)
{
  point start, end;
  free_header_t * block;
  char * color;
  char text[3];
  block = (free_header_t *)(memory + offset);
  start = offset_to_point(offset, memory_size, 0);
  end = offset_to_point(offset + block->size, memory_size, 1);
  color = (block->magic == MAGIC_FREE) ? BG_FREE: BG_ALLOC;

  int x, y;
  for (y=start.y; y < end.y; y++) {
    for (x=start.x; x < end.x; x++) {
      if (x == start.x && y == start.y) {
                // draw top left corner
        snprintf(text, 3, "|%s", label);
     } else if (x == start.x && y == end.y - 1) {
                // draw bottom left corner
        snprintf(text, 3, "|_");
     } else if (y == end.y - 1) {
                // draw bottom border
        snprintf(text, 3, "__");
     } else if (x == start.x) {
                // draw left border
        snprintf(text, 3, "| ");
     } else {
        snprintf(text, 3, "  ");
     }
     sprintf(graph[y][x], "%s%s"CL_RESET, color, text);            
  }
}
}

// Converts offset to coordinate
static point offset_to_point(int offset,  int size, int is_end)
{
    int pot[2] = {STAT_WIDTH, STAT_HEIGHT}; // potential XY
    int crd[2] = {0};                       // coordinates
    int sign = 1;                           // Adding/Subtracting
    int inY = 0;                            // which axis context
    int curr = size >> 1;                   // first bit to check
    point c;                                // final coordinate
    if (is_end) {
       offset = size - offset;
       crd[0] = STAT_WIDTH;
       crd[1] = STAT_HEIGHT;
       sign = -1;
    }
    while (curr) {
       pot[inY] >>= 1;
       if (curr & offset) {
         crd[inY] += pot[inY]*sign; 
      }
        inY = !inY; // flip which axis to look at
        curr >>= 1; // shift to the right to advance
     }
     c.x = crd[0];
     c.y = crd[1];
     return c;
  }