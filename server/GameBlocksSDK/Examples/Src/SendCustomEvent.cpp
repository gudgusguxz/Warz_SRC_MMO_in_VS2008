/*
Copyright 2012 GameBlocks, Inc.  All rights reserved.
*/

/*
GameBlocks API Sample: SendCustomEvent 

The purpose of this sample is to demonstrate how to use the API to:
1. Connect to GameBlocks server
2. Send a custom GameBlocks event to GameBlocks server
3. Receive a response message from GameBlocks server based on an incoming "CustomGameEvent" rule

*/


#include <GBClient/Inc/GBClient.h>
#include <windows.h> // for sleep()
#include <iostream>

using namespace std;
using namespace GameBlocks;

GBClient * gClient = NULL;

int HandleGameBlocksResponses()
{
	int messageCount = 0;

	//check if there are any messages in the incoming message queue
	if (gClient->Incoming_EventCount() > 0)
	{
		gClient->Incoming_PrepareEventForReading();

		//get incoming message information
		const char * messageName = gClient->Incoming_GetName();

		cout << endl << "GameBlocks response  " << endl 
			<< "  Message Name " << messageName << endl;

		//get incoming player ID as an int
		const int playerId = gClient->Incoming_GetPlayerId();
		//get incoming player ID as a string
		const char * playerIdStr = gClient->Incoming_GetPlayerId().c_str();

		//remove this message from the queue
		gClient->Incoming_PopMessage();
		messageCount++;
	}

	return messageCount;
}

int main() 
{
	// create a global psclient instance
	gClient = new GBClient();

	// initialize socket subsystem
	//This should not be called if application initialized sockets. GameBlocks must be initialized after socket system is initialized
	gClient->InitSocketsSubsystem();

	// start gameblocks client using config files
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

	// connect to gameblocks server asynchronously
	result = gClient->ConnectUsingConfig();
	if (result == GBCLIENT_ERROR_OK) 
	{
		cout << endl << "Waiting to connect..." << endl;
	}
	else
	{
		cout << endl << "Connect failed!" << endl;
	}

	// wait for connection to gameblocks to be established and send event
	bool bEventSent = false;

	while(true)
	{
		gClient->Tick();

		if(gClient->Connected())
		{
			// handle responses from gameblocks server and break when a message is received which concludes the sample
			if(HandleGameBlocksResponses()>0)
			{
				break;
			}
			
			/// [Send Custom Event]
			// send a custom event once
			if(!bEventSent)
			{
				cout << endl << "Connected to GameBlocks server" << endl;
				cout << endl << "Preparing custom event..." << endl;

				// NOTE:  The 3rd parameter below is the PlayerId.  Various types
				// may be passed, see the GBPublicPlayerId constructors. 0 or NULL 
				// may also be passed for events which do not map to a specific target.
				// PlayerId is used by the back end to link events to players.  The ID
				// must uniquely identify a player across multiple sessions, 
				// e.g. a potential PlayerId could be a Steam Unique ID.
				gClient->PrepareEventForSending("CustomGameEventName",NULL, "1X25100P");
				gClient->AddKeyValueString("Clan","Wolverines");
				gClient->AddKeyValueInt("Rank",15);
				gClient->AddKeyValueFloat("Health",30.8f);
				gClient->AddKeyValueVector3D("Location",102.1f,129.4f,254.6f);
				gClient->AddKeyValueVector2D("ZoneCoord",10.0f,20.0f);

				const GameBlocks::EGBClientError result = gClient->SendEvent();
				if(result == GBCLIENT_ERROR_OK)
				{
					bEventSent = true;
					cout << endl << "Send custom event success" << endl;
					cout << endl << "Waiting for response from GameBlocks server..." << endl;
				}
			}
			/// [Send Custom Event]
		} 

		Sleep(10);
	}

	// delete psclient instance
	gClient->Close();
	delete gClient;
	gClient = NULL;

	cout << endl << "Sample completed" << endl << endl;

	//wait for user input to exit application
	system("pause");
}