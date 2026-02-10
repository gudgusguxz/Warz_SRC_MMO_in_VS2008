/*
Copyright 2012 GameBlocks, Inc.  All rights reserved.
*/

/*
GameBlocks API Sample: SendReservedEvent 

The purpose of this sample is to demonstrate how to use the API to:
1. Connect to GameBlocks server
2. Send a reserved GameBlocks event to GameBlocks server
3. Receive a response message from GameBlocks server based on an incoming "PlayerJoin" rule

*/


#include <GBClient/Inc/GBClient.h>
#include <windows.h> // for sleep()
#include <iostream>

using namespace std;
using namespace GameBlocks;

GBClient * gClient = NULL;

/// [Incoming Events]
int HandleGameBlocksResponses()
{
	int messageCount = 0;

	// check if there are any messages in the incoming message queue
	if (gClient->Incoming_EventCount() > 0)
	{
		gClient->Incoming_PrepareEventForReading();

		// get incoming message information
		const char * messageName = gClient->Incoming_GetName();
		GBPublicSourceId gbSourceId = gClient->Incoming_GetSourceId();
		const char * sourceId = gbSourceId.c_str();
		GBPublicPlayerId gbPlayerId = gClient->Incoming_GetPlayerId();
		const char * playerId = gbPlayerId.c_str();
		cout << endl << "GameBlocks response  " << endl 
			<< "  Message name " << messageName << endl 
			<< "  Source Id " << sourceId  << endl 
			<< "  Player Id " << playerId  << endl;

	// use other GBClient functions here to extract event information
	// and then act upon it

		// remove this message from the queue
		gClient->Incoming_PopMessage();
		messageCount++;
	}

	return messageCount;
}
/// [Incoming Events]

int main() 
{
	/// [Init GBClient Order]
	// create a global psclient instance
	gClient = new GBClient();

	// initialize socket subsystem
	// This should not be called if application initialized sockets. GameBlocks must be initialized after socket system is initialized
	gClient->InitSocketsSubsystem();

	// start gameblocks client using config files
	// NOTE: the config file is located relative to the GBClient Integration
	// executable.  See the sample config for reference.
	EGBClientError result = gClient->LoadConfigFromXML(
		"../../../Config/samples_config.xml");
	if (result == GBCLIENT_ERROR_OK)
	{
		cout << "Config load success  " << endl;
	}
	else
	{
		cout << endl << "Config load failed!" << endl;
	}

	// connect to gameblocks server asynchronously.  The config should have
	// been successfully loaded or set manually with valid properties via
	// SetConfig before calling Connect.
	result = gClient->ConnectUsingConfig();
	if (result == GBCLIENT_ERROR_OK) 
	{
		cout << endl << "Waiting to connect..." << endl;
	}
	else
	{
		cout << endl << "Connect failed!" << endl;
	}

	// NOTE: at this point the connection is pending, check
	// GBClient::Connected() in your game integration when working
	// with the active client.
	/// [Init GBClient Order]

	// wait for connection to gameblocks to be established and send event
	bool bEventSent = false;

	while(true)
	{
		/// [Service GBClient]
		// gClient should be serviced every frame if possible.
		// e.g. this should go in main engine loop
		gClient->Tick();

		if(gClient->Connected())
		{
			// your integration should also service the GBClient by handling
			// messages.
			// handle responses from gameblocks server and break when a message is received which concludes the sample
			if(HandleGameBlocksResponses()>0)
			{
				break;
			}
			// ...
			// send other events which need to be sent periodically
			/// [Service GBClient]

			/// [Sending a Reserved Event]
			if(!bEventSent)
			{
				cout << endl << "Connected to GameBlocks server" << endl;
				cout << endl << "Preparing reserved event..." << endl;

				const GameBlocks::EGBClientError result = Event_PlayerJoin_Send(
					gClient,
					NULL,
					"1X25100P",
					"John Smith",
					"0.0.0.0");
				if(result == GBCLIENT_ERROR_OK)
				{
					bEventSent = true;
					// send reservedin_Send success" << endl;
					cout << endl << "Waiting for response from GameBlocks server..." << endl;
				}
				else
				{
					cout << endl << "Event_PlayerJoin_Send failed!" << endl;
					break;
				}
			}
			/// [Sending a Reserved Event]
		} 

		Sleep(10);
	}

	/// [GBClient Cleanup]
	// clean up psclient instance
	gClient->Close();
	delete gClient;
	gClient = NULL;
	/// [GBClient Cleanup]

	cout << endl << "Sample completed" << endl << endl;

	//wait for user input to exit application
	system("pause");
}