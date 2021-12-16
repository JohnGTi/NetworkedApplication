#pragma once

#include "BaseState.h"
#include "PlayerCharacter.h"

#include <set>


class SimulationState : public BaseState
{
private:
	// Private constructors and assignment operators ensures the class functions as a singleton.
	//
	SimulationState();
	SimulationState(const SimulationState& other);
	SimulationState& operator=(const SimulationState& other) = default;

	// Local pointers to framework objects (Framework, SFML and ClientConsole).
	sf::RenderWindow* window;
	Input* input;
	ClientConsole* clientConsole;

	// Player controlled game object (GameObject).
	PlayerCharacter player;

	// Pointer to the custom mouse cursor, to draw.
	GameObject* cursor;

	// A local pointer to a collection of active clients, for ease of access.
	std::map<std::string, ClientCombatant>* activeCombatants;

	// Whether or not the prediction/interpolation computation is active for this session.
	bool predictionOn;


	// Default functions for rendering to the screen.
	void BeginDraw();
	void EndDraw();
public:
	// Declare core application and state transition method.
	//
	void Init(ApplicationInterface* app_inter);

	void Enter(ApplicationInterface* app_inter);
	void Release() {}

	void HandleInput();
	void Update(float deltaTime);
	void Render();


	// Return a static class reference, following the singleton design pattern.
	//
	static BaseState& GetInstance();
};