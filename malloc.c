#include <unistd.h>
#include <stddef.h>

struct header{
    size_t data;
    int free;
    struct header *next;
};

static head = NULL;

struct header *getLastBlock(struct header *last){
    while(last->next != NULL){
        last=last->next;
    }
    return last;
}

void *malloc(size_t size){

    if(size<=0){
        return NULL;
    }
    size_t real_size = ((size+7)/8)*8;
    size_t header_size = sizeof(struct header);
    struct header* block;

    if(head == NULL){
        block = sbrk(real_size + header_size);
        if(block == (void *) -1) return NULL;
        head=block;
    }else{
        struct header *last= getLastBlock(head);
        block = sbrk(real_size + header_size);
        if(block == (void *) -1) return NULL;
        last->next = block;
    }

    block->size= real_size;
    block->free = 0;
    block->next = NULL;
    


    
    return (block + 1);
}
