#ifndef __OBD_EV_STRUCT_H__
#define __OBD_EV_STRUCT_H__

typedef ev_io ev_io_t;
typedef ev_loop ev_loop_t;

typedef struct OBD_ev_struct{
	ev_loop_t *loop;
	ev_io_t socket_accept;
	ev_io_t socket_read;
	void (*init)(OBD_ev_t*);
	void (*accept_init)(OBD_ev_t*,s32_t);
	void (*accept_start)(OBD_ev_t*);
	void (*read_init)(OBD_ev_t*,s32_t);
	void (*read_start)(OBD_ev_t*);
	void (*accept_cb)(ev_loop_t*,ev_io_t*,s32_t);
	void (*read_cb)(ev_loop_t*,ev_io_t*,s32_t);
	void (*run)(OBD_ev_t*)
	void (*release)(OBD_ev_t*);
}OBD_ev_t;

static inline void OBD_read_init(OBD_ev_t *ev, s32_t socket)
{
	ev_io_init(&ev->socket_read,ev->read_cb,socket,EV_READ);
}

static inline void OBD_read_start(OBD_ev_t *ev)
{
	ev_io_start(ev->loop,&ev->socket_read);	
}

static inline void OBD_accept_init(OBD_ev_t *ev,s32_t socket)
{
	ev_in_init(&ev->socket_accept,ev->accept_cb,socket,EV_READ);
}

static inline void OBD_accept_start(OBD_ev_t *ev)
{
	ev_io_start(ev->loop,&ev->socket_accept);	
}

static inline void OBD_ev_run(OBD_ev_t *ev)
{
	ev_run(ev->loop,0);	
}

static inline void OBD_ev_release(OBD_ev_t *ev)
{
	
}

static inline void OBD_ev_init(OBD_ev_t *ev)
{
	ev->loop = ev_default_loop(0);
	ev->init = OBD_ev_init;
	ev->accept_init = OBD_accept_init;
	ev->accept_start = OBD_accept_start;
	ev->read_init = OBD_read_init;
	ev->read_start = OBD_read_start;
	ev->run = OBD_ev_run;
	ev->release = OBD_ev_release;
}

static inline void OBD_ev_set_accept_cb(OBD_ev_t *ev ,void(*cb)(ev_loop_t *,ev_io_t *,s32_t))
{
	ev->accept_cb = cb;	
}
static inline void OBD_ev_set_read_cb(OBD_ev_t *ev, void(*cb)(ev_loop_t *,ev_io_t *, s32_t))
{
	ev->read_cb = cb;	
}



#endif
