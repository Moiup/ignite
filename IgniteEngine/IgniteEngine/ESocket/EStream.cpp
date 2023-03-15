#include "EStream.h"

EStream::EStream() :
	_esocket{}
{
	;
}

EStream::EStream(const EStream& stream) {
	_esocket = stream._esocket;
}

EStream::~EStream() {
}

uint32_t EStream::Recv(uint32_t buf_size, char* buf) {
	uint32_t read_size{};

	read_size = recv(_esocket.GetSocket(), buf, buf_size, 0);

	return read_size;
}

INT32 EStream::Send(char* buf, uint32_t size) {
	INT32 i_send_result = send(
		_esocket.GetSocket(), buf, size, 0
	);

	if (i_send_result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return 0;
	}

	return i_send_result;
}

INT32 EStream::Send_test(SOCKET s, char* buf, uint32_t size) {
	std::cout << s << std::endl;
	INT32 i_send_result = send(
		s, buf, size, 0
	);

	if (i_send_result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		return i_send_result;
	}

	return 0;
}

INT32 EStream::Close() {
	INT32 i_result{};

	//if (_esocket == nullptr)
	//	return 0;

	i_result = shutdown(_esocket.GetSocket(), SD_SEND);
	if (i_result == SOCKET_ERROR) {
		std::cerr << "shutdown failed." << std::endl;
	}

	return i_result;
}

ESocket& EStream::GetESocket() {
	return _esocket;
}

void EStream::SetESocket(ESocket& es) {
	_esocket = es;
}