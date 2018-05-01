#include "BluetoothSerial.h"
#include <WiFi.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

String bufferReceive = "";
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  SerialBT.begin("ESP32test");
  // Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      if (bufferReceive == "SCAN") {
        Serial.println("scan start");
        int n = WiFi.scanNetworks();
        Serial.println("scan done");
        if (n > 0) {
          for (int i = 0; i < n; i++) {
            SerialBT.print(WiFi.SSID(i));
            SerialBT.print(',');
            delay(0); // nop
          }
        }
        SerialBT.print('\n');
      }
      bufferReceive = "";
    } else {
      bufferReceive += c;
    }
    
    if (bufferReceive == "CONNECT:") {
      String parameter[10];
      int i = 0;
      while(1) {
        if (SerialBT.available()) {
          char c2 = SerialBT.read();
          // Serial.printf("Receive: %c\n", c2);
          if (c2 == '\t') {
            i++;
          } else if (c2 == '\n') {
            break;
          } else {
            parameter[i] += c2;
          }
        }
        delay(0); // nop
      }
      String ssid = parameter[0];
      String password = parameter[1];
      Serial.println("SSID: " + ssid);
      Serial.println("PASSWORD: " + password);
      SerialBT.print("OK\n");
      bufferReceive = "";
    }
  }
  delay(20);
}
