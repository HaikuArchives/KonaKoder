/*
	File:	CDDBLooper.cpp
	Author:	Chip Paul(cpaul@resgen.com)
	Function:
		A looper handling communications to the CDDB Server,
		and loading of cached CDDB entries.
*/

#include "CDDBLooper.h"
//#include <socket.h>
//
//#include <Preferences.h>
//#include "JukeboxView.h"

// Global preferences object.

static const char *quit_msg = "quit\n";
static const char *cache_dir = "Jukebox";
//static const char *DEFAULT_SERVER = "us.cddb.com";

CDDBLooper::CDDBLooper(const char *name, long priority, const char *server, int server_port) : BLooper(name, priority) {
	strcpy(cddbServer,server);
	port = server_port;
	Run();
}
CDDBLooper::~CDDBLooper() {
}
void CDDBLooper::Quit() {
	BLooper::Quit();
}
// A helper function for computing the disc id.
int CDDBLooper::cddb_sum(int n) {
	char buf[12];
	char *p;
	unsigned long ret = 0;
	
	/* For backward compatibility this algoritm must not change */
	sprintf(buf, "%u", n);
	for (p=buf; *p != '\0'; p++)
		ret += (*p-'0');
	return ret;
}

// Computes the disc id based on the # of tracks and the toc.
unsigned long CDDBLooper::cddb_discid(int tot_trks) {
	int i;
	int n=0;
	int t=0;
	
	/* For backward compatibility this algoritm must not change */
	for (i=0;i<tot_trks;i++) {
		n+=cddb_sum((cdtoc[i].min*60)+cdtoc[i].sec);
		t+=	((cdtoc[i+1].min*60)+cdtoc[i+1].sec) -
			((cdtoc[i].min*60)+cdtoc[i].sec);
	}
	return (((n%0xff) << 24) + (t << 8) + tot_trks);
}

void CDDBLooper::MessageReceived(BMessage *msg) {
	int8 numTracks;
	long discID;
	const char *category;
	const char *strdiscID;
	const char *realDiscID;
	ssize_t bogus;
	switch (msg->what) {
	case CDDB_LOOKUP:
		msg->FindInt8("numTracks",&numTracks);
		msg->FindData("tocData", B_RAW_TYPE, (const void **)&cdtoc, &bogus);
		replyTo = msg->ReturnAddress();
		CDDBLookup(numTracks);
		break;
	case CDDB_SITES:
		replyTo = msg->ReturnAddress();
		CDDBSites();
		break;	
	case CDDB_QUERY:
		msg->FindInt8("numTracks",&numTracks);
		msg->FindInt32("disc_id",&discID);
		CDDBQuery(discID,numTracks);
		break;
	case CDDB_READ:
		msg->FindString("category",&category);
		msg->FindString("disc_id",&strdiscID);
		msg->FindString("real_id",&realDiscID);
		CDDBRead(category,strdiscID,realDiscID);
		break;
	default:
		BLooper::MessageReceived(msg);
		break;
	}
}

// Initializes the socket and connects to an address.
status_t CDDBLooper::OpenCDDBConnection() {
//	int result;
	
	if (cddbServer[0] == 0) {
		return B_CANCELED;
	}
	
	
	// Make sure we can have net connection
	hostent *h;
	h = gethostbyname(cddbServer);
	if (!h) {
		aerror("", "Error resolving hostname a\n");
		return B_IO_ERROR;
	}
	uint32 hostaddr = *(uint32 *)h->h_addr_list[0];
	if (hostaddr==-1) {
		aerror("", "Error resolving hostname b\n");
		return B_IO_ERROR;
	}
	

	sockaddr_in address;
	::memset(&address,0,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = hostaddr;
	
	sock_id = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id < 0) {
		return B_IO_ERROR;
	}
	
	if (::connect(sock_id, (sockaddr *)&address, sizeof(address)) < 0) {
		return B_IO_ERROR;
	}
	
	/*
	// Address to use
	BTSAddress *addr= new BTSAddress(AF_INET,888,cddbServer);
	if (addr==NULL) {
		printf("Could not construct address\n");
		return B_IO_ERROR;	
	}
		
	// Open up the socket
	sock = new BTSSocket(SOCK_STREAM,0);
//	result = sock->Open();
	// Check for errors
	if (sock->ID()<0) {
		printf("Couldn not open socket\n");
	}
	int *a;
	printf("size %d\n",sizeof(a));
	// Connect to server
	result = sock->ConnectToAddress(*addr);
	// Check for errors
	if (result<0) {
		printf("Couldn not Connect socket\n");
		sock->Close();
		return B_IO_ERROR;
	}
	*/
	
	return B_OK;
}

