#ifndef SETTINGS_H
#include "Settings.h"
#endif

EncodeSettings::EncodeSettings()
{
	GetSettings();
}

EncodeSettings::~EncodeSettings()
{
}

void EncodeSettings::SaveSettings()
{
	if (prefs.HasString("device")) {
		prefs.ReplaceString("device", CDDevice.Path());
	} else {
		prefs.AddString("device", CDDevice.Path());
	};
	
	if (prefs.HasRect("window")) {
		prefs.ReplaceRect("window", windowLocation);
	} else {
		prefs.AddRect("window", windowLocation);
	};
	
	if (prefs.HasInt32("bitrate")) {
		prefs.ReplaceInt32("bitrate", bitrate);
	} else {
		prefs.AddInt32("bitrate", bitrate);
	};
	
	if (prefs.HasString("savedir")) {
		prefs.ReplaceString("savedir", saveDirectory.Path());
	} else {
		prefs.AddString("savedir", saveDirectory.Path());
	};
	
	if (prefs.HasString("templatedir")) {
		prefs.ReplaceString("templatedir", templateDirectory.Path());
	} else {
		prefs.AddString("templatedir", templateDirectory.Path());
	};

	ssize_t msize = genres.FlattenedSize();
	char *data = (char *)malloc(msize);
	if (genres.Flatten(data, msize) != B_OK) {
		aerror("","Unable to flatten the Genres so I could save them.");
		return;
	};
	
	if (prefs.HasData("genres", B_MESSAGE_TYPE)) {
		prefs.ReplaceData("genres", B_MESSAGE_TYPE, data, msize);
	} else {
		prefs.AddData("genres", B_MESSAGE_TYPE, data, msize);
	};
	
	if (prefs.HasInt32("defaultgenre")) {
		prefs.ReplaceInt32("defaultgenre", defaultGenre);
	} else {
		prefs.AddInt32("defaultgenre", defaultGenre);
	};
	
	if (prefs.HasString("CDDBServer")) {
		prefs.ReplaceString("CDDBServer", CDDBServer);
	} else {
		prefs.AddString("CDDBServer", CDDBServer);
	};
	
	prefs.Save();
}

void EncodeSettings::GetSettings()
{
	prefs.Load();
	
	
	InitDevice();
	InitWindow();
	InitCDDBServer();
	InitBitrate();
	InitSaveDir();
	InitTemplateDir();
	InitGenres();
	InitDefaultGenre();
}

void EncodeSettings::InitDevice()
{
	status_t 	status;
	const char 	*data;
	
	status = prefs.FindString("device", &data);
	
	if (status != B_OK) {
		aerror("", "No CD player in your preferences.  I'm going to find one on my own.");
		CDDevice = FindCD("/dev/disk");
		return;
	};
	
	CDDevice = BPath(data);
	if (CDDevice.InitCheck() != B_OK) {
		aerror("", "Prefs gave us back a bogus CD Player.  Looking for another.");
		CDDevice = FindCD("/dev/disk");
		return;
	}
	
	
	int32 devfd = open(data, O_RDONLY);
	device_geometry g; 
	if(ioctl(devfd, B_GET_GEOMETRY, &g, sizeof(g)) >= 0) {
				if(g.device_type == B_CD)
				{ 
					return;
				}
			}
	close(devfd);
	aerror("", "The device is your preferences is no longer a CD Player.  Finding another.");
	CDDevice = FindCD("/dev/disk");
	return;
}

void EncodeSettings::InitWindow()
{
	status_t 	status;
	BRect 	data;
	windowLocation = BRect(40,40,440,440);
	
	status = prefs.FindRect("window", &data);
	if (status != B_OK) {
		return;
	};
	
	if (!(data.IsValid())) {
		return;
	};
	
	windowLocation = data;
	return;
}

void EncodeSettings::InitCDDBServer()
{
//	printf ("Initing CDDBServer...\n");
	status_t 	status;
	const char	*data;
	CDDBServer = strdup("us.cddb.com");
//	CDDBServer = strdup(""); //Don't point to CDDB(TM)'s server by default.
		
	status = prefs.FindString("CDDBServer", &data);
	
	if (status != B_OK) {
		aerror ("", "No CDDB Server in your Preferences. Please select one if you wish to use CDDB.\n");
		return;
	};
	
	CDDBServer = strdup(data);
	return;
}
void EncodeSettings::InitBitrate()
{
	status_t 	status;
	int32 	data;
	
	bitrate = 10; //192Kbps
	status = prefs.FindInt32("bitrate", &data);
	if (status != B_OK) {
		aerror("No default bitrate in your preferences.  I'm setting it to 192","");
		return;
	};
	
	if ((data < 0) || (data > 14)) {
		aerror("The default bitrate in your preferences is invalid. I'm setting it to 192", "");
		return;
	};
	bitrate = data;
}

