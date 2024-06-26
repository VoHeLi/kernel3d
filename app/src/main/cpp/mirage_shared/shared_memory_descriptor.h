#pragma once

#include <stdint.h>

#define NEW_SHARED(a, ...) new(sharedMemoryDescriptor->memory_allocate(sizeof(a))) a(__VA_ARGS__)

#define CTSM(a, T) ((T)sharedMemoryDescriptor->client_to_server_memory((void*)a))
#define STCM(a, T) ((T)sharedMemoryDescriptor->server_to_client_memory((void*)a))

#define NULL_BLOCK INT64_MAX
#define DEFAULT_SIZE 131072 //TODO ADAPT THIS //131072

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

    void* client_to_server_memory(void* addr);
    void* server_to_client_memory(void* addr);

    void memory_init_server();
    void memory_init_client();
    void* memory_allocate(size_t size);
    void memory_free(void* addr);

    //BY CONVENTION, ALL THE PTR ARE STORED WITH THE CLIENT BASE PTR
private:
    void* server_base_ptr;
    void* client_base_ptr;
    void* instance_ptr;
    memory_page_t available_blocks;
    memory_page_t first_block;
    enum memory_errno error_no;
    memory_page_t blocks[DEFAULT_SIZE];

    void memory_reorder();
    memory_page_t nb_consecutive_blocks(memory_page_t first);
    void initialize_buffer(memory_page_t start_index, size_t size);
};

