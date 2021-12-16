#pragma once

#include <SFML/Graphics.hpp>

#include "Framework/Input.h"  // (Robertson, 2020).
#include "Framework/GameObject.h"
#include "BaseState.h"
#include "ClientConsole.h"


// The ApplicationInterface class forms the basis of managing the class based finite state machine,
// for state transition between the matchmaking screen and game screen.



// Forward declaration to resolve circular dependency/include.
class BaseState;


class ApplicationInterface
{
private:
	// Pointers to framework objects (Framework and SFML).
	Input* input;
	sf::RenderWindow* window;

	// Pointer to the current state class.
	BaseState* currentState;

	// Console for interfacing with this application as a networked client.
	ClientConsole clientConsole;

	// Network address data to be filled by stream, and passed into relevant classes.
	std::string clientIP, serverIP;
	//
	unsigned short clientPort, serverPort;
	bool predictionActive;


	// Representative elements for the custom mouse cursor.
	GameObject customCursor;
	sf::Texture cursorTexture;

	// Initialise the cursor GameObject and prepare for use.
	void InitialiseCustomCursor();
	//
public:
	// Specified constructor and application core loop functions.
	ApplicationInterface(sf::RenderWindow* hwnd, Input* in);

	// Read address and port data for client binding and server communication.
	void ReadAddressFromFile();

	// Return the prediction computation status, as determined by file read.
	bool GetPredictionOn() { return predictionActive; }

	// Class based state management getter and setters.
	inline BaseState* GetCurrentState() const { return currentState; }
	void SetApplicationState(BaseState& newState);

	// Receive pointers to Framework, SFML and internal network objects.
	sf::RenderWindow* GetRenderWindow() { return window; };
	Input* GetInput() { return input; };
	ClientConsole* GetClientConsole() { return &clientConsole; }

	// So that the custom cursor can be updated by individual states.
	GameObject* GetCustomCursor() { return &customCursor; };


	void HandleInput();
	void Update(float deltaTime);
	void Render();
};