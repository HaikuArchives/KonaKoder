/*
	cddb.cpp - functions for accessing CDDB
*/
//#include <string.h>
#include "cddb.h"

struct toc cdtoc[100];

// A helper function for computing the disc id.
int cddb_sum(int n) {
	char buf[12];
	char *p;
	unsigned long ret = 0;
	
	/* For backward compatibility this algoritm must not change */
	sprintf(buf, "%u", n);
	for (p=buf; *p != '\0'; p++)
		ret += (*p-'0');
	return ret;
}

// Computes the disc id based on the # of tracks and the toc.
unsigned long cddb_discid(int tot_trks) {
	int i;
	int n=0;
	int t=0;
	
	/* For backward compatibility this algoritm must not change */
	for (i=0;i<tot_trks;i++) {
		n+=cddb_sum((cdtoc[i].min*60)+cdtoc[i].sec);
		t+=	((cdtoc[i+1].min*60)+cdtoc[i+1].sec) -
			((cdtoc[i].min*60)+cdtoc[i].sec);
	}
	return (((n%0xff) << 24) + (t << 8) + tot_trks);
}
