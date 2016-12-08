#ifndef __OBD_DATABASES_H__
#define __OBD_DATABASES_H__

#include <zdb.h>

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
	RedisConnection_t *pool;
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
#endif
