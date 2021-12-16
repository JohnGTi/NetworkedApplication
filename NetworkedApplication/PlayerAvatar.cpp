#include "PlayerAvatar.h"


PlayerAvatar::PlayerAvatar(const std::string textureFilename)
	: bodySize(sf::Vector2f(128.0f, 128.0f))
	, avatarTextureFilename(textureFilename)

	, movementCondition(PlayerState::Default)
	, stepVelocity(sf::Vector2f(0.0f, 0.0f))
	, movementRate(150.0f)
	
	, CASTQuadrant({ {'C', 90.0f}, {'A', 0.0f}, {'S', 180.0f}, {'T', 270.0f} })
	, lookQuadrant('A')
	, lookTheta(0.0f)

	, currentAnimation(NULL)
	, stance(0)
	, strikePeriod(0.0f)

	, personalID("sf::IpAddress::None4444")
	, predictionOn(false)
{}


void PlayerAvatar::InitaliseAvatar(sf::RenderWindow* win, Input* in, bool predictionActive)
{
	// Assign the framework pointers through base class (GameObject) functionality.
	setInput(in); setWindow(win);

	// Set the prediction/interpolation computation active status.
	predictionOn = predictionActive;

	// Prepare the specifics of the player object.
	setSize(bodySize);
	setOrigin((bodySize.x / 2), (bodySize.y / 2));
	setPosition(window->getSize().x / 2, window->getSize().y * 0.6);

	setFillColor(sf::Color::White);

	// Load and assign to texture, the player sprite sheet.
	avatarSpriteSheet.loadFromFile(avatarTextureFilename);
	setTexture(&avatarSpriteSheet);
	
	// Assign a section of the player sprite sheet to a particular frame of animation.
	InitialiseAvatarSpriteCycle();
}

void PlayerAvatar::InitialiseAvatarSpriteCycle() {
	int sheetDivisions = walkCycle.size();
	int rectangleTop = 0;

	for (int i = 0; i < sheetDivisions; i++) {

		// Rectangle subdivisions of the sprite sheet are applied as frames of animation.
		//
		walkCycle.at(i).setFrameSpeed(1.0f / 10.0f);
		walkCycle.at(i).addFrame(sf::IntRect(0, rectangleTop, 64, 64));
		walkCycle.at(i).addFrame(sf::IntRect(64, rectangleTop, 64, 64));
		//
		strikeAnimation.at(i).setFrameSpeed(1.25f / 10.f);
		strikeAnimation.at(i).addFrame(sf::IntRect(128, rectangleTop, 64, 64));
		strikeAnimation.at(i).addFrame(sf::IntRect(192, rectangleTop, 64, 64));
		//
		rectangleTop += 64;
	}
	//
	currentAnimation = &walkCycle.at(0);
}




void PlayerAvatar::DetermineAvatarLookDirection(sf::Vector2f pointLookAt) {

	// Point (A, B) is determined by comparison of the player and mouse position.
	sf::Vector2f crownAB;

	if (pointLookAt.x > getPosition().x && pointLookAt.y < getPosition().y) {

		// Point (A, B) is set as the third point of a triangle that will be constructed so that a
		// look at angle can be computed trigonometrically.
		//
		crownAB.x = getPosition().x;
		crownAB.y = pointLookAt.y;

		// The relationship between the player and mouse position determines
		// the CAST quadrant in which point (A, B) falls.
		lookQuadrant = 'A';
	}
	else if (pointLookAt.x > getPosition().x && pointLookAt.y > getPosition().y) {

		crownAB.x = pointLookAt.x;
		crownAB.y = getPosition().y;
		lookQuadrant = 'C';
	}
	//
	else if (pointLookAt.x < getPosition().x && pointLookAt.y > getPosition().y) {
		crownAB.x = getPosition().x;
		crownAB.y = pointLookAt.y;
		lookQuadrant = 'S';
	}
	//
	else {
		crownAB.x = pointLookAt.x;
		crownAB.y = getPosition().y;
		lookQuadrant = 'T';
	}


	// Compute the hypotenuse and adjacent of the triangle so that the cosine can be determined.
	//
	float adjacent = (((crownAB.x - getPosition().x) * (crownAB.x - getPosition().x)
		+ (crownAB.y - getPosition().y) * (crownAB.y - getPosition().y)));
	adjacent = sqrt(adjacent);

	float hypotenuse = ((pointLookAt.x - getPosition().x) * (pointLookAt.x - getPosition().x))
		+ ((pointLookAt.y - getPosition().y) * (pointLookAt.y - getPosition().y));
	hypotenuse = sqrt(hypotenuse);

	// Complete the trigonometry calculation for and find the look at direction (angle theta).
	lookTheta = acos(adjacent / hypotenuse);
	lookTheta = lookTheta * static_cast<__int8>(180.0f) / 3.1415f + CASTQuadrant.at(lookQuadrant);
}


void PlayerAvatar::AnimateCharacter(float deltaTime)
{
	if (movementCondition == PlayerState::Striking) {

		// Here, within some if statement, within some range of the strike duration...
		// ...the user and other collision detection might be carried out,
		// ...or the strike attempt might be flagged for another class to handle.
		strikePeriod += deltaTime;


		// For the actual strike animation period, of two frames, animate.
		if (strikePeriod <= currentAnimation->getFrameSpeed() * 2.0f) {

			currentAnimation = &strikeAnimation.at(stance);
			currentAnimation->animate(deltaTime);
		}
		else {
			// Following the strike animation period, transition into the default animations.
			currentAnimation->reset();
			currentAnimation = &walkCycle.at(stance);
		}
		//
		if (strikePeriod >= currentAnimation->getFrameSpeed() * 2.8f) {

			// Exit the striking state after the a period of almost three frames.
			movementCondition = PlayerState::Default;
			strikePeriod = 0.0f;
		}
	}
	else {
		// Divide the 360 degree range by the number of stances (drawings representing a look at).
		// Offset this value by 15, so that the cut off at which a stance is passed is offset.
		//	:	( 360 / 12 ) + 15 = 45.
		int lookAtCutOff = 45;
		//
		if (lookTheta > 345 || lookTheta < 15) { stance = 0; }
		else {
			for (int p = 1; p < walkCycle.size(); p++) {
				if (lookTheta < lookAtCutOff) {

					// Stance is set within the range of theta, to the cut off angle.
					stance = p; break;
				}
				// Increment the cut off angle by a single division (360 degrees / stances).
				else { lookAtCutOff += 30; }
			}
		}


		// Animate the player sprite according to the current stance (facing).
		currentAnimation = &walkCycle.at(stance);
	}


	if (movementCondition == PlayerState::Travelling) {
		currentAnimation->animate(deltaTime);
	}
	//
	// Update the player body according to the animation state.
	setTextureRect(currentAnimation->getCurrentFrame());
}
