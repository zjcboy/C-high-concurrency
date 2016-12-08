#ifndef __OBD_SERVER_H__
#define __OBD_SERVER_H__

#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "OBD_mq.h"
#include "MemoryPool.h"
#include "threadpool.h"
#include "types_define.h"

typedef struct sockaddr_in	sockaddr_in_t;
typedef struct network_struct{
	s32_t		socket;
	sockaddr_in_t	address;
	void (*addr_init)(net_t*);
	s8_t (*socket_init)(net_t*);
	s8_t (*socket_bind(net_t*);
	s8_t (*socket_listen)(net_t*);
	void (*release)(net_t*);


}net_t;

#define SERVER_PORT	11223
static inline void net_address_init(net_t *net)
{
	bzero(&net->address,sizeof(net->address));
	net->addr.sin_family = AF_INET;
	net->addr.sin_addr.s_addr = htons(INADDR_ANY);
	net->addr.sin_port = htons(SERVER_PORT);
}

static inline s8_t net_socket_init(net_t *net)
{
	net->socket = socket(PF_INET,SOCK_STREAM,0);
	return net->socket;
}


static inline s8_t net_socket_bind(net_t *net)
{
	setsockopt(net->socket,SOL_SOCKET,SO_REUSEADDR,&ops,sizeof(ops));
	return bind(net->socket,(struct sockaddr*)&net->address,sizeof(net->address));
}

static inline void net_socket_listen(net_t *net)
{
	listen(net->socket,32);
	
}

typedef struct ev_loop	ev_loop_t;
typedef struct ev_io	ev_io_t;
static typedef struct ev_struct{
	ev_loop_t	*loop;
	net_t		net;
	ev_io_t		accept;
	ev_signal	signal_watcher;
	ev_loop_t* (init)(ev_t*);
	void (*loop_run)(ev_t*);
	void (*ev_signal_cb)(ev_loop_t*, ev_signal *,s8_t);
	void (*ev_accept_cb)(ev_loop_t*, ev_io_t*,s32_t);
}ev_t;

static inline void ev_loop_run(ev_t *ev)
{
	ev_run(ev->loop,0);
}

static inline void ev_signal_callback(ev_loop_t *loop, ev_signal *w, s8_t revents)
{
	ev_break(loop,EVBREAK_ALL);	
}

static inline void read_func_cb(ev_loop_t *loop, ev_io_t *w, u32_t revents)
{
	ssize_t read;
	OBD_t *obd = malloc(sizeof(OBD_t));
	if(!obd)
		return ;
	OBD_init(obd);


}

static inline void ev_accept_callback(ev_loop_t *loop, ev_io_t *w, s32_t revents)
{
	sockaddr_in_t client_addr;
	u8_t client_len = sizeof(client_addr);
	u32_t client_socket;

	ev_io_t *w_client = (ev_io_t*)malloc(sizeof(ev_io_t));
	if(w_client = NULL)
		return ;
	if(EV_ERROR & revents)
		return ;
	client_socket = accept(w->fd,(struct sockaddr*)&client_addr,&client_len);
	if(client_socket < 0)
		return ;
	if(client_socket > SOCKET_ALLOC_MAX_CLIENT)
	{
		close(client_socket);
		return ;
	}

	if(libev_list[client_socket])
	{
		return ;	
	}
	ev_io_init(w_client,read_func_cb,client_socket,EV_READ);
	ev_io_start(loop,w_client);

	libev_list[client_socket] = wclient;
}

static inline s8_t ev_struct_init(ev_t *ev)
{
	s8_t ret = 0;
	ev->loop = ev_default_loop(0);
	ev->init = ev_struct_init;
	ev->ev_signal_cb = ev_signal_callback;
	ev->ev_accept_cb = ev_accept_callback;
	ev_signal_init(&ev->signal_watcher,ev->callback,SIGINT);
	ev_signal_start(ev->loop,&ev->signal_watcher);
	ev->net.socket_init = net_socket_init;
	ev->net.addr_init = net_address_init;
	ev->net.socket_bind = net_socket_bind;
	ev->net.socket_listen = net_socket_listen;

	ret = ev->net.socket_init(&ev->net);
	if(ret < 0)
		return ret;
	ev->net.addr_init(&ev->net);
	ret = ev->net.socket_bind(&ev->net);
	if(ret)
		return ret;
	ret = ev->net.socket_listen(&ev->net);
	if(ret)
		return ret;
	ev_io_init(&ev->accept,ev->ev_accept_cb,ev->socket,EV_READ);
	ev_io_start(ev->loop,&ev->accept);

	ev_run(ev->loop,0);

	return 0;
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
	ev_struct_init(&server->ev);
	server->init = OBD_server_init;
	server->release = OBD_server_release;
	return TRUE;

}

#endif
