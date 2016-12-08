#ifndef __OBD_STRUCT_H__
#define __OBD_STRUCT_H__

#define <stdio.h>
#define <stdlib.h>

typedef struct obd_struct{
	void	*data;
	char	*sql;
	char	*redis;
	boolean (*init)(OBD_t*);
	u8_t (*get_cmd)(OBD_t*);
	void (*filter_escape)(OBD_t*);
	boolean	(*check_sum)(OBD_t*);
	char* (*pack_to_sql)(OBD_t*);
	char* (*pack_to_redis)(OBD_t*);
	struct list_t	list;
}OBD_t;

static inline boolean ODB_init(OBD_t *obd)
{
	return FALSE;
}

#endif
