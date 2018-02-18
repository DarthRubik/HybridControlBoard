#ifndef QUEUE_H
#define QUEUE_H
#include "stddef.h"
#include "stdbool.h"

typedef struct {
	void** data;
	void** end;

	void** head;
	void** tail;
	size_t count;
} Queue_t;

void InitializeQueue(Queue_t* queue, void** buffer, size_t length);
void AddToQueue(Queue_t* queue, void* data);
void* GetFromQueue(Queue_t* queue);
bool DoesQueueHaveStuff(Queue_t* queue);


#endif
