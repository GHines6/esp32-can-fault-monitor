#include <ESP32-TWAI-CAN.hpp>

#define CAN_TX GPIO_NUM_25
#define CAN_RX GPIO_NUM_26
CanFrame rxFrame = { 0 };
uint8_t lastCounter = 0;
uint32_t lastFrameTime = 0;
const uint32_t FRAME_TIMEOUT_MS = 2500;
bool hasPreviousCounter = false;
bool dataTimeoutActive = false;
uint32_t lastHeartbeatTime = 0;
bool hasReceivedHeartbeat = false;
const uint32_t HEARTBEAT_TIMEOUT_MS = 2500;
bool heartbeatTimeoutActive = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Monitor Node On");
  ESP32Can.setPins(CAN_TX, CAN_RX);
  ESP32Can.setSpeed(ESP32Can.convertSpeed(125));

  if (ESP32Can.begin()){
    Serial.println("Startup Succeeded");
  }else{
    Serial.println("Startup Failed");
  }

}
void loop() {
  if (ESP32Can.readFrame(rxFrame, 100)) {

    if (rxFrame.identifier == 0x100 && rxFrame.data_length_code >= 7){
    Serial.println("----------------------------------");
    Serial.println("Frame arrived successfully");
    Serial.print("Sequence Counter: ");
    Serial.println(rxFrame.data[0]);
    Serial.print("CAN ID: ");
    Serial.println(rxFrame.identifier, HEX);
    int16_t accelX = (rxFrame.data[1] << 8) | rxFrame.data[2];
    float accelX_g = accelX / 16384.0f;
    Serial.print("Accel X: ");
    Serial.print(accelX_g, 3);
    Serial.println(" g");
    int16_t accelY = (rxFrame.data[3] << 8) | rxFrame.data[4];
    float accelY_g = accelY / 16384.0f;
    Serial.print("Accel Y: ");
    Serial.print(accelY_g, 3);
    Serial.println(" g");
    int16_t accelZ = (rxFrame.data[5] << 8) | rxFrame.data[6];
    float accelZ_g = accelZ / 16384.0f;
    Serial.print("Accel Z: ");
    Serial.print(accelZ_g, 3);
    Serial.println(" g\n");

    if (hasPreviousCounter) {
      uint8_t expectedCounter = lastCounter + 1;

      if (rxFrame.data[0] != expectedCounter) {
        Serial.println("ERROR DETECTED");
      }
    }
  if (dataTimeoutActive){
    Serial.println("DATA FRAME RECOVERED");
    dataTimeoutActive = false;
  }
    lastCounter = rxFrame.data[0];
    hasPreviousCounter = true;
    lastFrameTime = millis();
  }else if(rxFrame.identifier == 0x101 && rxFrame.data_length_code >= 2){
    Serial.print("CAN ID: ");
    Serial.println(rxFrame.identifier, HEX);
    Serial.println("Heartbeat received");

    if(rxFrame.data[1] == 1){
      Serial.println("Sensor Healthy\n----------------------------------");
    }else{
      Serial.println("SENSOR FAULT ACTIVE\n----------------------------------");
    }

    if (heartbeatTimeoutActive) {
  Serial.println("HEARTBEAT RECOVERED");
  heartbeatTimeoutActive = false;
}

    lastHeartbeatTime = millis();
    hasReceivedHeartbeat = true;
  }
  else if (rxFrame.identifier == 0x102 &&
         rxFrame.data_length_code >= 7) {
    Serial.print("CAN ID: ");
    Serial.println(rxFrame.identifier, HEX);
    int16_t gyroX = (rxFrame.data[1] << 8) | rxFrame.data[2];
    int16_t gyroY = (rxFrame.data[3] << 8) | rxFrame.data[4];
    int16_t gyroZ = (rxFrame.data[5] << 8) | rxFrame.data[6];
    float gyroX_dps = gyroX / 131.0f;
    float gyroY_dps = gyroY / 131.0f;
    float gyroZ_dps = gyroZ / 131.0f;
    Serial.print("Gyro X: ");
    Serial.print(gyroX_dps, 2);
    Serial.println(" deg/s");
    Serial.print("Gyro Y: ");
    Serial.print(gyroY_dps, 2);
    Serial.println(" deg/s");
    Serial.print("Gyro Z: ");
    Serial.print(gyroZ_dps, 2);
    Serial.println(" deg/s\n----------------------------------");
}
  }
  if (hasPreviousCounter && !dataTimeoutActive && millis() - lastFrameTime > FRAME_TIMEOUT_MS){
    Serial.println("DATA FRAME TIMEOUT");
    dataTimeoutActive = true;
  }
if (hasReceivedHeartbeat &&
    !heartbeatTimeoutActive &&
    millis() - lastHeartbeatTime > HEARTBEAT_TIMEOUT_MS) {

  Serial.println("HEARTBEAT TIMEOUT");
  heartbeatTimeoutActive = true;
}
}
