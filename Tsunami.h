// **************************************************************
//     Filename: Tsunami.h
// Date Created: 10/15/2016
//
//     Comments: Robertsonics Tsunami serial control library
//
// Programmers: Jamie Robertson, jamie@robertsonics.com
//
// **************************************************************
//
// Revision History
//
// Date      Description
// --------  -----------
//
// 10/15/16  First version created.

#ifndef _20161015_TSUNAMI_H_
#define _20161015_TSUNAMI_H_

#define TSUNAMI_NUM_OUTPUTS	4

// ==================================================================
// The following defines are used to control which serial class is
//  used. Uncomment only the one you wish to use. If all of them are
//  commented out, the library will use Hardware Serial
#define __TSUNAMI_USE_SERIAL1__
//#define __TSUNAMI_SERIAL2__
//#define __TSUNAMI_SERIAL3__
//#define __TSUNAMI_USE_ALTSOFTSERIAL__
// ==================================================================

#define CMD_TRACK_CONTROL				3
#define CMD_STOP_ALL					4
#define CMD_MASTER_VOLUME				5
#define CMD_TRACK_VOLUME				8
#define CMD_TRACK_FADE					10
#define CMD_RESUME_ALL_SYNC				11
#define CMD_SAMPLERATE_OFFSET			12

#define TRK_PLAY_SOLO		0
#define TRK_PLAY_POLY		1
#define TRK_PAUSE			2
#define TRK_RESUME			3
#define TRK_STOP			4
#define TRK_LOOP_ON			5
#define TRK_LOOP_OFF		6
#define TRK_LOAD			7

#ifdef __TSUNAMI_USE_ALTSOFTSERIAL__
#include "../AltSoftSerial/AltSoftSerial.h"
#else
#include <HardwareSerial.h>
#ifdef __TSUNAMI_USE_SERIAL1__
#define TsunamiSerial Serial1
#define __TSUNAMI_SERIAL_ASSIGNED__
#endif
#ifdef __TSUNAMI_USE_SERIAL2__
#define TsunamiSerial Serial2
#define __TSUNAMI_SERIAL_ASSIGNED__
#endif
#ifdef __TSUNAMI_USE_SERIAL3__
#define TsunamiSerial Serial3
#define __TSUNAMI_SERIAL_ASSIGNED__
#endif
#ifndef __TSUNAMI_SERIAL_ASSIGNED__
#define TsunamiSerial Serial
#endif
#endif

class Tsunami
{
public:
	Tsunami() {;}
	~Tsunami() {;}
	void start(void);
	void masterGain(int out, int gain);
	void stopAllTracks(void);
	void resumeAllInSync(void);
	void trackPlaySolo(int trk, int out, bool lock);
	void trackPlayPoly(int trk, int out, bool lock);
	void trackLoad(int trk, int out, bool lock);
	void trackStop(int trk);
	void trackPause(int trk);
	void trackResume(int trk);
	void trackLoop(int trk, bool enable);
	void trackGain(int trk, int gain);
	void trackFade(int trk, int gain, int time, bool stopFlag);
	void samplerateOffset(int out, int offset);

private:
	void trackControl(int trk, int code, int out, int flags);

#ifdef __TSUNAMI_USE_ALTSOFTSERIAL__
	AltSoftSerial TsunamiSerial;
#endif

};

#endif
