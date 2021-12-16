#include "ClientCombatant.h"
#include <iostream>

ClientCombatant::ClientCombatant()
	: PlayerAvatar("Art/PlayerSpriteSheet.png")
	, historicalBounds(3)
	, lookAtPoint(sf::Vector2f(0.f, 0.f))
{}

void ClientCombatant::ApplySpriteColourFill(const sf::Color& colour) {
	setFillColor(colour);
}




sf::Vector2f ClientCombatant::PredictPositional(float simulationTime)
{
	/*// Protect shared class member variables by way of mutex lock within current scope.
	std::unique_lock<std::mutex> positionalLock(positionalMutex);
	//
	positionalEvent.wait(positionalLock);*/


	// Dead reckoning.


	// Store a constant representation of the number of messages in the received backlog.
	const int positionalReceived = positionalHistory.size();
	//
	if (positionalReceived < historicalBounds) {
		return getPosition();
	}

	// Local variable indices for readability.
	int mostRecent = positionalReceived - 1; // The most up-to-date message.
	int secondMostRecent = positionalReceived - 2;

	// Note that the historical bounds previously sat at size 3. Quadratic implementation,
	// requiring a message history depth of three, was tested alongside linear prediction.

	sf::Vector2f positionalPrediction = LinearPrediction(positionalHistory.at(mostRecent),
		positionalHistory.at(secondMostRecent), simulationTime);



	// Create an instance of positional data, and push the predicted data onto the class memeber container.
	PositionalData forHistorical;
	//
	forHistorical.positionX = positionalPrediction.x;
	forHistorical.positionY = positionalPrediction.y;
	//
	predictionHistory.push_back(PositionalPair(forHistorical, simulationTime));

	if (predictionHistory.size() > historicalBounds) {
		// Iterate through and erase all elements prior to the back N.
		//
		for (auto it = predictionHistory.begin(); predictionHistory.size() != historicalBounds;) {

			it = predictionHistory.erase(it);
		}
	}



	// Store a constant representation of the number of stored previous predicitions.
	const int previousPredictions = predictionHistory.size();

	// Ensure there is enough of a backlog of data to perform interpolation.
	//
	if (previousPredictions >= historicalBounds)
	{
		// Local index variable for readability.
		int mostRecent = previousPredictions - 1;
		int secondMostRecent = previousPredictions - 2;

		// Perform a linear prediction based off of the previous predicted positions.
		sf::Vector2f historicalPrediction = LinearPrediction(predictionHistory.at(mostRecent),
			predictionHistory.at(secondMostRecent), simulationTime);

		// Return a linear interpolation between the prediction based off of actual positional data (communicated
		// through the server) and the prediction based off of previous predictions.
		
		return sf::Vector2f(
			positionalPrediction.x + 0.5f * (historicalPrediction.x - positionalPrediction.x),
			positionalPrediction.y + 0.5f * (historicalPrediction.y - positionalPrediction.y)
			);
	}


	// Wake up the key thread that is waiting on the condition variable.
	/*positionalLock.unlock();
	positionalEvent.notify_one();*/

	return positionalPrediction;
}

sf::Vector2f ClientCombatant::LinearPrediction(PositionalPair former, PositionalPair latter, float currentTime)
{
	// Locally assign positional data, for ease of access.
	PositionalData& formerPositional = former.positionalData;
	PositionalData& latterPositional = latter.positionalData;


	// Linear prediction computation.

	// What speed are we currently at in relation to the second most recent message?
	sf::Vector2f distanceFromLatter(
		formerPositional.positionX - latterPositional.positionX,
		formerPositional.positionY - latterPositional.positionY
	);
	float timeFromLatter = former.sentTimeStamp - latter.sentTimeStamp;
	//
	sf::Vector2f speedFromLatter(
		distanceFromLatter.x / timeFromLatter,
		distanceFromLatter.y / timeFromLatter
	);


	// Displacement of the object from the "current" position (the last known position;
	// the most recent message).
	//
	// Utlise the time between the current game time, and when the last message was received.
	float timeSinceLast = currentTime - former.sentTimeStamp; // / 0.1;

	sf::Vector2f displacementFromLast(
		speedFromLatter.x * timeSinceLast,
		speedFromLatter.y * timeSinceLast
	);

	// Determine a predicted value for the next remote player position.
	return sf::Vector2f(
		formerPositional.positionX + displacementFromLast.x,
		formerPositional.positionY + displacementFromLast.y
		);
}

void ClientCombatant::Update(float deltaTime, float simulationTime)
{
	// Check that there are contents to extract.
	if (!positionalHistory.empty()) {

		// First, assign the positonal data pointer, locally.
		PositionalData* positionalUpdate = &(positionalHistory.back().positionalData);

		// Update the combatant ground and look positional.
		if (predictionOn) {
			setPosition(PredictPositional(simulationTime));
		}
		else {
			setPosition(sf::Vector2f(positionalUpdate->positionX,
				positionalUpdate->positionY));
		}
		//
		lookAtPoint.x = positionalUpdate->lookAtX;
		lookAtPoint.y = positionalUpdate->lookAtY;
	}


	// Assign a value to theta, according to a vector from the player to
	// the mouse position. The angle theta can then be used to set the player facing.
	//
	DetermineAvatarLookDirection(lookAtPoint);
	AnimateCharacter(deltaTime);
}


bool ClientCombatant::ReceievedIsCurrent(const PositionalData& receivedPositional, float sentTimeStamp)
{
	/*// Protect shared class member variables by way of mutex lock within current scope.
	std::unique_lock<std::mutex> positionalLock(positionalMutex);*/


	// Verify that there is a history necessary to compare against.
	if (!positionalHistory.empty()) {

		// Test that the received message was sent at a time at least a period of the client send rate
		// further than the most up-to-date historical message.

		// As opposed to simply discarding an out-of-date message, the below check may be better performed for all
		// elements. A new message could then find its place within the collection, and so a late packet (if no 
		// later than the most dated element in the collection already) might still benefit the prediction computation.


		if (sentTimeStamp > positionalHistory.back().sentTimeStamp + sendRate) {

			// As the test was successful, this data can be considered up-to-date.
			positionalHistory.push_back(PositionalPair(receivedPositional, sentTimeStamp));


			// Historical data outside the inactivity bounds is discarded. The last N historical messages
			// required is determined by the period that the client might be required to time travel.

			if (positionalHistory.size() > historicalBounds) {
				// Iterate through and erase all elements prior to the back N.
				//
				for (auto it = positionalHistory.begin(); positionalHistory.size() != historicalBounds;) {

					it = positionalHistory.erase(it);
				}
			}
		}
		else {

			// Here, the data might be antiquated (more up-to-date data exists) or a duplicate (the
			// timestamp is not at least a send rate period of difference from the historical data).

			// The send rate difference provides a correct window for which a piece of data can be
			// considered equal, forgiving any floating point imprecision error that would come with an
			// equality comparison (==) of the respective timestamps.


			// Wake up the key thread that is waiting on the condition variable.
			/*positionalLock.unlock();
			positionalEvent.notify_one();*/

			return false;
		}
	}
	else
	{
		// There is no comparison to make between historical data. This data is essential.
		positionalHistory.push_back(PositionalPair(receivedPositional, sentTimeStamp));
	}

	// Wake up the key thread that is waiting on the condition variable.
	/*positionalLock.unlock();
	positionalEvent.notify_one();*/

	return true;
}