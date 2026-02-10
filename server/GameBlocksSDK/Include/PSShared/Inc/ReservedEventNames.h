//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef _RESERVEDEVENTNAMES_H_
#define _RESERVEDEVENTNAMES_H_

//////////////////////////////////////////////////////////////////////////////
// Events sent from the GameBlocks Client (Game Server)

//common event key names

//-----------------------------------------------------------------------------
// GBClient Info

	// gbclient version
#define GB_RESERVED_EVENT_GBVERSION "GBVersion"
#define GB_RESERVED_EVENT_GBVERSION_VERSION_KEY_NAME "Version"

// gbclient status
#define GB_RESERVED_EVENT_GBCLIENT_STATUS "ClientStatus"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_OUTGOING_KEY_NAME "OutMessage"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_INCOMING_KEY_NAME "InMessages"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_CONNECTS_KEY_NAME "Connects"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_SENDS_KEY_NAME "SentMessages"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_KEY_LOCALTIME "LocalTime"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_KEY_TIMEZONE_BIAS "TimeZoneBias"
#define GB_RESERVED_EVENT_GBCLIENT_STATUS_KEY_DROPPED_MESSAGES "DrpdMsgs"


// hearbeat
#define GB_RESERVED_EVENT_HEARTBEAT "Heartbeat"

// integration/client error reporting
#define GB_RESERVED_EVENT_GBCLIENT_ERROR "GBError"
#define GB_RESERVED_EVENT_GBCLIENT_ERROR_KEY_MESSAGE "Message"

//-----------------------------------------------------------------------------
// Game Server Info

//list players
#define GB_RESERVED_EVENT_PLAYER_LIST "PlayerList"
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_PLAYER_ID "PlayerId" 
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_PLAYER_NAME "PlayerName" 
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_IP "IP" 
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_PING "Ping" 
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_ELAPSED "Elapsed" 
#define GB_RESERVED_EVENT_PLAYER_LIST_KEY_TEAM "Team"

//location event
#define GB_RESERVED_EVENT_PLAYER_LOCATION_LIST "PlayerLocationList" // deprecated TODO: remove
#define GB_RESERVED_EVENT_PLAYER_LOCATION_LIST_GROUP_COUNT 3
#define GB_RESERVED_EVENT_PLAYER_LOCATION_LIST_NEW "PlrLocsNew"
#define GB_RESERVED_EVENT_PLAYER_LOCATION_LIST_NEW_GROUP_COUNT 5

// round results
#define GB_RESERVED_EVENT_MATCH_RESULTS_LIST "MatchResultsList"
#define GB_RESERVED_EVENT_MATCH_RESULTS_LIST_KEY_MATCH_LENGTH "TimeLength"
#define GB_RESERVED_EVENT_MATCH_RESULTS_LIST_WINNING_TEAM "WinningTeam"

// map
#define GB_RESERVED_EVENT_SOURCEINFO_NEW "SourceInfo"
#define GB_RESERVED_EVENT_SOURCEINFO "Map" // deprecated
#define GB_RESERVED_EVENT_SOURCEINFO_KEY_PROP_GAMEMODE "State"
#define GB_RESERVED_EVENT_SOURCEINFO_KEY_PROP_MAP "Name" // deprecated
#define GB_RESERVED_EVENT_SOURCEINFO_KEY_PROP_MAP_NEW "Map"

#define GB_RESERVED_EVENT_MAX_PLAYER_COUNT "MaxPlayerCount"
#define GB_RESERVED_EVENT_MAX_PLAYER_COUNT_KEY_COUNT "Count"
#define GB_RESERVED_EVENT_PLAYER_COUNT "PlayerCount"
#define GB_RESERVED_EVENT_PLAYER_COUNT_KEY_COUNT "Count"

//-----------------------------------------------------------------------------
// Player Info

