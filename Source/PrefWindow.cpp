#ifndef PREF_WINDOW_H
#include "PrefWindow.h"
#endif

int SortFunc(const void * a, const void * b)
{
	BStringItem **s1, **s2;
	s1 = (BStringItem **)a;
	s2 = (BStringItem **)b;
	return strcmp((*s1)->Text(), (*s2)->Text());
}

bool DeleteFunc(BListItem *a)
{
	BStringItem *b = (BStringItem *) a;
	delete b;
	return true;
}

PrefWindow::PrefWindow()
	: BWindow(BRect(50,50,400,360), "Preferences", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE)
{
	BMessage *tempMsg;
	float ph = 25;
	

	
	settings = ((EncoderApp *)be_app)->settings;
	tempMsg = new BMessage(B_PULSE);
	saveDirTC = new BTextControl (BRect(0,5,300,ph), "SaveDirTC", "Directory to save songs in: ", "", tempMsg);
	AddChild(saveDirTC);
	
	tempMsg = new BMessage(SAVE_BROWSE_BUTTON);
	saveDirB = new BButton (BRect(305,0,350,ph), "SaveDirB", "Browse...", tempMsg);
	AddChild(saveDirB);
	saveDirB->SetTarget(this);
	
	tempMsg = new BMessage(NEW_SAVE_DIR);
	saveDirFP = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_DIRECTORY_NODE, false, tempMsg, NULL, false, true);

	saveDirFP->SetTarget(this);
	saveDirFP->SetButtonLabel(B_DEFAULT_BUTTON, "Select");
	
	tempMsg = new BMessage(B_PULSE);
	templateDirTC = new BTextControl (BRect(0,ph+5,300,2*ph), "TemplateDirTC", "Template for Tracker layout: ", "", tempMsg);
	AddChild(templateDirTC);
	
	tempMsg = new BMessage(TEMPLATE_BROWSE_BUTTON);
	templateDirB = new BButton (BRect(305,ph+2,350,2*ph), "TemplateDirB", "Browse...", tempMsg);
	AddChild(templateDirB);
	templateDirB->SetTarget(this);

	tempMsg = new BMessage(NEW_TEMPLATE_DIR);
	templateDirFP = new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_DIRECTORY_NODE, false, tempMsg, NULL, false, true);
	templateDirFP->SetTarget(this);
	templateDirFP->SetButtonLabel(B_DEFAULT_BUTTON, "Select");
	
	tempMsg = new BMessage(B_PULSE);
	CDDBServerTC = new BTextControl(BRect(0,2*ph+2,350,3*ph), "CDDBServerTC", "CDDB Server: ", "", tempMsg);
	AddChild(CDDBServerTC);
	
	BBox *gb = new BBox(BRect(3,3*ph+2, 350, 11*ph), "GenreBB");
	gb->SetLabel("Edit Genres");
	gb->SetViewColor(255,255,255);
	AddChild(gb);
	
	tempMsg = new BMessage(GENRE_SEL_CHANGED);
	genreLV = new BListView(BRect(6,ph/2,325,5*ph), "GenreLV");
	genreLV->SetSelectionMessage(tempMsg);
	genreLV->SetTarget(this);
	
	genreSV = new BScrollView("GenreSV", genreLV, B_FOLLOW_NONE, B_WILL_DRAW, false, true, B_FANCY_BORDER);
//	gb->AddChild(genreSV);
	BBox *jb = new BBox(genreSV->Frame(), "JunkBox");
	genreSV->MoveTo(0,0);
	jb->SetViewColor(255,255,255);
	jb->AddChild(genreSV);	
	gb->AddChild(jb);
		
	genreLV->SortItems(SortFunc);
	
	genreTC = new BTextControl(BRect(4,5*ph + 10,300,6*ph), "GenreTC", "Genre To Add: ", NULL, NULL);
	gb->AddChild(genreTC);
	
	tempMsg = new BMessage(GENRE_ADD_BUTTON);
	genreAddB = new BButton(BRect(301,5*ph+5,345,6*ph), "GenreAddB", "Add It!", tempMsg);
	genreAddB->SetTarget(this);
	gb->AddChild(genreAddB);
	
	tempMsg = new BMessage(GENRE_DEL_BUTTON);
	genreDelB = new BButton(BRect(4,6*ph +6,345,7*ph), "GenreDelB", "Remove Selected Genre", tempMsg);
	genreDelB->SetEnabled(false);
	gb->AddChild(genreDelB);
	
	tempMsg = new BMessage(PREF_REVERT);
	genreRevertB = new BButton(BRect(0,11*ph+2,115,12*ph), "GenreRevertB", "Revert", tempMsg);
	AddChild(genreRevertB);
	
	tempMsg = new BMessage(PREF_CANCEL);
	genreCancelB = new BButton(BRect(116,11*ph + 2,230,12*ph), "GenreCancelB", "Cancel", tempMsg);
	AddChild(genreCancelB);
	
	tempMsg = new BMessage(PREF_OK);
	genreOKB = new BButton(BRect(231,11*ph + 2,350,12*ph), "GenreOKB", "OK", tempMsg);
	AddChild(genreOKB);
	
	//This will set all of the controls to reflect the current settings.
	RevertPrefs();
	
}

