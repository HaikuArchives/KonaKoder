#ifndef SETTINGS_H
#define SETTINGS_H

#include "util.h"
#include "Preference.h"
#include <Path.h>
#include <malloc.h>
#include <scsi.h>
#include <FindDirectory.h>
#include <Directory.h>
class EncodeSettings
{
public:
	EncodeSettings();
	~EncodeSettings();
	
	void SaveSettings();
	void GetSettings();
	
	BPath	CDDevice;
	BRect	windowLocation;
	int32	bitrate;  //this is the bitrate index, 1-14
	BPath	saveDirectory;
	BPath	templateDirectory;
	BMessage genres;
	int32	defaultGenre; //Index of default genre in genres message.
	char 	*CDDBServer;
private:
	
	Preference prefs;
	
	void	InitDevice();
	void	InitWindow();
	void	InitCDDBServer();
	void	InitBitrate();
	void	InitSaveDir();
	void	InitTemplateDir();
	void	InitGenres();
	void	InitDefaultGenre();
	BPath	FindCD(const char *p);
	void	BuildDefaultGenreList();
};

#endif