// chat
#define GB_RESERVED_EVENT_CHAT "Chat"
#define GB_RESERVED_EVENT_CHAT_KEY_MESSAGE "Text"
#define GB_RESERVED_EVENT_CHAT_KEY_TEAM_ONLY "TeamOnly"

	// the playerID can be stored in the pairs for events that need to 
	// (e.g. for events with multiple player data in the pairs)
#define GB_RESERVED_EVENT_KEY_PLAYER_ID "PlayerID"

//player join event
#define GB_RESERVED_EVENT_PLAYER_JOIN "PlayerJoin"
#define GB_RESERVED_EVENT_PLAYER_JOIN_KEY_NAME "Name"
#define GB_RESERVED_EVENT_PLAYER_JOIN_KEY_IP "IP"

//player leave event
#define GB_RESERVED_EVENT_PLAYER_LEAVE "PlayerLeave"
#define GB_RESERVED_EVENT_PLAYER_LEAVE_KEY_NAME "Name"

// player screenshot
#define GB_RESERVED_EVENT_PLAYER_SCREENSHOT_JPG "PlayerScreenshotJpg"
#define GB_RESERVED_EVENT_PLAYER_SCREENSHOT_JPG_IMAGE_DATA_KEY_NAME "ImageData"

// this event is written out as a result of the aimbot detector detecting
// a cheater
#define GB_RESERVED_EVENT_AIMBOT_DETECTED "AimBotDetected"
#define GB_RESERVED_EVENT_AIMBOT_DETECTED_VICTIM_KEY_NAME "VictimId"

// Aimbot data
#define GB_RESERVED_EVENT_MOVE_HISTORY "AimBotDetect" // "MoveHist"
#define GB_RESERVED_EVENT_MOVE_HISTORY_KEY_NUM_FRAMES "NumFrames"
#define GB_RESERVED_EVENT_MOVE_HISTORY_KEY_NUM_PLAYERS "NumPlayers"
#define GB_RESERVED_EVENT_MOVE_HISTORY_KEY_KILLER_WEAPON "KWeapId"
#define GB_RESERVED_EVENT_MOVE_HISTORY_KEY_KILLER_MOVER_TYPE "KMvrTyp"
#define GB_RESERVED_EVENT_MOVE_HISTORY_KEY_TARGET_MOVER_TYPE "TMvrTyp"
#define GB_RESERVED_EVENT_MOVE_HISTORY_TARGET_ID_KEY_NAME "TargetId"
#define GB_RESERVED_EVENT_MOVE_HISTORY_DEBUG "AimBotDebug" // "MoveHistDebug"
#define GB_RESERVED_EVENT_AIMBOT_JOB_SETUP_INFO "AbJobSetupInfo"
#define GB_RESERVED_EVENT_ORIGINAL_AGENT_JOB_EVENT_NAME "OrigAgtDataEvtName"
#define GB_RESERVED_EVENT_AGENT_JOB_TYPE_KEY_NAME "AgtJReqType"
#define GB_RESERVED_EVENT_PLAYER_AIMBOT_JOB_ID_KEY_NAME "AgCoordJId"
#define GB_RESERVED_EVENT_PLAYER_AIMBOT_RETURN_RECEIPT_KEY_NAME "AgtJobRtnReceipt"
#define GB_RESERVED_EVENT_PLAYER_AIMBOT_DETECTED_KEY_NAME "AbDetected"
#define GB_RESERVED_EVENT_PLAYER_AIMBOT_PROBABILITIES_KEY_NAME "AbProbabilities"
#define GB_RESERVED_EVENT_AGENT_VERSION_KEY_NAME "AgtVer"
#define GB_RESERVED_EVENT_AGENT_CONNECTION_COUNT_KEY_NAME "AgtConnCount"
#define GB_RESERVED_EVENT_AGENT_JOB_PROCESSING_TIME_KEY_NAME "AbJProcssingTime"
#define GB_RESERVED_EVENT_AGENT_JOB_RESULT_INCONCLUSIVE_KEY_NAME "AbJResInconRes"
#define GB_RESERVED_EVENT_JOB_REQUIRES_RETURN_RESULT_KEY_NAME "AbJReqReturnRes"
#define GB_RESERVED_EVENT_AIMBOT_AGENT_STATS_UPDATE_KEY_NAME "AbAgtCoordStatsUpd"
#define GB_RESERVED_EVENT_AIMBOT_JOB_SETUP_NUM_EVENTS_KEY_NAME "AbJSetupNumEvents"
#define GB_RESERVED_EVENT_AIMBOT_CHEAT_DETECTION_RESULTS_KEY_NAME "AbJCheatDetResSmry"

