#ifndef UTILITY_H
#define UTILITY_H

#include <SupportDefs.h>
#include <stdio.h>
#include <Alert.h>
//These are the message constants for the encode looper:
const uint32 NEW_STREAM = 0xbeef1000;
// NEW_STREAM tells the encoder to create a new stream.  It is sent by CDView. It contains:
// entry_ref "output" for the location of the output file.
// pointer "source" is a pointer to the CDInterface looper that the stream will come from.
// pointer "looper" is a pointer that CDInterface will use to identify the session.

const uint32 CLOSE_STREAM = 0xbeef1001;  //close the current stream.  It causes the current file to be closed.
const uint32 STREAM_DONE = 0xbeef1002;  // Sent to CDView indicating success or failure after the stream is closed.

//These are the constants for the CDInterface looper:
const uint32 CREATE_SESSION = 0xbeef1003;  // Sent by CDView to create a new session.
// CREATE_SESSION should contain the following members:
// Pointer named "looper" that identifies the encoder for the session
// Int named "track" that identifies the track number for the session
const uint32 UPDATE_STATUS_BAR = 0xbeef1004;  // Send to CDView to have it update a status bar.
// Int32 named "track" that id's the track.
// Int32 named "percent" that is the percent completed.
const uint32 DELETE_SESSION = 0xbeef1005;
// DELETE_SESSION contains:
// Pointer named "looper" that identifies the session

const uint32 SEND_TOC = 0xbeef1006;
// The SEND_TOC reply contains the TOC in the raw data named "TOC"

const uint32 SEND_AUDIO_BLOCK = 0xbeef1007;
const uint32 ENCODER_PROGESS = 0xbeef1008; // a message sent to the CDView indicating progress.
const uint32 AUDIO_BLOCK = 0xbeef1009;
// Sent from CDInterface to encoder.  It contains two data buffers "Right Buffer" and "Left Buffer".
// If these are missing, it indicates no more data.


//These are the constants for messages in CDView
const uint32 SCAN_CD_BUTTON = 0xbeef100a;
const uint32 ENCODE_CD_BUTTON = 0xbeef100b;
const uint32 PREFS_BUTTON = 0xbeef100c;
const uint32 GENRE_MENU_ITEM = 0xbeef100d;
const uint32 BITRATE_MENU_ITEM = 0xbeef100e;

//These are the constants for messages in PrefWindow
const uint32 GENRE_ADD_BUTTON = 0xbeef100f;
const uint32 GENRE_DEL_BUTTON = 0xbeef1010;
const uint32 SAVE_BROWSE_BUTTON = 0xbeef1011;
const uint32 TEMPLATE_BROWSE_BUTTON = 0xbeef1012;
const uint32 PREF_REVERT = 0xbeef1013;
const uint32 PREF_CANCEL = 0xbeef1014;
const uint32 PREF_OK = 0xbeef1015;
const uint32 NEW_SAVE_DIR = 0xbeef1016;
const uint32 NEW_TEMPLATE_DIR = 0xbeef1017;
const uint32 UPDATE_GENRES = 0xbeef1018;
const uint32 GENRE_SEL_CHANGED = 0xbeef1019;
//These are constants for buffer sizes in 16-bit stereo samples:
const uint32 MP3_BUFFER_SIZE = 1152;
const uint32 CDDA_BUFFER_SIZE = 588;

#define APP_SIGNATURE_SHORT "x-vnd-wdm-konakoder"
#define APP_SIGNATURE_LONG "application/x-vnd-wdm-konakoder"
#define MIME_TYPE "audio/x-mpeg"
void aerror(const char *a, const char *b);

#endif

