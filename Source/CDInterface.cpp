#ifndef CD_INTERFACE
#include "CDInterface.h"
#endif

CDSession::CDSession (int32 bsize) :
	firstFrame(0), lastFrame(0), currentFrame(0)
{
	encoder = NULL;
	track = -1;
	sessionID = -1;
	buffSize = sizeof(int16) * 2 * CDDA_BUFFER_SIZE * bsize;
	buffFrames = bsize;
	buffer = (uint16 *)malloc (buffSize);
	buffer2 = (uint16 *)malloc (buffSize);
	currentEncodeBlock = NULL;
	next = NULL;
}

CDSession::~CDSession()
{
	free(buffer);
	free(buffer2);
}

CDInterface::CDInterface (const char *dev) :
	BLooper(dev)
{
	sessionList = NULL;
	device = open(dev, 0);
	if (device <= 0) {
		aerror("Error opening CD Device", dev);
	};
	Run();
}

CDInterface::~CDInterface()
{
	close(device);
	while (sessionList != NULL) {
		CDSession *temp = sessionList->next;
		delete (sessionList);
		sessionList = temp;
	};
}

void CDInterface::MessageReceived(BMessage *m)
{
	switch (m->what) {
		case CREATE_SESSION:
			CreateSession(m);
			break;
		case DELETE_SESSION:
			DeleteSession(m);
			break;
		case SEND_TOC:
			SendTOC(m);
			break;
		case SEND_AUDIO_BLOCK:
			SendAudioBlock(m);
			break;
		default:
			BLooper::MessageReceived(m);
	};
}

void CDInterface::CreateSession(BMessage *m)
{
	CDSession *newsession = new CDSession(150);

	newsession->cdView = m->ReturnAddress();
	
	if (m->HasInt32("track")) {
		newsession->track = m->FindInt32("track");
	} else {
		aerror("CDI:", "CDI: create session has no track.");
		return;
	};
	
	if (m->HasPointer("looper")) {
		m->FindPointer("looper", (void **)&newsession->encoder);
	} else {
		aerror("CDI:", "CDI: create session has no looper.");
		return;
	};
	
	newsession->next = sessionList;
	sessionList = newsession;
	
	if (ioctl(device, B_SCSI_GET_TOC, &toc)) {
		aerror("", "I was unable to read the CD properly. If you just put the CD into the player, please wait a few seconds and click Scan CD again.");
		return;
	};
	uint8 *t = toc.toc_data;
	//First we need to find the track entry in the TOC;
	for (uint8 x = 0; x <= t[3] - t[2]; x++) {
		if (t[4 + 8 * x + 2] == newsession->track) {
			newsession->firstFrame = MSF(t[x*8 + 11], t[x*8 + 10], t[x*8 + 9]);
			newsession->lastFrame = MSF(t[(x+1)*8 + 11], t[(x+1)*8 + 10], t[(x+1)*8 + 9]) - 1;
		};
	};
	MSF length = newsession->lastFrame - newsession->firstFrame + 1;
	newsession->numFrames = length.frames();
	newsession->currentFrame = newsession->firstFrame;
	
	newsession->currentBlock = 0;
	newsession->numBlocks = float(newsession->numFrames * (float(CDDA_BUFFER_SIZE) / float(MP3_BUFFER_SIZE)));
	newsession->lastSample = NULL;
}

void CDInterface::DeleteSession(BMessage *m)
{
	if (sessionList == NULL)
		return;

	BLooper *tloop;
	m->FindPointer("looper", (void **)&tloop);
	CDSession *tsession1 = sessionList->next;
	CDSession *tsession2 = sessionList;
	
	if (sessionList->encoder == tloop) {
		delete sessionList;
		sessionList = tsession1;
		return;
	};
	
	while (tsession1) {
		if (tsession1->encoder == tloop) {
			tsession2->next = tsession1->next;
			delete tsession1;
			break;
		};
		tsession2 = tsession1;
		tsession1 = tsession1->next;
	};
}

void CDInterface::SendTOC(BMessage *m)
{
	if (!ioctl(device, B_SCSI_GET_TOC, &toc)) {
	
	BMessenger m1 = m->ReturnAddress();
		BMessage reply(SEND_TOC);
		reply.AddData("TOC", B_RAW_TYPE, &toc, sizeof(scsi_toc));
		m->SendReply(&reply);
	} else {
		aerror ("", "I was unable to read the CD properly. If you just put the CD into the player, please wait a few seconds and click Scan CD again.");
		return;
	};
}