// Weapon Cheat data
#define GB_RESERVED_EVENT_WEAPONFIRE_HISTORY "WeaponFireHist"
#define GB_RESERVED_EVENT_WEAPONFIRE_KEY_NUM_IMPACT_FRAMES "NumFrames"
#define GB_RESERVED_EVENT_WEAPONFIRE_KEY_NUM_PROJ_FRAMES "ProjFC"
#define GB_RESERVED_EVENT_WEAPONFIRE_KEY_TIMESTAMP "ElapsedMs"

// Internal - Agent events
#define GB_RESERVED_EVENT_AGENT_COORDINATOR_RECEIVED_JOB_RESULT "AgtCoordRecJRes"
#define GB_RESERVED_EVENT_AGENT_CONNECT_HANDSHAKE_DATA_REQUEST "AgtConnHandDataReq"
#define GB_RESERVED_EVENT_AGENT_CONNECT_HANDSHAKE_DATA_RESPONSE "AgtConnHandDataResp"
#define GB_RESERVED_EVENT_AGENT_JOB_DATA "AgtJobDataEvt"
#define GB_RESERVED_EVENT_AGENT_JOB_RESULT "AgtJobProcRes"
#define GB_RESERVED_EVENT_AGENT_JOB_COMPLETED_SUCCESSFUL_KEY_NAME "AgtJComplSuccess"
// Keep Alive Message Aimbot Agents send to the Agent Coordinator
#define GB_RESERVED_EVENT_AIMBOT_AGENT_KEEP_ALIVE "AgtKeepAlive"

#define GB_RESERVED_EVENT_KEY_PRESSED "KeyPressed"
#define GB_RESERVED_EVENT_KEY_PRESSED_KEY_VALUE_VIRTUAL "VKey"

//////////////////////////////////////////////////////////////////////////////
// Events sent from the GameBlocks Server to the GameBlocks Client (Game Server)
// These incoming messages should be handled by the Game Server.

#define GB_RESERVED_EVENT_ACTION_KEY_MESSAGE "Message" // generic message sent along with actions to client
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_ENGLISH		"Message" // currently mapped to default message!
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_SPANISH		"Spa"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_MANDARIN		"Man"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_GERMAN			"Ger"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_FRENCH			"Fr"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_POLISH			"Pol"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_JAPANESE		"Jap"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_ITALIAN		"Ita"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_CZECH			"Cz"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_RUSSIAN		"Rus"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_DUTCH			"Dut"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_BRAZILIAN_PORTUGUESE "Bra"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_PORTUGUESE "Por"
#define GB_RESERVED_EVENT_ACTION_MESSAGE_KEY_KOREAN			"Kor"
#define GB_RESERVED_EVENT_ACTION_GLOBAL_MESSAGE "GlobalMessage"
#define GB_RESERVED_EVENT_ACTION_KILL "Kill"
#define GB_RESERVED_EVENT_ACTION_KICK "Kick"
#define GB_RESERVED_EVENT_ACTION_KICK_KEY_DAYS "DurationDays"
#define GB_RESERVED_EVENT_ACTION_MESSAGE "Message"
//#define GB_RESERVED_EVENT_ACTION_CHANGETEAM "ChangeTeam"
#define GB_RESERVED_EVENT_ACTION_SEND_SCREENSHOT "SendScreenShot"

#endif