void EncodeSettings::InitSaveDir()
{
	status_t 	status;
	const char 	*data;
	
	status = prefs.FindString("savedir", &data);
	
	if (status != B_OK) {
		aerror("", "I couldn't find a default directory to save songs in, so I'm setting it to /boot/home/Music");
		find_directory(B_USER_DIRECTORY, &saveDirectory);
		saveDirectory.Append("Music");
		return;
	};
	saveDirectory = BPath(data);
	return;
}

void EncodeSettings::InitTemplateDir()
{
	status_t 	status;
	const char 	*data;
	
	status = prefs.FindString("templatedir", &data);

	if (status != B_OK) {
		aerror("", "I couldn't find a default template directory, so I'm setting it to /boot/home/Music/Template");
		find_directory(B_USER_DIRECTORY, &templateDirectory);
		templateDirectory.Append("Music/Template");
		return;
	};
	templateDirectory = BPath(data);
	return;
}


void EncodeSettings::InitGenres()
{
	status_t 	status;
	ssize_t 	size;
	const char 	*data;
	
	status = prefs.FindData("genres",B_MESSAGE_TYPE, (const void **)&data, &size);
	
	if (status != B_OK) {
		BuildDefaultGenreList();
		return;
	};
	
	BMessage tm;
	if (tm.Unflatten(data) != B_OK) {
		BuildDefaultGenreList();
		return;
	};
	
	if (tm.HasString("genre") == false) {
		BuildDefaultGenreList();
		return;
	};
	
	genres = tm;
	return;
}

void EncodeSettings::BuildDefaultGenreList()
{
	aerror("","I could not find your genre list.  I'm creating a new one.");
	BMessage tm;
	tm.AddString("genre", "Blues");
	tm.AddString("genre", "Classical");
	tm.AddString("genre", "Country");
	tm.AddString("genre", "Dance");
	tm.AddString("genre", "Disco");
	tm.AddString("genre", "Ethnic");
	tm.AddString("genre", "Folk");
	tm.AddString("genre", "Funk");
	tm.AddString("genre", "HipHop");
	tm.AddString("genre", "Jazz");
	tm.AddString("genre", "R & B");
	tm.AddString("genre", "Reggae");
	tm.AddString("genre", "Rock");
	tm.AddString("genre", "Ska");
	tm.AddString("genre", "Spoken Word");
	tm.AddString("genre", "Techno");
	genres = tm;
}

void EncodeSettings::InitDefaultGenre()
{
	status_t 	status;
	int32 		data;
	
	defaultGenre = 5; 
	status = prefs.FindInt32("defaultgenre", &data);
	if (status != B_OK) {
		return;
	};
	
	int32 temp;
	type_code junk;
	genres.GetInfo("genre", &junk, &temp);
	if ((data < 0) || (data > temp)) {
		return;
	};
	defaultGenre = data;
}

BPath EncodeSettings::FindCD(const char *directory)
{ 
	BDirectory dir; 
	BPath p;
	dir.SetTo(directory); 
	if(dir.InitCheck() != B_NO_ERROR) { 
		p.Unset();
		return p; 
	} 
	dir.Rewind(); 
	BEntry entry; 
	while(dir.GetNextEntry(&entry) >= 0) { 
		BPath path; 
		const char *name; 
		entry_ref e; 
		
		if(entry.GetPath(&path) != B_NO_ERROR) 
			continue; 
		name = path.Path(); 		
		
		if(entry.GetRef(&e) != B_NO_ERROR) 
			continue; 

		if(entry.IsDirectory()) { 
			if(strcmp(e.name, "floppy") == 0) 
				continue; // ignore floppy (it is not silent) 
			BPath path = FindCD(name);
			if(path.InitCheck() == B_OK)
				return path;
		} 
		else { 
			int devfd; 
			device_geometry g; 

			if(strcmp(e.name, "raw") != 0) 
				continue; // ignore partitions 

			devfd = open(name, O_RDONLY); 
			if(devfd < 0) 
				continue; 

			if(ioctl(devfd, B_GET_GEOMETRY, &g, sizeof(g)) >= 0) {
				if(g.device_type == B_CD)
				{ 
					return BPath(name);
				}
			}
			close(devfd);
		} 
	}
	p.Unset();
	return p;
}


