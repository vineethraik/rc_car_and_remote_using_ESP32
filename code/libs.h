
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncUDP.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>

#include <ArduinoJson.h>


#ifndef declaration_first
#define declaration_first
enum {CAR,REMOTE};
AsyncUDP udp;
#endif

#ifndef car_h
#define car_h
#include "car.h"
#endif

#ifndef remote_h
#define remote_h
#include "remote.h"
#endif


#ifndef OTA_h
#define OTA_h
#include "OTA.h"
#endif

#ifndef telnet_h
#define telnet_h
#include "telnet.h"
#endif

#ifndef credentials_h
#define credentials_h
#include "credentials.h"
#endif

using namespace std;

#ifndef declaration
#define declaration
WiFiMulti wifiMulti;
#endif

