#include "ClientConsole.h"

#include <iostream>


ClientConsole::ClientConsole()
	: consoleState(ConsoleState::Ready)

    , CLIENTIP("sf::IpAddress::None"), CLIENTPORT(1024)
    , SERVERIP("sf::IpAddress::None"), SERVERPORT(4444)

    , networkStability(NetworkStability::Good)
    , networkInstabilityFlag(false)

    , simulationTime(0.f)
    , outgoingTimeStamp(0.f)

    , inactiveResponsePeriod(sf::seconds(2.f))
    , incomingManager(NULL)
    , killConsole(false)

    , outgoingType(MessageType::Positional)
{}

ClientConsole::~ClientConsole()
{
    // Wait for the worker thread to finish, and destroy its private reference.
    incomingManager->join();
    delete incomingManager;
}


void ClientConsole::SetServerAddress(sf::IpAddress serverIP, unsigned short serverPort)
{
    SERVERIP = serverIP;
    SERVERPORT = serverPort;
}

void ClientConsole::BindClient(sf::IpAddress clientIP, unsigned short clientPort)
{
    // Assign the set IP address and port number for binding.
    CLIENTIP = clientIP;
    CLIENTPORT = clientPort;

    // Bind the dedicated UDP socket to the any available port.
    // //
    if (UDPClientSocket.bind(CLIENTPORT) == sf::UdpSocket::Done)
    {
        // Communicate binding success to console.
        printf("Successfully bound to port %d at address %s.\n",
            UDPClientSocket.getLocalPort(), CLIENTIP.toString().c_str());
    }
    else {
        // Communicate that binding the socket has failed.
        printf("Failed to bind the socket to the specified port.\n");

        // Attempt to bind the socket to any port.
        while (UDPClientSocket.bind(sf::UdpSocket::AnyPort) != sf::UdpSocket::Done) {
            //
            printf("Failed to bind to an available port.\n");
        }

        // The loop of binding failure has been escaped.
        printf("Successfully bound to port %d at address %s.\n",
            UDPClientSocket.getLocalPort(), CLIENTIP.toString().c_str());
    }


    // Reference the bound socket by selector, for the purpose of listening with a timeout.
    //
    clientSocketSelector.add(UDPClientSocket);
}



void ClientConsole::ReceivePositional(sf::Packet& packet)
{
    // Protect shared class member variables by way of mutex lock within current scope.
    //std::unique_lock<std::mutex> lock(memberMutex);

    // A local time scale type instance, for extracting the simulated time scale data.
    PositionalData receivedPositional;
    float sentTimeStamp;


    // Test the extraction operation for the boolean returned, so as to protect against
    // the packet failing (fail on read).

    if (packet >> receivedPositional >> sentTimeStamp) {

        // An unknown client will not be found within the active combatants container.

        if (activeCombatants.find(receivedPositional.playerIdentifier) == activeCombatants.end())
        {
            // An unknown (newly connected) requires an avater to be constructed, for graphical representation.
            activeCombatants.insert({ {receivedPositional.playerIdentifier, ClientCombatant()} });

            // Push the client identifier onto the vector of newly joined combatants.
            newCombatants.push_back(receivedPositional.playerIdentifier);
        }

        // The received data's relevance and currency is validated prior to accepting
        // (pushing onto the client specific container of historical messages).
        //
        activeCombatants.at(receivedPositional.playerIdentifier).ReceievedIsCurrent(receivedPositional, sentTimeStamp);
    }
}

void ClientConsole::ReceiveTimeScale(sf::Packet& packet)
{
    // Protect shared class member variables by way of mutex lock within current scope.
    //std::unique_lock<std::mutex> lock(memberMutex);

    // A local time scale type instance, for extracting the simulated time scale data.
    TimeScaleData receivedTimeScale;
    float sentTimeStamp;


    // Test the extraction operation for the boolean returned, so as to protect against
    // the packet failing.

    if (packet >> receivedTimeScale >> sentTimeStamp)
    {
        // The period between an initial send message (returned by the server, for convenience) and
        // the arrival time of the server response is halved and used to sync with the server's time.
        // 
        // A better time II.

        simulationTime = sentTimeStamp + (simulationTime - receivedTimeScale.clientTimeStamp) / 2;
        //
        // (The server time elapsed, offset by the estimated period of one data trip).
        synchronisationData = receivedTimeScale;
        consoleState = ConsoleState::TimeSynced;
    }
}


sf::Socket::Status ClientConsole::ReceiveWithTimeout(sf::Packet& packet, sf::IpAddress& address, unsigned short& port, sf::Time timeout)
{
    // As noted in the 04 Practical, selection functionality provides the means of detection of dropped packets.
    // 
    // In this case, however, the time-out value is biased towards the service of detecting a dead relationship
    // between the game server (zero communication after two whole seconds), though the client cannot actually
    // determine whether or not the relationship was severed, or if the quiet is a result of packet loss.
    //
    // The resolve is the same, regardless (though a window period that might more precisely act on either condition
    // could most certainly be worked towards with some testing: A shorter window period will more properly respond to
    // packet loss conditions - condense the window period for the range in which packets are expected, maybe only
    // receive with a time-out in a state where a consistent flow of available packets are expected).


    if (clientSocketSelector.wait(timeout)) {
        
        // There is data for the socket to receive, and so:
        return UDPClientSocket.receive(packet, address, port);
    }
    else {
        // The wait has exceeded the time-out period.
        return sf::Socket::NotReady;
    }
}

