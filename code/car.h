#define horn_pin 13
#define left_indicator_pin 23
#define right_indicator_pin 22
#define headlight_pin 21
#define breaklight_pin 19

#define left_enable_pin 27
#define right_enable_pin 14
#define left_p_pin 32
#define left_n_pin 33
#define right_p_pin 25
#define right_n_pin 26

#include "libs.h"

unsigned long last_packet_time = 0;
int max_lag_delay = 1000;

bool horn = false;
unsigned long horn_time_now = 0;
#define horn_wait_time 100

const bool led_inverted = true;

bool headlight = false;

bool left_indicator = false;

bool right_indicator = false;

bool breaklight = false;
unsigned long break_time_now = 0;
#define break_wait_delay 1000


class car {

  void handle_car(String);
  void move_forword();
  void move_backword();
  void move_left();
  void move_right();
  void move_forword_left();
  void move_forword_right();
  void move_backword_left();
  void move_backword_right();
  void stop();

public:
  void init();
  void handle();
  static void process_data(String);
} car;

void car::init() {
  pinMode(left_enable_pin, OUTPUT);
  pinMode(right_enable_pin, OUTPUT);
  pinMode(left_p_pin, OUTPUT);
  pinMode(left_n_pin, OUTPUT);
  pinMode(right_p_pin, OUTPUT);
  pinMode(right_n_pin, OUTPUT);


  pinMode(horn_pin, OUTPUT);
  pinMode(left_indicator_pin, OUTPUT);
  pinMode(right_indicator_pin, OUTPUT);
  pinMode(headlight_pin, OUTPUT);
  pinMode(breaklight_pin, OUTPUT);


  WiFi.mode(WIFI_AP);
  //WiFi.begin(ssid, password);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.softAP(CAR_SSID, CAR_PSK);
  delay(2000);
  if (udp.listen(UDP_server_port)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.softAPIP());
    udp.onPacket([](AsyncUDPPacket packet) {
      //Serial.print("Length: ");
      //Serial.print(packet.length());
      //Serial.print(", Data: ");
      //Serial.write(packet.data(), packet.length());
      //Serial.println();
      //reply to the client
      //packet.printf("Got %u bytes of data", packet.length());
      process_data(String((char*)packet.data()));
      last_packet_time = millis();
    });
  }
}

void car::handle() {
  static unsigned long current_time = 0;
  Serial.print("back:");
  Serial.println(breaklight);

  if (millis() - last_packet_time > max_lag_delay) {
    stop();
  }

  if (horn) {
    digitalWrite(horn_pin, HIGH);
    if (millis() - horn_time_now > horn_wait_time) {
      horn = false;
      digitalWrite(horn_pin, LOW);
    }
  }else{
    digitalWrite(horn_pin, LOW);
  }

  if (headlight) {
    digitalWrite(headlight_pin, HIGH ^ led_inverted);
  } else {
    digitalWrite(headlight_pin, LOW ^ led_inverted);
  }

  if (left_indicator) {
    digitalWrite(left_indicator_pin, HIGH ^ led_inverted);
  } else {
    digitalWrite(left_indicator_pin, LOW ^ led_inverted);
  }

  if (right_indicator) {
    digitalWrite(right_indicator_pin, HIGH ^ led_inverted);
  } else {
    digitalWrite(right_indicator_pin, LOW ^ led_inverted);
  }

  if (breaklight) {
    digitalWrite(breaklight_pin, HIGH ^ led_inverted);
    if (millis() - break_time_now > break_wait_delay) {
      breaklight = false;
      digitalWrite(breaklight_pin, LOW ^ led_inverted);
    }
  }
}

void car::process_data(String data) {
  DynamicJsonDocument doc(250);

  if (!deserializeJson(doc, data)) {  //checking for proper data
    String type = doc["type"];
    if (type.equals("data")) {
      car temp;
      temp.handle_car(data);


    } else {
      Serial.println("Non DATA type recieved");
    }

  } else {
    Serial.println("BAD DATA");
  }
}

void car::handle_car(String data) {
  DynamicJsonDocument doc(250);
  deserializeJson(doc, data);
  String cmd = doc["cmd"];
  int speed = doc["speed"];
  //cmd="fdxvc";
  static bool headlight_last_state = false;
  static bool left_indicator_last_state = false;
  static bool right_indicator_last_state = false;

  static bool break_light_lock=false;



  analogWrite(left_enable_pin, (255 * speed) / 100);
  analogWrite(right_enable_pin, (255 * speed) / 100);

  if (cmd.equals("F")) {
    move_forword();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("B")) {
    move_backword();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("L")) {
    move_left();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("R")) {
    move_right();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("FL")) {
    move_forword_left();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("FR")) {
    move_forword_right();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("BL")) {
    move_backword_left();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("BR")) {
    move_backword_right();
    break_light_lock=false;
    breaklight = false;
  } else if (cmd.equals("S")) {
    stop();
    if(!break_light_lock){
      break_light_lock=true;
      breaklight=true;
      break_time_now=millis();
    }
  }

  if (doc["horn"]) {
    horn = true;
    horn_time_now = millis();
  }

  if ((headlight_last_state)&&(!doc["headlight"])) {
    headlight=!headlight;
  }
  headlight_last_state=doc["headlight"];

  if(left_indicator_last_state&&(!doc["left_indicator"])){
    if(left_indicator){
      left_indicator=false;
    }else{
      left_indicator=true;
      right_indicator=false;
    }
  }
  left_indicator_last_state=doc["left_indicator"];

  if(right_indicator_last_state&&(!doc["right_indicator"])){
    if(right_indicator){
      right_indicator=false;
    }else{
      left_indicator=false;
      right_indicator=true;
    }
  }
  right_indicator_last_state=doc["right_indicator"];
  

}

void car::move_forword() {

  digitalWrite(left_p_pin, true);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, true);
  digitalWrite(right_n_pin, false);
}

void car::move_backword() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, true);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, true);
}

void car::move_left() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, true);
  digitalWrite(right_p_pin, true);
  digitalWrite(right_n_pin, false);
}

void car::move_right() {
  digitalWrite(left_p_pin, true);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, true);
}

void car::move_forword_left() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, true);
  digitalWrite(right_n_pin, false);
}

void car::move_forword_right() {
  digitalWrite(left_p_pin, true);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, false);
}

void car::move_backword_left() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, true);
}

void car::move_backword_right() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, true);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, false);
}

void car::stop() {
  digitalWrite(left_p_pin, false);
  digitalWrite(left_n_pin, false);
  digitalWrite(right_p_pin, false);
  digitalWrite(right_n_pin, false);
}