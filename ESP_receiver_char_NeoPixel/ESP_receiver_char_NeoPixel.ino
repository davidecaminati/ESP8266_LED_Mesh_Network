/* Libraries */
// Arduinojson 6.11.4
// NeoPixelBus by Makuna  2.5.0
// Painless Mesh by Coopdis ...   1.4.2
// TaskerScheduler by Anatoli  3.0.2

/*  Board */
// tested with WEEMOS D1 R2 & mini in default configuration


/* Pins */
// Signal for Neopixel RX pin (second pin from top,  right side)
// GND pin marked as G (second pin from bottom,  right side)

/* NOTE */
// GND must be in common Neopixel and MCU

#include "painlessMesh.h"
#include <NeoPixelBus.h>

const uint16_t PixelCount = 3; // this example assumes 4 pixels, making it smaller will cause a failure

uint16_t colorSaturation = 5; // intensity

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  
  if (msg == "0"){
    colorSaturation = 0;
  }
  if (msg == "1"){
    colorSaturation = 10;
  }
  if (msg == "2"){
    colorSaturation = 20;
  }
  
  if (msg == "a"){
    //RgbColor red(colorSaturation, 0, 0);
    //RgbColor green(0, colorSaturation, 0);
    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, black);
    strip.SetPixelColor(2, black);
    strip.Show();
  }

  if (msg == "b"){
    //RgbColor red(colorSaturation, 0, 0);
    //RgbColor green(0, colorSaturation, 0);
    strip.SetPixelColor(0, black);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, black);
    strip.Show();
  }

}

void newConnectionCallback(uint32_t nodeId) {
    //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    //Serial.printf("PIPPO - > ");

}

void changedConnectionCallback() {
    //Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  //Serial.begin(115200);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();
}

void loop() {
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}