#include "MatchmakingState.h"
#include "SimulationState.h"


MatchmakingState::MatchmakingState()
	: window(NULL), input(NULL), cursor(NULL)
{}


void MatchmakingState::Enter(ApplicationInterface* app_inter)
{
	// Initialise, if this is the first transition into matchmaking...
	if (pollApplication == NULL) {

		Init(app_inter);

	} else { /**/ }



	// The 'MatchmakingState' ultimately does not handle matchmaking, which is a little embarrassing.
	// See the 'ApplicationInterface' for reading in address data and handling of the client console.


}

void MatchmakingState::Init(ApplicationInterface* app_inter)
{
	// Assign pointer to the application interface for class access.
	pollApplication = app_inter;

	window = pollApplication->GetRenderWindow();
	input = pollApplication->GetInput();

	cursor = pollApplication->GetCustomCursor();
}


void MatchmakingState::HandleInput()
{
	// Press Enter to transition to the game state.
	if (input->isKeyDown(sf::Keyboard::Enter)) {

		// Set the Enter key up so as to prevent the input being read as a HOLD.
		input->setKeyUp(sf::Keyboard::Enter);

		// Poll the application interface to handle the state change.
		pollApplication->SetApplicationState(SimulationState::GetInstance());
	}
}

void MatchmakingState::Render()
{
	BeginDraw();

	window->draw(*cursor);
	
	EndDraw();
}


// Default functions for rendering to the screen.
//
void MatchmakingState::BeginDraw() {

	// Clear the scene (overwrite with a pretty pink colour).
	window->clear(sf::Color(255, 173, 173));
}
//
//
void MatchmakingState::EndDraw() {

	// Display on screen what has been rendered to the window.
	window->display();
}



BaseState& MatchmakingState::GetInstance()
{
	static MatchmakingState singleton;
	//
	// Return a static reference.
	//		(A pointer may be more difficult to manage away from memory leakage).
	return singleton;
}