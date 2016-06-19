#ifndef MSF_CLASS
#define MSF_CLASS

#ifndef UTILITY_H
#include "util.h"
#endif

const int32 FPS = 75;	// Frames per second
const int32 SPM = 60;	// Seconds per minute

class MSF
{
public:
	MSF (int32 frames, int32 seconds =0, int32 minutes =0);
	int32 f() {return fs;};
	int32 s() {return ss;};
	int32 m() {return ms;};
	int32 frames() {return ((((ms * SPM) + ss) * FPS) + fs);};

	MSF& operator+= (int32 rhs);	
	MSF& operator+= (MSF rhs);
	
	friend MSF operator+ (MSF lhs, MSF rhs);
	friend MSF operator- (MSF lhs, MSF rhs);
	friend bool operator< (MSF lhs, MSF rhs);
	friend bool operator> (MSF lhs, MSF rhs);
	friend bool operator== (MSF lhs, MSF rhs);
	friend bool operator<= (MSF lhs, MSF rhs);
	friend bool operator>= (MSF lhs, MSF rhs);
private:

	int32 fs;
	int32 ss;
	int32 ms;
};

#endif
	