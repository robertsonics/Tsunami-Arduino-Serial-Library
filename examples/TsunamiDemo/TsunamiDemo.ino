// ****************************************************************************
//       Sketch: Tsunami Serial Control Example
// Date Created: 11/22/2016
//
//     Comments: Demonstrates advanced, two-way, serial control of the Tsunami
//               from a 3.3V Arduino.
//
//  Programmers: Jamie Robertson, info@robertsonics.com
//
//  WARNING: The Tsunami is a 3.3V device. Attempting to connect it to a 5V
//   Arduino (such as an UNO) without level-shifters, will likely damage your
//   Tsunami!
//
// ****************************************************************************
//
// To use this sketch with an Arduino, you'll need:
//
// 1) An Arduino with an operating voltage of 3.3V, *** NOT 5V ***!
// 2) Download and install the Metro library.
// 3) Connect 3 wires from the Arduino to the Tsunami's serial connector:
//
//    Arduino       Tsunami
//    =======       =======
//    GND  <------> GND
//    TXn  <------> RX
//    RXn  <------> TX
//
//    If you're using an Arduino with at least one extra hardware serial port,
//    such as an M0 or Teensy, the Tsunami library is already set to use
//    Serial1. If you need to change that, or want to use AltSoftSerial, you
//    must make a small edit the Tsunami.h file, according the instructions in
//    that file.
//
// 4) Download and install the demo wav files onto the Tsunami's microSD
//    card. You can find them here:
//
//    http://robertsonics.com/2015/04/25/arduino-serial-control-tutorial/
//
//    You can certainly use your own tracks instead, although the demo may
//    not make as much sense. If you do, make sure your tracks are at least
//    10 to 20 seconds long and have no silence at the start of the file.
//
// 5) Open the Arduino Serial Monitor window to see status messages.

#include <Metro.h>              // Include the Metro library header
#include <Tsunami.h>            // Include the Tsunami library header
//#include <AltSoftSerial.h>    // Optional for using AltSoftSerial

#define LED 13                  // our LED

Tsunami tsunami;                // Our Tsunami object

Metro gLedMetro(500);           // LED blink interval timer
Metro gSeqMetro(6000);          // Sequencer state machine interval timer

byte gLedState = 0;             // LED State
int  gSeqState = 0;             // Main program sequencer state
int  gRateOffset = 0;           // Tsunami sample-rate offset
int  gNumTracks;                // Number of tracks on SD card

char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string


// ****************************************************************************
void setup() {
  
  // Serial monitor
  Serial.begin(9600);
 
  // Initialize the LED pin
  pinMode(LED,OUTPUT);
  digitalWrite(LED,gLedState);

  // We should wait for the Tsunami to finish reset before trying to send
  // commands.
  delay(1000);

  // Tsunami startup at 57600
  tsunami.start();
  delay(10);
  
  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  tsunami.stopAllTracks();
  tsunami.samplerateOffset(0, 0);
  
  // Enable track reporting from the Tsunami
  tsunami.setReporting(true);
  
  // Allow time for the Tsunami to respond with the version string and
  //  number of tracks.
  delay(100); 
  
}


// ****************************************************************************
// This program uses a Metro timer to create a sequencer that steps through
//  states at 6 second intervals - you can change this rate above. Each state
//  Each state will demonstrate a Tsunami serial control feature.
//
//  In this example, some states wait for specific audio tracks to stop playing
//  before advancing to the next state.

