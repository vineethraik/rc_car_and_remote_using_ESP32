#include "libs.h"



WiFiServer serv(23);
WiFiClient client;

class telnet {
public:
  void init();
  int available();
  String read();
  void print(String);
  void println(String);
  void handle();
} telnet;

void telnet::init() {
  serv.begin();
  serv.setNoDelay(true);
}

void telnet::print(String s) {
  if (WiFi.status() == WL_CONNECTED) {
    if (serv.hasClient()) {
      if (!client || !client.connected()) {
        if (client) client.stop();
        client = serv.available();
        client.write(s.c_str());
      } else {
        client.stop();
        client = serv.available();
        client.write(s.c_str());
      }
    } else {
      if (client && client.connected()) {
        client.write(s.c_str());
      }
    }
  }
}

void telnet::println(String s) {
  print(s + "\n\r");
}

void telnet::handle() {
  if (WiFi.status() == WL_CONNECTED) {
    if (serv.hasClient()) {
      if (!client || !client.connected()) {
        if (client) client.stop();
        client = serv.available();
        client.write("Connected\n\r");
        client.write("ip: ");
        client.write(WiFi.localIP().toString().c_str());
      }
    }
  }
}

int telnet::available() {
  if (client && client.connected()) {
    return client.available();
  } else {
    return -1;
  }
}

String telnet::read() {
  String s="";
  while(client.available()){
    s+=char(client.read());
  }
  return s;
}