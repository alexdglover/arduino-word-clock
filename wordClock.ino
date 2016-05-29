#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_DATA_PIN 6
#define Plus5MinuteButtonPin 4
#define Minus5MinuteButtonPin 2
#define ColorButtonPin 8
uint32_t c;

//Use this variable button to determine if button is being held
static unsigned long ColorButtonLastPressed = 0;

int word_IT[]={0};
#define word_IT_length 1
int word_IS[]={2};
#define word_IS_length 1
int word_TENm[]={4,5};
#define word_TENm_length 2
int word_HALF[]={6,7,8};
#define word_HALF_length 3
int word_TWENTY[]={9,10,11,12};
#define word_TWENTY_length 4
int word_QUARTER[]={13,14,15,16,17};
#define word_QUARTER_length 5
int word_FIVEm[]={18,19,20};
#define word_FIVEm_length 3
int word_MINUTES[]={21,22,23,24,25};
#define word_MINUTES_length 5
int word_TWO[]={27,28};
#define word_TWO_length 2
int word_ONE[]={29,30};
#define word_ONE_length 2
int word_TO[]={32};
#define word_TO_length 1
int word_PAST[]={33,34,35};
#define word_PAST_length 3
int word_THREE[]={36,37,38};
#define word_THREE_length 3
int word_FOUR[]={39,40,41};
#define word_FOUR_length 3
int word_FIVE[]={42,43,44};
#define word_FIVE_length 3
int word_EIGHT[]={45,46,47,48};
#define word_EIGHT_length 4
int word_SEVEN[]={49,50,51};
#define word_SEVEN_length 3
int word_SIX[]={52,53};
#define word_SIX_length 2
int word_NINE[]={54,55,56};
#define word_NINE_length 3
int word_TEN[]={57,58};
#define word_TEN_length 2
int word_ELEVEN[]={59,60,61,62};
#define word_ELEVEN_length 4
int word_OCLOCK[]={63,64,65,66};
#define word_OCLOCK_length 4
int word_TWELVE[]={68,69,70,71};
#define word_TWELVE_length 4

int heartbeat_LED[] = {67};
#define heartbeat_LED_length 1

int i;
int  hour=12, minute=55, second=00;
static unsigned long msTick =0;  // the number of Millisecond Ticks since we last 
                                 // incremented the second counter
