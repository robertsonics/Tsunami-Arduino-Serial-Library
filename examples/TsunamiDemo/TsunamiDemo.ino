// ****************************************************************************
//       Sketch: TsunamiDemo
// Date Created: 10/15/2016
//
//     Comments: Demonstrates basic serial control of the Tsunami from an
//               Arduino.
//
//  Programmers: Jamie Robertson, jamie@robertsonics.com
//
// ****************************************************************************
//
// To use this sketch, you'll need to:
//
// 1) Run this on a 3.3V Arduino board.
// 2) Download and install the Metro library.
// 3) Connect 2 wires from the Arduino to the Tsunami's serial connector:
//
//    Arduino          Tsunami
//    =======          ===========
//    GND  <---------> GND
//    TXn  ----------> RX
//    RXn  <---------- TX
//
// 4) Download and install the demo wav files onto the WAV Trigger's microSD
//    card. You can find them here:
//
//    You can certainly use your own tracks instead, although the demo may
//    not make as much sense. If you do, make sure your tracks are at least 10
//    to 20 seconds long and have no silence at the start of the file.

#include <Metro.h>
//#include <AltSoftSerial.h>
#include <Tsunami.h>

#define LED 13                // our LED

Tsunami tsunami;              // Our Tsunami object

Metro gLedMetro(500);         // LED blink interval timer
Metro gSeqMetro(7000);        // Our sequencer state machine interval timer

byte gLedState = 0;           // LED State
int  gSeqState = 0;           // Our sequencer state
int  gRateOffset = 0;         // Sample-rate offset


// ****************************************************************************
void setup() {
  
  // Serial monitor
  Serial.begin(9600);
 
  // Initialize the LED pin
  pinMode(LED,OUTPUT);
  digitalWrite(LED, gLedState);

  // Tsunami startup at 57600
  tsunami.start();
  
  // If the Arduino is powering the Tsunami, we should wait for the WAV Trigger
  //  to finish reset before trying to send commands.
  delay(1000);
  
  // If we're not powering the Tsunami, send a stop-all command in case it
  //  was already playing tracks. If we are powering the Tsunami, it doesn't
  //  hurt to do this.
  tsunami.stopAllTracks();
  
}


// ****************************************************************************
// This program uses a Metro timer to create a sequencer state machine that steps
//  through states at 6 second intervals - you can change this rate above.
//  Each state will execute a new wavTrigger command.

void loop() {
  
int i;

  // If time to do so, perform the next WAV Trigger task and then increment
  //  the state machine variable 
  if (gSeqMetro.check() == 1) {
      
      switch (gSeqState) {
  
          // Demonstrates how to fade in a music track 
          case 0:
              tsunami.masterGain(0, 0);                   // Reset the master gain to 0dB                 
              tsunami.trackGain(2, -40);                  // Preset Track 2 gain to -40dB
              tsunami.trackPlayPoly(2, 0, true);          // Start Track 2
              tsunami.trackFade(2, 0, 2000, false);       // Fade Track 2 to 0dB
          break;

          // Demonstrates how to cross-fade music tracks
          case 1:
              //tsunami.trackCrossFade(2, 1, 0, 3000);    // Cross-fade Track 2 to Track 1
              tsunami.trackGain(1, -40);
              tsunami.trackPlayPoly(1, 0, true);
              tsunami.trackFade(1, 0, 3000, false);
              delay(2000);
              tsunami.trackFade(2, -40, 3000, true);
          break;
                                 
          // Fade down music and start looping dialog
          case 2:
              tsunami.trackFade(1, -6, 500, false);
              tsunami.trackLoop(4, 1);                    // Enable Track 4 looping
              tsunami.trackPlayPoly(4, 0, true);          // Start Track 4 poly
          break;

          // Honk the horn 2 times
          case 3:
              tsunami.trackPlayPoly(5, 0, true);      // Start Track 5 poly
              delay(500);
              tsunami.trackStop(5);                   // Stop Track 5
              delay(250);
              tsunami.trackPlayPoly(5, 0, true);      // Start Track 5 poly
              delay(500);
              tsunami.trackStop(5);                   // Stop Track 5
          break;
          
          // Fade out and stop dialog
          case 4:
              tsunami.trackLoop(4, 0);                // Disable Track 4 looping
              tsunami.trackFade(4, -50, 5000, true);  // Fade Track 4 to -50dB and stop
          break;
          
          // This demonstrates playing musical instrument samples, with decay on
          //  release
          case 5:
              tsunami.masterGain(0, -8);              // Lower main volume
              tsunami.trackPlayPoly(6, 0, true);      // Play first note
              delay(1000);
              tsunami.trackPlayPoly(7, 0, true);      // Play second note
              delay(1000);
              tsunami.trackPlayPoly(8, 0, true);      // Play third note
              delay(1000);
              tsunami.trackFade(6, -50, 5000, true);  // Fade Track 6 to -50dB and stop
              tsunami.trackFade(7, -50, 5000, true);  // Fade Track 7 to -50dB and stop
              tsunami.trackFade(8, -50, 5000, true);  // Fade Track 8 to -50dB and stop
          break;
   
          // Demonstrates preloading tracks and starting them in sample-sync, and
          //  real-time samplerate control (pitch bending);
          case 6:
              tsunami.trackLoad(6, 0, true);          // Load and pause Track 6
              tsunami.trackLoad(7, 0, true);          // Load and pause Track 7
              tsunami.trackLoad(8, 0, true);          // Load and pause Track 8
              tsunami.resumeAllInSync();              // Start all in sample sync
              for (i = 0; i < 100; i++) {
                gRateOffset -= 200;
                tsunami.samplerateOffset(0, gRateOffset);
                delay(10);
              }
              for (i = 0; i < 100; i++) {
                gRateOffset += 200;
                tsunami.samplerateOffset(0, gRateOffset);
                delay(10);
              }
              delay(500);
              tsunami.stopAllTracks();                // Stop all
         break;
         
           
      } // switch
 
      // Increment our state
      gSeqState++;
      if (gSeqState > 6)
          gSeqState = 0;
          
  } // if (gWTrigState.check() == 1)
 
  // If time to do so, toggle the LED
  if (gLedMetro.check() == 1) {
      if (gLedState == 0) gLedState = 1;
      else gLedState = 0;
      digitalWrite(LED, gLedState);
  } // if (gLedMetro.check() == 1)

  // Delay 30 msecs
  delay(30);
}

