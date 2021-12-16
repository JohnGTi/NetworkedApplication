#pragma once

#include "BaseState.h"


class MatchmakingState : public BaseState
{
private:
	// Private constructors and assignment operators ensures the class functions as a singleton.
	//
	MatchmakingState();
	MatchmakingState(const MatchmakingState& other);
	MatchmakingState& operator=(const MatchmakingState& other) = default;

	// Local pointers to framework objects (Framework and SFML).
	Input* input;
	sf::RenderWindow* window;

	// Pointer to the custom mouse cursor, to draw.
	GameObject* cursor;

	// A string representative of keyboard input, to validate.
	std::string predictionInput;

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
	void Update(float deltaTime) {}
	void Render();


	// Return a static class reference, following the singleton design pattern.
	//
	static BaseState& GetInstance();
};