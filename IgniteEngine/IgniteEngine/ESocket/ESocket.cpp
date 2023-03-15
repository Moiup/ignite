#include "ESocket.h"

ESocket::ESocket() : _socket{ INVALID_SOCKET } { ; }

ESocket::ESocket(const ESocket& es) {
	_socket = es._socket;
}

ESocket::~ESocket() {
	;
}

void ESocket::SetSocket(SOCKET s) {
	_socket = s;
}

SOCKET ESocket::GetSocket() {
	return _socket;
}

void ESocket::Close() {
	if (_socket != INVALID_SOCKET) {
		closesocket(_socket);
	}
}