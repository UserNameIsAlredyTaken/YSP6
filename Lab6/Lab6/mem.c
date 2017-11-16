#include "mem.h"
#include <string.h>
#define SIZE_OF_PAGE 4096

static size_t page_round(size_t size){
	if (size%SIZE_OF_PAGE) {
		return size + (SIZE_OF_PAGE - size%SIZE_OF_PAGE);
	}else {
		return size;
	}
}

void* heap_init(size_t initial_size){
	void* first_chunk = mmap(HEAP_START, page_round(initial_size),
		PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	mem* head = HEAP_START;
	head->next = NULL;
	head->capacity = page_round(initial_size) - sizeof(mem);	
	head->is_free = 1;
	return first_chunk + sizeof(mem);
}

static mem* look_for_chunk(mem* chunk, size_t const query){
	while(chunk!=NULL){
		if(chunk->capacity>= query && chunk->is_free==1){
			return chunk;
		}
		chunk = chunk->next;
	}
	return NULL;
}

static  mem* get_last_chunk(mem* chunk){
	while (chunk->next!=NULL){
		chunk = chunk->next;
	}
	return chunk;
}

static mem* enlarge_chunk(mem* chunk, size_t const query){
	char* enlarge_start = (char*)chunk + chunk->capacity + sizeof(mem);
	size_t enlarge_size = query - chunk->capacity;
	size_t enlarge_real_size = page_round(enlarge_size);
	mem* additional_chunk = NULL;
	if(mmap(enlarge_start, enlarge_real_size,
		PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE| MAP_FIXED, -1, 0)!=MAP_FAILED){
		if(enlarge_real_size-enlarge_size>sizeof(mem)+DEBUG_FIRST_BYTES){
			additional_chunk = (mem*)(enlarge_start + enlarge_size);
			additional_chunk->capacity = enlarge_real_size - enlarge_size - sizeof(mem);
			additional_chunk->is_free = 1;
			additional_chunk->next = NULL;
		}
		chunk->capacity = query;
		chunk->next = additional_chunk;
		return chunk;
	}else{
		return NULL;
	}
}

static mem* create_new_chunk(mem* last_chunk, size_t const query){
	char* new_address = (char*)last_chunk + last_chunk->capacity + sizeof(mem);
	size_t new_size = query + sizeof(mem);
	size_t allocated_real_size = page_round(new_size);
	mem* new_chunk;
	mem* additional_chank = NULL;
	if((new_chunk =  mmap(new_address, allocated_real_size,
		PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0)) == MAP_FAILED &&
		(new_chunk = mmap(NULL, allocated_real_size,
			PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED){
		return NULL;
	}
	if(allocated_real_size- new_size>sizeof(mem)+DEBUG_FIRST_BYTES){
		additional_chank = (mem*)(new_address + new_size);
		additional_chank->capacity = allocated_real_size - new_size - sizeof(mem);
		additional_chank->is_free = 1;
		additional_chank->next = NULL;
	}
	last_chunk->next = new_chunk;
	new_chunk->capacity = query;
	new_chunk->is_free = 0;
	new_chunk->next = additional_chank;
	return new_chunk;
}

void* malloc(size_t query){	
	query = query<DEBUG_FIRST_BYTES?DEBUG_FIRST_BYTES:query;
	mem* chunk = look_for_chunk(HEAP_START, query);
	if(chunk==NULL){
		heap_init(1);
	}
	mem* new = NULL;
	if(chunk){
		if(chunk->capacity-query<sizeof(mem)+DEBUG_FIRST_BYTES){
			/*couldn't be splited*/
			chunk->is_free = 0;
		}else{
			/*could be splited*/
			new = (mem*)((char*)chunk + sizeof(mem) + query);
			new->capacity = chunk->capacity - sizeof(mem) - query;
			new->is_free = 1;
			new->next = chunk->next;
			chunk->next = new;
			chunk->capacity = query;
			chunk->is_free = 0;
		}
	}else{
		chunk = get_last_chunk(HEAP_START);
		if(chunk->is_free && enlarge_chunk(chunk,query)){
			chunk->is_free = 0;
		}else if(new = create_new_chunk(chunk,query)){
			chunk = new;
			chunk->is_free = 0;
		}else{
			return NULL;
		}
	}
	return (char*)chunk + sizeof(mem);
}

static mem* get_chunk(mem* start, char* block){
	if(block!=NULL){
		while (start != NULL) {
			if (start == (mem*)(block - sizeof(mem))) {
				return start;
			}
			start = start->next;
		}
	}	
	return NULL;
}

static mem* get_prev(mem* start, mem* chunk){
	while(start!=NULL){
		if(start->next==chunk){
			return start;
		}
		start = start->next;
	}
	return NULL;
}

void free(void* block){
	mem* chunk = get_chunk(HEAP_START, block);
	mem* prev = get_prev(HEAP_START, chunk);
	if(chunk){
		chunk->is_free = 1;
		/*if(chunk->next&&chunk->next->is_free){
			chunk->capacity += chunk->next->capacity + sizeof(mem);
			chunk->next = chunk->next->next;
		}
		if(prev&&prev->is_free){
			prev->capacity += chunk->capacity + sizeof(mem);
			prev->next = chunk->next;
		}*/
	}
}

void* realloc(void* ptr, const size_t new_size){
	void* new_ptr = malloc(new_size);
	if(ptr==NULL){
		return new_ptr;
	}
	mem* mem_ptr = (mem*)ptr - 1;
	if(new_size==0){
		free(ptr);
	}else if(mem_ptr->capacity<new_size){
		memcpy(new_ptr, ptr, mem_ptr->capacity);
		free(ptr);
	}else{
		memcpy(new_ptr, ptr, new_size);	
		free(ptr);
	}
	return new_ptr;
}