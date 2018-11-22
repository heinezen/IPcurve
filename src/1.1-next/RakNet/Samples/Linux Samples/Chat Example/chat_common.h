#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H 1

// Commonalities between client/server, e.g.
// shared data structures and enumerations

// Example host-data that describes the instance of a particular
// server; this structure is user-defined and optional.
// However the client and server must share the exact
// same structure to be compatible.
struct StaticHostDataStruct
{
	enum
	{
		MAX_MOTD_LENGTH = 50
	,	MAX_SERVER_NAME_LENGTH = 40
	} ;

	// Message of the day (options), replace with whatever
	char MOTD[MAX_MOTD_LENGTH+1] ;
	char serverName[MAX_SERVER_NAME_LENGTH+1] ;
} ;

// Example client-data that describes the player on a
// particular connection; this structure is user-defined
// and optional.
// However, both the client and server must share the exact
// same definitions.

struct StaticClientDataStruct
{
	enum { MAX_NAME_LENGTH = 40 } ;

	char name[MAX_NAME_LENGTH + 1] ;
} ;

#endif // CHAT_COMMON_H
