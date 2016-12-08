#ifndef __OBD_MEMORYPOOL_H__
#define __OBD_MEMORYPOOL_H__

typedef struct memory_pool{
	MemoryPool_t	*pool;
	void* (*get)(memory_t*);
	void (*put)(void*);
}memory_t;

static inline void* memory_pool_get(memory_t *memory)
{
	return Malloc(memory->pool);
}

static inline void memory_pool_put(void* memory)
{
	Free(memory);
}

typedef struct OBD_memory_pool{
	memory_t	memory;
	u32_t		length;
	boolean (*init)(OBD_memory_t*);
	void (*release)(OBD_memory_t*);
}OBD_memory_t;

static inline void OBD_memory_pool_release(OBD_memory_t* OBD_memory)
{
	if(!OBD_memory)
		return FALSE;
	if(!OBD_memory->memory.pool)
		return FALSE;

	Release_MemoryPool(OBD_memory->memory.pool);
	OBD_memory->length = 0;
}

static inline boolean OBD_memory_pool_init(OBD_memory_t* OBD_memory, u32_t length)
{
	if(!OBD_memory)
		return FALSE;

	OBD_memory->memory.pool = Create_MemoryPool(length);
	if(!OBD_memory->memory.pool)
		return FALSE;
	OBD_memory->memory.get = memory_pool_get;
	OBD_memory->memory.put = memory_pool_put;
	OBD_memory->init = OBD_memory_pool_init;
	OBD_memory->length = length;
	OBD_memory->release = OBD_memory_pool_release;
	return TRUE;
}

typedef struct OBD_data_memory_pool{
	memory_t	*memory;
	u32_t		length;
}Data_memory_t;

static inline void Data_memory_pool_release(Data_memory_t* Data_memory)
{
	if(!Data_memory)
		return FALSE;
	if(!Data_memory->memory.pool)
		return FALSE;

	Release_MemoryPool(Data_memory->memory.pool);
	Data_memory->length = 0;
}

static inline boolean Data_memory_pool_init(Data_memory_t* Data_memory, u32_t length)
{
	if(Data_memory)
		return FALSE;

	Data_memory->memory.pool = Create_MemoryPool(length);
	if(!Data_memory->memory.pool)
		return FALSE;
	Data_memory->memory.get = memory_pool_get;
	Data_memory->memory.put = memory_pool_put;
	Data_memory->init = Data_memory_pool_init;
	Data_memory->length = length;
	Data_memory->release = Data_memory_pool_release;
	return TRUE;
}



#endif