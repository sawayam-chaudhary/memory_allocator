#include <unistd.h>
#include <stddef.h>

#define flag 0x1
#define mask ~0x7

struct header{
    size_t data_flag;
    struct header *next;
    struct header *prev;
};

static struct header* head = NULL;

size_t get_size(struct header *block){
    return block->data_flag & mask;
}

int is_free(struct header *block){
    return !(block->data_flag & flag);
}

void set_free(struct header *block, size_t size){
    block->data_flag = (size & mask);
}

void set_busy(struct header *block, size_t size){
    block->data_flag = (size & mask) | flag;
}




struct header *find_block(struct header **last, size_t size){
    struct header *current = head;
    while(current){
        if(is_free(current) && get_size(current) >= size){
            return current;
            }
    *last = current;
    current=current->next;
}
return NULL;
}

void free(void *ptr){
    if(!ptr) return;

    struct header *block = (struct header*) ptr -1;
    set_free(block, get_size(block));

    if(block->next && is_free(block->next)){
        size_t size= get_size(block) + sizeof(struct header) + get_size(block->next);
        set_free(block,size);
        block->next=block->next->next;
        if(block->next){
        block->next->prev=block;
        }
    }
    if(block->prev && is_free(block->prev)){
        struct header *prev_block = block->prev;
        size_t size = get_size(block) + sizeof(struct header) + get_size(prev_block);
        set_free(prev_block,size);
        prev_block->next = block->next;
        if(block->next){
        block->next->prev= prev_block;
        }
    }
}

void split(struct header *block, size_t size){
    size_t full_size = get_size(block);
    if(full_size >= size + sizeof(struct header) + 8){
    struct header *new_block = (struct header*)((char*)block + sizeof(struct header) + size);
    set_free(new_block, full_size - size - sizeof(struct header));
    new_block->next = block->next;
    new_block->prev=block;
    if(new_block->next){
        new_block->next->prev=new_block;
    }
    
    block->next=new_block;
    set_busy(block, size);
    }
}

void mm_checkheap(){
    struct header *current = head;
    while(current){
        size_t size = get_size(current);
        int free_status = is_free(current);
        printf("[Addr: %p] | Size: %zu | Status: %s | Next: %p\n", 
                (void*)current, size, free_status ? "FREE" : "BUSY", (void*)current->next);
        current=current->next;
    }
}

void *malloc(size_t size){

    if(size<=0){
        return NULL;
    }
    size_t real_size = ((size+7)/8)*8;
    size_t header_size = sizeof(struct header);
    struct header* block = head;
    struct header* last = NULL;
    if(head == NULL){
        block = sbrk(real_size + header_size);
        if(block == (void *) -1) return NULL;
        head=block;
        set_busy(head,real_size);
    }else{
        
        block=find_block(&last, real_size);
        if(block){
            if(get_size(block) >= real_size + sizeof(struct header) + 8){
                split(block, real_size);
            }
            set_busy(block,real_size);
        }else{
        block = sbrk(real_size + header_size);
        if(block == (void *) -1) return NULL;
        last->next = block;
    }
    }
    

    set_busy(block, real_size);
    block->prev=last;
    block->next = NULL;
    


    
    return (block + 1);
}
