Tsunami-Arduino-Serial-Library
==================================

Tsunami Serial Control Arduino Library

Tsunami is a 3.3V device and its inputs, specifically the triggers and serial RX
pins are not 5V tolerant. For this reason, you must either use a 3.3V Arduino or
a level shifter.

The library assumes that you are using an Arduino with at least one extra hardware
serial port, such as an Teensy or M0, and by default uses Serial1. If you need to
change this, or wish to use AltSoftSerial, you'll need to make one small change to
the library's **Tsunami.h** file. Near the top of the file, look for:

```
// ==================================================================
// The following defines are used to control which serial class is
//  used. Uncomment only the one you wish to use. If all of them are
//  commented out, the library will use Hardware Serial
#define __WT_USE_SERIAL1__
//#define __WT_USE_SERIAL2__
//#define __WT_USE_SERIAL3__
//#define __WT_USE_ALTSOFTSERIAL__
// ==================================================================
```

Comment out the `__WT_USE_SERIAL1__` line and uncomment the line corresponding
to the port you want to use. If all the lines are commented out, the library
library will use Serial.

I make no attempt to throttle the amount of messages that are sent. If you send
continuous volume or sample-rate commands at full speed, you risk overflowing
Tsunami's serial input buffer and/or causing clicks in Tsunami's audio output
due to excessive serial interrupt processing stealing cycles from audio playback.
If you are connecting a continuous controller that can change rapidly for volume
or sample-rate control, you should use a timer to send changes only every 10 or
more msecs. You can, of course, experiment with this. If you're only ever playing
1 or 2 tracks at a time, you'll likely be able to get away with sending volume
changes more frequently than if you are playing 12 tracks at a time.

The library includes example sketches that demonstrate many of the library
commands with a set of demo tracks which you can download from

http://robertsonics.com/2015/04/25/arduino-serial-control-tutorial/

Usage:
======

In all cases below, the range for **t** (track number) is 1 through 4096, the range
for **out** (output number) is 1 through 4 for the stereo firmware, and 1 through 8
for the mono firmware.

Tsunami tsunami;

**tsunami.start()** - you must call this method first to initialize the serial
  communications.

**tsunami.getVersion(char *pDst, int len)** - this function will return **len** bytes of
  the Tsunami version string to the location specified by **pDst**. The function
  returns TRUE if successful, and FALSE if the string is not available. This
  function requires bi-directional communication with Tsunami.

**tsunami.getNumTracks()** - Returns number of tracks on Tsunami's microSD card
  This function requires bi-directional communication with Tsunami.

**tsunami.setReporting(bool enable)** - this function enables (TRUE) or disables
  (FALSE) track reporting. When enabled, the Tsunami will send a message whenever
  a track starts or ends, specifying the track number. Provided you call update()
  periodically, the library will use these messages to maintain status of all tracks,
  allowing you to query if particular tracks are playing or not.
  
**tsunami.setTriggerBank(int bank)** - this function sets the trigger bank. The bank
  range is 1 - 32. Each bank will offset the normal trigger function track assignment
  by 16. For bank 1, the default, trigger one maps to track 1. For bank 2, trigger 1
  maps to track 17, trigger 2 to track 18, and so on.
  
**tsunami.update()** - this function should be called periodically when reporting is
  enabled. Doing so will process any incoming serial messages and keep the track status
  up to date.

**tsunami.isTrackPlaying(int trk)** - If reporting has been enabled, this function can be
  used to determine if a particular track is currently playing.

**tsunami.flush()** - This function clears Tsunami's communication buffer and resets
  the local track status info.

**tsunami.masterGain(int out, int gain)** - this function immediately sets the gain of the
  specific stereo output to the specified value. The range for gain is -70 to +4. If
  audio is playing, you will hear the result immediately. If audio is not playing,
  the new gain will be used the next time a track is started.

**tsunami.samplerateOffset(int out, int offset)** - this function immediately sets sample-rate offset,
  or playback speed / pitch, of the specified stereo output. The range for
  for the offset is -32767 to +32676, giving a speed range of 1/2x to 2x, or a
  pitch range of down one octave to up one octave. If audio is playing, you will
  hear the result immediately. If audio is not playing, the new sample-rate offset
  will be used the next time a track is started.

