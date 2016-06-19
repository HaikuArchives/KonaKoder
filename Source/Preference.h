/*

	PrefClient library
	
*/

#ifndef _PREFERENCE_H
#define _PREFERENCE_H

#include "Message.h"
#include "Window.h"
#include <string.h>

class Preference : public BMessage
{
	private:
				enum {
					USEPREF ='use ',
					SAVEPREF='save',
					LOADPREF='load',
					PREF    ='pref'
				};
	
				static char servername[];
				char *target;
				BMessenger postman;
				void PrepareTransfer();
				void FinishTransfer();

	public:		Preference(char *ident=NULL);
		virtual ~Preference();
		status_t	Use();
		status_t	Save();
		status_t	Load();
};

void FitRectOnScreen(BRect *rect,window_look wl=B_TITLED_WINDOW_LOOK);


#endif
