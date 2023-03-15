#pragma once

#include "InitChildPoint.h"
#include <vector>

#include "ESocket/EStream.h"

class InitNetworkProtocole
{
private:
	InitHeader _header;
	std::vector<InitChildPoint> _points;

public:
	InitNetworkProtocole();

	void read(EStream& stream);

	const InitHeader& getHeader() const;
	const std::vector<InitChildPoint>& getPoints() const;

};

