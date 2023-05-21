/*To communicate via ESP-NOW, you need to know the MAC Address of the ESP32 receiver. 
That’s how you know to which device you’ll send the data to.

Each ESP32 has a unique MAC Address and that’s how we
 identify each board to send data to it using ESP-NOW*/
 #include "WiFi.h"
 
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}