**tsunami.trackPlaySolo(int t, int out, bool lock)** - this function stops any and all tracks
  that are currently playing and starts track number **t** from the beginning. The track is
  routed to the specified stereo output. If **lock** is TRUE, the track will not be subject to
  Tsunami's voice stealing algorithm.

**tsunami.trackPlayPoly(int t, int out, bool lock)** - this function starts track number
  **t** from the beginning, blending it with any other tracks that are currently playing,
  including potentially another copy of the same track. The track is routed to the specified
  stereo output. If **lock** is TRUE, the track will not be subject to Tsunami's voice stealing
  algorithm.
  
**tsunami.trackLoad(int t, int out, bool lock)** - this function loads track number **t**
  and pauses it at the beginning of the track. Loading muiltiple tracks and then un-pausing
  the all with resumeAllInSync() function below allows for starting multiple tracks in
  sample sync. The track is routed to the specified stereo output. If **lock** is TRUE, the
  track will not be subject to Tsunami's voice stealing algorithm.
  
**tsunami.trackStop(int t)** - this function stops track number **t** if it's currently
  playing. If track t is not playing, this function does nothing. No other
  tracks are affected.
  
**tsunami.trackPause(int t)** - this function pauses track number **t** if it's currently
  playing. If track t is not playing, this function does nothing. Keep in mind
  that a paused track is still using one of the 8 voice slots. A voice allocated
  to playing a track becomes free only when that sound is stopped or the track
  reaches the end of the file (and is not looping).
  
**tsunami.trackResume(int t)** - this function resumes track number **t** if it's currently
  paused. If track number **t** is not paused, this function does nothing.
  
**tsunami.trackLoop(int t, bool enable)** - this function enables (true) or disables
  (false) the loop flag for track **t**. This command does not actually start a track,
  only determines how it behaves once it is playing and reaches the end. If the
  loop flag is set, that track will loop continuously until it's stopped, in which
  case it will stop immediately but the loop flag will remain set, or until the loop
  flag is cleared, in which case it will stop when it reaches the end of the track.
  This command may be used either before a track is started or while it's playing.
  
**tsunami.trackGain(int t, int gain)** - this function immediately sets the gain of
  track **t** to the specified value. The range for gain is -70 to +10. A value of
  0 (no gain) plays the track at the nominal value in the wav file. This is the
  default gain for every track until changed. A value of -70 is completely
  muted. If the track is playing, you will hear the result immediately. If the
  track is not playing, the gain will be used the next time the track is started.
  Every track can have its own gain.
  
  Because the effect is immediate, large changes can produce ubrupt results. If
  you want to fade in or fade out a track, send small changes spaced out at regular
  intervals. Increment or decrementing by 1 every 20 to 50 msecs produces nice
  smooth fades. Better yet, use the trackFade() function below.
  
**tsunami.stopAllTracks()** - this commands stops any and all tracks that are currently
  playing.
  
**tsunami.resumeAllInSync()** - this command resumes all paused tracks within the same
  audio buffer. Any tracks that were loaded using the trackLoad() function will start
  and remain sample locked (in sample sync) with one another.

**tsunami.trackFade(int t, int gain, int time, bool stopFlag)** - this command initiates
  a hardware volume fade on track number **t** if it is currently playing. The track
  volume will transition smoothly from the current value to the target gain in the
  specified number of milliseconds. If the stopFlag is non-zero, the track will be
  stopped at the completion of the fade (for fade-outs.)

**tsunami.setInputMix(int mix)** - this function controls the routing of the audio input
  channels. For bits 1 through 4, a "1" causes the 2 input channels to be mixed into
  the corresponding output pair. As an example, to route the audio input to output pairs
  1, 2 and 4, the syntax is: setInputMix(IMIX_OUT1 | IMIX_OUT2 | IMIX_OUT4); The routing
  is immediate and does no ramping, so to avoid pops, be sure that the input is quiet
  when switching.

**tsunami.setMidiBank(int bank)** - this function sets the MIDI bank. The bank range
  is 1 - 32. Each bank will offset the MIDI Note number to track assignment by 128.
  For bank 1, the default, MIDI Note number maps to track 1. For bank 2, MIDI Note
  number 1 maps to track 129, MIDI Note number 2 to track 130, and so on.
 

  




  
