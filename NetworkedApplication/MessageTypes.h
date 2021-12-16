#pragma once

enum class MessageType {
	Positional
	, SimulationTimeScale
};

// Explicit conversion of state type to int through static cast.
template <typename T>
constexpr typename std::underlying_type<T>::type ToUnderlying(T sysnav_state) {
	return static_cast<typename std::underlying_type<T>::type>(sysnav_state);
}



// Define a series of custom user types; message data types packed to send.
// Provide operator overloads for the extension of packet functionality; handling user types.

// Note, "Floating-point types should normally have their fixed-size equivalent too, but in practice
// this is not needed (at least on platforms where SFML runs), float and double types always have the
// same size, 32 bits and 64 bits respectively," (Gomila, 2021).

struct PositionalData {
	//
	std::string playerIdentifier = "sf::IpAddress::None4444";
	float positionX = 0.f, positionY = 0.f;
	float lookAtX = 0.f, lookAtY = 0.f;
};

static sf::Packet& operator <<(sf::Packet& packet, const PositionalData& positionalData)
{
	// Extract the contents of the packet according to a PositionalData structure.

	return packet << positionalData.playerIdentifier
		<< positionalData.positionX << positionalData.positionY
		<< positionalData.lookAtX << positionalData.lookAtY;
}

static sf::Packet& operator >>(sf::Packet& packet, PositionalData& positionalData)
{
	// Extract the contents of the packet according to a PositionalData structure.

	return packet >> positionalData.playerIdentifier
		>> positionalData.positionX >> positionalData.positionY
		>> positionalData.lookAtX >> positionalData.lookAtY;
}



struct TimeScaleData {
	//
	std::string overwriteIdentifier = "sf::IpAddress::None4444";
	float overwritePositionX = 0.f, overwritePositionY = 0.f;
	float clientTimeStamp = 0.f;
};


static sf::Packet& operator <<(sf::Packet& packet, const TimeScaleData& timeScaleData)
{
	// Extract the contents of the packet according to a TimeScaleData structure.

	return packet << timeScaleData.overwriteIdentifier
		<< timeScaleData.overwritePositionX << timeScaleData.overwritePositionY
		<< timeScaleData.clientTimeStamp;
}

static sf::Packet& operator >>(sf::Packet& packet, TimeScaleData& timeScaleData)
{
	// Extract the contents of the packet according to a TimeScaleData structure.

	return packet >> timeScaleData.overwriteIdentifier
		>> timeScaleData.overwritePositionX >> timeScaleData.overwritePositionY
		>> timeScaleData.clientTimeStamp;
}


// Gomila, L. (2021) 'Using and Extending Packets'. Available at: https://www.sfml-dev.org/tutorials/2.5/network-packet.php . (Accessed: 17 November 2021).