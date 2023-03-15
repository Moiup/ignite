#include "InitNetworkProtocole.h"

InitNetworkProtocole::InitNetworkProtocole() :
	_header{}
{
	;
}

void InitNetworkProtocole::read(EStream& stream) {
	stream.Recv(sizeof(InitHeader), (char *) & _header);

	_points.resize(_header.nb_points);
	stream.Recv(
		_points.size() * sizeof(_points[0]),
		(char*)_points.data()
	);
}

const InitHeader& InitNetworkProtocole::getHeader() const {
	return _header;
}
const std::vector<InitChildPoint>& InitNetworkProtocole::getPoints() const {
	return _points;
}