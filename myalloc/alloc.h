#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGESIZE 4096 //size of memory to allocate from OS
#define MINALLOC 8 //allocations will be 8 bytes or multiples of it

char* page_address;
char* return_address;

int free_memory;
int buffer_index;


struct info {
    int chunk;
    bool isfree;
    char* address;
};
    

// function declarations
int init_alloc();
int cleanup();
char *alloc(int);
void dealloc(char *);
