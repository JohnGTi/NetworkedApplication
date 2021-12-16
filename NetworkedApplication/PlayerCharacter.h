#pragma once

#include "PlayerAvatar.h"
#include "MessageTypes.h"
#include "ClientConsole.h"


// As the player character class developed, it became clearer that its functionality
// might have been more suited to the SimulationState: this game is the player.
// The PlayerAvatar, then, could more strictly represent a player (this or a known client), graphically.
//
// The restructuring of this functionality was not carried out (job cut for time/deprioritised).



class PlayerCharacter : public PlayerAvatar
{
private:
	// For local access to the client console.
	ClientConsole* clientConsole;

	// Comprising a single, dedicated outgoing message: the SimulationState will
	// call for and pass these objects into the client console, to send.
	MessageType messageTypeHeader;
	sf::Packet messagePacket;
	
	// A simple helper function for dictating player movement.
	float UpdatePlayerMovement(int WASDKey, float velocity, float direction);

	// Constrain an object to the bounds of the level.
	void ConstrainToLevelBounds(sf::Vector2f &position);

	// Determine the outgoing message type to be passed onto the client console.
	void DetermineMessage();

public:
	PlayerCharacter();

	// Receive the client console, for access to received message updates.
	void ReceiveConsole(ClientConsole* console);


	// Key simulation method overrides.
	void handleInput();
	void update(float deltaTime);
};