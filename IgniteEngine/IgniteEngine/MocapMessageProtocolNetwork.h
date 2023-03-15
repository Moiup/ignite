#pragma once

#include "HeaderMessageProtocoleNetwork.h"
#include "MessageProtocoleNetwork.h"
#include "SecondaryMocapHeaderNetwork.h"

#include "ESocket/EStream.h"

#include <vector>

class MocapMessageProtocolNetwork
{
private:
	HeaderMessageProtocoleNetwork _header;
	std::vector<SecondaryMocapHeaderNetwork> _secondary_headers;
	std::vector<std::vector<MessageProtocoleNetwork>> _points;

public:
	MocapMessageProtocolNetwork();

	const HeaderMessageProtocoleNetwork& getHeader() const;
	const std::vector<SecondaryMocapHeaderNetwork>& getSecondaryHeaders() const;
	const std::vector<std::vector<MessageProtocoleNetwork>>& getPoints() const;

	void read(EStream& stream);
};

