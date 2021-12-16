#pragma once

#include "Framework/GameObject.h"
#include "Framework/Animation.h"

#include <SFML/Network/Packet.hpp>

#include <map>
#include <array>


enum class PlayerState {
	Default
	, Travelling
	, Striking
};


class PlayerAvatar : public GameObject
{
protected:
	// Some avatar state attributes.
	PlayerState movementCondition;
	//
	sf::Vector2f bodySize;
	sf::Vector2f stepVelocity;
	//
	float movementRate;


	// Attributes relating to the avatar animation cycles.
	sf::Texture avatarSpriteSheet;
	const std::string avatarTextureFilename;
	//
	std::array<Animation, 12> walkCycle;
	std::array<Animation, 12> strikeAnimation;
	Animation* currentAnimation;
	//
	int stance;
	float strikePeriod;

	// Attributes for determination of avatar orientation/stance.
	std::map<char, float> CASTQuadrant;
	//
	char lookQuadrant;
	float lookTheta;

	// A string personal identifier, so that an avatar can be recognised as a client.
	std::string personalID;

	// Whether or not the prediction/interpolation computation is active for this client.
	bool predictionOn;

	// Set up the avatar animation cycles.
	void InitialiseAvatarSpriteCycle();

	// Determine the player's stance, according to the look direction.
	void DetermineAvatarLookDirection(sf::Vector2f pointLookAt);

	// Animate the avatar character, according to state.
	void AnimateCharacter(float deltaTime);

public:
	PlayerAvatar(const std::string textureFilename);
	void InitaliseAvatar(sf::RenderWindow* win, Input* in, bool predictionActive);

	// Return the current 'look at' angle, theta.
	float GetLookAtTheta() { return lookTheta; }
};