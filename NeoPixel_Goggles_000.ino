// Based on Adafruit's goggle sketch - https://learn.adafruit.com/kaleidoscope-eyes-neopixel-led-goggles-trinket-gemma/software
// Low power NeoPixel goggles example.  Makes a nice blinky display with just a few LEDs on at any time.

//      First right eye, pixel #5 is the LED closest to the bridge of the nose
//      Second left eye, pixel #29 is the LED closest to the bridge of the nose

#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define PIN 0
  #define LED 1
  #define PIN_BUTTON_2 2
  #define PIN_BUTTON_3 3
  #define PIN_BUTTON_4 4

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);

int i;
int ring2_offset = 1;  //  Testing
int bright3 = 255;            // BURN ALL THE THINGS
int bright2 = 127;   //  Med
int bright1 = 31;   //  Low
int scanner1[] = {14,15, 0, 1,2,3,4,5,31,16,17,18,19,20,21,22}; // got to be a better way...
int scanner2[] = {13,12,11,10,9,8,7,6,30,29,28,27,26,25,24,23}; // got to be a better way...

int default_brightness = bright1; //  round(255/16); // 1/3 brightness
int change_modes_default = 8*1000; //  Time to change modes
int change_modes = change_modes_default; //  Time to change modes
int speed_med = 40;
int speed_slow = 50;

uint8_t  mode   = 0, // Current animation effect
         offset = 0; // Position of spinny eyes
uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

//  Used to realign the two rings, this way spinning can be synchronized
int realign(int desired_position)
  {
  desired_position += ring2_offset;
  if (desired_position <16) {desired_position += 16; }
  if (desired_position >31) {desired_position -= 16; }
  return desired_position;
  }

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pinMode(LED, OUTPUT);  //  Onboard LED
  pinMode(PIN_BUTTON_2, INPUT);  //  There's a button input!
  pinMode(PIN_BUTTON_3, INPUT);  //  There's a button input!
  pinMode(PIN_BUTTON_4, INPUT);  //  There's a button input!
  digitalWrite(PIN_BUTTON_2, HIGH);  //  Using the internal resistors, so the button will read LOW when pressed
  digitalWrite(PIN_BUTTON_3, HIGH);  //  Using the internal resistors, so the button will read LOW when pressed
  digitalWrite(PIN_BUTTON_4, HIGH);  //  Using the internal resistors, so the button will read LOW when pressed

  pixels.begin();
  pixels.setBrightness(default_brightness); 
  prevTime = millis();
}

void loop() {
  uint8_t  i;
  uint32_t t;

//  If button on pin 2 is pressed, make it brighter!  Otherwise, regular brightness
  pixels.setBrightness(! digitalRead(PIN_BUTTON_2) ? bright2 : default_brightness );

//  If button on pin 3 is pressed, change modes
  if ( ! digitalRead(PIN_BUTTON_3) ) 
    { change_modes = 750; } else { change_modes = change_modes_default; }
//  If button on pin 4 is pressed, turn the lenses on bright red
  if (! digitalRead(PIN_BUTTON_4))
    {
    for (i=0;i<32;i++) 
      {
      if (! digitalRead(PIN_BUTTON_3)) { pixels.setPixelColor(i,0xFFFFFF); }
      else { pixels.setPixelColor(i,0xFF0000); }      
      }
    pixels.setBrightness(bright3); pixels.show(); delay(500); 
    pixels.setBrightness(bright1); 
    }
  else 
   {
     switch(mode) {
      //  Wave Scanner, moving right
      case 0: wave_scanner_right(); wave_scanner_left(); break;
  
      //  Larson Scanner
      case 1: larson_scanner(); break;
  
      // Spinny wheels (8 LEDs on at a time)
      case 2: spin_wheels(); break;
  
      // Infinity Symbol
      case 3: infinity_scanner(); break;
  
      // Random sparks - just one LED on at a time - all one color!
      case 4: sparks(); break; 
  
      // Sirens
      case 5: sirens(); break;
      }
  
    t = millis();
    if((t - prevTime) > change_modes) {      // Every X seconds...
      mode++;                        // Next mode
      if(mode > 5) {                 // End of modes?
        mode = 0;                    // Start modes over
        color >>= 8;                 // Next color R->G->B
        if(!color) color = 0xFF0000; // Reset to red
      }
      for(i=0; i<32; i++) pixels.setPixelColor(i, 0);
      prevTime = t;
    }
   }
}

//  Wave Scanner, moving right
void wave_scanner_right()
  {   
  for (i=0;i<16;i++)
    { 
    pixels.setPixelColor(scanner1[i],color);
    pixels.setPixelColor(scanner2[i],color);
    pixels.show();
    delay(speed_slow);
    pixels.setPixelColor(scanner1[i],0);
    pixels.setPixelColor(scanner2[i],0);
    }
  }

//  Wave Scanner, moving left
void wave_scanner_left()
  {
  for (i=15;i>=0;i--)
    { 
    pixels.setPixelColor(scanner1[i],color);
    pixels.setPixelColor(scanner2[i],color);
    pixels.show();
    delay(speed_slow);
    pixels.setPixelColor(scanner1[i],0);
    pixels.setPixelColor(scanner2[i],0);
    }
  }

