#pragma once

#include <SFML/Network.hpp>

#include "PlayerAvatar.h"
#include "MessageTypes.h"

#include <mutex>
#include <condition_variable>

// Mutex for protection of resources shared across the initial and worker threads.
static std::mutex memberMutex;
static std::mutex positionalMutex;
//
static std::condition_variable positionalEvent;


// The constant rate at which messages are communicated client-side.
const float sendRate = 0.1f;


struct PositionalPair {
	//
	PositionalData positionalData;
	float sentTimeStamp;

	// Constructor overide.
	PositionalPair(PositionalData positional, float time)
		: positionalData(positional), sentTimeStamp(time) {}
};


class ClientCombatant : public PlayerAvatar
{
private:
	// Retain a history of received positional data, and predicted positions.
	std::vector<PositionalPair> positionalHistory;
	std::vector<PositionalPair> predictionHistory;

	// The depth for which historical data is stored.
	const int historicalBounds;

	// Positional data that can be used to determine the look direction and stance of
	// a remote player's avatar representation (, for animation).
	sf::Vector2f lookAtPoint;

	sf::Vector2f LinearPrediction(PositionalPair former, PositionalPair latter, float currentTime);

public:
	ClientCombatant();
	//
	// Overwrite the PlayerAvatar' initialised drawable object colour.
	void ApplySpriteColourFill(const sf::Color& colour);


	// Determine that the received data is neither old nor a duplicate, for processing.
	bool ReceievedIsCurrent(const PositionalData& receivedPositional, float sentTimeStamp);
	//
	std::vector<PositionalPair>* GetPositionalHistory() { return &positionalHistory; }

	// Run remote player ground position prediction.
	sf::Vector2f PredictPositional(float simulationTime);

	// Key combatant state update function.
	void Update(float deltaTime, float simulationTime);
};