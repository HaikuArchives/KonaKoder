#ifndef CD_WINDOW
#define CD_WINDOW

#ifndef CD_VIEW
#include "CDView.h"
#endif

#include <Window.h>
#include "EncoderApp.h"
//#include "MWindow.h"

class CDView;

class CDWindow : public BWindow
{
public:
	CDWindow(const char *name);
	~CDWindow();
	
	void MessageReceived(BMessage *m);
	bool QuitRequested();
private:
	CDView *ourView;
	static const BRect defaultRect;
	static const char *devPath;

};


#endif