/*
	cddb.h - functions and structures for accessing CDDB
*/
#ifndef CDDB_H
#define CDDB_H
#include <stdio.h>
#include <SupportDefs.h>

struct toc {
	int32 min;
	int32 sec;
	int32 frame;
};
struct track_info {
	uchar track_num;
	char track_name[64];
	struct toc start;
	struct toc end;
	struct toc length;
};

struct cd_info {
	char disc_title[128];
	char disc_artist[128];
	int numtracks;
	struct track_info track[100];
	int playOrder[100];
	int shuffleIndex;
};

//struct toc cdtoc[100];

//int cddb_sum(int n);
//unsigned long cddb_discid (int tot_trks);


#endif