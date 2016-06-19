#ifndef CD_INTERFACE
#define CD_INTERFACE

#ifndef MSF_CLASS
#include "MSF.h"
#endif

#include <Looper.h>
#include <Message.h>
#include <scsi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"

class CDSession 
{
public:
	CDSession(int32 buffSize = 128);
	~CDSession();
	
	BLooper			*encoder;
	BMessenger		cdView;
	MSF				firstFrame;
	MSF				lastFrame;
	MSF				currentFrame;			// frame offset that we should read the next
											// buffer from.
	uint16			*currentEncodeBlock;	// pointer into buffer indicating where
											// the next mp3 buffer should start.
	uint16			*lastSample;			// either NULL, or a pointer into buffer indicating the
											// last sample of the track.
	int32			track;
	int32			sessionID;
	uint16			*buffer;
	uint16			*buffer2;
	int32			buffSize;
	int32			buffFrames;
	int32			numFrames;
	int32			numBlocks;			//number of MP3 blocks;
	int32			currentBlock;		//current MP3 block;
	
	CDSession	*next;
};

class CDInterface : public BLooper
{
public:
	CDInterface(const char *dev);
	~CDInterface();
	
	void MessageReceived(BMessage *msg);

private:
	scsi_toc		toc;
	scsi_read_cd	read_cd;
	int32			device;
	CDSession		*sessionList;
	void	FillBuffer(CDSession *s);
	void	CreateSession(BMessage *m);
	void	DeleteSession(BMessage *m);
	void	SendTOC(BMessage *m);
	void	SendAudioBlock(BMessage *m);
};

#endif