PrefWindow::~PrefWindow()
{
}


void PrefWindow::MessageReceived(BMessage *m)
{
	switch (m->what) {
	case SAVE_BROWSE_BUTTON:
		saveDirFP->Show();
		break;
	case TEMPLATE_BROWSE_BUTTON:
		templateDirFP->Show();
		break;
	case NEW_SAVE_DIR:
		{
		entry_ref r;
		if (m->FindRef("refs", &r) == B_OK) {
			BEntry e(&r, true);
			saveDirTC->SetText(BPath(&e).Path());
		};
		break;
		};
	case NEW_TEMPLATE_DIR:
		{
		entry_ref r;
		if (m->FindRef("refs", &r) == B_OK) {
			BEntry e(&r, true);
			templateDirTC->SetText(BPath(&e).Path());
		};
		break;
		};
	case GENRE_ADD_BUTTON:
		{
		const char *t, *t2;
		t = genreTC->Text();
		if (strlen(t) == 0)
			break;
		 
		BStringItem *tempSI;
		int32 i = 0;
		while ((tempSI = (BStringItem *)genreLV->ItemAt(i++)) != NULL) {
			t2 = tempSI->Text();
			if (!strcmp(t, t2)) 
				return;
		};
		tempSI = new BStringItem(genreTC->Text());
		genreLV->AddItem(tempSI);
		genreLV->SortItems(SortFunc);
		genreTC->SetText("");
		
		break;
		};
	case GENRE_DEL_BUTTON:
		genreLV->RemoveItem(genreLV->CurrentSelection());
		break;
	case GENRE_SEL_CHANGED:
		genreDelB->SetEnabled(genreLV->CurrentSelection() >= 0);
		break;
	case PREF_OK:
		SavePrefs();
		Hide();
		break;
	case PREF_CANCEL:
		RevertPrefs();
		Hide();
		break;
	case PREF_REVERT:
		RevertPrefs();
		break;
	};
}

void PrefWindow::SavePrefs()
{
	settings->saveDirectory.SetTo(saveDirTC->Text());
	settings->templateDirectory.SetTo(templateDirTC->Text());
	
	free (settings->CDDBServer);
	settings->CDDBServer = strdup(CDDBServerTC->Text());
	
	BMessage tm;
	BStringItem *tempSI;
	int32 i = 0;
	while ((tempSI = (BStringItem *)genreLV->ItemAt(i++)) != NULL) {
		tm.AddString("genre", tempSI->Text());
	};
	
	settings->genres = tm;
	
	//Now we just need to tell the CDView to update the genrePUM.
	EncoderApp *ea = (EncoderApp *)be_app;
	
	BMessenger *m = new BMessenger(NULL, ea);
	BMessage *msg = new BMessage(UPDATE_GENRES);
	m->SendMessage(msg);
	delete m;
	delete msg;
}

void PrefWindow::RevertPrefs()
{
	entry_ref r;
	
	saveDirTC->SetText(settings->saveDirectory.Path());
	
	if (get_ref_for_path(settings->saveDirectory.Path(), &r) == B_OK) {
		saveDirFP->SetPanelDirectory(&r);
	};

	templateDirTC->SetText(settings->templateDirectory.Path());

	if (get_ref_for_path(settings->templateDirectory.Path(), &r) == B_OK) {
		templateDirFP->SetPanelDirectory(&r);
	};

	CDDBServerTC->SetText(settings->CDDBServer);
	
	genreLV->DoForEach(DeleteFunc);
	genreLV->MakeEmpty();
	
	int32 gCount;
	type_code gType;
	BStringItem *tempSI;
	const char *gName;
	if (settings->genres.GetInfo("genre", &gType, &gCount) != B_OK) {
		aerror("","Something is wrong with the genre's in PrefWindow!");
	} else {
		for (int32 i = 0; i <= gCount - 1; i++) {
			if (settings->genres.FindString("genre", i, &gName) != B_OK) {
				aerror("","Problem creating genre pop-up menu");
			};
			tempSI = new BStringItem(gName);
			genreLV->AddItem(tempSI);
		};
	};
	genreLV->SortItems(SortFunc);

}
