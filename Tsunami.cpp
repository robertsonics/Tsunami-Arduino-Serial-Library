// **************************************************************
//     Filename: Tsunami.cpp
// Date Created: 10/15/2016
//
//     Comments: Robertsonics Tsunami serial control library
//
// Programmers: Jamie Robertson, jamie@robertsonics.com
//
// **************************************************************

#include "Tsunami.h"


// **************************************************************
void Tsunami::start(void) {

  TsunamiSerial.begin(57600);
}

// **************************************************************
void Tsunami::masterGain(int out, int gain) {

uint8_t txbuf[8];
unsigned short vol;
uint8_t o;

  o = out & 0x03;
  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x08;
  txbuf[3] = CMD_MASTER_VOLUME;
  txbuf[4] = o;
  vol = (unsigned short)gain;
  txbuf[5] = (uint8_t)vol;
  txbuf[6] = (uint8_t)(vol >> 8);
  txbuf[7] = 0x55;
  TsunamiSerial.write(txbuf, 8);
}

// **************************************************************
void Tsunami::trackPlaySolo(int trk, int out, bool lock) {

int flags = 0;

  if (lock)
	  flags |= 0x01;
  trackControl(trk, TRK_PLAY_SOLO, out, flags);
}

// **************************************************************
void Tsunami::trackPlayPoly(int trk, int out, bool lock) {
  
int flags = 0;

  if (lock)
	  flags |= 0x01;
  trackControl(trk, TRK_PLAY_POLY, out, flags);
}

// **************************************************************
void Tsunami::trackLoad(int trk, int out, bool lock) {
  
int flags = 0;

  if (lock)
	  flags |= 0x01;
  trackControl(trk, TRK_LOAD, out, flags);
}

// **************************************************************
void Tsunami::trackStop(int trk) {

  trackControl(trk, TRK_STOP, 0, 0);
}

// **************************************************************
void Tsunami::trackPause(int trk) {

  trackControl(trk, TRK_PAUSE, 0, 0);
}

// **************************************************************
void Tsunami::trackResume(int trk) {

  trackControl(trk, TRK_RESUME, 0, 0);
}

// **************************************************************
void Tsunami::trackLoop(int trk, bool enable) {
 
  if (enable)
    trackControl(trk, TRK_LOOP_ON, 0, 0);
  else
    trackControl(trk, TRK_LOOP_OFF, 0, 0);
}

// **************************************************************
void Tsunami::trackControl(int trk, int code, int out, int flags) {
  
uint8_t txbuf[10];
uint8_t o;

  o = out & 0x03;
  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x0a;
  txbuf[3] = CMD_TRACK_CONTROL;
  txbuf[4] = (uint8_t)code;
  txbuf[5] = (uint8_t)trk;
  txbuf[6] = (uint8_t)(trk >> 8);
  txbuf[7] = (uint8_t)o;
  txbuf[8] = (uint8_t)flags;
  txbuf[9] = 0x55;
  TsunamiSerial.write(txbuf, 10);
}

// **************************************************************
void Tsunami::stopAllTracks(void) {

uint8_t txbuf[5];

  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_STOP_ALL;
  txbuf[4] = 0x55;
  TsunamiSerial.write(txbuf, 5);
}

// **************************************************************
void Tsunami::resumeAllInSync(void) {

uint8_t txbuf[5];

  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_RESUME_ALL_SYNC;
  txbuf[4] = 0x55;
  TsunamiSerial.write(txbuf, 5);
}

// **************************************************************
void Tsunami::trackGain(int trk, int gain) {

uint8_t txbuf[9];
unsigned short vol;

  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x09;
  txbuf[3] = CMD_TRACK_VOLUME;
  txbuf[4] = (uint8_t)trk;
  txbuf[5] = (uint8_t)(trk >> 8);
  vol = (unsigned short)gain;
  txbuf[6] = (uint8_t)vol;
  txbuf[7] = (uint8_t)(vol >> 8);
  txbuf[8] = 0x55;
  TsunamiSerial.write(txbuf, 9);
}

// **************************************************************
void Tsunami::trackFade(int trk, int gain, int time, bool stopFlag) {

uint8_t txbuf[12];
unsigned short vol;

  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x0c;
  txbuf[3] = CMD_TRACK_FADE;
  txbuf[4] = (uint8_t)trk;
  txbuf[5] = (uint8_t)(trk >> 8);
  vol = (unsigned short)gain;
  txbuf[6] = (uint8_t)vol;
  txbuf[7] = (uint8_t)(vol >> 8);
  txbuf[8] = (uint8_t)time;
  txbuf[9] = (uint8_t)(time >> 8);
  txbuf[10] = stopFlag;
  txbuf[11] = 0x55;
  TsunamiSerial.write(txbuf, 12);
}

// **************************************************************
void Tsunami::samplerateOffset(int out, int offset) {

uint8_t txbuf[8];
unsigned short off;
uint8_t o;

  o = out & 0x03;
  txbuf[0] = 0xf0;
  txbuf[1] = 0xaa;
  txbuf[2] = 0x08;
  txbuf[3] = CMD_SAMPLERATE_OFFSET;
  txbuf[4] = (uint8_t)o;
  off = (unsigned short)offset;
  txbuf[5] = (uint8_t)off;
  txbuf[6] = (uint8_t)(off >> 8);
  txbuf[7] = 0x55;
  TsunamiSerial.write(txbuf, 8);
}





