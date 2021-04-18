/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <FastLED.h>

/**** LED Setup ***/
#define TEMPERATURE OvercastSky
// FastLED provides these pre-conigured incandescent color profiles:
//     Candle, Tungsten40W, Tungsten100W, Halogen, CarbonArc,
//     HighNoonSun, DirectSunlight, OvercastSky, ClearBlueSky,
// FastLED provides these pre-configured gaseous-light color profiles:
//     WarmFluorescent, StandardFluorescent, CoolWhiteFluorescent,
//     FullSpectrumFluorescent, GrowLightFluorescent, BlackLightFluorescent,
//     MercuryVapor, SodiumVapor, MetalHalide, HighPressureSodium,

#define LED_PIN     4

// Information about the LED strip itself
#define NUM_LEDS    30
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS+5];
int count = 1;
int programms = 3;
int brightness = 15;
//const char* str_array[] = {
//  "Init",
//  "Regenbogen",
//  "Lauflicht",
//  "MinzSinus"
//  };
String prog_name = "init";

/**** WiFi Setup ***/
// Replace with your network credentials
const char* ssid = "Datensumpf";
const char* password = "73159062725671256796";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String LED_state = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//**************************** setup() ******************************
void setup() {

  // *** LEDs ***
  Serial.begin(115200);
  // attachInterrupt(digitalPinToInterrupt(FNK_SW), my_interrupt_handler, CHANGE);
  delay( 3000 );
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( 1 );
  FastLED.setTemperature( TEMPERATURE );
  Serial.print("setup completed");
  delay(500);
  
  // *** WiFi ***
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}
//************************ loop() ****************************************
void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";              
              Plus();
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              LED_state = "on";
              brightness = 15;
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              LED_state = "off";
              brightness = 0;
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #555555; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #4CAF50;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Change program " + prog_name + "</p>");
            // If the LED_state is off, it displays the ON button       
            if (LED_state=="on") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>Power " + LED_state + "</p>");
            // If the LED_state is off, it displays the ON button       
            if (LED_state=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  FastLED.setBrightness(brightness);
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
}

// *** Funktionen ****

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

void Plus() {
  count++ ;
  if (count > programms) {
    count = 1;
  }
  switch(count){
    case 1: (prog_name = "Regenbogen"); break;
    case 2: (prog_name = "Lauflicht"); break;
    case 3: (prog_name = "MinzSinus"); break;
    default: (prog_name = "Häh?"); break;
  }
}







/*Probleme :
 * Adc
 * Pinvergabe: LED/taster
 * interrupt
 */
