#ifndef ENCODE_APP
#include "EncoderApp.h"
#endif

EncoderApp::EncoderApp() :
	BApplication(APP_SIGNATURE_LONG)
{

}

EncoderApp::~EncoderApp()
{
	delete settings;
}


void EncoderApp::MessageReceived(BMessage *m)
{
	switch (m->what) {
	case UPDATE_GENRES:
	{
		BMessenger *mes = new  BMessenger(NULL, ourWin);
		BMessage *msg = new BMessage(UPDATE_GENRES);
		mes->SendMessage(msg);
		delete mes;
		delete msg;
		break;
	};
	default:
		BApplication::MessageReceived(m);
	};


}

void EncoderApp::CheckIndexes()
{
	const char *indexes[6] = {"Audio:Album", "Audio:Artist", "Audio:Genre", "Audio:Title",
							"Audio:Track", "Audio:Year"};
	const char *index;
	struct index_info info;
	dev_t device;
	bool allThere;
	int32 pos = 0;
	int32 numIndexes = 6;
	BAlert *ba;
	fs_info fi;
	char displayString[B_FILE_NAME_LENGTH + 512];
	
	while ((device = next_dev(&pos)) >= 0) {
		allThere = true;
		if (fs_stat_dev(device, &fi) != B_OK) {
			aerror ("","Problem with fs_stat_dev...\n");
		};
		for (int32 i = 0; i < numIndexes; i++) {
			index = indexes[i];
			if (fs_stat_index(device, index, &info) == -1) {
				allThere = false;
			};
		};
		
		if (!allThere) {
			uint32 f = fi.flags;
			if ((B_FS_HAS_QUERY & f) && (B_FS_HAS_ATTR & f) && (B_FS_HAS_MIME & f)) {
				sprintf (displayString, "The disk \"%s\" does not appear to have all of the indices for the attributes this program adds to MP3 files it creates.  This can cause\
 problems if you wish to do searches on your MP3 files.  Would you like to add indices for the following attributes now?\n\
 Album\n\
 Artist\n\
 Title\n\
 Genre\n\
 Year\n\
 Track", fi.volume_name);
				
				ba = new BAlert("Index Missing", displayString, "Yes", "No");
				if (ba->Go() == 0) {
					fs_create_index(device, indexes[0], B_STRING_TYPE, 0);
					fs_create_index(device, indexes[1], B_STRING_TYPE, 0);
					fs_create_index(device, indexes[2], B_STRING_TYPE, 0);
					fs_create_index(device, indexes[3], B_STRING_TYPE, 0);
					fs_create_index(device, indexes[4], B_INT32_TYPE, 0);
					fs_create_index(device, indexes[5], B_INT32_TYPE, 0);
				};
			};
		};
	};

	BMimeType mime("audio/x-mpeg");
	
	if (mime.InitCheck() != B_OK) {
		aerror("","Oh shit, InitCheck failed!");
	};

	BMessage m;

	m.AddString("attr:public_name", "Track");
	m.AddString("attr:name", "Audio:Track");
	m.AddInt32("attr:type", B_INT32_TYPE);
	m.AddBool("attr:editable", true);
	m.AddBool("attr:viewable", true);
	m.AddInt32("attr:width", 25);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);

	m.AddString("attr:name", "Audio:Artist");
	m.AddString("attr:public_name", "Artist");
	m.AddInt32("attr:type", B_STRING_TYPE);
	m.AddBool("attr:viewable", true);
	m.AddBool("attr:editable", true);
	m.AddInt32("attr:width", 75);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);
	
	m.AddString("attr:name", "Audio:Album");
	m.AddString("attr:public_name", "Album");
	m.AddInt32("attr:type", B_STRING_TYPE);
	m.AddBool("attr:viewable", true);
	m.AddBool("attr:editable", true);
	m.AddInt32("attr:width", 100);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);
	
	m.AddString("attr:name", "Audio:Genre");
	m.AddString("attr:public_name", "Genre");
	m.AddInt32("attr:type", B_STRING_TYPE);
	m.AddBool("attr:viewable", true);
	m.AddBool("attr:editable", true);
	m.AddInt32("attr:width", 75);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);
	
	m.AddString("attr:name", "Audio:Title");
	m.AddString("attr:public_name", "Title");
	m.AddInt32("attr:type", B_STRING_TYPE);
	m.AddBool("attr:viewable", true);
	m.AddBool("attr:editable", true);
	m.AddInt32("attr:width", 175);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);
	
	m.AddString("attr:name", "Audio:Year");
	m.AddString("attr:public_name", "Year");
	m.AddInt32("attr:type", B_INT32_TYPE);
	m.AddBool("attr:viewable", true);
	m.AddBool("attr:editable", true);
	m.AddInt32("attr:width", 50);
	m.AddInt32("attr:alignment", B_ALIGN_LEFT);
	m.AddBool("attr:extra", true);

	
	if (!mime.IsInstalled()) {
		mime.Install();
		aerror("","MimeType installed!");
	};
	BMessage m2;
	if (mime.GetAttrInfo(&m2) != B_OK) {
		ba = new BAlert("","It appears that the Tracker on this system does not know how to display the attributes that this program\
 adds to the MP3 files it creates.  Would you like me to teach it?", "Yes", "No");
 		if (ba->Go() == 0) {
			if (mime.SetAttrInfo(&m) != B_OK) {
				aerror("a", "Our SetAttrInfo call seems to have failed!");
			};
		
			char *s1="MPEG Layer3 Compressed Audio";
			char *s2="MP3";
			mime.SetLongDescription(s1);
			mime.SetShortDescription(s2);
			mime.Install();

		};
	} else {
		bool changes = false;
		
		for (int32 i=0; m.HasString("attr:name", i); i++) {
			bool found = false;
			for (int32 j=0; m2.HasString("attr:name", j); j++) {
				if (!strcmp(m.FindString("attr:name", i), m2.FindString("attr:name", j))) {
					found = true;
				};
			};
			if (!found) {
				m2.AddString("attr:name", m.FindString("attr:name", i));
				m2.AddString("attr:public_name", m.FindString("attr:public_name", i));
				m2.AddInt32("attr:type", m.FindInt32("attr:type", i));
				m2.AddBool("attr:viewable", m.FindBool("attr:viewable", i));
				m2.AddBool("attr:editable", m.FindBool("attr:editable", i));
				m2.AddInt32("attr:width", m.FindInt32("attr:width", i));
				m2.AddInt32("attr:alignment", m.FindInt32("attr:alignment", i));
				m2.AddBool("attr:extra", true);
				changes = true;
			};
		};
		if (changes) {
			ba = new BAlert("","It appears that the Tracker on this system does not know how to display all of the attributes that this program\
 adds to the MP3 files it creates.  Would you like me to teach it?", "Yes", "No");
 			if (ba->Go() == 0) {
				if (mime.SetAttrInfo(&m2) != B_OK) {
					aerror("a", "Our SetAttrInfo call seems to have failed!");
				};
			};
		};
	};
}

void EncoderApp::ReadyToRun()
{
	settings = new EncodeSettings();
	ourWin = new CDWindow("KonaKoder");
	pw = new PrefWindow();
	pw->Hide();
	pw->Show();
	
	settings->SaveSettings();
	CheckIndexes();
	ourWin->Show();

}

bool EncoderApp::QuitRequested()
{
	pw->PostMessage(B_QUIT_REQUESTED, pw);
	return true;
}