// Closes down the socket connection to the CDDB server
status_t CDDBLooper::CloseCDDBConnection() {
	status_t err=B_OK;
//	if (sock) {
//		err =  sock->Close();
//	}
//	delete sock;
	return err;
}

// Give the hello handshake
void CDDBLooper::CDDBHello() {
	char *response;
	
	// Get the user's login
	char username[256];
	if (!getusername(username,256))
		strcpy(username,"unknown");
	
	// Lookup the user's host name
	char hostname[MAXHOSTNAMELEN];
	hostent *h = NULL;
	if (gethostname(hostname,MAXHOSTNAMELEN)) {
		h=gethostbyname(hostname);
		if (!h)
			strcpy(hostname,"unknown");
		else
			strcpy(hostname,h->h_name);
	} else strcpy(hostname,"unknown");
	delete h;

	// Shake hands
	char helo[256];
	sprintf(helo,"cddb hello %s %s Jukebox v1.0\n",username,hostname);
	SendString(helo);
	response = ReceiveString();
	SetStatusMessage(response);
	delete response;
}

// Send the quit message and closes teh connection.
void CDDBLooper::CDDBQuit() {
	char *response;
	SendString(quit_msg);
	response = ReceiveString();
	CloseCDDBConnection();
}
// Performs a simple query, and handles inexact matches.
void CDDBLooper::CDDBQuery(unsigned long discID, int numTracks) {
	char *response;
	char query[256];
	char temp[32];
	char strCDDBid[9];
	
	// Compute the total length of the CD.
	long cd_length = cdtoc[numTracks].min*60+cdtoc[numTracks].sec;
	// Format the query
	sprintf(query,"cddb query %08lx %d ",discID,numTracks);
	// Add frame offsets
	for (int i=0;i<numTracks;i++) {
		sprintf(temp, "%ld ",cdtoc[i].min*4500+cdtoc[i].sec*75+cdtoc[i].frame);
		strcat(query,temp);
	}
	// Finish it off with the total Cd length.
	sprintf(temp,"%ld\n",cd_length);
	strcat(query,temp);
	// Send it off.
	SendString(query);
	response = ReceiveString();
	SetStatusMessage(response);
	//	printf("CDid is %08x\n",cddb_id);
	
	// Error retrieving cd info
	if (response[0] != '2') {
		CDDBQuit();
		return;
	}
	sprintf(strCDDBid,"%08lx",discID);
	
	char junk[256];
	char category[80];
	// Handling of 211 Inexact Match code
	if (sscanf(response,"211 %s",&junk)) {
		BRect rc(500,500,750,650);
		InexactMatchWindow *matchWindow = new InexactMatchWindow(rc);
		matchWindow->Show();
		BMessage *matchMakerMsg;
		response = ReceiveString();
		while(strcmp(response,".")) {
			matchMakerMsg = new BMessage(ADD_CD);
			matchMakerMsg->AddString("info",response);
			matchMakerMsg->AddString("real_id",strCDDBid);
			matchWindow->PostMessage(matchMakerMsg,NULL,this);
			delete response;
			delete matchMakerMsg;
			response = ReceiveString();
		}
		delete response;
		
	} else {
		// Grab the category from the response
		if (!sscanf(response,"200 %s %s",&category,&junk)) {
			// Hmm, didn't find it.  Try under "misc"
			strcpy(category,"misc");
		}
		CDDBRead(category,strCDDBid,strCDDBid);
	}
}
void CDDBLooper::CDDBRead(const char *category, const char *discid, const char *realid) {
	char *response;
	char query[256];

	// Read the CDDB Info
	sprintf(query,"cddb read %s %s\n",category,discid);
	SendString(query);
	response = ReceiveString();
	SetStatusMessage(response);
	if (!strstr(response,"210")) {
		CDDBQuit();
		return;
	}
	// Get the file ready for writing...
	char cache_path[256];
	BFile *cdFile;
	BDirectory *cacheDir;
	BDirectory *cdDir;
//	char *buffer;
	//char *line;
	// Compute the file name the disc would be in...
	find_directory(B_USER_SETTINGS_DIRECTORY,0,true,cache_path,256);
	cacheDir = new BDirectory(cache_path);
	cdDir = new BDirectory();
	if (cacheDir->InitCheck()!=B_OK) printf("Could not set directory %s\n",cache_path);
	cacheDir->CreateDirectory(cache_dir,cdDir);
	strcat(cache_path,"/");
	strcat(cache_path,cache_dir);
	cdDir->SetTo(cache_path);
	if (cdDir->InitCheck()!=B_OK) printf("Could not set directory %s/Jukebox\n",cache_path);
	
	// Create the file object.
	cdFile = new BFile(cdDir,realid,B_WRITE_ONLY|B_CREATE_FILE);
	
	// Error checking.
	if (cdFile)
		if (!cdFile->IsWritable())  {
			cdFile->Unset();
			cdFile = NULL;
		}
	SetStatusMessage("Writing CD info to cache.");
	
	// Get rid of all that garbage.
//	while (!strstr(response,"DTITLE")) {
//		delete response;
//		response=ReceiveString();
//		if (cdFile) cdFile->Write(response,strlen(response));
//		if (cdFile) cdFile->Write("\n",1);
//	}
//	
//	// Ah, found the title.
//	if (strstr(response,"DTITLE")) {
//		char *parse_str;
//		char *title;
//		char *artist;
//		// Try to parse the title/author via a slash or dash
//		parse_str = strchr(response,'=')+1;
//		
//		artist = strtok(parse_str,"/-");
//		title = strtok(NULL,"/-");
//		if (title) title++;
//		else title = artist;
//		
//		
//		// and store into the global structure (mainly just for the
//		//   DragDocView
//		strcpy(fCDInfo.disc_title,title);
//		strcpy(fCDInfo.disc_artist,artist);
//	}
//	
//	
//	// Read in the track names, and replace all the old untitled menu
//	//   items with the new, titled ones.
//	int i=0;
//	char *tempTrackName;
//	char trackname[256];
//	response = ReceiveString();
//	
//	if (cdFile) cdFile->Write(response,strlen(response));
//	if (cdFile) cdFile->Write("\n",1);
//	strcpy(trackname,"");
//	do {
//		int trackNum;
//		if (!sscanf(response,"TTITLE%d%=%s",&trackNum)) break;
//		tempTrackName=strchr(response,'=')+1;
//		if (trackNum==i) {
//			strcat(trackname," ");
//			strcat(trackname,tempTrackName);
//		} else {
//			// Store track name in global cd_info structure.
//			strcpy(fCDInfo.track[i].track_name,trackname);
//			strcpy(trackname,tempTrackName);
//			i++;
//		}
//		delete response;
//		response = ReceiveString();
//		if (cdFile) cdFile->Write(response,strlen(response));
//		if (cdFile) cdFile->Write("\n",1);
//	} while (!strstr(response,"EXTD"));
//	
//	// Add the last one.
////	strcpy(fCDInfo.track[i].track_name,trackname);
//	// Read the rest of the data, mainly just to put in the cached copy
//	response = ReceiveString();
	while (response[0]!='.') {
		if (cdFile) cdFile->Write(response,strlen(response));
		if (cdFile) cdFile->Write("\n",1);
		delete response;
		response = ReceiveString();
	}
//	BMessage *msg = new BMessage(CD_INFO);
//	msg->AddData("cd_info",B_RAW_TYPE,&fCDInfo,sizeof(cd_info));
//	replyTo.SendMessage(msg);
//	delete msg;

	delete cdFile;
	CDDBQuit();
}
void CDDBLooper::CDDBSites() {
	char *response;
	status_t err;
	
	// Talk to the CDDB server
	err = OpenCDDBConnection();
	if (err != B_OK) {
		(new BAlert("JukeBox","Network Error: Could not open socket.","Sorry"))->Go();
		CloseCDDBConnection();
		return;
	}
	
	// Let 'em know we're connecting
	SetStatusMessage("Connecting to CDDB Server.");
	response = ReceiveString();
	SetStatusMessage(response);
	delete response;
	
	// Handshake with the server
	CDDBHello();

	char query[256];

	// Perform sites query
	sprintf(query,"sites\n");
	SendString(query);
	response = ReceiveString();
	SetStatusMessage(response);
	if (!strstr(response,"210")) {
		CDDBQuit();
		return;
	}
	delete response;
	
	// Read in and parse up the sites
	char *site;
	char *str_port;
	int16 port;
	char *latitude;
	char *longitude;
	char *description;
	
	response = ReceiveString();
	BMessage *msg = new BMessage(SITE_INFO);
	while(strcmp(response,".")) {
		site = strtok(response," ");
		str_port = strtok(NULL," ");
		port = atoi(str_port);
		latitude = strtok(NULL," ");
		longitude = strtok(NULL," ");
		description = strtok(NULL," ");
		
		msg->AddString("site",site);
		msg->AddString("latitude",latitude);
		msg->AddString("longitude",longitude);
		msg->AddString("description",description);
		msg->AddInt16("port",port);
		delete response;
		response = ReceiveString();
	}
	delete response;
	replyTo.SendMessage(msg);
	delete msg;
	
	// Close off the connection
	CDDBQuit();
}
void CDDBLooper::CDDBWrite() {
//	char *response;
}
void CDDBLooper::CDDBSearch() {
//	char *response;
}
void CDDBLooper::CDDBProto() {
//	char *response;
}
void CDDBLooper::SetStatusMessage(const char *msgString) {
//	printf(msgString);
	BMessage *msg = new BMessage(STATUS_MSG);
	msg->AddString("status_msg",msgString);
	replyTo.SendMessage(msg);
	delete msg;
}

