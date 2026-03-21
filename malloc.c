#include <unistd.h>
#include <stddef.h>

struct header{
    size_t data;
    int free;
    struct header *next;
    struct header *prev;
};

static head = NULL;

struct header *getLastBlock(struct header *last){
    while(last->next != NULL){
        last=last->next;
    }
    return last;
}

struct header *find_block(struct header **last, size_t size){
    struct header *current = head;
    while(current != NULL && !(current->free==0 && current->data>=size)){
            *last=current;
            current=current->next;
            }
    return current;
}

void free(void *ptr){
    if(!ptr) return;

    struct header *block = (struct header*) ptr -1;
    block->free=1;

    if(block->next && block->next->free){
        block->size += sizeof(struct header) + block->next->size;
        block->next=block->next->next;
        block->next->prev=block;
    }
    if(block->prev && block->prev->free){
        struct header *prev_block = block->free;
        prev_block->size += sizeof(struct header) + block->next->size;
        prev_block->next = block->next;
        if(block->next){
        block->next->prev= prev_block;
        }
    }
}

void split(TreeNode *block, size_t size){
    struct header *new_block = (struct header*)((char*)block + sizeof(header) + size);
    new_block->size = block->size - size - sizeof(struct header);
    new_block->free=1;
    new_block->next = block->next;
    new_block->prev=block;
    if(new_block->next){
        new_block->next->prev=new_block;
    }
    block->size=size;
    block->next=new_block;
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
        block=find_block(&last, real_size);
        if(block){
            block->free = 0;
        }else{
        block = sbrk(real_size + header_size);
        if(block == (void *) -1) return NULL;
        last->next = block;
    }
    }

    block->size= real_size;
    block->free = 0;
    block->prev=last;
    block->next = NULL;
    


    
    return (block + 1);
}
