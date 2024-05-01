#include "Dispatcher.h"

Dispatcher::Dispatcher()
{
	;
}

void Dispatcher::setQueue(Queue* queue) {
	_queue = queue;
}

void Dispatcher::create() {
	_command_buffer = _queue->allocateCommandBuffer();
}

void Dispatcher::destroy() {
	_command_buffer.free();
}