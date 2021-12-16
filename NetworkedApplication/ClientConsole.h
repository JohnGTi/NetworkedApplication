#pragma once

#include "ClientCombatant.h"

#include <thread>
#include <atomic>


enum class ConsoleState {
	Default
	, Ready
	, TimeSynced
	, Playing
	, Disconnected
};

enum class NetworkStability {
	Unstable
	, Poor
	, Good
};


class ClientConsole
{
private:
	// Client network IP address and port, to bind.
	sf::IpAddress CLIENTIP, SERVERIP;
	unsigned short CLIENTPORT, SERVERPORT;

	// The current console active state.
	ConsoleState consoleState;

	// The console's simple determination of stability over the network.
	NetworkStability networkStability;
	bool networkInstabilityFlag;

	// The client representative of time is a single float value, to be updated upon
	// receipt of a SimulationTimeScale message type.
	float simulationTime;
	
	// Store the local/simulated time at which the last outgoing message was sent.
	float outgoingTimeStamp;



	// This client's dedicated UDP socket.
	sf::UdpSocket UDPClientSocket;

	// The sf::SocketSelector functionality presents the opportunity to register the fact that
	// expected packets are not being received.
	sf::SocketSelector clientSocketSelector;
	sf::Time inactiveResponsePeriod;

	// A worker thread for managing incoming packets.
	std::thread* incomingManager;
	//

	// (Protected) representative of the client console's active state.
	std::atomic<bool> killConsole;


	// A map of other clients active in the shared game.
	std::map<std::string, ClientCombatant> activeCombatants;

	// A vector of string client identifiers, to alert the simulation state of new combatants.
	std::vector<std::string> newCombatants;

	// Comprising a single, dedicated outgoing message.
	MessageType outgoingType;
	sf::Packet outgoingPacket;

	// Track, store, keep a history of messages received.
	TimeScaleData synchronisationData;



	// The incoming packet manager thread function.
	void IncomingManager();

	// Receive data with a status return on receive and timeout.
	sf::Socket::Status ReceiveWithTimeout(sf::Packet &packet, sf::IpAddress& address, unsigned short& port, sf::Time timeout);

	// Receive data, according to message type.
	void ReceivePositional(sf::Packet& packet);
	void ReceiveTimeScale(sf::Packet& packet);

	// Test that the client is capable of play.
	bool ActiveClient();


public:
	ClientConsole();
	~ClientConsole();


	// Essential console initialisation and update functions, such as binding the UDP socket
	// and initiating the incoming messages worker thread.
	void SetServerAddress(sf::IpAddress serverIP, unsigned short serverPort);
	//
	void BindClient(sf::IpAddress clientIP, unsigned short clientPort);
	void InitiateSelection();
	//
	void Update(float deltaTime);


	// Update the constant outgoing message by type and packet contents.
	void SetOutgoingMessage(const sf::Packet &packet);

	// Retrieve any collection of (newly /)recognised clients.
	std::map<std::string, ClientCombatant>* GetActiveCombatants() { return &activeCombatants; }
	std::vector<std::string>* GetNewCombatants() { return &newCombatants; }

	// Return/update the console state.
	ConsoleState GetConsoleState() { return consoleState; }
	void SetConsoleState(ConsoleState state) { consoleState = state; }

	// Return the locally stored time scale data.
	TimeScaleData GetTimeScaleData() { return synchronisationData; }

	// Receive the game simulation time.
	float GetSimulationTime() { return simulationTime; }
};