void CDDBLooper::CDDBLookup(int numTracks) {

//	uint32 type = '    ';
	bool fCDDB;
//	char strCDDBid[9];
	// Calculate disc id
	unsigned long cddb_id = cddb_discid(numTracks);
	fCDInfo.numtracks = numTracks;
	
	// Try to load it out of cache
	if (LoadCachedData(cddb_id)) return;

	fCDDB = true;
	
	// Don't bother, they don't want networking anyway
	if (!fCDDB) {
		SetStatusMessage("Jukebox by Chip Paul "B_UTF8_COPYRIGHT"1997");
	//	return;
	}
	
	
	
	char *response;
	status_t err;
	
	// Talk to the CDDB server
	err = OpenCDDBConnection();
	if (err == B_CANCELED)
		return;
	else if (err != B_OK) {
		(new BAlert("JukeBox","Network Error: Could not open socket.","Sorry"))->Go();
		CloseCDDBConnection();
		return;
	}
	
	// Let 'em know we're connecting
	SetStatusMessage("Connecting to CDDB Server.");
	response = ReceiveString();
	SetStatusMessage(response);
	delete response;
	
	// Handshake with the server
	CDDBHello();
	CDDBQuery(cddb_id,numTracks);
	LoadCachedData(cddb_id);
}

