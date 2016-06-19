#ifndef SONGINFO_H
#define SONGINFO_H

#ifndef MP3_ENCODER
#include "MP3Encoder.h"
#endif

#include <TextControl.h>
#include <Entry.h>

class SongInfo
{
public:
	SongInfo(int track);
	~SongInfo();
	
	const 	uint32	GetTrack() {return track;};
	
	const 	uint32 	GetYear() {return year;};
	void	SetYear(const uint32 year);
	
	const	uint32	GetCPU() {return cpu;};
	void	SetCPU(const uint32 c) {cpu = c;};
	
	const	uint32	GetBitrate() {return bitrate;};
	void	SetBitrate(const uint32 b);
	
	const 	uint32	GetBitrateIndex() {return bitrateIndex;};
	void	SetBitrateIndex(const uint32 b);
	
	const 	char 	*GetArtist();
	void	SetArtist(const char *a);
	
	const 	char	*GetAlbum();
	void	SetAlbum(const char *a);
	
	const 	char	*GetTitle();
	void	SetTitle(const char *a);
	
	const 	char*GetGenre();
	void	SetGenre(const char *a);
	
	EncoderInterface 	*GetEncoder();
	void	SetEncoder(EncoderInterface *e);

	BTextControl		*GetTextControl();
	void	SetTextControl(BTextControl *t);
	
	BEntry				GetEntry();
	void	SetEntry(BEntry e);
	
private:
	uint32 track;
	uint32 year;
	uint32 bitrate;
	uint32 bitrateIndex;
	char *artist;
	char *album;
	char *title;
	char *genre;
	EncoderInterface *ei;
	uint32 cpu;
	BTextControl *tc;
	BEntry entry;
};

#endif