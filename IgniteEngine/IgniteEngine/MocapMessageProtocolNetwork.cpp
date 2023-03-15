#include "MocapMessageProtocolNetwork.h"

MocapMessageProtocolNetwork::MocapMessageProtocolNetwork() :
	_header{},
	_secondary_headers{},
	_points{}
{
	;
}

const HeaderMessageProtocoleNetwork& MocapMessageProtocolNetwork::getHeader() const {
	return _header;
}

const std::vector<SecondaryMocapHeaderNetwork>& MocapMessageProtocolNetwork::getSecondaryHeaders() const {
	return _secondary_headers;
}

const std::vector<std::vector<MessageProtocoleNetwork>>& MocapMessageProtocolNetwork::getPoints() const {
	return _points;
}

void MocapMessageProtocolNetwork::read(EStream& stream) {
	stream.Recv(sizeof(_header), (char *)&_header);
	
	_secondary_headers.resize(_header.nb_skeletons);
	_points.resize(_header.nb_skeletons);
	
	for (uint32_t i = 0; i < _header.nb_skeletons; i++) {
		stream.Recv(
			sizeof(_secondary_headers[i]),
			(char*)&_secondary_headers[i]
		);
		
		_points[i].resize(_secondary_headers[i].nb_points);
		
		stream.Recv(
			_points[i].size() * sizeof(_points[i][0]),
			(char*)_points[i].data()
		);
	}
}