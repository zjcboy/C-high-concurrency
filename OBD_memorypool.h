#ifndef __OBD_MEMORYPOOL_H__
#define __OBD_MEMORYPOOL_H__

#include "FULMemoryPool/MemoryPool.h"
#include "FULMemoryPool/MemoryPoolDef.h"
#include "OBD_struct.h"
#include "types_define.h"

typedef struct memory_pool{
	MemoryPool_t *pool;
	u16_t length;
	boolean (*init)(memory_t*,u16_t);
	void* (*get)(memory_t*);
	void (*put)(void*);
	void (*release)(memory_t);
}memory_t;

static inline void* memory_pool_get(memory_t *memory)
{
	return Malloc(memory->pool);
}

static inline void memory_pool_put(void* memory)
{
	Free(memory);
}

static inline void memory_pool_release(memory_t *memory)
{
	if(!memory->pool)
		return;
	DestroyMemoryPool(memory->pool);
	memory->length = 0;
}

static inline boolean memory_pool_init(memory_t *memory, u16_t length)
{
	if(memory->pool)
		return FALSE;
	memory->pool = CreateMemoryPool(length);
	if(!memory->pool)
		return FALSE;
	memory->length = length;
	memory->get = memory_pool_get;
	memory->put = memory_pool_put;
	memory->release = memory_pool_release;
}

#if 0
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

	DestroyMemoryPool(&OBD_memory->memory.pool);
	OBD_memory->length = 0;
}

static inline boolean OBD_memory_pool_init(OBD_memory_t* OBD_memory, u32_t length)
{
	if(!OBD_memory)
		return FALSE;

	OBD_memory->memory.pool = CreateMemoryPool(length);
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

	DestroyMemoryPool(&Data_memory->memory.pool);
	Data_memory->length = 0;
}

static inline boolean Data_memory_pool_init(Data_memory_t* Data_memory, u32_t length)
{
	if(Data_memory)
		return FALSE;

	Data_memory->memory.pool = CreateMemoryPool(OBD_DATA_LENGTH);
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

#endif
