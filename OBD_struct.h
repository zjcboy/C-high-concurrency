#ifndef __OBD_STRUCT_H__
#define __OBD_STRUCT_H__

#define <stdio.h>
#define <stdlib.h>

typedef struct obd_struct{
	u8_t	cmd;
	void	*data;
	char	*sql;
	char	*redis;
	void (*filter_escape)(OBD_t*);
	boolean	(*check_sum)(OBD_t*);
	char* (*pack_to_sql)(OBD_t*);
	char* (*pack_to_redis)(OBD_t*);
	struct list_t	list;
}OBD_t;

#endif
