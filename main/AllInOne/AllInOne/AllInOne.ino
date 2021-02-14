// FastLED provides these pre-conigured incandescent color profiles:
//     Candle, Tungsten40W, Tungsten100W, Halogen, CarbonArc,
//     HighNoonSun, DirectSunlight, OvercastSky, ClearBlueSky,
// FastLED provides these pre-configured gaseous-light color profiles:
//     WarmFluorescent, StandardFluorescent, CoolWhiteFluorescent,
//     FullSpectrumFluorescent, GrowLightFluorescent, BlackLightFluorescent,
//     MercuryVapor, SodiumVapor, MetalHalide, HighPressureSodium,

#include <FastLED.h>
#define LED_PIN     3
#define FNK_SW      2
#define TEMPERATURE OvercastSky
int potPin = 2;   
int pot = 0;

// Information about the LED strip itself
#define NUM_LEDS    30
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS+5];
int count = 3;
int programms = 3;
bool sw = 1;


void setup() {

  Serial.begin(115200);
  pinMode(FNK_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FNK_SW), my_interrupt_handler, CHANGE);
  delay( 3000 );
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( 1 );
  FastLED.setTemperature( TEMPERATURE );
  Serial.print("setup completed");
  delay(500);
}

void loop()
{
   pot = analogRead(potPin);
   FastLED.setBrightness(pot/4);
  if (count == 1) {
    myRainbow();
    //Abfrage();
  }

  if (count == 2) {
    WeissBlink();
  }

  if (count == 3) {
    GruenLed();
    //Abfrage();
  }

  if (count > programms) {
    count = 1;
  }
  FastLED.delay(35);

  
}


void myRainbow() {
  //macht halt regenbögen?!?!
  static uint8_t starthue = 0;
  fill_rainbow( leds , NUM_LEDS, --starthue, 10);
  FastLED.show();
  FastLED.delay(35);

}
void GruenLed () {
fill_solid(leds , NUM_LEDS, CRGB(77, 217, 61));
  //FastLED.clear();
  for (int i = 0; i < NUM_LEDS ; i++) {
    // Turn our current led on to white, then show the leds
    leds[i].nscale8(quadwave8(i*16));
    }
  FastLED.show();
  delay(50);

}
void WeissBlink() {
  //Das Licht rennt weg!!
  for (int whiteLed = 0; whiteLed < NUM_LEDS ; whiteLed++) {
    // Turn our current led on to white, then show the leds
    leds[whiteLed].nscale8(48);
    leds[whiteLed + 1] = CRGB::White;
    leds[whiteLed + 2] = CRGB::White;
    leds[whiteLed + 3] = CRGB::White;
    leds[whiteLed + 4] = CRGB::White;
    leds[whiteLed + 5] = CRGB::White;
    FastLED.show();
    delay(50);

  }
}
void my_interrupt_handler() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 300)
  {
    Plus();
  }
  last_interrupt_time = interrupt_time;
}

void Plus() {
  count++ ;
}
