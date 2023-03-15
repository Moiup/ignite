#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

class ESocket
{
public:

private:
	SOCKET _socket;

public:
	ESocket();
	ESocket(const ESocket& es);
	~ESocket();

	void SetSocket(SOCKET s);
	SOCKET GetSocket();

	void Close();
};