void loop() {
  
int i;

  // Call update on the Tsunami to keep the track playing status current.
  tsunami.update();
  
  // Check if the sequencer timer has elapsed and perform the appropriate
  //  state action if so. States 3 and 5 wait for tracks to stop playing and
  //  are therefore not in the metro event. They are instead polled after the
  //  metro check.
  if (gSeqMetro.check() == 1) {
      
      switch (gSeqState) {
  
          // State 0: Demonstrates how to fade in a music track 
          case 0:
              // First retrieve and print the version and number of tracks        
              if (tsunami.getVersion(gTsunamiVersion, VERSION_STRING_LEN)) {
                  Serial.print(gTsunamiVersion);
                  Serial.print("\n");
                  gNumTracks = tsunami.getNumTracks();
                  Serial.print("Number of tracks = ");
                  Serial.print(gNumTracks);
                  Serial.print("\n");
              }
              else
                  Serial.print("WAV Trigger response not available");
              tsunami.samplerateOffset(0, 0);        // Reset sample rate offset to 0
              tsunami.masterGain(0, 0);              // Reset the master gain to 0dB
              
              tsunami.trackGain(2, -40);             // Preset Track 2 gain to -40dB
              tsunami.trackPlayPoly(2, 0, true);     // Start Track 2
              tsunami.trackFade(2, 0, 2000, false);  // Fade Track 2 to 0dB over 2 sec
              gSeqState = 1;                         // Advance to state 1
          break;

          // State 1: Demonstrates how to cross-fade music tracks
          case 1:
              tsunami.trackGain(1, -40);             // Preset Track 1 gain to -40dB
              tsunami.trackPlayPoly(1, 0, true);     // Start Track 1
              tsunami.trackFade(1, 0, 3000, false);  // Fade Track 1 up to 0db over 3 secs
              tsunami.update();
              delay(2000);                           // Wait 2 secs
              tsunami.trackFade(2, -40, 3000, true); // Fade Track 2 down to -40dB over 3 secs and stop
              Serial.print("Waiting for Track 2 to finish... ");
              gSeqState = 2;                         // Advance to state 2
          break;
                                 
          // State 3: Honk the horn 2 times
          case 3:
              tsunami.trackPlayPoly(5, 0, true);     // Start Track 5 poly
              tsunami.update();
              delay(500);
              tsunami.trackStop(5);                  // Stop Track 5
              tsunami.update();
              delay(250);
              tsunami.trackPlayPoly(5, 0, true);     // Start Track 5 poly
              tsunami.update();
              delay(500);
              tsunami.trackStop(5);                  // Stop Track 5
              gSeqState = 4;                         // Advance to state 4
          break;
          
          // State 4: Fade out and stop dialog
          case 4:
              tsunami.trackLoop(4, 0);               // Disable Track 4 looping
              tsunami.trackFade(4, -50, 5000, true); // Fade Track 4 to -50dB and stop
              Serial.print("Waiting for Track 4 to finish... ");
              gSeqState = 5;                         // Advance to state 5
          break;
            
          // State 6: Demonstrates preloading tracks and starting them in sample-
          //  sync, and real-time samplerate control (pitch bending);
          case 6:
              tsunami.trackLoad(6, 0, true);         // Load and pause Track 6
              tsunami.trackLoad(7, 0, true);         // Load and pause Track 7
              tsunami.trackLoad(8, 0, true);         // Load and pause Track 8
              tsunami.resumeAllInSync();             // Start all in sample sync

              // Decrement the sample rate offset from 0 to -32767 (1 octave down)
              //  in 10 ms steps
              gRateOffset = 0;
              for (i = 0; i < 127; i++) {
                gRateOffset -= 256;
                tsunami.samplerateOffset(0, gRateOffset);
                delay(10);
              }
              gRateOffset = -32767;
              tsunami.samplerateOffset(0, gRateOffset);
              
              // Hold for 1 second
              delay(1000);
              
              // Now increment to +32767 (1 octave up) in 10ms steps
              for (i = 0; i < 255; i++) {
                gRateOffset += 256;
                tsunami.samplerateOffset(0, gRateOffset);
                delay(10);
              }
              gRateOffset = 32767;
              tsunami.samplerateOffset(0, gRateOffset);
              
              // Hold for 1 second, the stop all tracks
              delay(1000);
              tsunami.stopAllTracks();               // Stop all
              gSeqState = 0;                         // Advance to state 0
          break;
            
      } // switch
           
  } // if (gSeqState.check() == 1)
 
  // State 2: Wait for Track 2 to stop, then fade down the music and start the
  //  dialog track looping.
  if (gSeqState == 2) {
      gSeqMetro.reset();                             // Reset the sequencer metro
      if (!tsunami.isTrackPlaying(2)) {
          Serial.print("Track 2 done\n");
          tsunami.trackFade(1, -6, 500, false);      // Lower the music volume
          tsunami.trackLoop(4, 1);                   // Enable Track 4 looping
          tsunami.trackPlayPoly(4, 0, true);         // Start Track 4 poly
          gSeqState = 3;                             // Advance to state 3;
      }       
  }

  // State 5: Wait for Track 4 to stop, then play three tracks sequentially and
  //  stop all with a 5 sec fade to -50dB. This is how you can implement MIDI
  //  Note-On/Off control for: MIDI -> Arduino -> WAV Trigger.
  if (gSeqState == 5) {
      gSeqMetro.reset();
      if (!tsunami.isTrackPlaying(4)) {
          Serial.print("Track 4 done\n");
          tsunami.masterGain(0, -8);                 // Lower main volume
          tsunami.trackPlayPoly(6, 0, true);         // Play first note
          tsunami.update();
          delay(1000);
          tsunami.trackPlayPoly(7, 0, true);         // Play second note
          tsunami.update();
          delay(1000);
          tsunami.trackPlayPoly(8, 0, true);         // Play third note
          tsunami.update();
          delay(1000);
          tsunami.trackFade(6, -50, 5000, true);     // Fade Track 6 to -50dB and stop
          tsunami.trackFade(7, -50, 5000, true);     // Fade Track 7 to -50dB and stop
          tsunami.trackFade(8, -50, 5000, true);     // Fade Track 8 to -50dB and stop
          gSeqState = 6;
      }       
  }
  
  // If time to do so, toggle the LED
  if (gLedMetro.check() == 1) {
      if (gLedState == 0) gLedState = 1;
      else gLedState = 0;
      digitalWrite(LED, gLedState);
  } // if (gLedMetro.check() == 1)

  // Delay 30 msecs
  delay(30);
}

