// Robertson, P. (2020) GitHub template repository for CMP105. Available at: https://github.com/Abertay-University-SDI/CMP105_W1 (CMP105_W1/Week1/CMP105App/Framework/). (Accessed: 19 September 2019).
// : 'windowProcess()' organisation and Input Framework (the Input class has been developed past its original condition for some improved functionality).


// *** \\


#include "ApplicationInterface.h"


void windowProcess(sf::RenderWindow* window, Input* input) { // (Robertson, 2020).

	// Handle window events.

	sf::Event event;
	while (window->pollEvent(event)) {

		switch (event.type) {

		case sf::Event::Closed: window->close(); break;

			// Window resize functionality has been cut as a feasibility measure of scope.
			// 
			// Window size packets might've been sent alongside the standard positional data, though other work
			// would have to be done to recentre all clients for restriction to a closed play area.
		/*case sf::Event::Resized:
			window->setView(sf::View(sf::FloatRect(0.0f, 0.0f, (float)event.size.width, (float)event.size.height)));
			break;*/
			//
			// Update the input class, according to input events.
			//
		case sf::Event::KeyPressed:
			input->setKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			input->setKeyUp(event.key.code);
			break;
		case sf::Event::MouseMoved:
			input->setMousePosition(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left) {
				input->setLeftMouse(Input::MouseState::DOWN);
			}
			else if (event.mouseButton.button == sf::Mouse::Right) {
				input->setRightMouse(Input::MouseState::DOWN);
			}
			else if (event.mouseButton.button == sf::Mouse::Middle) {
				input->setMiddleMouse(Input::MouseState::DOWN);
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left) {
				input->setLeftMouse(Input::MouseState::UP);
			}
			else if (event.mouseButton.button == sf::Mouse::Right) {
				input->setRightMouse(Input::MouseState::UP);
			}
			else if (event.mouseButton.button == sf::Mouse::Middle) {
				input->setMiddleMouse(Input::MouseState::UP);
			}
			break;
		case sf::Event::MouseWheelScrolled:
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
				input->setMouseVerticalWheel(Input::MouseState::DOWN,
					event.mouseWheelScroll.delta);
			}
			break;
		default: // Don't handle other events.
			break;
		}
	}
}


int main() {

	printf("Fast-Action Networked Game Application. Client-side.\n\n\n");



    sf::RenderWindow window(sf::VideoMode(1200, 675), "SFML Network Application.");

    // Initialise the input object.
    Input input;

    // Initialise objects for delta time.
    sf::Clock mainClock;
    float deltaTime;


	// Create an interface for management of the current application state.
	ApplicationInterface applicationInterface(&window, &input);


    while (window.isOpen()) {

        // Process window events...
        windowProcess(&window, &input);

		// Calculate delta time. How much time has passed since
		// it was last calculated (in seconds) and restart the clock.
		deltaTime = mainClock.restart().asSeconds();


		// Call the key application (core) loop functions.
		//
		applicationInterface.HandleInput();
		applicationInterface.Update(deltaTime);
		applicationInterface.Render();


		// Update input class, handle pressed keys (Must be done last).
		input.update();
    }

    return 0;
}