void ClientConsole::IncomingManager()
{
    // Test the shared resource to validate that the console is running.
    //
    while (!killConsole) {

        // Initialise receive paramaters. The below function will fill their contents.
        sf::Packet receivePacket;
        //
        sf::IpAddress receiveIP;
        unsigned short receivedPort;

        sf::Socket::Status receiveStatus = ReceiveWithTimeout(receivePacket, receiveIP, receivedPort, inactiveResponsePeriod);
        if (receiveStatus == sf::Socket::Done)
        {
            // First, validate that this message was received from the server.
            if (receiveIP == SERVERIP && receivedPort == SERVERPORT)
            {
                // Extract the message header, and resolve according to the indicated message type.
                sf::Uint8 messageHeader;

                while (!receivePacket.endOfPacket() && receivePacket >> messageHeader) {
                    switch (messageHeader)
                    {
                    case (ToUnderlying(MessageType::Positional)):
                        //
                        ReceivePositional(receivePacket);
                        break;
                    case (ToUnderlying(MessageType::SimulationTimeScale)):
                        //
                        if (consoleState == ConsoleState::Ready) {
                            ReceiveTimeScale(receivePacket);
                        }
                        break;
                    default: break;
                    }
                }
            }
        }
        else if (receiveStatus == sf::Socket::NotReady && consoleState == ConsoleState::Playing) {
            //
            // If the data scape is barren for a period longer than the inactive response period,
            // AND this is the case during play (where communication is expected), kick.

            // KICK TO MATCHMAKING || 'DISCONNECT.'
        }
    }
}



bool ClientConsole::ActiveClient() {

    // An active client has established a relationship to a game server, or is ready to.

    return (consoleState >= ConsoleState::Ready && consoleState <= ConsoleState::Playing);

    // Below code is not reached and is for later use.

    // A client is active if a valid SERVERIP and SERVERPORT is addressable.
    return (SERVERIP != sf::IpAddress::None
        && SERVERPORT > static_cast<unsigned short>(1024));
}

void ClientConsole::Update(float deltaTime) {

    // Update the local clock by delta time passed.
    simulationTime += deltaTime;

    // Sending messages is an active, not reactive/automated process.
    // Handle outgoing under the condition that the SERVERIP and SERVERPORT  h a v e  been secured.

    if (ActiveClient()) {

        // Limit the rate at which outgoing player state updates are sent.
        //
        if (simulationTime >= (outgoingTimeStamp + sendRate)) {

            // Timestamp the outgoing packet.
            outgoingPacket << simulationTime;

            for (unsigned int attempts = 0; attempts < 3; attempts++) {

                if (UDPClientSocket.send(outgoingPacket, SERVERIP, SERVERPORT) != sf::UdpSocket::Done) {

                    // Even simple graphical message functionality might be without the scope
                    // of this application (as a product of time feasibility).
                    // 
                    // Here, a subtle (not screen centred, maybe lower left) buffering icon might
                    // communicate poor networking conditions.
                    
                    // Upon a third consequetive unsuccessful attempt, toggle the connection stability flag.
                    /*if (attempts == 2) {
                        //
                        if (networkInstabilityFlag && networkStability != NetworkStability::Unstable) {

                            switch (networkStability) {
                            case (NetworkStability::Good): networkStability = NetworkStability::Poor; break;
                            case (NetworkStability::Poor): networkStability = NetworkStability::Unstable; break;
                            };
                        }
                        networkInstabilityFlag = true;

                        // An unstable connection will be recognised later in the application
                        // and the player will be booted to matchmaking.
                    }*/
                }
                else {
                    // A successful send re-establishes the house understanding of network stability.
                    /*if (networkStability != NetworkStability::Good || !networkInstabilityFlag) {
                        
                        // The severity of the assessment of network stability has not been properly actualised.
                        // The assesment of a strong connection on a single successful send may be generous.
                        //
                        // This is partly to blame for lacking an elegant toggle functionality (for switching between the
                        // NetworkStability states); implementing a proper state machine for this case has been deprioritised
                        // (a simple state transition std::map was considered).

                        networkStability = NetworkStability::Good;
                        networkInstabilityFlag = false;
                    }*/
                    break; }
            }
            // A message has been communicated to the server.
            outgoingTimeStamp = simulationTime;
        }
    }
}

void ClientConsole::SetOutgoingMessage(const sf::Packet &packet) {
    //
    outgoingPacket.clear();
    outgoingPacket = packet;
}

void ClientConsole::InitiateSelection()
{
    // Data availability is checked by a worker thread seeking to mitigate the stasis affect of
    // blocking and the period of waiting for receipt of data.

    incomingManager = new std::thread([this] { IncomingManager(); });
}


// Note, for self, that the error (Warning C26812: Prefer 'enum class' over 'enum' (Enum.3)
//                                                      The enum type type-name is unscoped. Prefer 'enum class' over 'enum' (Enum.3))
// has been supressed (instances of the error flagged as a result of external library code, not my own work).