// Loads a CDDB entry out of the cache
bool CDDBLooper::LoadCachedData(uint32 discid) {
	char cache_path[256];
	char disc_name[9];
	BFile *cdFile;
	BDirectory *cacheDir;
	BDirectory *cdDir;
	char *buffer;
	off_t size;
	char *line;
	
	SetStatusMessage("Checking cache...\n");
	
	// Compute the file name the disc would be in...
	find_directory(B_USER_SETTINGS_DIRECTORY,0,true,cache_path,256);
	cacheDir = new BDirectory(cache_path);
	cdDir = new BDirectory();
	if (cacheDir->InitCheck()!=B_OK) printf("Could not set directory %s\n",cache_path);
	cacheDir->CreateDirectory(cache_dir,cdDir);
	strcat(cache_path,"/");
	strcat(cache_path,cache_dir);
	cdDir->SetTo(cache_path);
	if (cdDir->InitCheck()!=B_OK) printf("Could not set directory %s/Jukebox\n",cache_path);
	// Create the file object.
	sprintf(disc_name,"%08lx",discid);
	cdFile = new BFile(cdDir,disc_name,B_READ_ONLY);
	// Error checking.
	if (!cdFile) return false;
	if (!cdFile->IsReadable()) return false;
	SetStatusMessage("Reading CD info from cache.");
	// Read the buffer
	cdFile->GetSize(&size);
	buffer = new char[size+1];
	char *delete_me = buffer;
	cdFile->Read(buffer,size);
	buffer[size] = '\0';
	
	//Set all of our song titles to ""
	for (int i = 0; i < 100; i++) {
		fCDInfo.track[i].track_name[0] = 0;
	};

	
	char temp_title[1024];
	temp_title[0] = 0;
	// Grab out the stuff...
	line = strtok(buffer,"\n");
	buffer += strlen(line)+1;


	while (line) {
//		printf ("The line is:\n----------------------------\n%s\n-------------------------\n", line);

		// Ah, found the title.
		if (!strncmp(line,"DTITLE",6)) {
//			printf ("Let's go to work on the title.\n");
			line = strchr(line,'=') +1;
			strcat(temp_title, line);
		}

		if (!strncmp(line,"TTITLE",6)) {
		// Read in a track name
//			printf ("Let's go to work on a song title.\n");

			int trackNum;
			if (!sscanf(line,"TTITLE%d=",&trackNum)) break;
//			printf ("Checked track %d.\n", trackNum);
			line=strchr(line,'=')+1;
//			printf ("It is temporarily %s.\n", line);

//			printf ("Copying it into fCDInfo.\n");
			strcat(fCDInfo.track[trackNum].track_name,line);

		};
		line = strtok(NULL,"\n");

//		printf ("Time for the next iteration.\n");
	}
	
	//Now we need to split the DTITLE up into the Title and Artist.
	// Send the info out to the caller
	char *artist;
	char *title;
	artist = strtok(temp_title,"/-");
	title = &temp_title[strlen(artist) + 1];
	
	//Chop any leading spaces
	if (artist) {
		while (*artist ==' ') artist++;
	};
	
	//Chop any trailing space
	if (artist[strlen(artist) - 1] == ' ') { artist[strlen(artist) - 1] = 0;};

	if (title) {
		while (*title ==' ') title++;
	} else title = artist;

	strcpy(fCDInfo.disc_title,title);
	strcpy(fCDInfo.disc_artist,artist);	
	
	BMessage *msg = new BMessage(CD_INFO);
	msg->AddData("cd_info",B_RAW_TYPE,&fCDInfo,sizeof(cd_info));
	replyTo.SendMessage(msg);
	delete msg;
	
	// Clean up
	delete cacheDir;
	delete cdDir;
	delete cdFile;
	delete delete_me;
	return true;
}
// Sends a string over the socket
status_t CDDBLooper::SendString(const char *str) {
//	if (!sock)
//		return B_ERROR;
//	sock->Send(str,strlen(str));
	send(sock_id, str, strlen(str), 0);
	return B_OK;
}

// Reads a string from the socket
char *CDDBLooper::ReceiveString() {
	int buffer_size = 1024;
	char *return_str;
	char *buffer= new char[buffer_size];
	int recd_bytes=-1;
	int num_bytes = 0;
	while (recd_bytes != 0 && num_bytes<buffer_size-1 && buffer[num_bytes-1]!='\n') {
		recd_bytes = recv(sock_id,buffer+num_bytes,1,0);
		num_bytes += recd_bytes;
	}
	buffer[num_bytes-2] = '\0';
	return_str = new char[num_bytes];
	strcpy(return_str,buffer);
	delete[] buffer;
	return return_str;
}
