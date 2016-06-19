#ifndef CD_VIEW
#define CD_VIEW

#ifndef SONGINFO_H
#include "SongInfo.h"
#endif

#include <View.h>
#include <Button.h>
#include <StatusBar.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <NodeInfo.h>
#include <fs_attr.h>
#include "util.h"
#include "MP3Encoder.h"
#include "CDInterface.h"
#include "CDDBLooper.h"
#include "EncoderApp.h"
#include "Settings.h"
#include "PrefWindow.h"

class CDView : public BView
{
public:
	CDView(const BRect b, const char *device);
	~CDView();
	
	void	AttachedToWindow();
	void	MessageReceived(BMessage *msg);
	void	FrameResized(float width, float height);
	
	bool	QuitRequested();	//Not an inherited function, but serves the
								//same purpose as BLooper::QuitRequested()

private:
	EncodeSettings	*settings;
	BStringView		*statusSV;
	BTextControl 	*artistTC;
	BTextControl	*titleTC;
	BTextControl	*yearTC;
	BMenuField		*genreMF;
	BPopUpMenu		*genrePUM;
	BMenuField		*bitrateMF;
	BPopUpMenu		*bitratePUM;
	BView			*trackCV;
	BScrollView		*trackSV;
	char 			*devicePath;
	BButton			*scanB;
	BButton			*encodeB;
	BButton			*prefsB;
	
	CDDBLooper		*cddbl;
	CDInterface		*cdi;
	
	float			preferredHeight;
	bool		encoding;
	int32		nextTrack;
	int32		numTracks;
	int32		cpus;
	EncoderInterface	*encoders[B_MAX_CPU_COUNT + 1];
	BStatusBar			*statusBars[B_MAX_CPU_COUNT + 1];
	SongInfo *songs[100];
	
	void	AddMIMEInfo(SongInfo *s);
	void	SetStatus(BMessage *msg);
	void	ScanCD();
	void	EncodeCD();
	void	ReceivedTOC(BMessage *msg);
	void	ReceivedCDDB(BMessage *msg);
	void	StreamDone(BMessage *msg);	
	void 	EncodeTrack(int32 t, int32 cpu);
	void	SetSongInfo();
	void	UpdateStatusBars(BMessage *m);
	void	UpdateGenres();
};

#endif