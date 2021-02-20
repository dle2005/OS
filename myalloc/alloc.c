#include "alloc.h"
struct info memory[4096];

int init_alloc() {
    if((page_address = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE,
            MAP_ANON|MAP_SHARED, -1, 0)) == MAP_FAILED) {
        perror("init_alloc()");
        return -1;
    }

    free_memory = PAGESIZE;
    buffer_index = 0;

    return_address = page_address;

    for(int i = 0; i < 4096; i++) {
        memory[i].chunk = 0;
        memory[i].isfree = true;
        memory[i].address = NULL;
    }

    return 0;
}

int cleanup() {
    for(int i = 0; i < 4096; i++) {
        memory[i].chunk = 0;
        memory[i].isfree = true;
        memory[i].address = NULL;
    }

    page_address = NULL;
    return_address = NULL;

    free_memory = 0;
    buffer_index = 0;

    if(munmap(page_address, PAGESIZE) == -1) {
        perror("cleanup()");
        return -1;
    }
    return 0;
}

char *alloc(int buffer) {
    if(buffer % 8 != 0 || buffer > PAGESIZE || free_memory == 0) {
        perror("alloc()");
        return NULL;
    }

    int count = 0;

    for(int i = 0; i < 4096; i++) {
        if(memory[buffer_index].isfree) count++;
        else count = 0;

        if(count == buffer) {
            buffer_index = buffer_index + 1;
            for(int i = 0; i < buffer; i++) 
                memory[buffer_index - buffer + i].isfree = false;
            memory[buffer_index - buffer].chunk = buffer;
            memory[buffer_index - buffer].address = page_address + 
                buffer_index - buffer;
            return_address = memory[buffer_index - buffer].address;
            free_memory -= buffer;
            buffer_index = buffer_index % 4096;
            break;
        }
        
        buffer_index = (buffer_index + 1) % 4096;
    }
    return return_address;
}

void dealloc(char *buffer) {
    int dealloc_index;
    
    for(int i = 0; i < 4096; i++) {
        if(memory[i].address == buffer) {
            dealloc_index = i;
            break;
        }
    }
    free_memory += memory[dealloc_index].chunk;

    for(int i = dealloc_index; i < dealloc_index +memory[dealloc_index].chunk; i++)
        memory[i].isfree = true;
    memory[dealloc_index].chunk = 0;
    memory[dealloc_index].address = NULL;
}

