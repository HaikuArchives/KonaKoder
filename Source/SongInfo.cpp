#ifndef SONGINFO_H
#include "SongInfo.h"
#endif

SongInfo::SongInfo(int t)
{
	track = t;
	year = 0;
	artist = NULL;
	album = NULL;
	title = NULL;
	genre = NULL;
	ei = NULL;
}

SongInfo::~SongInfo()
{
	free (artist);
	free (album);
	free (title);
	free (genre);
}


void SongInfo::SetYear(const uint32 y)
{
	year = y;
}

void SongInfo::SetBitrate(const uint32 b)
{
	bitrate = b;
}

void SongInfo::SetBitrateIndex(const uint32 b)
{
	bitrateIndex = b;
}

const char *SongInfo::GetArtist()
{
	return artist;
}

void SongInfo::SetArtist(const char *a)
{
	if (artist) free(artist);
	if (a != NULL) {
		artist = strdup(a);
	};
}

const char *SongInfo::GetAlbum()
{
	return album;
}

void SongInfo::SetAlbum(const char *a)
{
	if (album) free(album);
	if (a != NULL) {
		album = strdup(a);
	};
}

const char *SongInfo::GetTitle()
{
		return title;
}

void SongInfo::SetTitle(const char *a)
{
	if (title) free (title);
	if (a != NULL) {
		title = strdup(a);
	};
}

const char *SongInfo::GetGenre()
{
		return genre;
}

void SongInfo::SetGenre(const char *a)
{
	if (genre) free(genre);
	if (a != NULL) {
		genre = strdup(a);
	};
}
EncoderInterface *SongInfo::GetEncoder()
{
	return ei;
}

void SongInfo::SetEncoder(EncoderInterface *a)
{
	ei = a;
}

BTextControl *SongInfo::GetTextControl()
{
	return tc;
}

void SongInfo::SetTextControl(BTextControl *a)
{
	tc = a;
}

BEntry SongInfo::GetEntry()
{
	return entry;
}

void SongInfo::SetEntry(BEntry e)
{
	entry = e;
}