//  Larson Scanner
void larson_scanner()
  {
    int scanner[] = {13,5,30,23,23,30,5,13};  //  It uses less memory to just delcare it this way...
    for(i=0;i<8;i++)
      {
      pixels.setPixelColor(scanner[i],color);
      pixels.show();
      delay(100);
      pixels.setPixelColor(scanner[i],0);
      pixels.show();
      }
  }

void sparks()
  {
    i = random(32);
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(20);
    pixels.setPixelColor(i, 0);
  }

void spin_wheels()
  {
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(((offset + i) & 7) < 2) c = color; // 4 pixels on...
      pixels.setPixelColor(   i, c); // First eye
      pixels.setPixelColor(realign(31-i-4), c); // Second eye (flipped)
    }
    pixels.show();
    offset++;
    delay(speed_slow);
  }

void infinity_scanner()
  {
    for(i=0;i<32;i++)
      {
      int infinitypixels[] = {6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,29,28,27,26,25,24,23,22,21,20,19,18,17,16,31,30};
      pixels.setPixelColor(infinitypixels[i],color);
      pixels.show();
      delay(speed_med);
      pixels.setPixelColor(infinitypixels[i],0);
      }
  }

void sirens()
  {
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(((offset + i) & 7) < 2) c = 0xFF0000;  // 4 red pixels on
      pixels.setPixelColor(   i, c);            // First eye
      if(((offset + i) & 7) < 2) c = 0x0000FF;  // 4 blue pixels on...
      pixels.setPixelColor(realign(31-i-4), c);  // Second eye (flipped)
    }
    pixels.show();
    offset++;
    delay(speed_slow);
  }

/*

//  Wave Scanner
void wave_scanner()
  { 
  int scanner1[] = {14,15, 0, 1,2,3,4,5,31,16,17,18,19,20,21,22}; // got to be a better way...
  int scanner2[] = {13,12,11,10,9,8,7,6,30,29,28,27,26,25,24,23}; // got to be a better way...
  for (i=0;i<16;i++)
    { 
    pixels.setPixelColor(scanner1[i],color);
    pixels.setPixelColor(scanner2[i],color);
    pixels.show();
    delay(speed_slow);
    pixels.setPixelColor(scanner1[i],0);
    pixels.setPixelColor(scanner2[i],0);
    }
  for (i=15;i>=0;i--)
    { 
    pixels.setPixelColor(scanner1[i],color);
    pixels.setPixelColor(scanner2[i],color);
    pixels.show();
    delay(speed_slow);
    pixels.setPixelColor(scanner1[i],0);
    pixels.setPixelColor(scanner2[i],0);
    }
  }

//  Larson Scanner
void larson_scanner()
  {
    int scanner[] = {13,5,30,23,23,30,5,13};  //  It uses less memory to just delcare it this way...
    for(i=0;i<8;i++)
      {
      pixels.setPixelColor(scanner[i],color);
      pixels.show();
      delay(100);
      pixels.setPixelColor(scanner[i],0);
      pixels.show();
      }
  }

//  Right side spins red clockwise, left side spins blue counterclockwise - very cool :)
void sirens()
  {
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(((offset + i) & 7) < 2) c = 0xFF0000;  // 4 red pixels on
      pixels.setPixelColor(   i, c);            // First eye
      if(((offset + i) & 7) < 2) c = 0x0000FF;  // 4 blue pixels on...
      pixels.setPixelColor(realign(31-i-4), c);  // Second eye (flipped)
    }
    pixels.show();
    offset++;
    delay(speed_med);
  }

//  Single LED spinning in an infinity pattern across both lenses
void infinity_scaner()
  {
    for(i=0;i<32;i++)
      {
      int infinitypixels[] = {6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,29,28,27,26,25,24,23,22,21,20,19,18,17,16,31,30};
      pixels.setPixelColor(infinitypixels[i],color);
      pixels.show();
      delay(25);
      pixels.setPixelColor(infinitypixels[i],0);
      pixels.show();
      }
  }

//  Only one LED sparkling
void sparks()
  {
    i = random(32);
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(20);
    pixels.setPixelColor(i, 0);
    break;
  }

//  Spinning wheels, properly aligned
void spin_wheels()
  {
    for(i=0; i<16; i++) {
      uint32_t c = 0;
      if(((offset + i) & 7) < 2) c = color; // 4 pixels on...
      pixels.setPixelColor(   i, c); // First eye
      pixels.setPixelColor(realign(31-i-4), c); // Second eye (flipped)
    }
    pixels.show();
    offset++;
    delay(speed_slow);
  }

//  Flashes one light, delays, then the other - strobe(speed_slow,300); pixels.setBrightness(default_brightness); 
void strobe(int strobeon, int strobeoff)
  {
  pixels.setBrightness(255); 
  for (i=0;i<16;i++)
    { pixels.setPixelColor(i,0xFFFFFF); pixels.setPixelColor(i+16,0);  }  //  Right on, Left off
  pixels.show(); delay(strobeon);
  for (i=0;i<16;i++) 
    { pixels.setPixelColor(i,0); pixels.setPixelColor(i+16,0);  } //  All off
  pixels.show(); delay(strobeoff);
  for (i=0;i<16;i++)
     { pixels.setPixelColor(i+16,0xFFFFFF); pixels.setPixelColor(i,0);  }  //  Right off, Left on
  pixels.show(); delay(strobeon);
  for (i=0;i<16;i++) 
    { pixels.setPixelColor(i,0); pixels.setPixelColor(i+16,0);  } //  All off
  pixels.show(); delay(strobeoff);
  }
*/