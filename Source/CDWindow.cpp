#ifndef CD_WINDOW
#include "CDWindow.h"
#endif

//const BRect CDWindow::defaultRect = BRect(40,40,440,440);
//const char *CDWindow::devPath = "/dev/disk/scsi/0/5/0/raw";
CDWindow::CDWindow(const char *name) :
	BWindow (((EncoderApp *)be_app)->settings->windowLocation, name, B_DOCUMENT_WINDOW, 0)
{
	EncoderApp *my_app = (EncoderApp *)be_app;
	ourView = new CDView (Bounds(), my_app->settings->CDDevice.Path());
	AddChild(ourView);
}

CDWindow::~CDWindow()
{
	RemoveChild(ourView);
	delete ourView;
}

void CDWindow::MessageReceived(BMessage *m)
{
	switch (m->what) {
	case UPDATE_GENRES:
	{
		BMessenger *mes = new BMessenger(ourView);
		BMessage *msg = new BMessage(UPDATE_GENRES);
		mes->SendMessage(msg);
		delete mes;
		delete msg;
		break;
	};	
	default:
		BWindow::MessageReceived(m);
	};
}

bool CDWindow::QuitRequested()
{
	((EncoderApp *)be_app)->settings->windowLocation = Frame();
	((EncoderApp *)be_app)->settings->SaveSettings();
	ourView->QuitRequested();
	be_app->PostMessage(B_QUIT_REQUESTED, be_app);
	return true;
}
