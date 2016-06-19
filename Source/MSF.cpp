#ifndef MSF_CLASS
#include "MSF.h"
#endif

MSF::MSF(int32 fr, int32 se, int32 mi)
{
//printf ("Creating MSF with %d %d %d\n", fr, se, mi);
	ms = mi;
	ss = se;
	
	// Roll excess frames into seconds...
	if (fr >= FPS) {
		ss += fr / FPS;
		fs = fr % FPS;
	} else {
		fs = fr;
	};
	
	// Roll excess seconds into minutes...
	if (ss >= SPM) {
		ms += ss / SPM;
		ss %= SPM;
	};
//printf ("Created MSF with %d %d %d\n", fs, ss, ms); 
}

MSF operator+ (MSF lhs, MSF rhs)
{
	int32 f1 = rhs.frames();
	int32 f2 = lhs.frames();
	return MSF(f1 + f2);
}

MSF operator- (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return MSF(f1 - f2);
}

MSF& MSF::operator+=(int32 rhs)
{
	fs += rhs;
	
	// Roll excess frames into seconds...
	if (fs >= FPS) {
		ss += fs / FPS;
		fs %= FPS;
	};
	
	// Roll excess seconds into minutes...
	if (ss >= SPM) {
		ms += ss / SPM;
		ss %= SPM;
	};
	
	return *this;
}

MSF& MSF::operator+=(MSF rhs)
{
	*this+=rhs.frames();
	return *this;
}

bool operator< (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return (f1 < f2);
}

bool operator> (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return (f1 > f2);
}

bool operator<= (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return (f1 <= f2);
}

bool operator>= (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return (f1 >= f2);
}

bool operator== (MSF lhs, MSF rhs)
{
	int32 f1 = lhs.frames();
	int32 f2 = rhs.frames();
	return (f1 == f2);
}