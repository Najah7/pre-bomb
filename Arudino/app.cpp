#include "ESP8266.h"

ESP8266 wifi(Serial1);

// Wi-Fi SSID
#define SSID        "xxxx"//ルーターのSSID
// Wi-Fi PASSWORD
#define PASSWORD    "xxxx"//ルーターのパスワード
// サーバーのホスト名
#define HOST_NAME   "xxxx.com"//ホスト
// ポート番号
#define HOST_PORT   80

#define DISTANCE_THRESHOLD  15.0

#define C5 1046.502

const char sendLINE[] PROGMEM = "GET https://xxxx.com/send HTTP/1.0\r\nHost: xxxx.com\r\nUser-Agent: arduino\r\n\r\n";

float Vcc = 5.0;

void restartESP8266() {
  while (1) {
    Serial.print("Restarting ESP8266...");
    if (wifi.restart()) {
      Serial.println(" OK");
      break;
    } else {
      Serial.println(" Failed");
      Serial.print("Trying to kick...");
      while (1) {
        if (wifi.kick()) {
          Serial.println(" OK");
          break;
        } else {
          Serial.println(" Failed");
          Serial.println("Waiting 5 seconds and retrying...");
          delay(5000);
        }
      }
    }
  }
}

void setupWiFi() {
  Serial.begin(9600);
  Serial.println("Setup begin");

  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.println("Set mode to Station + SoftAP OK");
  } else {
    Serial.println("Set mode to Station + SoftAP Failed");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.println("Join AP success");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.println("Join AP failure");
  }

  if (wifi.disableMUX()) {
    Serial.println("Single OK");
  } else {
    Serial.println("Single Failed");
  }

  Serial.println("Setup end");
}

bool connectToServer() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.println("Create TCP OK");
    return true;
  } else {
    Serial.println("Create TCP Failed");
    return false;
  }
}

void sendRequest(const char* request) {
  char sendStr[280];
  strcpy_P(sendStr, request);
  wifi.send((const uint8_t*)sendStr, strlen(sendStr));
  Serial.print("Sent request: ");
  Serial.println(sendStr);
}

String receiveResponse() {
  uint8_t buffer[340] = {0};
  String response = "";
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);

  if (len > 0) {
    for (uint32_t i = 0; i < len; i++) {
      response += (char)buffer[i];
    }
  }

  response.trim();
  Serial.print("Received response: ");
  Serial.println(response);

  return response;
}

void processResponse(const String& response) {
  int lastLF = response.lastIndexOf('\n');
  int resultCodeLength = response.length();
  String resultString = response.substring(lastLF + 1, resultCodeLength);
  resultString.trim();

  Serial.print("Result string: ");
  Serial.println(resultString);
}

float measureDistance() {
  // Measure distance using the infrared sensor
  float dist1 = Vcc*analogRead(A0)/1023;
  float dist2 = 26.549 * pow(dist1, -1.2091);
  return dist2;
}

void setup() {
  pinMode(13, OUTPUT);  // Assuming an LED is connected to pin 13

  restartESP8266();
  setupWiFi();
}

void loop() {
  if (connectToServer()) {
    float distance = measureDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance >= DISTANCE_THRESHOLD) {
      sendRequest(sendLINE);

      String response = receiveResponse();
      processResponse(response);

      tone(11, C5);
      delay(1000);
      noTone(11);
      delay(1000);
      
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
    }
    // 1日待機（本当は電源を落としても良かった。）
    delay(86400000);
  }
}