#ifndef ENCODE_APP
#define ENCODE_APP

#ifndef UTILITY_H
#include "util.h"
#endif

#ifndef CD_INTERFACE
#include "CDInterface.h"
#endif

#ifndef MP3_ENCODER
#include "MP3Encoder.h"
#endif

#ifndef CDDBLOOPER_H
#include "CDDBLooper.h"
#endif

#ifndef CD_WINDOW
#include "CDWindow.h"
#endif

#ifndef SETTINGS_H
#include "Settings.h"
#endif

#ifndef PREF_WINDOW_H
#include "PrefWindow.h"
#endif

#include <Application.h>
#include <fs_index.h>
#include <fs_info.h>

class CDWindow;
class PrefWindow;

class EncoderApp : public BApplication
{
public:
	EncoderApp();
	~EncoderApp();
	void MessageReceived(BMessage *m);
	void ReadyToRun();
	bool QuitRequested();
	
	EncodeSettings *settings;
	PrefWindow *pw;
private:
	void CheckIndexes();
	CDWindow *ourWin;

};

#endif

	