int  count;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino LED_DATA_PIN number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(72, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  Serial.begin(9600);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  pinMode(Plus5MinuteButtonPin, INPUT_PULLUP); 
  pinMode(Minus5MinuteButtonPin, INPUT_PULLUP); 
  pinMode(ColorButtonPin, INPUT_PULLUP); 

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  c = strip.Color(0, 0, 255); // Start with blue

  Serial.println("Beginning strand test");
  strandTest();
  turnLedsOff();
  paintWord(heartbeat_LED, heartbeat_LED_length);
  delay(250);
  
  displayTime();
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  //uint32_t c = strip.Color(255, 0, 0);

  if ( millis() - msTick >999) {
    Serial.println("A second has slapsed");
    Serial.print("Numeric time is: ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
    msTick=millis();
    second++;
    // Flash the onboard Pin13 Led so we know something is hapening!
    //paintWord(heartbeat_LED, heartbeat_LED_length);
    strip.setPixelColor(67, strip.Color(255, 0, 0));
    strip.show();
    delay(100);
    strip.setPixelColor(67, strip.Color(0, 0, 0));
    strip.show();
  }
    
  //test to see if we need to increment the time counters
  if (second==60) 
  {
    incrementTime();
    displayTime();
  }

  if ( (digitalRead(Plus5MinuteButtonPin) == 0 ) ) 
  // the forward button is down
  // and it has been more than one second since we
  // last looked
  {
    minute=(((minute/5)*5) +4); // We only want to add 4 here, as increment time will add another minute 
    second=0;
    incrementTime();
    second++;  // Increment the second counter to ensure that the name
    // flash doesnt happen when setting time
    displayTime();
  }

  if ( (digitalRead(Minus5MinuteButtonPin) == 0 ) ) 
  // the forward button is down
  // and it has been more than one second since we
  // last looked
  {
    minute=(((minute/5)*5) -6); // We need to subtract 6 here, as increment time will add another minute 
    if (minute < -1){
      minute = 54;
      hour--;
      if (hour < 1){
        hour = 12;
      }
    }
    second=0;
    incrementTime();
    second++;  // Increment the second counter to ensure that the name
    // flash doesnt happen when setting time
    displayTime();
  }

  if ( (digitalRead(ColorButtonPin) == 0 )) 
  // the color button is down
  // and it has been more than one second since we
  // last looked
  {
    uint32_t this_c = strip.Color(255, 0, 0);
    if (c == strip.Color(0, 0, 255)){ // If it's already blue...
      Serial.println("Changed color to green");
      this_c = strip.Color(0, 255, 0); // set it to green
    }
    if (c == strip.Color(0, 255, 0)){ //If it's already green...
      Serial.println("Changed color to red");
      this_c = strip.Color(255, 0, 0); // set it to red
    }
    if (c == strip.Color(255, 0, 0)){ // If it's already red...
      Serial.println("Changed color to white");
      this_c = strip.Color(255, 255, 255); // set it to white
    }
    if (c == strip.Color(255, 255, 255)){ // If it's already white...
      Serial.println("Changed color to blue");
      this_c = strip.Color(0, 0, 255); // set it back to blue
    }
    ColorButtonLastPressed = millis();
    c = this_c;
    displayTime();
    
    // Try to remove button jitters
    delay(200);
  }
}

void incrementTime(){
  // increment the time counters keeping care to rollover as required
  second=0;
  if (++minute >= 60) {
    minute=0;
    if (++hour == 13) {
      hour=1;  
    }
  }
}


void turnLedsOff(){
  colorWipe(strip.Color(0, 0, 0), 0); // Turn all LEDs off as fast as possible
}

void paintWord(int wordArray[], int wordLength) {
  
  for (i = 0; i < wordLength; i++){
//    Serial.println("Word array has length: ");
//    Serial.println(wordLength);
//    Serial.println("Painting pixel # ");
//    Serial.println(wordArray[i]);
    strip.setPixelColor(wordArray[i], c);
    //delay(5);
  }
  strip.show();
}

void displayTime(void){

  // start by clearing the display to a known state
  turnLedsOff();
  Serial.print("Numeric time is: ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  int thisHour = hour;

  paintWord(word_IT, word_IT_length);
  paintWord(word_IS, word_IS_length);
  Serial.print("It is ");

  // now we display the appropriate minute counter
  if ((minute>4) && (minute<10)) { 
    paintWord(word_FIVEm, word_FIVEm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Five Minutes ");
  } 
  if ((minute>9) && (minute<15)) { 
    paintWord(word_TENm, word_TENm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Ten Minutes ");
  }
  if ((minute>14) && (minute<20)) {
    paintWord(word_QUARTER, word_QUARTER_length);
      Serial.print("Quarter ");
  }
  if ((minute>19) && (minute<25)) { 
    paintWord(word_TWENTY, word_TWENTY_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Twenty Minutes ");
  }
  if ((minute>24) && (minute<30)) { 
    paintWord(word_TWENTY, word_TWENTY_length);
    paintWord(word_FIVEm, word_FIVEm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Twenty Five Minutes ");
  }  
  if ((minute>29) && (minute<35)) {
    paintWord(word_HALF, word_HALF_length);
    Serial.print("Half ");
  }
  if ((minute>34) && (minute<40)) { 
    paintWord(word_TWENTY, word_TWENTY_length);
    paintWord(word_FIVEm, word_FIVEm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Twenty Five Minutes ");
  }  
  if ((minute>39) && (minute<45)) { 
    paintWord(word_TWENTY, word_TWENTY_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Twenty Minutes ");
  }
  if ((minute>44) && (minute<50)) {
    paintWord(word_QUARTER, word_QUARTER_length);
    Serial.print("Quarter ");
  }
  if ((minute>49) && (minute<55)) { 
    paintWord(word_TENm, word_TENm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Ten Minutes ");
  } 
  if (minute>54) { 
    paintWord(word_FIVEm, word_FIVEm_length);
    paintWord(word_MINUTES, word_MINUTES_length);
    Serial.print("Five Minutes ");
  }
  if ((minute < 35) && (minute >4)){
    paintWord(word_PAST, word_PAST_length);
    Serial.print("Past ");
  }
  else if ((minute >= 35) && (minute <=59)){
    paintWord(word_TO, word_TO_length);
    thisHour++;
    if (thisHour > 12){
      thisHour = 1;
    }
    Serial.print("To ");
  }

  // Light up the correct word for the current hour
  switch (thisHour) {
    case 1:
      paintWord(word_ONE, word_ONE_length);
      Serial.print("One ");
      break;
    case 2:
      paintWord(word_TWO, word_TWO_length);
      Serial.print("Two ");
      break;
    case 3:
      paintWord(word_THREE, word_THREE_length);
      Serial.print("Three ");
      break;
    case 4:
      paintWord(word_FOUR, word_FOUR_length);
      Serial.print("Four");
      break;
    case 5:
      paintWord(word_FIVE, word_FIVE_length);
      Serial.print("Five ");
      break;
    case 6:
      paintWord(word_SIX, word_SIX_length);
      Serial.print("Six ");
      break;
    case 7:
      paintWord(word_SEVEN, word_SEVEN_length);
      Serial.print("Seven ");
      break;
    case 8:
      paintWord(word_EIGHT, word_EIGHT_length);
      Serial.print("Eight ");
      break;
    case 9:
      paintWord(word_NINE, word_NINE_length);
      Serial.print("Nine ");
      break;
    case 10:
      paintWord(word_TEN, word_TEN_length);
      Serial.print("Ten ");
      break;
    case 11:
      paintWord(word_ELEVEN, word_ELEVEN_length);
      Serial.print("Eleven ");
      break;
    case 12:
      paintWord(word_TWELVE, word_TWELVE_length);
      Serial.print("Twelve ");
      break;
    case 0:
      paintWord(word_TWELVE, word_TWELVE_length);
      Serial.print("Twelve ");
      break;
  }

  if (minute <= 4){
    paintWord(word_OCLOCK, word_OCLOCK_length);
    Serial.print("o'clock");
  }
  Serial.println("");
}

void strandTest(){
    paintWord(word_IT, word_IT_length);
  delay(100);
  turnLedsOff();
  paintWord(word_IS, word_IS_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TENm, word_TENm_length);
  delay(100);
  turnLedsOff();
  paintWord(word_HALF, word_HALF_length);
  delay(100);
  turnLedsOff();
  paintWord(word_QUARTER, word_QUARTER_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TWENTY, word_TWENTY_length);
  delay(100);
  turnLedsOff();
  paintWord(word_FIVEm, word_FIVEm_length);
  delay(100);
  turnLedsOff();
  paintWord(word_MINUTES, word_MINUTES_length);
  delay(100);
  turnLedsOff();
  paintWord(word_PAST, word_PAST_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TO, word_TO_length);
  delay(100);
  turnLedsOff();
  paintWord(word_ONE, word_ONE_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TWO, word_TWO_length);
  delay(100);
  turnLedsOff();
  paintWord(word_THREE, word_THREE_length);
  delay(100);
  turnLedsOff();
  paintWord(word_FOUR, word_FOUR_length);
  delay(100);
  turnLedsOff();
  paintWord(word_FIVE, word_FIVE_length);
  delay(100);
  turnLedsOff();
  paintWord(word_SIX, word_SIX_length);
  delay(100);
  turnLedsOff();
  paintWord(word_SEVEN, word_SEVEN_length);
  delay(100);
  turnLedsOff();
  paintWord(word_EIGHT, word_EIGHT_length);
  delay(100);
  turnLedsOff();
  paintWord(word_NINE, word_NINE_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TEN, word_TEN_length);
  delay(100);
  turnLedsOff();
  paintWord(word_ELEVEN, word_ELEVEN_length);
  delay(100);
  turnLedsOff();
  paintWord(word_TWELVE, word_TWELVE_length);
  delay(100);
  turnLedsOff();
  paintWord(word_OCLOCK, word_OCLOCK_length);
  delay(100);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
