

#include "shared_memory_descriptor.h"

shared_memory_descriptor::shared_memory_descriptor() {

}

shared_memory_descriptor::~shared_memory_descriptor() {

}

void *shared_memory_descriptor::get_instance_ptr() {
    return instance_ptr;
}

void shared_memory_descriptor::set_instance_ptr(void *ptr) {
    instance_ptr = ptr;
}

enum memory_errno shared_memory_descriptor::get_error_no() {
    return error_no;
}

void shared_memory_descriptor::memory_init() {
    available_blocks = DEFAULT_SIZE;
    first_block = 0;

    for(int i = 0; i < DEFAULT_SIZE-1; i++){
        blocks[i] = i+1;
    }

    blocks[DEFAULT_SIZE-1] = NULL_BLOCK;

    error_no = E_SUCCESS;
}

void *shared_memory_descriptor::memory_allocate(size_t size) {
    //WE ALLOCATE ONE MORE BLOCK ONE THE LEFT, And WE RETURN THE NEXT POINTER, SO THAT WE HAVE THE SIZE TO FREE MEMORY
    size += 8;

    if(size > available_blocks*8){
        error_no = E_NOMEM;
        return (void*)-1;
    }

    memory_page_t parent_block = -1;
    memory_page_t block_data = first_block;

    memory_page_t second_try = 0;
    while(blocks[blocks[block_data]] != NULL_BLOCK){
        memory_page_t consecutives = nb_consecutive_blocks(block_data);
        if(size <= consecutives*8){
            //ALLOCATE
            memory_page_t end_block_data = blocks[block_data];
            for(memory_page_t i = 0; i < size-8; i+=8){
                end_block_data = blocks[end_block_data];
            }
            //initialize_buffer(block_data, size);

            if(parent_block == -1){
                first_block = end_block_data;
            }
            else{
                blocks[parent_block] = end_block_data;
            }

            available_blocks -= (size-1)/8+1;

            error_no = E_SUCCESS;

            blocks[block_data] = size-8;

            memory_page_t address = (8*(block_data+1));

            address += (memory_page_t)&blocks[0];

            return (void*)address;
        }

        parent_block = block_data;
        block_data = blocks[block_data];

        //2.b
        if(blocks[blocks[block_data]] == NULL_BLOCK){
            if(second_try == 0){
                second_try = 1;
                memory_reorder();
                block_data = first_block;
            }
            else{
                break;
            }
        }
    }

    //2.b
    error_no = E_SHOULD_PACK;
    return (void*)-1;
}

void shared_memory_descriptor::memory_free(void *addressPtr) {
    //addressPtr -= &m.blocks[0];
    memory_page_t address = ((memory_page_t) addressPtr - (memory_page_t)&blocks[0]) / 8;

    address -= 1;
    memory_page_t size = blocks[address] + sizeof(memory_page_t); //(8)


    memory_page_t old_first_block = first_block;
    first_block = address;
    memory_page_t last_block_offset = (size-1)/8;
    for(memory_page_t i = 0; i < last_block_offset; i++){
        blocks[address+i] = address+i+1;
    }
    blocks[address+last_block_offset] = old_first_block;

    available_blocks += last_block_offset+1;

    error_no = E_SUCCESS;
}

memory_page_t shared_memory_descriptor::nb_consecutive_blocks(memory_page_t first) {
    memory_page_t block_data = first;
    memory_page_t value = 1;

    while(block_data != NULL_BLOCK){
        if(block_data + 1 == blocks[block_data]){
            value++;
        }
        else{
            break;
        }

        block_data = blocks[block_data];
    }
    error_no = E_SUCCESS;

    return value;
}

void shared_memory_descriptor::initialize_buffer(memory_page_t start_index, size_t size) {
    char* ptr = (char*)&blocks[start_index];
    for(memory_page_t i=0; i<size; i++) {
        ptr[i]=0;
    }
}

void shared_memory_descriptor::memory_reorder() {
    for(memory_page_t i = available_blocks-1; i >= 0; i--){
        //Pour que ça marche avec m.first_block aussi
        if(blocks[first_block] < first_block){
            memory_page_t block = first_block;
            memory_page_t next_block = blocks[first_block];
            memory_page_t next_next_block = blocks[blocks[first_block]];

            first_block = next_block;
            blocks[next_block] = block;
            blocks[block] = next_next_block;
        }

        //Et avec tous les suivants
        memory_page_t current_block = first_block;
        for(memory_page_t j = 0; j < i-1; j++){
            if(blocks[blocks[current_block]] < blocks[current_block]){
                memory_page_t block = blocks[current_block];
                memory_page_t next_block = blocks[blocks[current_block]];
                memory_page_t next_next_block = blocks[blocks[blocks[current_block]]];

                blocks[current_block] = next_block;
                blocks[next_block] = block;
                blocks[block] = next_next_block;
            }
            current_block = blocks[current_block];
        }
    }
}
