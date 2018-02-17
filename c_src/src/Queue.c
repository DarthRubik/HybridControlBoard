#include "Queue.h"
#include "Error.h"

void InitializeQueue(Queue_t* queue, void** buffer, size_t length)
{
	queue->data = buffer;
	queue->end = buffer+length;
	queue->head = buffer;
	queue->tail = buffer;
	queue->count = 0;
}
void AddToQueue(Queue_t* queue, void* data){
	*queue->head = data;
	
	queue->count++;
	if (queue->count > queue->end - queue->data){
		DeclareError("Queue overflowed");
	}

	queue->head++;
	if (queue->head >= queue->end)
	{
		queue->head = queue->data;
	}

}

void* GetFromQueue(Queue_t* queue)
{
	void* data = *queue->tail;

	queue->count--;
	if (queue->count == -1)
	{
		DeclareError("Queue underflowed");
		return (void*)0xBADDA1A;
	}
	queue->tail++;
	if (queue->tail >= queue->end)
	{
		queue->tail = queue->data;
	}
	return data;
}

