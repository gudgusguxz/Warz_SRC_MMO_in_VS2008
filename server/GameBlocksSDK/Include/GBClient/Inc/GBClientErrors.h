//=============================================================================
// GBClient.h
// Copyright 2013 GameBlocks LLC, All Rights Reserved.
//=============================================================================

#ifndef GBCLIENT_ERRORS_H_
#define GBCLIENT_ERRORS_H_

namespace GameBlocks
{

// detailed connection status query result
enum EGBConnectStatus
{
	GB_CONNECT_STATUS_DISCONNECTED = 0, // connection state starts as disconnected, and returns to disconnected when explicitly closed
	GB_CONNECT_STATUS_URL_RESOLVE_IN_PROGRESS,
	GB_CONNECT_STATUS_SOCKET_CONNECTION_IN_PROGRESS,
	GB_CONNECTION_STATUS_CONNECTED,
	GB_CONNECTION_STATUS_CONNECTION_CLOSED_SEND_FAIL,
	GB_CONNECTION_STATUS_CONNECTION_LOST // connection failed.  the client will try to reconnect via a timer when in this state
};

/// enum EGBClientError
/// This is the error code type returned by the Client interface.
enum EGBClientError
{
	GBCLIENT_ERROR_OK = 0,
	GBCLIENT_ERROR = 1, /// general error.
	GBCLIENT_ERROR_DISABLED = 2, /// feature is disabled, either due to entire client being disabled or component disabled.
	GBCLIENT_ERROR_NOT_CONNECTED = 3,
	GBCLIENT_ERROR_SOURCE_NOT_FOUND = 4,
	GBCLIENT_ERROR_NO_CONFIG_LOADED = 5,
	GBCLIENT_ERROR_OUT_OF_MEMORY = 6,
	GBCLIENT_ERROR_SOCKET_CONNECT_FAILED = 7,
	GBCLIENT_ERROR_NO_INCOMING_EVENTS = 8,
	GBCLIENT_ERROR_MESSAGE_SIZE_EXCEEDED = 9,
	GBCLIENT_ERROR_VALUE_SIZE_EXCEEDED = 10,
	GBCLIENT_ERROR_INVALID_EVENT = 11,
	GBCLIENT_ERROR_INVALID_ID = 12,
	GBCLIENT_ERROR_INVALID_KEY = 13,
	GBCLIENT_ERROR_INVALID_PARAM = 14,
	GBCLIENT_ERROR_FILE_NOT_FOUND = 15,
	GBCLIENT_ERROR_MALFORMED_EVENT = 16,
	GBCLIENT_ERROR_INVALID_INDEX = 17,
	GBCLIENT_ERROR_BUFFER_SIZE_EXCEEDED = 18,
	GBCLIENT_ERROR_CONNECTION_SEND_FAILED = 19,
	GBCLIENT_ERROR_INVALID_SOURCE_ID = 20,
	GBCLIENT_ERROR_PAIR_NAME_NOT_FOUND = 21
};

}; // gameblocks

#endif // GBCLIENT_ERRORS_H_