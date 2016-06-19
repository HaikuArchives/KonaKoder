#ifndef MSF_CLASS
#include "MSF.h"
#endif

#ifndef ENCODER_APP
#include "EncoderApp.h"
#endif

#include "util.h"
#include <scsi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Application.h>
int main()
{
	new EncoderApp();
	be_app->Run();

	delete be_app;
}