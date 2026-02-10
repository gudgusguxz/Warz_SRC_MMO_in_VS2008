// Copyright 2013 GameBlocks, Inc.  All rights reserved.

#include <iostream>
#include <windows.h> // for sleep()
#include <GBClient/Inc/GBClient.h>
#include "GameSim.h"
#include "GameBlocksWrapper.h"

using std::cout;
using std::endl;

int RunSample()
{
	cout 
	<< endl 
	<< "----------------------------------------------------" 
	<< endl
	<< "First PersonShooter Sample Start" 
	<< endl 
	<< endl;

	FirstPersonShooter::GameSim gameSim;

	// In this example, all portions of the actual GameBlocks integration are
	// surrounded by GAMEBLOCKS_BEGIN and GAMEBLOCKS_END comments.  This practice allows
	// for easy searching for the integration in the client project source code.
	// GAMEBLOCKS_BEGIN
	FirstPersonShooter::GameBlocksWrapper * wrapper = FirstPersonShooter::GameBlocksWrapper::Instance();
	wrapper->ConnectAsync(); // start connection

	// Pass in this sim struct for example, but in a real integration
	// the GameBlocksWrapper class will have access to whatever game systems
	// are needed to get relevant game state information.
	wrapper->SetGameSim(&gameSim);

	// Example waits for connection to GameBlocks.
	// In a real game integration, ConnectAsync should be called as early
	// as possible during the engine intialization, after sockets have been
	// initialized.  That way by the time events are ready to be sent
	// the connection has been established.  Events simply fail to send
	// if the connection is not ready yet, but the game server shouldn't
	// have to rely on the GameBlocks connection for any reason.
#ifdef GAMEBLOCKS_ENABLED
	while(!wrapper->IsConnected())
	{
		wrapper->Tick(0.f);

		Sleep(100);
	}
#else
	cout << endl << "GAMEBLOCKS_ENABLED not defined.  Sample completed" << endl << endl;

	// wait for user input to exit application
	system("pause");
#endif // GAMEBLOCKS_ENABLED
	// GAMEBLOCKS_END

	// this is the simulated game loop
	bool isSimActive = true;
	while (isSimActive)
	{
		const float deltaTime = 1.f; // arbitrary example value
		isSimActive = gameSim.Tick(deltaTime);

		// GAMEBLOCKS_BEGIN
		if (isSimActive)
		{
			wrapper->Tick(deltaTime);
		}
		// GAMEBLOCKS_END

		Sleep(10);
	}

	wrapper->Cleanup();

	return 0;
}


int main() 
{
	RunSample();

	cout << endl << "Sample completed" << endl << endl;

	// wait for user input to exit application
	system("pause");
}
