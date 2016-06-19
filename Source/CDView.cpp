#ifndef CD_VIEW
#include "CDView.h"
#endif

CDView::CDView(const BRect b, const char *device) :
	BView (b, device, B_FOLLOW_ALL_SIDES, B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE)
{
	settings = ((EncoderApp *)be_app)->settings;
	devicePath = strdup(device);
	numTracks = 0;
	float labelWidth = 40;

	BMessage *tempMsg;
	float preferredWidth;
		


	cddbl = new CDDBLooper("CDDBLooper", B_NORMAL_PRIORITY, settings->CDDBServer, 8880);		
	cdi = new CDInterface(devicePath);
	
	tempMsg = new BMessage(B_PULSE);
	artistTC = new BTextControl(BRect(0,0,b.right,160), "ArtistTC", "Artist:  ", "Unknown", tempMsg, B_FOLLOW_LEFT_RIGHT);
	AddChild(artistTC);
	artistTC->GetPreferredSize(&preferredWidth, &preferredHeight);
	artistTC->ResizeTo(b.right, preferredHeight);
	artistTC->SetDivider(labelWidth);

	tempMsg = new BMessage(B_PULSE);
	titleTC = new BTextControl(BRect(0,preferredHeight + 1,b.right,2*preferredHeight), "TitleTC", "Title: ", "Unknown", tempMsg, B_FOLLOW_LEFT_RIGHT);
	titleTC->SetDivider(labelWidth);
	AddChild(titleTC);
	
	tempMsg = new BMessage(B_PULSE);
	yearTC = new BTextControl(BRect(0,2*preferredHeight + 1, b.right/3, 3*preferredHeight), "YearTC", "Year:", "0", tempMsg);
	yearTC->SetDivider(labelWidth);
	AddChild(yearTC);
	
	genrePUM = new BPopUpMenu("genrePUM");
	
	int32 gCount;
	type_code gType;
	BMenuItem *tempMI;
	const char *gName;
	if (settings->genres.GetInfo("genre", &gType, &gCount) != B_OK) {
		aerror("","Something is wrong with the genre's in CDView!");
	} else {
		for (int32 i = 0; i <= gCount - 1; i++) {
			if (settings->genres.FindString("genre", i, &gName) != B_OK) {
				aerror("","Problem creating genre pop-up menu");
			};
			tempMsg = new BMessage(GENRE_MENU_ITEM);
			tempMI = new BMenuItem(gName, tempMsg);
			genrePUM->AddItem(tempMI);
		};
	};
	genrePUM->ItemAt(settings->defaultGenre)->SetMarked(true);
	genreMF = new BMenuField(BRect((b.right / 3) + 1,2*preferredHeight + 1,2*(b.right/3),3*preferredHeight), "GenreMF", "Genre:", genrePUM);
	genreMF->SetDivider(labelWidth);
	AddChild(genreMF);
	
	bitratePUM = new BPopUpMenu("bitratePUM");
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("32", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("40", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("48", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("56", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("64", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("80", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("96", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("112", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("128", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("160", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("192", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("224", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("256", tempMsg);
	bitratePUM->AddItem(tempMI);
	tempMsg = new BMessage(BITRATE_MENU_ITEM);
	tempMI = new BMenuItem("320", tempMsg);
	bitratePUM->AddItem(tempMI);

	bitratePUM->ItemAt(settings->bitrate)->SetMarked(true);
	
	bitrateMF = new BMenuField(BRect(2*(b.right/3)+1,2*preferredHeight+1,b.right,3*preferredHeight), "bitrateMF", "Bitrate:", bitratePUM);
	bitrateMF->SetDivider(labelWidth);
	AddChild(bitrateMF);
	
	//Set up the status bars
	system_info si;
	get_system_info(&si);
	cpus = si.cpu_count;
//	cpus = 4;	//This is here if you want to pretend you have a different number of CPUs.
	if (cpus > B_MAX_CPU_COUNT)
		cpus = B_MAX_CPU_COUNT;
	char tempString[25];
	for (int i = 1; i <= cpus; i++) {
		encoders[i] = new EncoderInterface();
		sprintf(tempString, "CPU #%d: ", i);
		float bottom = b.bottom - ((cpus - i) * 50);
		statusBars[i] = new BStatusBar(BRect(25,bottom - 50, b.right - 25, bottom), "Status", tempString);
		statusBars[i]->SetText("idle");
		statusBars[i]->SetResizingMode(B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT);
		AddChild(statusBars[i]);
	};
	
	float trackRectBottom = b.bottom - 50 * cpus - 10;

	
	trackCV = new BView (BRect(12,3 * preferredHeight + 10,b.right-25,trackRectBottom - preferredHeight - 1), "Track Listing", B_FOLLOW_ALL_SIDES, 0);

	for (int i = 0; i < 100; i++) {
		songs[i] = new SongInfo(i+1);
	};
	
	trackSV = new BScrollView("Track SV", trackCV, B_FOLLOW_ALL_SIDES, B_WILL_DRAW, false, true);
	BScrollBar *sb = trackSV->ScrollBar(B_VERTICAL);
	
	float stepSmall, stepBig;
	sb->GetSteps(&stepSmall, &stepBig);
	AddChild(trackSV);
	
	tempMsg = new BMessage(SCAN_CD_BUTTON);
	scanB = new BButton(BRect(0,trackRectBottom-preferredHeight,b.right/3,trackRectBottom), "Scan Button", "Scan CD", tempMsg, B_FOLLOW_BOTTOM);
	tempMsg = new BMessage(ENCODE_CD_BUTTON);
	encodeB = new BButton(BRect(b.right/3,trackRectBottom-preferredHeight,2*b.right/3,trackRectBottom), "Encode Button", "Start Encoding", tempMsg, B_FOLLOW_BOTTOM);
	encodeB->SetEnabled(false);
	tempMsg = new BMessage(PREFS_BUTTON);
	prefsB = new BButton(BRect(2*b.right/3,trackRectBottom-preferredHeight,b.right,trackRectBottom), "Prefs Button", "Preferences...", tempMsg, B_FOLLOW_BOTTOM);
	AddChild(scanB);
	AddChild(encodeB);
	AddChild(prefsB);
	
	statusSV = new BStringView(BRect(0,b.bottom - preferredHeight, b.right,b.bottom), "Status", "");
	AddChild(statusSV);
	encoding = false;
}

CDView::~CDView()
{
//	RemoveChild(artistTC);
//	RemoveChild(titleTC);
//	RemoveChild(bitratePUM);
//	BMenuItem *tempMI;
//	for (int32 i = 0; bitratePUM->CountItems();) {
//		printf ("Delete'ing bitrate MI.\n");
//		tempMI = bitratePUM->RemoveItem((int32)0);
//		delete tempMI;
//	};
//	delete artistTC;
//	delete titleTC;
//	delete bitratePUM;
//	be_app->Lock();
//	be_app->PostMessage(B_QUIT_REQUESTED);
//	be_app->Unlock();
}

void CDView::AttachedToWindow()
{
	BScrollBar *sb = trackSV->ScrollBar(B_VERTICAL);
	float preferredHeight;
	float preferredWidth;
	artistTC->GetPreferredSize(&preferredWidth, &preferredHeight);

	sb->SetSteps(4.0, preferredHeight);
	
	scanB->SetTarget(this);
	encodeB->SetTarget(this);
	prefsB->SetTarget(this);
	bitratePUM->SetTargetForItems(this);
	genrePUM->SetTargetForItems(this);
}

void CDView::FrameResized(float, float)
{
	float t = yearTC->Frame().top;
	float h = yearTC->Frame().Height();
	float w = Frame().right;
	
	yearTC->ResizeTo(w/3,h);
	genreMF->ResizeTo(w/3,h);
	bitrateMF->ResizeTo(w/3,h);
	
	genreMF->MoveTo(w/3, t);
	bitrateMF->MoveTo(2*w/3, t);
	
	t = scanB->Frame().top;
	h = scanB->Frame().Height();
	
	scanB->ResizeTo(w/3,h);
	encodeB->ResizeTo(w/3,h);
	prefsB->ResizeTo(w/3,h);
	
	encodeB->MoveTo(w/3,t);
	prefsB->MoveTo(2*w/3,t);
}
	

void CDView::MessageReceived(BMessage *m)
{
	switch (m->what) {
		case UPDATE_GENRES:
			UpdateGenres();
			break;
		case CD_INFO:
			ReceivedCDDB(m);
			break;
		case SEND_TOC:
			ReceivedTOC(m);
			break;
		case SCAN_CD_BUTTON:
			ScanCD();
			break;
		case ENCODE_CD_BUTTON:
			EncodeCD();
			break;
		case PREFS_BUTTON:
			((EncoderApp *)be_app)->pw->Lock();
			((EncoderApp *)be_app)->pw->Show();
			((EncoderApp *)be_app)->pw->Unlock();
			break;
		case STATUS_MSG:
			SetStatus(m);
			break;
		case STREAM_DONE:
			StreamDone(m);
			break;
		case UPDATE_STATUS_BAR:
			UpdateStatusBars(m);
			break;
		case BITRATE_MENU_ITEM:
			settings->bitrate = bitratePUM->IndexOf(bitratePUM->FindMarked());
			break;
		case GENRE_MENU_ITEM:
			settings->defaultGenre = genrePUM->IndexOf(genrePUM->FindMarked());
		default:
			BView::MessageReceived(m);
	};
}

void CDView::SetStatus(BMessage *m)
{
	if (!m->HasString("status_msg")) {
		return;
	} else {
		const char *s;
		m->FindString("status_msg", &s);
		statusSV->SetText(s);
	};
}

void CDView::ScanCD()
{
	BMessage *m = new BMessage(SEND_TOC);
	cdi->PostMessage(m, cdi, this);
}

void CDView::EncodeCD()
{


	if (encoding) {
		return;
	};

	encoding = true;
	nextTrack = 1;
	
	SetSongInfo();
	
	//Setup the Album directory to save in...
	SongInfo *si = songs[nextTrack];
	char *temps = strdup(si->GetArtist());
	char *temps2;
	while (NULL != (temps2 = strchr(temps, '/'))) {
		*temps2 = '_';
	};
	
	BDirectory *d = new BDirectory (settings->saveDirectory.Path());
	
	if (d->InitCheck() != B_OK) {
		char s[B_PATH_NAME_LENGTH + 1024];
		sprintf(s, "The directory you have chosen to save files in, \"%s\", does not exist. \
Please create it, or use the Preferences window to select one that does exist.", settings->saveDirectory.Path());
		
		BAlert *a = new BAlert ("createDir", s, "OK",NULL, NULL,
		B_WIDTH_FROM_LABEL, B_OFFSET_SPACING, B_WARNING_ALERT);
		a->Go();
		encoding = false;
		return;
		
	};
	
	if (!d->Contains(temps, B_DIRECTORY_NODE)) {
		d->CreateDirectory(temps, NULL);
	};
	d->SetTo(d, temps);
	
	free(temps);
	temps = strdup(si->GetAlbum());
	while (NULL != (temps2 = strchr(temps, '/'))) {
		*temps2 = '_';
	};
	
	if (!d->Contains(temps, B_DIRECTORY_NODE)) {
		d->CreateDirectory(temps, NULL);
	};
	d->SetTo(d, temps);

				
	free(temps);	
	for (int i = 1; i <= cpus; i++) {
		if (nextTrack <=  numTracks) {
			EncodeTrack(nextTrack++, i);
		} else {
			return;
		};	
	};

}

void CDView::ReceivedTOC(BMessage *m)
{
	struct toc tdata[100];
	const scsi_toc *t;
	ssize_t s;
	m->FindData("TOC", B_RAW_TYPE, (const void **)&t, &s);
	BMessage *n = new BMessage(CDDB_LOOKUP);
	for (int x = 0; x < 100; x++) {
		tdata[x].min = t->toc_data[9 + 8*x];
		tdata[x].sec = t->toc_data[10 + 8*x];
		tdata[x].frame = t->toc_data[11 + 8*x];
	};
	n->AddData("tocData", B_RAW_TYPE, tdata, 300);
	int8  i = t->toc_data[3] - t->toc_data[2] + 1;
	n->AddInt8("numTracks", i);
	cddbl->PostMessage(n, cddbl, this);
	delete n;
	while (trackCV->CountChildren()) {
		BView *temp = trackCV->ChildAt(0);
		trackCV->RemoveChild(temp);
		delete temp;
	}; 

	
	for (int j = 1; j <= numTracks; j++) {
		delete songs[j];
		songs[j] = NULL;
	};
	
	numTracks = i;
	
	float rightSide = trackCV->Bounds().right;
	for (int j = 0; j < numTracks; j++) {
		char name[32];
//		int tn = cdinfo->track[i].track_num;
		sprintf (name, "Track %d: ", j+1);
		BMessage *tempMsg = new BMessage(B_PULSE);
		BTextControl *tempTC = new BTextControl(BRect(0,preferredHeight*j + 1, rightSide, preferredHeight*(j+1)), "TrackTC", name, "", tempMsg, B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
		tempTC->SetDivider(75);
		trackCV->AddChild(tempTC);
		float scrollMax = preferredHeight*(j+1) - trackCV->Bounds().Height();
		BScrollBar *sb = trackSV->ScrollBar(B_VERTICAL);
		sb->SetRange(0, scrollMax);
		
		SongInfo *ts = new SongInfo(j+1);
		ts->SetTextControl(tempTC);
		songs[j+1] = ts;
	};

	encodeB->SetEnabled(true);
}

void CDView::ReceivedCDDB(BMessage *m)
{
	struct cd_info *cdinfo;
	ssize_t bogus;
	if (m->FindData("cd_info", B_RAW_TYPE, (const void **)&cdinfo, &bogus) != B_OK) {
		return;
	};
	
	artistTC->SetText(cdinfo->disc_artist);
	titleTC->SetText(cdinfo->disc_title);
	
	while (trackCV->CountChildren()) {
		BView *temp = trackCV->ChildAt(0);
		trackCV->RemoveChild(temp);
		delete temp;
	}; 

	for (int i = 1; i <= numTracks; i++) {
		delete songs[i];
		songs[i] = NULL;
	};
	
	numTracks = cdinfo->numtracks;
	
	float rightSide = trackCV->Bounds().right;
	for (int i = 0; i < cdinfo->numtracks; i++) {
		char name[32];
//		int tn = cdinfo->track[i].track_num;
		sprintf (name, "Track %d: ", i+1);
		BMessage *tempMsg = new BMessage(B_PULSE);
		BTextControl *tempTC = new BTextControl(BRect(0,preferredHeight*i + 1, rightSide, preferredHeight*(i+1)), "TrackTC", name, cdinfo->track[i].track_name, tempMsg, B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
		tempTC->SetDivider(75);
		trackCV->AddChild(tempTC);
		float scrollMax = preferredHeight*(i+1) - trackCV->Bounds().Height();
		BScrollBar *sb = trackSV->ScrollBar(B_VERTICAL);
		sb->SetRange(0, scrollMax);
		
		SongInfo *ts = new SongInfo(i+1);
		ts->SetArtist(cdinfo->disc_artist);
		ts->SetAlbum(cdinfo->disc_title);
		ts->SetTitle(cdinfo->track[i].track_name);
		ts->SetTextControl(tempTC);
		ts->SetGenre("rock");
		songs[i+1] = ts;
	};
	


}

void CDView::StreamDone(BMessage *m) 
{
	statusSV->SetText("Encoder is Done.\n");
	

	BMessenger mess = m->ReturnAddress();
	BLooper *lp;
	EncoderInterface *eip = NULL;
	for (int i = 1; i <= numTracks; i++)
	{
		if ((songs[i]->GetEncoder()) == mess.Target(&lp)) {
			BMessage *tm = new BMessage(DELETE_SESSION);
			tm->AddPointer("looper", mess.Target(&lp));
			cdi->Lock();
			cdi->PostMessage(tm);
			cdi->Unlock();
			AddMIMEInfo(songs[i]);
			eip = (EncoderInterface *)lp;
			
			statusBars[songs[i]->GetCPU()]->SetText("idle");

		};
	};
	

	int32 c = 0;
	for (int i = 1; i <= cpus; i++) {
		if (encoders[i] == eip)
			c = i;
	};
	
	if (c == 0) {
		aerror("Couldn't find encoder in StreamDone.", "Could not find encoder in StreamDone.");
		return;
	};
	
	if (encoding && (nextTrack <= numTracks) && eip) {
		EncodeTrack(nextTrack++, c);
	} else {
		encoding = false;
	};
}

void CDView::EncodeTrack(int32 t, int32 cpu)
{
	SongInfo *si = songs[t];
	
	si->SetEncoder(encoders[cpu]);
	si->SetCPU(cpu);
	
	char tempString[32];
	sprintf(tempString, "CPU #%ld: ", cpu);
	statusBars[cpu]->Reset(tempString);
	sprintf(tempString, "Encoding Track %ld", t);
	statusBars[cpu]->SetText(tempString);
	statusBars[cpu]->SetTrailingText("0.0%");
	char *temps = strdup(si->GetArtist());
	char *temps2;
	while (NULL != (temps2 = strchr(temps, '/'))) {
		*temps2 = '_';
	};
	
	BDirectory *d = new BDirectory (settings->saveDirectory.Path());
	if (!d->Contains(temps, B_DIRECTORY_NODE)) {
		d->CreateDirectory(temps, NULL);
	};
	d->SetTo(d, temps);
	
	free(temps);
	temps = strdup(si->GetAlbum());
	while (NULL != (temps2 = strchr(temps, '/'))) {
		*temps2 = '_';
	};
	
	if (!d->Contains(temps, B_DIRECTORY_NODE)) {
		d->CreateDirectory(temps, NULL);
	};
	d->SetTo(d, temps);



	free(temps);
	temps = strdup(si->GetTitle());
	while (NULL != (temps2 = strchr(temps, '/'))) {
		*temps2 = '_';
	};

	
	BMessage *m = new BMessage(CREATE_SESSION);
	m->AddPointer("looper", encoders[cpu]);
	m->AddInt32("track", t);
	cdi->PostMessage(m, cdi, this);
	delete m;

	
	m = new BMessage(NEW_STREAM);
	BEntry en = BEntry(d, temps);
	en.Remove();	//Blow away the file so that we are starting fresh.
	
	
	
	entry_ref ref;
	en.GetRef(&ref);
	si->SetEntry(en);
	
	//Create the template file and add the MIME stuff...
	BFile bf = BFile(&en, B_CREATE_FILE | B_READ_WRITE);
	AddMIMEInfo(si);
	bf.Unset();

	if (t == 1) {
	//Copy from template dir if it was track #1.
		BNode *tn= new BNode(settings->templateDirectory.Path());
		if (tn->InitCheck() == B_OK) {
			struct attr_info atti;
			if (tn->GetAttrInfo("_trk/columns", &atti) == B_OK) {
				void *attr = malloc(atti.size);
				if (tn->ReadAttr("_trk/columns", atti.type, 0, attr, atti.size) == atti.size) {
					d->WriteAttr("_trk/columns", atti.type, 0, attr, atti.size);
				};
				free(attr);
			};
			if (tn->GetAttrInfo("_trk/columns_le", &atti) == B_OK) {
				void *attr = malloc(atti.size);
				if (tn->ReadAttr("_trk/columns_le", atti.type, 0, attr, atti.size) == atti.size) {
					d->WriteAttr("_trk/columns_le", atti.type, 0, attr, atti.size);
				};
				free(attr);
			};
			if (tn->GetAttrInfo("_trk/viewstate", &atti) == B_OK) {
				void *attr = malloc(atti.size);
				if (tn->ReadAttr("_trk/viewstate", atti.type, 0, attr, atti.size) == atti.size) {
					d->WriteAttr("_trk/viewstate", atti.type, 0, attr, atti.size);
				};
				free(attr);
			};
			if (tn->GetAttrInfo("_trk/viewstate_le", &atti) == B_OK) {
				void *attr = malloc(atti.size);
				if (tn->ReadAttr("_trk/viewstate_le", atti.type, 0, attr, atti.size) == atti.size) {
					d->WriteAttr("_trk/viewstate_le", atti.type, 0, attr, atti.size);
				};
				free(attr);
			};
			if (tn->GetAttrInfo("_trk/windframe", &atti) == B_OK) {
				void *attr = malloc(atti.size);
				if (tn->ReadAttr("_trk/windframe", atti.type, 0, attr, atti.size) == atti.size) {
					d->WriteAttr("_trk/windframe", atti.type, 0, attr, atti.size);
				};
				free(attr);
			};

		};
		delete tn;
	};
	
	m->AddRef("output", &ref);
	m->AddPointer("source", cdi);
	m->AddInt32("bitrate", si->GetBitrate());
	m->AddInt32("bitrateIndex", si->GetBitrateIndex());
	encoders[cpu]->PostMessage(m, encoders[cpu], this);
	delete m;
	
	delete d;
}

void CDView::AddMIMEInfo(SongInfo *s)
{
	BEntry e(s->GetEntry());
	BNode n(&e);
	if (n.InitCheck() != B_OK) {
		aerror ("AddMIMEInfo", "AddMIMEInfo could not set BNode");
		return;
	};
	n.WriteAttr("Audio:Artist", B_STRING_TYPE, 0, s->GetArtist(), strlen(s->GetArtist()) + 1);
	n.WriteAttr("Audio:Album", B_STRING_TYPE, 0, s->GetAlbum(), strlen(s->GetAlbum()) + 1);
	n.WriteAttr("Audio:Genre", B_STRING_TYPE, 0, s->GetGenre(), strlen(s->GetGenre()) + 1);
	n.WriteAttr("Audio:Title", B_STRING_TYPE, 0, s->GetTitle(), strlen(s->GetTitle()) + 1);
	uint32 bogus = s->GetTrack();
	n.WriteAttr("Audio:Track", B_INT32_TYPE, 0, &bogus, sizeof(int32));
	bogus = s->GetYear();
	n.WriteAttr("Audio:Year", B_INT32_TYPE, 0, &bogus, sizeof(int32));

	BNodeInfo ni(&n);
	ni.SetType("audio/x-mpeg");
}

void CDView::SetSongInfo()
{
	const char *artist = artistTC->Text();
	const char *album = titleTC->Text();
	uint32 year = atoi(yearTC->Text());
	
	BMenuItem *tempMI = genrePUM->FindMarked();
	const char *genre = tempMI->Label();
	
	tempMI = bitratePUM->FindMarked();
	const char *bitrate = tempMI->Label();
	
	uint32 bitrateIndex = bitratePUM->IndexOf(tempMI) + 1;
	
	for (int i = 1; i <= numTracks; i++) {
		songs[i]->SetArtist(artist);
		songs[i]->SetAlbum(album);
		songs[i]->SetGenre(genre);
		songs[i]->SetTitle(songs[i]->GetTextControl()->Text());
		songs[i]->SetBitrate(atoi(bitrate));
		songs[i]->SetYear(year);
		songs[i]->SetBitrateIndex(bitrateIndex);
	};

}
bool CDView::QuitRequested()
{
	cddbl->PostMessage(B_QUIT_REQUESTED, cddbl);
	cdi->PostMessage(B_QUIT_REQUESTED, cdi);
	return true;
}

void CDView::UpdateStatusBars(BMessage *m)
{
	int32 track;
	float percent, current;
	m->FindInt32("track", &track);
	m->FindFloat("percent", &percent);
	current = statusBars[songs[track]->GetCPU()]->CurrentValue();
//	printf ("Updating status bar from %f to %f\n", current, percent);
	if (percent > current) {
		statusBars[songs[track]->GetCPU()]->Update(percent - current);
		char tempString[5];
		sprintf (tempString, "%2.1f%%", percent);
		statusBars[songs[track]->GetCPU()]->SetTrailingText(tempString);
	};

};

void CDView::UpdateGenres()
{
	int32 gCount;
	type_code gType;
	
	//First, save the selected item, and remove it from the menu
	BMenuItem *selectedMI = genrePUM->FindMarked();
	genrePUM->RemoveItem(selectedMI);
	
	
	BMenuItem *tempMI;
	BMessage *tempMsg;
	//Take everything else out of the menu.
	while ((tempMI = genrePUM->RemoveItem(int32(0))) != NULL) {
		delete tempMI;
	}
	
	const char *gName;
	if (settings->genres.GetInfo("genre", &gType, &gCount) != B_OK) {
		aerror("","Something is wrong with the genre's in CDView!");
	} else {
		for (int32 i = 0; i <= gCount - 1; i++) {
			if (settings->genres.FindString("genre", i, &gName) != B_OK) {
				aerror("","Problem creating genre pop-up menu");
			};
			tempMsg = new BMessage(GENRE_MENU_ITEM);
			if (strcmp(gName, selectedMI->Label())) {
				tempMI = new BMenuItem(gName, tempMsg);
				genrePUM->AddItem(tempMI);
			} else {
				genrePUM->AddItem(selectedMI);
				selectedMI->SetMarked(true);
			};
		};
	};
	if (genrePUM->FindMarked() == NULL) {
		genrePUM->ItemAt(0)->SetMarked(true);
	};
	genrePUM->Invalidate();
}