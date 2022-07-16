#define max_adc 4095
#define center 1850
#define remote_base_buffer 150

#define y_remote_pin 34
#define x_remote_pin 35

#define horn_button_pin 32
#define left_indicator_button_pin 33
#define right_indicator_button_pin 27
#define headlight_button_pin 14





#include "libs.h"

enum ERROR_CODE { NO_ERROR,
                  WIFI_DISCONNECTED };
int error_state = WIFI_DISCONNECTED;
TaskHandle_t error_handle_task;

void error_led_handler(void* param) {
  while (true) {
    yield();
    static unsigned long t_now;
    static int delay = 0;
    static bool error = false;
    static bool led_state = false;
    switch (error_state) {
      case NO_ERROR:
        error = false;
        break;
      case WIFI_DISCONNECTED:
        error = true;
        delay = 250;
        break;
    }
    if (error) {
      if (millis() - t_now > delay) {
        t_now = millis();
        led_state = !led_state;
        digitalWrite(LED_BUILTIN, led_state);
      }
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

class remote {
public:
  void init();
  void handle();
  void send_remote_data();
} remote;

void remote::init() {
  pinMode(y_remote_pin,INPUT);
  pinMode(x_remote_pin,INPUT);
  pinMode(horn_button_pin,INPUT_PULLUP);
  pinMode(left_indicator_button_pin,INPUT_PULLUP);
  pinMode(right_indicator_button_pin,INPUT_PULLUP);
  pinMode(headlight_button_pin,INPUT_PULLUP);

  xTaskCreatePinnedToCore(
    error_led_handler,
    "Task2",
    10000,
    NULL,
    1,
    &error_handle_task,
    1
  );

 


  WiFi.mode(WIFI_STA);

  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  //WiFi.begin(CAR_SSID, CAR_PSK);
  wifiMulti.addAP(CAR_SSID, CAR_PSK);
  //WiFi.softAP(APSSID, APPSK);

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi Failed");

    delay(1000);
  }

  

  if (udp.connect(IPAddress(192, 168, 4, 1), 1234)) {
    Serial.println("UDP connected");
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      //reply to the client
      //packet.printf("Got %u bytes of data", packet.length());
    });
    //Send unicast
    udp.print("Hello Server!");
  }
}

void remote::handle() {
  static unsigned long current_time = 0;

  while (wifiMulti.run() != WL_CONNECTED) {
    error_state = WIFI_DISCONNECTED;
    yield();
  }
  error_state = NO_ERROR;

  if (millis() - current_time > 50) {
    current_time = millis();
    send_remote_data();
  }
}

void remote::send_remote_data() {
  DynamicJsonDocument doc(250);  // used to convert all data into json format
  
  //int x_value = random(0, max_adc);
  //int y_value = random(0, max_adc);
  int y_value = analogRead(y_remote_pin);
  int x_value = analogRead(x_remote_pin);

  doc["horn"]=!digitalRead(horn_button_pin);
  doc["left_indicator"]=!digitalRead(left_indicator_button_pin);
  doc["right_indicator"]=!digitalRead(right_indicator_button_pin);
  doc["headlight"]=!digitalRead(headlight_button_pin);



  Serial.print(x_value);
  Serial.print(":");
  Serial.println(y_value);

  String cmd = "";
  int speed;

  if ((x_value>((center)+remote_base_buffer))&&(y_value>((center)+remote_base_buffer))) {
    cmd="FR";
    speed=max(map(y_value,((center)+remote_base_buffer),max_adc,0,100),map(y_value,0,((center)-remote_base_buffer),100,0));
  } else if ((x_value>((center)+remote_base_buffer))&&(y_value<((center)-remote_base_buffer))) {
    cmd="BR";
    speed=max(map(y_value,((center)-remote_base_buffer),0,100,0),map(y_value,0,((center)-remote_base_buffer),100,0));
  } else if ((x_value<((center)-remote_base_buffer))&&(y_value>((center)+remote_base_buffer))) {
    cmd="FL";
    speed=max(map(y_value,((center)+remote_base_buffer),max_adc,0,100),map(x_value,0,((center)-remote_base_buffer),100,0));
  } else if ((x_value<((center)-remote_base_buffer))&&(y_value<((center)-remote_base_buffer))) {
    cmd="BL";
    speed=max(map(y_value,((center)-remote_base_buffer),0,100,0),map(x_value,0,((center)-remote_base_buffer),100,0));
  } else if (x_value>((center)+remote_base_buffer)) {
    cmd="R";
    speed=map(x_value,((center)+remote_base_buffer),max_adc,0,100);
  } else if (x_value<((center)-remote_base_buffer)) {
    cmd="L";
    speed=map(x_value,0,((center)-remote_base_buffer),100,0);
  } else if (y_value>((center)+remote_base_buffer)) {
    cmd="F";
    speed=map(y_value,((center)+remote_base_buffer),max_adc,0,100);
  } else if (y_value<((center)-remote_base_buffer)) {
    cmd="B";
    speed=map(y_value,0,((center)-remote_base_buffer),100,0);
  } else {
    cmd="S";
    speed=0;
  }

/*
speed=map((max((abs(x_value-(center))-remote_base_buffer),(abs(y_value-(center))-remote_base_buffer))),0,((center)-remote_base_buffer),0,100);

  if(cmd.equals("F")){
    speed=map(y_value,((center)+remote_base_buffer),max_adc,0,100);
  }else if(cmd.equals("L")){
    speed=map(x_value,0,((center)-remote_base_buffer),100,0);
  }else if(cmd.equals("R")){
    speed=map(x_value,((center)+remote_base_buffer),max_adc,0,100);
  }else if(cmd.equals("B")){
    speed=map(y_value,0,((center)-remote_base_buffer),100,0);
  }else if(cmd.equals("BL")){
    speed=max(map(y_value,((center)-remote_base_buffer),0,100,0),map(x_value,0,((center)-remote_base_buffer),100,0));
  }else if(cmd.equals("BR")){
    speed=max(map(y_value,((center)-remote_base_buffer),0,100,0),map(y_value,0,((center)-remote_base_buffer),100,0));
  }else if(cmd.equals("FL")){
    speed=max(map(y_value,((center)+remote_base_buffer),max_adc,0,100),map(x_value,0,((center)-remote_base_buffer),100,0));
  }else if(cmd.equals("FR")){
    speed=max(map(y_value,((center)+remote_base_buffer),max_adc,0,100),map(y_value,0,((center)-remote_base_buffer),100,0));
  }else{
    speed=0;
  }

  /**/



  doc["type"]="data";
  doc["cmd"]=cmd;
  doc["speed"]=speed;
  


  


  String temp="";
  serializeJson(doc, temp);

  udp.print(temp);

}












