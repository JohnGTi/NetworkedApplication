#include "ApplicationInterface.h"
#include "MatchmakingState.h"

#include <fstream>


ApplicationInterface::ApplicationInterface(sf::RenderWindow* hwnd, Input* in)
	: window(hwnd), input(in)
	, currentState(&MatchmakingState::GetInstance())

	, clientIP("sf::IpAddress::None"), serverIP("sf::IpAddress::None")
	, clientPort(1024), serverPort(4444)
	, predictionActive(false)
{
	// Read and locally assign address data for client binding and server communication.
	ReadAddressFromFile();

	// Set the server address data within the client console.
	clientConsole.SetServerAddress(serverIP, serverPort);

	// Through the server console, create a UDP socket and bind it to the server port.
	clientConsole.BindClient(clientIP, clientPort);

	// Receive incoming messages in executing infinite timeout selection.
	clientConsole.InitiateSelection();


	// Initialise the custom mouse cursor.
	InitialiseCustomCursor();


	// Initialise the default state.
	currentState->Enter(this);
}

void ApplicationInterface::ReadAddressFromFile()
{
	// Specifiy the file to read from, and the file access mode 'r.'
	FILE* addressFile;
	fopen_s(&addressFile, "Readable/AddressandPortData.txt", "r");

	// If the file failed to load, communicate the error and signal an abort.
	if (addressFile == NULL) {
		//
		fputs("Error opening server/client address file.\n", stderr); abort();
	}


	// Perform fscanf and extrapolate the contents of the address data text file.
	//
	fscanf_s(addressFile, "The server network IP address and port, respectively.%*c");
	fscanf_s(addressFile, "%16s %hu%*c", serverIP.c_str(), 16, &serverPort);
	//
	//
	fgetc(addressFile);
	fscanf_s(addressFile, "Similarly, the client network IP address and port.%*c");
	fscanf_s(addressFile, "%16s %hu%*c", clientIP.c_str(), 16, &clientPort);


	// Construct a string buffer that the prediction/interpolation toggle can be read to.
	std::string switchBuffer;
	fgetc(addressFile);
	fscanf_s(addressFile, "The program may be run with no client-side prediction and interpolation, to demonstrate (On/Off).%*c");
	fscanf_s(addressFile, "%3s", switchBuffer.c_str(), 3);

	/*printf("(%s, %hu).\n", serverIP.c_str(), serverPort);
	printf("(%s, %hu).\n", clientIP.c_str(), clientPort);
	printf("(%s).\n", switchBuffer.c_str());*/

	// If it is read that the prediction/interpolation computation is to be in effect,
	// then predictionActive is true.

	if (memcmp(switchBuffer.c_str(), "On", 2) == 0) {
		predictionActive = true;
	}
}

void ApplicationInterface::InitialiseCustomCursor()
{
	// Load and assign the cursor texture image.
	cursorTexture.loadFromFile("Art/CustomPointer.png");
	customCursor.setTexture(&cursorTexture);
	//
	customCursor.setSize(sf::Vector2f(20, 20));
	customCursor.setOrigin(sf::Vector2f(customCursor.getSize().x / 2.f, customCursor.getSize().y / 2.f));
	customCursor.setRotation(270.f);

	// Hide the default mouse cursor.
	window->setMouseCursorVisible(false);
}


void ApplicationInterface::SetApplicationState(BaseState& newState)
{
	// Clean up the old state, switch to the new one.
	currentState->Release();
	currentState = &newState;

	// Initialise the new state.
	currentState->Enter(this);
}



void ApplicationInterface::HandleInput()
{
	// Press Esc key to close window.
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		window->close();
	}
	//
	// Is the above reasonable for the scope of this application:
	// verify intention to close (past a single key press)?


	// Handle input relating directly to the current application state.
	currentState->HandleInput();
}


void ApplicationInterface::Update(float deltaTime)
{
	// Update the current application state.
	currentState->Update(deltaTime);

	// Update the cursor position to track the mouse pointer.
	customCursor.setPosition(input->getMouseX(), input->getMouseY());


	// Update the client console so that outgoing messages are processed.
	clientConsole.Update(deltaTime);
}


void ApplicationInterface::Render()
{
	// Render necessary current state UI, (non-/)player sprites, effects...
	//
	currentState->Render();
	//
}