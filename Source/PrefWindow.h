#ifndef PREF_WINDOW_H
#define PREF_WINDOW_H

#include <FilePanel.h>
#include <Box.h>

#include "Settings.h"
#include "EncoderApp.h"

int SortFunc(const void * a, const void * b);
bool DeleteFunc(BListItem *a);

class PrefWindow : public BWindow
{
public:
	PrefWindow();
	~PrefWindow();
	
	void MessageReceived (BMessage *m);
private:
	EncodeSettings *settings;
	
	BTextControl 	*saveDirTC;
	BButton			*saveDirB;
	BFilePanel		*saveDirFP;
	
	BTextControl	*templateDirTC;
	BButton			*templateDirB;
	BFilePanel		*templateDirFP;
	
	BTextControl	*CDDBServerTC;
	
	BListView		*genreLV;
	BScrollView		*genreSV;
	BTextControl	*genreTC;
	BButton			*genreAddB;
	BButton			*genreDelB;
	
	BButton			*genreRevertB;
	BButton			*genreOKB;
	BButton			*genreCancelB;
	
	void			SavePrefs();
	void			RevertPrefs();
};

#endif