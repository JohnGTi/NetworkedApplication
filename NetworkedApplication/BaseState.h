#pragma once


#include "ApplicationInterface.h"

// Forward declaration to resolve circular dependency/include.
class ApplicationInterface;


class BaseState
{
private:

	// ...abstract base class for application state transition.
public:

	virtual void Init(ApplicationInterface* app_inter) = 0;

	virtual void Enter(ApplicationInterface* app_inter) = 0;
	virtual void Release() = 0;

	virtual void HandleInput() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;


	virtual ~BaseState() {};

protected:

	BaseState();


	// For individual state access.
	//
	ApplicationInterface* pollApplication;
	//
	float stateTimer;
};