void CDInterface::FillBuffer(CDSession *s)
{
	scsi_read_cd read_info;
//	for (int32 x = 0; x < s->buffSize / 2; x++) {
//		s->buffer[x] = 0;
//	};
	if (s->currentFrame > s->lastFrame) {
		return;
	};
	
	read_info.start_m = s->currentFrame.m();
	read_info.start_s = s->currentFrame.s();
	read_info.start_f = s->currentFrame.f();
	
	s->currentFrame += s->buffFrames;
	
	MSF length(0);
	if (s->currentFrame <= s->lastFrame) {
		length = MSF(s->buffFrames);
	} else {
		MSF temp (read_info.start_f, read_info.start_s, read_info.start_m);
		length = s->lastFrame - temp;
		s->lastSample = s->buffer + (2 * length.frames() * CDDA_BUFFER_SIZE);
	};
	
	read_info.length_m = length.m();
	read_info.length_s = length.s();
	read_info.length_f = length.f();
	
//	printf ("Reading from %d:%d:%d for %d:%d:%d\n", read_info.start_m, read_info.start_s, read_info.start_f, 
//														read_info.length_m, read_info.length_s, read_info.length_f);
	read_info.buffer_length = s->buffSize;
	read_info.buffer = (char *)s->buffer;
	read_info.play = false;

	int32 tries = 0;
	int32 errors;
	do {
		errors = 0;

		read_info.buffer = (char *)s->buffer;
	
		if (ioctl(device, B_SCSI_READ_CD, &read_info) != B_NO_ERROR) {
			aerror("CDDA", "CDDA error reading");
		};
			
		read_info.buffer = (char *)s->buffer2;

		if (ioctl(device, B_SCSI_READ_CD, &read_info) != B_NO_ERROR) {
			aerror("CDDA", "CDDA error reading");
		};
						
				
		for (int i = 0; i < s->buffSize / 2; i++) {
			if (s->buffer[i] != s->buffer2[i]) {
				errors++;
			};
		};
	} while (errors > s->buffFrames * tries++);

	
	s->currentEncodeBlock = s->buffer;
}



void CDInterface::SendAudioBlock(BMessage *m)
{
	BLooper *tloop;
	BMessage *reply = new BMessage(AUDIO_BLOCK);
	uint16 mp3_left_buff[MP3_BUFFER_SIZE];
	uint16 mp3_right_buff[MP3_BUFFER_SIZE];
	m->FindPointer("looper", (void **)&tloop);
	CDSession *session = sessionList;
	while (session) {
		if (session->encoder == tloop)
			break;
		session = session->next;
	};
	
	if (!session) {
		aerror ("SendAudioBlock problem", "Couldn't find session in sessionList.");
		return;
	};

	int32 tBlock;
	if (m->FindInt32("Block", &tBlock) != B_OK) {
		aerror("", "CDInterface could not find Block");
	};
	if (tBlock != (session->currentBlock + 1)) {
		aerror("", "CDInterface: SendBlock request had the wrong Block!");
	};
	
	if ((session->lastSample == NULL) || ((session->lastSample) > (session->currentEncodeBlock))) {
		for (uint x = 0; x < MP3_BUFFER_SIZE; x++) {
			if ((session->currentEncodeBlock == NULL) || 
				(session->currentEncodeBlock >= session->buffer + (session->buffSize / 2))) {
				FillBuffer(session);
			};
			mp3_left_buff[x] = *(session->currentEncodeBlock++);
			mp3_right_buff[x] = *(session->currentEncodeBlock++);
		};
		reply->AddData("Right Buffer", B_RAW_TYPE, mp3_right_buff, sizeof(uint16) * MP3_BUFFER_SIZE);
		reply->AddData("Left Buffer", B_RAW_TYPE, mp3_left_buff, sizeof(uint16) * MP3_BUFFER_SIZE);
	};
	
	reply->AddInt32("Block", ++(session->currentBlock));
//	tloop->Lock();
	if (tloop->PostMessage(reply) != B_OK) {
		aerror ("CDI:", "CDI: couldn't send the reply.");
	};
//	tloop->Unlock();
	delete reply;
	
	// Now we send a status update...
	BMessage *sm = new BMessage(UPDATE_STATUS_BAR);

//	printf ("CDI sayz: %d frames out of %d\n", current.frames(), session->numFrames);
	sm->AddFloat("percent", float(session->currentBlock)/float(session->numBlocks) * 100.0);
	sm->AddInt32("track", session->track);
//	if (session->cdView.LockTarget()) {
		session->cdView.SendMessage(sm);
//		BLooper *junkLooper;
//		session->cdView.Target(&junkLooper);
//		junkLooper->Unlock();
//	};
	delete sm;
}
	
			
	
	