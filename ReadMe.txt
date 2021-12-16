# NetworkedApplication


A UDP client application; part of a fast-action networked computer game using the Simple and Fast
Multimedia Library.

Gabriel Tierney - 1803088

***

Please update the X:\...\Readable\AddressandPortData.txt file with convenient Client and Server IP and Port data.
This application attempts communication with a server from launch.
For connectivity, ensure the partner NetworkedServer application is also running.

The application initialises with a splash screen state.
Press enter to take control of the player character.
Press Esc at any time to quit the application.

The player character navigates by way of the WASD keys.
Its look direction is controlled by the mouse position.

Though collision with other players is a feature that escaped development,
the player is equipped with a striking action and animation (Press Left Mouse Button).

Run additional instances of the application executable, so that new players may occupy the same game.
To test the program with and without prediction/interpolation computation, ammend the same
AddressandPortData.txt as required.

***


Please find the required Demonstration Video, linked below on Microsoft Stream, also a part of this
submission.
	https://web.microsoftstream.com/video/9a2ce13f-33f4-4485-8bf6-686be00af134


Please also find the complete project, accessible as a GitHub repository, linked below.
	https://github.com/JohnGTi/NetworkedApplication


***


Simple and Fast Multimedia Library (2018) SFML Team (2.5.1) [cross-platform software development library].
Available at: https://www.sfml-dev.org/download/sfml/2.5.1/ (Accessed: 05 November 2021).


	Please download the SFML 2.5.1 Visual C++ 15 (2017) - 32-bit.


Store the 'SFML-2.5.1' folder in X:\...\TierneyGabriel_1803088_CMP303\Client\NetworkedApplication\NetworkedApplication .

This solution was built with dynamically linked libraries.
Follow the official tutorial for creating and configuring an SFML project at
					https://www.sfml-dev.org/tutorials/2.5/start-vc.php .

For your convenience, the Debug and Release modules, respecitvely (for dynamic linking):

sfml-graphics-d.lib;sfml-window-d.lib;sfml-audio-d.lib;sfml-network-d.lib;sfml-system-d.lib;
sfml-graphics.lib;sfml-window.lib;sfml-audio.lib;sfml-network.lib;sfml-system.lib;

.