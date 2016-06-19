
/*
	File:	CDDBLooper.h
	Author:	Chip Paul(cpaul@resgen.com)
	Function:
		A looper handling communications to the CDDB Server,
		and loading of cached CDDB entries.
	Incoming Messages:
		CDDB_LOOKUP
			"numTracks", int8,  number of tracks on disk
		CDDB_SITES
			none
	OutGoing Messages:
		CD_INFO
			"cd_info", struct cd_info,  CD track information
		STATUS_MSG
			"status_msg", string, information on current status
		SITE_INFO
			"site", string, internet address of server
			"port", int16, port to connect to
			"latitude", string, latitude of server location
			"longitude", string, longitude of server location
			"description", string, description of physical location
*/

#ifndef CDDBLOOPER_H
#define CDDBLOOPER_H



#include <Looper.h>
#include <Messenger.h>
#include <netdb.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <stdlib.h>
#include <string.h>
#include "InexactMatchWindow.h"
#include <Alert.h>

#include "cddb.h"


// Incoming messages
#define CDDB_LOOKUP 'lkup'
#define CDDB_SITES	'site'
#define CDDB_WRITE	'wrte'
#define CDDB_QUERY	'qury'
#define CDDB_READ	'read'
#define CDDB_SEARCH	'srch'

// Outgoing messages
#define CD_INFO		'info'
#define STATUS_MSG 	'smsg'
#define SITE_INFO	'stsi'


class CDDBLooper : public BLooper {
private:
	// The server to connect to
	char cddbServer[64];
	int32 sock_id;
	int port;
	
	// CD information structure
	cd_info fCDInfo;
	struct toc *cdtoc;
	BMessenger replyTo;
	// Sends a string over the socket
	status_t	SendString(const char *str);
	// Receives a string over the socket
	char *		ReceiveString();
	// Opens the connection
	status_t	OpenCDDBConnection();
	// Closes the connection
	status_t	CloseCDDBConnection();
	// Loads the cached data
	bool LoadCachedData(uint32 discID);
	// Sends the hello command
	void CDDBHello();
	// Sends the quit command
	void CDDBQuit();
	// Performs a query command
	void CDDBQuery(unsigned long discID, int numTracks);
	// Performs a read command
	void CDDBRead(const char *category, const char *discid, const char *realid);
	// Performs a sites command
	void CDDBSites();
	// Performs a write command
	void CDDBWrite();
	// Perfroms a search command
	void CDDBSearch();
	// Peforms a proto command
	void CDDBProto();
	
	// Does Hello, Query, Read, Quit
	void CDDBLookup(int numTracks);
	void SetStatusMessage(const char *msgString);
	
	int cddb_sum(int n);
	unsigned long cddb_discid (int tot_trks);
public:
	CDDBLooper(const char *name, long priority, const char *server, int port);
	~CDDBLooper();
	void	Quit();
	void	MessageReceived(BMessage *message);
	
};

#endif
