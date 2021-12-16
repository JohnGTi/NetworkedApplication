#include "PlayerCharacter.h"

#include <iostream>


PlayerCharacter::PlayerCharacter()
	: PlayerAvatar("Art/PlayerSpriteSheet.png")
	, messageTypeHeader(MessageType::Positional)

	, clientConsole(NULL)
{}

void PlayerCharacter::ReceiveConsole(ClientConsole* console) {
	clientConsole = console;
}




float PlayerCharacter::UpdatePlayerMovement(int WASDKey, float velocity, float direction) {

	if (input->isKeyDown(WASDKey) == true) {

		movementCondition = PlayerState::Travelling;
		return movementRate * direction; }
	//
	return velocity;
}

void PlayerCharacter::handleInput()
{
	// The player cannot travel under travel lock condition, so bypass movement update.
	if (movementCondition != PlayerState::Striking) {

		// WASD to direct the player along the XY plane...
		stepVelocity.y = UpdatePlayerMovement(sf::Keyboard::W, stepVelocity.y, -1.0f);
		stepVelocity.x = UpdatePlayerMovement(sf::Keyboard::A, stepVelocity.x, -1.0f);
		//
		stepVelocity.y = UpdatePlayerMovement(sf::Keyboard::S, stepVelocity.y, 1.0f);
		stepVelocity.x = UpdatePlayerMovement(sf::Keyboard::D, stepVelocity.x, 1.0f);


		// If the player is not already striking...
		if (input->isLeftMousePressed() == true) {
			//
			movementCondition = PlayerState::Striking;
		}
	}
}





void PlayerCharacter::ConstrainToLevelBounds(sf::Vector2f &position) {

	// Define the paramaters at which an object can escape the display.
	sf::Vector2f give(window->getSize().x * 0.05f, window->getSize().y * 0.05f);


	if (position.x <= -give.x || position.x >= window->getSize().x + give.x) {
		position.x = getPosition().x; }
	//
	if (position.y <= -give.y || position.y >= window->getSize().y + give.y) {
		position.y = getPosition().y; }
}


void PlayerCharacter::update(float deltaTime)
{
	// Compute player travel under travel conditions.
	if (movementCondition == PlayerState::Travelling) {

		// Initialise a position variable, for future collision comparison.
		sf::Vector2f playerPosition(stepVelocity * deltaTime);

		// Update the player position according to frame time travel.
		//
		ConstrainToLevelBounds(playerPosition += getPosition());
		setPosition(playerPosition);
	}


	// Assign a value to theta, according to a vector from the player to
	// the mouse position. The angle theta can then be used to set the player facing.
	//
	DetermineAvatarLookDirection(sf::Vector2f(input->getMouseX(), input->getMouseY()));
	AnimateCharacter(deltaTime);


	// Reset the player travel conditions, to be updated according to input, next frame.
	stepVelocity = sf::Vector2f(0.0f, 0.0f);
	//
	if (movementCondition != PlayerState::Striking) {
		movementCondition = PlayerState::Default;
	}


	// With the updated player state, the nature and contents of the update
	// message to be sent to the server can be determined.
	DetermineMessage();
}

void PlayerCharacter::DetermineMessage()
{
	// Upon receipt of simulation time scale data. Here, the player can identify
	// a newly established relationship, and resolve the server's send.
																	//
	if (clientConsole->GetConsoleState() == ConsoleState::TimeSynced)
	{
		// Overwrite player/client attributes upon server authority (truly initialise).
		TimeScaleData trueInitial = clientConsole->GetTimeScaleData();
		personalID = trueInitial.overwriteIdentifier;
		//
		setPosition(trueInitial.overwritePositionX, trueInitial.overwritePositionY);

		clientConsole->SetConsoleState(ConsoleState::Playing);
	}


	// Package the necessary positional data by insertion.
	messageTypeHeader = MessageType::Positional;
	//
	PositionalData positionalData;
	positionalData.playerIdentifier = personalID;
	//
	positionalData.positionX = getPosition().x;
	positionalData.positionY = getPosition().y;
	//
	positionalData.lookAtX = input->getMouseX();
	positionalData.lookAtY = input->getMouseY();
	//
	messagePacket.clear();
	messagePacket << static_cast<sf::Uint8>(ToUnderlying(messageTypeHeader)) << positionalData;


	// The player update function ultimately decides on the outgoing message content.
	// This data is, after the above preparations, sent to the client console for processing.
	//
	clientConsole->SetOutgoingMessage(messagePacket);
}