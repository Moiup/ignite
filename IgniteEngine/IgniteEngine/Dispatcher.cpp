#include "Dispatcher.h"

Dispatcher::Dispatcher()
{
	;
}

void Dispatcher::setQueue(Queue* queue) {
	_queue = queue;
}

void Dispatcher::submit() {
	_queue->submit();
}

void Dispatcher::wait() {
	_queue->wait();
}

void Dispatcher::submitAndWait() {
	submit();
	wait();
}