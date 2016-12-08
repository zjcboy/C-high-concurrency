#include <stdio.h>
#include <stdlib.h>
#include "OBD_server.h"
#include "OBD_memorypool.h"
#include "OBD_network.h"

static network_t network;
static server_t server;

typedef ev_io	ev_io_t;
typedef ev_loop ev_loop_t;
static ev_io_t socket_accept;
static ev_loop *loop;

static void socket_accept_cb(ev_loop_t *loop,ev_io_t *w, s32_t revents)
{
		
}

int main(int argv, char **argc)
{
	int ret = 0;
	network_t *net = &network;
	server_t *s = &server;

	net_init(net);
	net->addr_init(net);
	if(net->socket_init(net))
		return -1;	
	if(net->socket_bind(net))
		return -1;
	if(net->socket_listen(net))
		return -1;

	loop = ev_default_loop(0);
	ev_io_init(&socket_accept,socket_accept_cb,net->socket,EV_READ);
	ev_io_start(loop,&socket_accept);

	ev_run(loop,0);

	return 0;

}
