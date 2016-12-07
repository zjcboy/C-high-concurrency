#ifndef __OBD_SERVER_H__
#define __OBD_SERVER_H__

#include <zdb.h>
#include "OBD_mq.h"
#include "MemoryPool.h"
#include "threadpool.h"
#include "types_define.h"

typedef struct mysql_struct{
	ConnectionPool_t	*pool;
	boolean (*init)(mysql_t*);
	Connection_t* (*get)(mysql_t*);
	void(*release)(mysql_t*);
}mysql_t;

static inline Connection_t* mysql_get_connection(mysql_t *mysql)
{
	return get_Connection(mysql->pool);
}

static inline void mysql_release(mysql_t *mysql)
{
	release_ConnectionPool(mysql->pool);
}

static inline boolean mysql_init(mysql_t *mysql)
{
	URL_t url = "mysql://localhost/root@123456@charset=UTF8";
	mysql->pool = get_ConnectionPool(url);
	if(!mysql->pool)
		return FALSE;
	mysql->init = mysql_init;
	mysql->get = mysql_get_connection;
	mysql->release = mysql_release;
	return TRUE;
}

typedef struct redis_struct{
	RedisConnection_t	*pool;
	boolean (*init)(redis_t*);
	void* (*get)(redis_t*);
	void (*release)(redis_t*);
}redis_t;

static inline Connection_t* redis_get_connection(redis_t *redis)
{
	return get_Connection(redis->pool);
}

static inline void redis_release(redis_t *redis)
{
	release_ConnectionPool(redis->pool);
}

static inline boolean redis_init(redis_t *redis)
{
	if(redis)
		return FALSE;
	redis->pool = get_redis_connectionpool("127.0.0.1","6379");
	redis->init = redis_init;
	redis->get = redis_get_connection;
	redis->release = redis_release;
	return TRUE;
}

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


typedef struct threadpool_struct{
	threadpool_t	*pool;
	u8_t		count;
	boolean (*init)(tpool_t*);
	void* (*get)(tpool_t*);
	void (*release)(tpool_t*);
}tpool_t;

static inline void* threadpool_get(tpool_t *tpool)
{
	return threadpool_get(tpool->pool);
}

static inline void threadpool_release(tpool_t *tpool)
{
	Release_threadpool(tpool->pool);
	tpool->count = 0;
}

static inline boolean threadpool_init(tpool_t* tpool,u16_t count)
{
	if(!tpool)
		return FALSE;

	tpool->pool = Create_threadpool(count);
	if(!tpool->pool)
		return FALSE;
	tpool->count = count;
	tpool->init = threadpool_init;
	tpool->get = threadpool_get;
	tpool->release = threadpool_release;
	return TRUE;
}

typedef struct OBD_server_struct{
	mq_t			mq;
	ev_t			ev;
	mysql_t		 	mysqlpool;
	redis_t			redis;
	OBD_memory_t		obd_mempool;
	Data_memory_t		data_mempool;
	tpool_t			thread_pool;
	boolean (*init)(server_t*);
	void (*release)(server_t*);
}server_t;

static inline void OBD_server_release(server_t *server)
{
	threadpool_release(&server->thread_pool);
	Data_memory_pool_release(&server->data_mempool);
	OBD_memory_pool_release(&server->obd_mempool);
	redis_release(&server->redis);
	mysql_release(&server->mysqlpool);
	mq_release(&server->mq);
}

static inline boolean OBD_server_init(server_t *server,u16_t length)
{
	if(!server)
		return FALSE;
	if(mq_init(&server->mq))
		return FALSE;
	if(mysql_init(&server->mysqlpool))
		return FALSE;
	if(redis_init(&server->redis))
		return FALSE;
	if(OBD_memory_pool_init(&server->obd_mempool))
		return FALSE;
	if(Data_memory_pool_init(&server->data_mempool))
		return FALSE;
	if(threadpool_init(&server->thread_pool,length))
		return FALSE;

	server->init = OBD_server_init;
	server->release = OBD_server_release;
	return TRUE;

}

#endif
