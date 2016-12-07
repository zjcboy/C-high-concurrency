#ifndef __OBD_MQ_H__
#define __OBD_MQ_H__

#include "types_define.h"

typedef struct obd_list{
	list_t *prev;
	list_t *next;
	void (*init)(list_t*);
}list_t;

typedef struct obd_mq{
	list_t		list;
	mutex_lock	mq_lock;
	void	(*init)(mq_t*);
	list_t* (*get)(mq_t*);
	void 	(*put)(mq_t*);
	boolean (*is_empty)(mq_t*);
	void 	(*release)(mq_t*);
}mq_t;

#define container_of(ptr,type,member) \
	({ \
		const typeof(((type*)0)->member)* __mptr = (ptr); \
		(type*)((char*)__mptr-offsetof(type,member)); \
	})

#define list_entry(ptr,type,member) \
	container_of(ptr,type,member)

#define list_for_each_entry(pos,head,member) \
	for(pos = list_entry((head)->next,typeof(*pos),member); \
		&pos->member !=head; \
		pos = list_entry(pos->member.next,typeof(*pos),member))

static inline void list_init(list_t *list)
{
	list->next = list;
	list->prev = list;
}

static inline list_t* list_del_back(list_t *head)
{
	list_t *current = head->prev;
	head->prev = current->prev;
	current->prev->next = head;
	current->init(current);
	return current;
}

static inline list_t *mq_get(mq_t *mq)
{
	mutex_lock(&mq->mq_lock);
	list_t * current = list_del_back(&mq->list);
	mutex_unlock(&mq->mq_lock);
	return current;
}

static inline void list_add_front(list_t* head, list_t *current)
{
	current->next = head->next;
	head->next->prev = current;
	head->next = current;
	current->prev = head;
}


#endif
