#ifndef __OBD_THREADPOOL_H__
#define __OBD_THREADPOOL_H__

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

#endif