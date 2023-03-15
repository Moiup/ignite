#pragma once

#include "ESocket.h"
#include <vector>

class EStream
{
public:

private:
	ESocket _esocket;

public:
	EStream();
	EStream(const EStream& stream);
	~EStream();

	// Buf size is the size of the buffer to read.
	// Read size is the real number of bytes read.
	uint32_t Recv(uint32_t buf_size, char* buf);
	INT32 Send(char* buf, uint32_t size);
	INT32 Send_test(SOCKET s, char* buf, uint32_t size);
	INT32 Close();

	ESocket& GetESocket();
	void SetESocket(ESocket& es);

private:
};

