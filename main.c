#include <stdio.h>
#include <stdlib.h>
#include "OBD_server.h"
#include "OBD_memorypool.h"
#include "OBD_network.h"

static network_t network;
static server_t server;
static OBD_ev_t ev;

typedef ev_io	ev_io_t;
typedef ev_loop ev_loop_t;

static void socket_read_cb(ev_loop_t *loop,ev_io_t *w, s32_t revents)
{
	server_t *s = &server;
	ssize_t ret = 0;
	OBD_t *obd = (OBD_t*)s->obd_mempool.get(&s->obd_mempool);
	obd->data = (char*)s->data_mempool.get(&s->data_mempool);
	OBD_init(obd);
	ret = recv(w->fd,obd->data,s->data_mempool.length,0);
	if(ret <= 0)
	{
		printf("read error\n");
		s->data_mempool.put((void*)obd->data);
		s->obd_mempool.put((void*)obd);
		return;
	}

	s->mq.put(&s->mq,&obd->list);


}

static void socket_accept_cb(ev_loop_t *loop,ev_io_t *w, s32_t revents)
{
	OBD_ev_t *pev = &ev;
	sockaddr_in_t client_addr;
	s32_t client_socket;
	socklen_t addr_len = sizeof(client_addr);
	if(EV_ERROR & revents)
		return;
	client_socket = accept(w->fd,(sockaddr_t *)&client_addr,&addr_len);
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
	pev->read_init(pev,client_socket);
	pev->read_start(pev);
}

int main(int argv, char **argc)
{
	int ret = 0;
	network_t *net = &network;
	server_t *s = &server;
	OBD_ev_t *pev = &ev;

	if(FALSE == OBD	server_init(s,sizeof(OBD_t)))
		return -1;
	

	OBD_ev_init(pev);
	OBD_ev_set_accept_cb(pev,socket_accept_cb);
	OBD_ev_set_read_cb(pev,socket_read_cb);

	net_init(net);
	net->addr_init(net);
	if(net->socket_init(net))
		return -1;	
	if(net->socket_bind(net))
		return -1;
	if(net->socket_listen(net))
		return -1;

	ev->accept_init(pev,net->socket);
	ev->accept_start(pev);

	ev->run(pev);

	


	return 0;

}
