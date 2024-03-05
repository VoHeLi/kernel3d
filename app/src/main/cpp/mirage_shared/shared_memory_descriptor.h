#pragma once

#include <stdint.h>

#define NULL_BLOCK INT64_MAX
#define DEFAULT_SIZE 131072 //TODO ADAPT THIS

typedef int64_t memory_page_t;

enum memory_errno {
    E_SUCCESS,			/* success */
    E_NOMEM,			/* error: not enough memory */
    E_SHOULD_PACK,		/* error: not enough consecutive blocks */
};

class shared_memory_descriptor {

public:
    shared_memory_descriptor();
    ~shared_memory_descriptor();

    void* get_instance_ptr();
    void set_instance_ptr(void* ptr);
    enum memory_errno get_error_no();

    void memory_init();
    void* memory_allocate(size_t size);
    void memory_free(void* addr);

private:
    void* instance_ptr;
    memory_page_t available_blocks;
    memory_page_t first_block;
    enum memory_errno error_no;
    memory_page_t blocks[DEFAULT_SIZE];

    void memory_reorder();
    memory_page_t nb_consecutive_blocks(memory_page_t first);
    void initialize_buffer(memory_page_t start_index, size_t size);
};

