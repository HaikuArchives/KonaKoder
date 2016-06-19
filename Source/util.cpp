#ifndef UTILITY_H
#include "util.h"
#endif

void aerror(const char *a, const char *b)
{
	char s[1024];
	sprintf (s, "%s %s\n", a, b);
//	printf ("%s\n", s);
	BAlert *al = new BAlert (a, s, "OK");
	al->Go();
	return;
}
