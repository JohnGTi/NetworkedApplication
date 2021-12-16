#include "SimulationState.h"
#include "MatchmakingState.h"
#include <iostream>


SimulationState::SimulationState()
	: window(NULL), input(NULL)
	, clientConsole(NULL)

	, cursor(NULL)
	, activeCombatants(NULL)
	, predictionOn(false)
{}


void SimulationState::Enter(ApplicationInterface* app_inter)
{
	// Initialise, if this is the first transition into matchmaking...
	if (pollApplication == NULL) {

		Init(app_inter);

	}
	else { /**/ }


}

void SimulationState::Init(ApplicationInterface* app_inter)
{
	// Assign pointer to the application interface for class access.
	pollApplication = app_inter;

	window = pollApplication->GetRenderWindow();
	input = pollApplication->GetInput();
	clientConsole = pollApplication->GetClientConsole();
	//
	cursor = pollApplication->GetCustomCursor();

	// Set the prediction/interpolation computation active status.
	predictionOn = pollApplication->GetPredictionOn();

	// Share the polled framework pointers as required.
	player.InitaliseAvatar(window, input, false);
	player.ReceiveConsole(clientConsole);

	// Assign collection reference to a local pointer, for ease of access.
	activeCombatants = clientConsole->GetActiveCombatants();
}



void SimulationState::HandleInput()
{
	// Press Enter to return to the matchmaking state.
	if (input->isKeyDown(sf::Keyboard::Enter)) {

		// Set the Enter key up so as to prevent the input being read as a HOLD.
		input->setKeyUp(sf::Keyboard::Enter);

		// Poll the application interface to handle the state change.
		pollApplication->SetApplicationState(MatchmakingState::GetInstance());
	}//


	// The player class is responsible for the handling/control of its own GameObject.
	player.handleInput();
}

void SimulationState::Update(float deltaTime)
{
	// Check for newly joined players, designate and initialise as game combatants.
	if (!clientConsole->GetNewCombatants()->empty()) {

		// Assign collection reference to a local pointer.
		std::vector<std::string>* newCombatants = clientConsole->GetNewCombatants();


		for (std::string id : *newCombatants) {


			// Share the polled framework pointers as required, and create a player (enemy) avatar.
			activeCombatants->at(id).InitaliseAvatar(window, input, predictionOn);

			// Remote players are simply distinguished by a difference in colour (remote players
			// are shadowed, for clarity).
			activeCombatants->at(id).ApplySpriteColourFill(sf::Color(25, 25, 25));
			
			// 
			// Functionality in the construction of the PlayerAvatar provides a convenient method
			// of supplying a unique sprite sheet reference, though the drawing of another player
			// character type was considered a non-essential development.
		}
		// Clear the contents of the new combatants collection; the remote players are now properly recognised.
		newCombatants->clear();
	}


	// Update the local positional, look and state data of the remote players.
	for (auto& combatant : *activeCombatants) {

		combatant.second.Update(deltaTime, clientConsole->GetSimulationTime());


		// Note that as the console requires knowledge of a client representation so that it might handle clients,
		// the ClientCombatant class cannot include the ClientConsole, to access its public contents.

		// This failure of structure is a fairly simple solve, and the nature of the confusion is evident in the
		// class name 'ClientCombatant.'
		
		// Here a 'Client' class should more strictly detail itself relevant to its
		// function as a networking item. The 'Combatant' part should more strictly represent the graphical component
		// of a remote player. A 'Client' and 'Combatant' instance could be easily paired by client/player ID.
	}
	
	// Update the simulation as encapsulated by item.
	player.update(deltaTime);

	// Update the cursor position to revolve in relation to the player.
	cursor->setRotation(player.GetLookAtTheta());
}

void SimulationState::Render()
{
	BeginDraw();

	// Draw order is determined by a player's y-axis positional value, so as to illustrate
	// an effect of a plane sloped away from the camera (front facing isometric).

	// Player avatars are inserted into a set collection with a custom comparison operator
	// for ordering by the screen height value. This collection is then traversed, and avatars
	// are drawn in the new order.


	// An ordered collection of player avatars, for a proper draw order.
	auto cmp = [](PlayerAvatar* left, PlayerAvatar* right) { return left->getPosition().y < right->getPosition().y; };
	

	std::set<PlayerAvatar*, decltype(cmp)> playerAvatars(cmp);
	//
	playerAvatars.insert(&player);
	for (auto& combatant : *activeCombatants)
	{
		playerAvatars.insert(&combatant.second);
	}


	// Note that proper templating methods might resolve type erasure errors that prevented the
	// playerAvatars container from being defined as a class member.
	
	// This development would save a cycle of iteration through the activeCombatants std::map,
	// as elements could be inserted as the remote player representations were updated in SimulationState::Update,
	// all that would be required in the Render function would be a simple clearing of the container.


	for (auto avatar : playerAvatars)
	{
		window->draw(*avatar);
	}
	// UI elements are drawn, finally.
	window->draw(*cursor);

	EndDraw();
}


// Default functions for rendering to the screen.
//
void SimulationState::BeginDraw() {

	// Clear the scene (overwrite with a pretty pink colour).
	window->clear(sf::Color(0, 203, 254));
}
//
//
void SimulationState::EndDraw() {

	// Display on screen what has been rendered to the window.
	window->display();
}



BaseState& SimulationState::GetInstance()
{
	static SimulationState singleton;
	//
	// Return a static reference.
	//		(A pointer may be more difficult to manage away from memory leakage).
	return singleton;
}