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
	void* (*get)(mysql_t*);
	void(*release)(mysql_t*);
}mysql_t;

typedef struct redis_struct{
	RedisConnection_t	*pool;
	boolean (*init)(redis_t*);
	void* (*get)(redis_t*);
	void (*release)(redis_t*);
}redis_t;

typedef struct memory_pool{
	MemoryPool_t	*pool;
	boolean (*init)(memory_t*);
	void* (*get)(memory_t*);
	void (*release)(memory_t*);
}memory_t;

typedef struct OBD_memory_pool{
	memory_t	*memory;
	u32_t		length;
}OBD_memory_t;

typedef struct OBD_data_memory_pool{
	memory_t	*memory;
	u32_t		length;
}Data_memory_t;

typedef struct threadpool_struct{
	threadpool_t	*pool;
	u8_t		length;
	boolean (*init)(tpool_t*);
	void* (*get)(tpool_t*);
	void (*release)(tpool_t*);
}tpool_t;

typedef struct OBD_server_struct{
	mq_t			*mq;
	ev_t			*ev;
	mysql_t		 	*mysqlpool;
	redis_t			*redis;
	OBD_memory_t		*obd_mempool;
	Data_memory_t		*data_mempool;
	tpool_t			*thread_pool;
	boolean (*init)(server_t*);
	void (*release)(server_t*);
}server_t;

#endif
