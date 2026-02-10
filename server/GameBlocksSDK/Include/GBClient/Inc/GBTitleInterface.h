//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef GB_TITLE_INTERFACE_H_
#define GB_TITLE_INTERFACE_H_

#include "GBPublicId.h"

namespace GameBlocks
{

class GBClient;

/// @class GBTitleInterface
/// Helper class to automatically trigger client integration events.  It 
/// notifies the client when certain time-based reserve events should be sent, 
/// and also notifies the client when incoming events arrive for processing.
/// If this class is used and registered with the GBClient via the 
/// GBClient::SetTitleInterface() call, the incoming event queue is pumped 
/// by the GBClient internally and does not need to be serviced directly.
///
class GBTitleInterface
{
public:
	/// helper to store off a ref to the GBClient for use in processing events 
	/// by the derived class.
	void SetClient(GBClient * client);

	//---------------------------------------------------------------------------
	// Outgoing Event Triggers

	/// When this is called the client should send the PlayerCount 
	/// reserved event.
	virtual void SendPlayerCount() {};

	/// When this is called the client should send the MaxPlayerCount 
	/// reserved event.
	virtual void SendMaxPlayerCount() {};

	/// When this is called the client should send the PlayerList reserved 
	/// event.
	virtual void SendPlayerList() {};

	/// When this is called the client should send the PlayerLocationList 
	/// reserved event.
	virtual void SendPlayerLocationList() {};

	/// When this is called the client should submit Aimbot Detector frame 
	/// data to the GBClient.
	virtual void ProcessAimbotFrame() {};

	//---------------------------------------------------------------------------
	// Incoming Event Handlers

	/// When this is called the client should Kick the specified player 
	/// and present the given message to the player.  The banDaysRemaining 
	/// will be greater than 0 if the player is currently banned and it will 
	/// contain the remaining ban time.
	virtual void HandleActionKick(const GameBlocks::GBPublicPlayerId & playerId, 
		const char * message, float banDaysRemaining) {};

	/// When this is called the client should Kill the specified player 
	/// and present the given message to the player.
	virtual void HandleActionKill(const GameBlocks::GBPublicPlayerId & playerId, 
		const char * message) {};

	/// When this is called the client should send the given message to the given 
	/// player.  If localization is configured for the title, specific localized 
	/// text can be retrieved by using Incoming_GetPairValueString with the desired 
	/// GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_* language.
	virtual void HandleActionMessagePlayer(const GameBlocks::GBPublicPlayerId & playerId, 
		const char * message) {};

	/// When this is called the client should send the given message to all
	/// players.  If localization is configured for the title, specific localized 
	/// text can be retrieved by using Incoming_GetPairValueString with the desired 
	/// GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_* language.
	virtual void HandleActionGlobalMessage(const char * message) {};

	/// When this is called the client should notify the given player client 
	/// to generate a screenshot and submit it back to the Game Server which should 
	/// then send the screenshot to the GameBlocks server via the SendScreenShot 
	/// reserved event.
	virtual void HandleActionSendScreenShot(const GameBlocks::GBPublicPlayerId & playerId) {};

	/// HandleCustomAction is called for any non-reserved events which
	/// come in to the client.  Consult the Title Configuration document
	/// to see the list of custom actions which your title should handle.
	virtual void HandleCustomAction() {};

protected:

	GBClient * GetClient() const;

private:

	GBClient * m_gbClient;
}; // class GBTitleInterface

} // namespace GameBlocks

#endif // #define GB_TITLE_INTERFACE_H_