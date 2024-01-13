#include "espnow.h"

extern char ChipMAC_S[18];
extern struct PacketData {
  int16_t lxAxisValue;
  int16_t lyAxisValue;
  int16_t rxAxisValue;
  int16_t ryAxisValue;
 
  int16_t channel1;
  int16_t channel2;
  int16_t channel3;
  int16_t channel4;  
  int16_t channel5;
  int16_t channel6;
  int16_t channel7;
  int16_t channel8;  
} data;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

bool ESPNowInit(void) {
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return false;
  } else {
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);
  //esp_wifi_config_espnow_rate();
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, ChipMAC_S, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  } else {
    Serial.println("Succes: Added peer");
  } 

  //esp_now_get_peer();
  return true;
}

void ESPNowDataSend(void) {
  esp_err_t result = esp_now_send((uint8_t*)&ChipMAC_S, (uint8_t*)&data, sizeof(data));
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }   
}