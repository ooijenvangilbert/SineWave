// draw sine wave on LED Matrix screen
#include <FastLED.h>
#define FASTLED_INTERNAL

#define LED_PIN     3
#define NUM_LEDS    288
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_ROWS    18
#define NUM_COLS    16
CRGB leds[NUM_LEDS];

int led_pos=0;

// convert X,Y position to a linear array, with zigzag wiring
// position 1,1 is lower-left corner, first row
// first row wiring is left-to-right
// second row wiring is right-to-left
int LEDArrayPosition(int x, int y){
  // do some bounds checking 
  if (x>NUM_COLS) x=NUM_COLS;
  if (x<1) x=1;
  if (y>NUM_ROWS) y = NUM_ROWS;
  if (y<1) y=1;

  if (y%2==0){
    led_pos = ((y) * NUM_COLS) - x;  // even row
  } else {
    led_pos = x + ((y-1) * NUM_COLS) -1;  // odd row 
  }
  return led_pos;
}

// draw a single pixel on the matrix screen at specified color
void DrawPixel(uint8_t x, uint8_t y, CRGB pixelcolor){
  leds[LEDArrayPosition(x, y)] += pixelcolor;
}

// shift the whole LED matrix display right by 1 column
void ShiftLEDMatrixRight(uint8_t step=1){
  // shift to right
  for (int y=1; y<= NUM_ROWS; y++)
  {
    for (int x=NUM_COLS; x>=1; x--){
      leds[LEDArrayPosition(x, y)] = leds[LEDArrayPosition(x-1, y)];
    }
  }
  // clear first column to black
  for (int y=0;y<=NUM_ROWS; y++){
    leds[LEDArrayPosition(1, y)] = CRGB::Black;
  }
}


// shift the whole LED matrix display left by 1 column
void ShiftLEDMatrixLeft(uint8_t step=1){
  // shift right to left 
  for (int y=1; y<= NUM_ROWS; y++)
  {
    for (int x=1; x<NUM_COLS; x++){
      leds[LEDArrayPosition(x, y)] = leds[LEDArrayPosition(x+1, y)];
    }
  }
  // clear last column to black
  for (int y=0;y<=NUM_ROWS; y++){
    leds[LEDArrayPosition(NUM_COLS, y)] = CRGB::Black;
  }
}

void setup() {
  Serial.begin(115200);
  delay( 500 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );  
}


uint8_t loop_ndx=1;
uint8_t posX;
uint8_t posY;
uint8_t pos2Y;
uint8_t cosY;
uint8_t cos2Y;
uint8_t triY;
uint8_t hue;
uint8_t deltahue=4;

void loop() {

  hue = hue + deltahue;
  // draw scope grid/center line
  for (int i=1; i<=NUM_COLS; i++){
    // DrawPixel(i, NUM_ROWS/2 + 1, CHSV(hue, 200, 10));
    leds[LEDArrayPosition(i, NUM_ROWS/2 + 1)] = CHSV(hue, 200, 60);
  }

  // draw the sine wave
  // compute y values based on x values
  // when we reach the last column, shift left, and keep drawing on the last column
  uint8_t sineY = sin8_C(loop_ndx);       // sine: input theta 0..255; output 0..255
  uint8_t sine2Y = sin8_C(loop_ndx+128);  // sine: input theta 0..255; output 0..255

  uint8_t cosineY = cos8(loop_ndx);   // cosine: input theta 0..255; output 0..255
  uint8_t cosine2Y = cos8(loop_ndx+128);   // cosine: input theta 0..255; output 0..255
  
  uint8_t triangleY = triwave8(loop_ndx);   // cosine: input theta 0..255; output 0..255

  // map sineY (0..255) to posY (0..NUM_ROWS)
  posY = map(sineY, 0, 255, 1, NUM_ROWS);
  pos2Y = map(sine2Y, 0, 255, 1, NUM_ROWS);

  cosY = map(cosineY, 0, 255, 1, NUM_ROWS);
  cos2Y = map(cosine2Y, 0, 255, 1, NUM_ROWS);

  triY = map(triangleY, 0, 255, 1+5, NUM_ROWS-5);
  posX++;
  
  loop_ndx += 8;    // nice values: 6, 7, 9, [11], 12

  // if we reach edge of led matrix screen
  if (posX >= NUM_COLS){
    ShiftLEDMatrixLeft(1);
    posX = NUM_COLS;
    // draw last pixel of grid line
    // DrawPixel(NUM_COLS, NUM_ROWS/2 + 1, CHSV(hue, 200, 10));
    leds[LEDArrayPosition(NUM_COLS, NUM_ROWS/2 + 1)] = CHSV(hue, 200, 60);
  }
  // wrap around
  if (loop_ndx >= 255){
    loop_ndx = 1;
  }

  // draw the sinewave
  DrawPixel(posX, posY, CRGB::Green);
  DrawPixel(posX, pos2Y, CRGB::Cyan);

  // draw the cosinewave
  DrawPixel(posX, cosY, CRGB::Red);
  DrawPixel(posX, cos2Y, CRGB::Purple);

  // draw the triangular wave
  // DrawPixel(posX, triY, CRGB::White);

  // light them up!
  FastLED.show(); 

  // slow everything 
  delay(35);
}