

// MIT License
// https://github.com/gonzalocasas/arduino-uno-dragino-lorawan/blob/master/LICENSE
// Based on examples from https://github.com/matthijskooijman/arduino-lmic
// Basen on examples from http://www.steves-internet-guide.com/arduino-sending-receiving-json-mqtt/
// Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman

// Adaptions: Andreas Spiess, hunsm-19
#include <lmic.h>
#include <hal/hal.h>
//#include <credentials.h>
#include "Wire.h"
#include <ArduinoJson.h>

#ifdef CREDENTIALS
static const u1_t NWKSKEY[16] = NWKSKEY1;
static const u1_t APPSKEY[16] = APPSKEY1;
static const u4_t DEVADDR = DEVADDR1;
#else
static const u1_t NWKSKEY[16] = { 0x09, 0xC7, 0x3A, 0xD4, 0xC6, 0xAF, 0x21, 0xCD, 0xF8, 0xE9, 0x70, 0x94, 0x3C, 0x98, 0xD5, 0x37 };
static const u1_t APPSKEY[16] = { 0x7F, 0x5D, 0x24, 0xA4, 0xEF, 0x30, 0xBA, 0xC3, 0x8B, 0x07, 0x79, 0xED, 0x3A, 0x73, 0x86, 0xCD };
static const u4_t DEVADDR = 0x260BCA49;
#endif

#define address 0x40

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui(u1_t* buf) {}
void os_getDevEui(u1_t* buf) {}
void os_getDevKey(u1_t* buf) {}


static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20;

// Pin mapping Dragino Shield
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,
  .dio = { 2, 6, 7 },
};

char dtaUart[15];
char dtaLen = 0;
uint8_t Data[100] = { 0 };
uint8_t buff[100] = { 0 };

uint8_t buf[4] = { 0 };
uint16_t data, data1;
float temp;
float hum;

// Payload to send (uplink)
StaticJsonDocument<200> jsonDoc;
String jsonString;

void onEvent(ev_t ev) {
  if (ev == EV_TXCOMPLETE) {
    Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    // Schedule next transmission
    os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
  }
}

void do_send(osjob_t* j) {

  readReg(0x00, buf, 4);
  data = buf[0] << 8 | buf[1];
  data1 = buf[2] << 8 | buf[3];
  temp = ((float)data * 165 / 65535.0) - 40.0;
  hum = ((float)data1 / 65535.0) * 100;

  jsonDoc["ADC_CH0V"] = 0.275;
  jsonDoc["BatV"] = 4;
  jsonDoc["Digital_IStatus"] = "L";
  jsonDoc["Door_status"] = "OPEN";
  jsonDoc["EXTI_Trigger"] = "FALSE";
  jsonDoc["Hum_SHT"] = hum;
  jsonDoc["TempC1"] = 0;
  jsonDoc["TempC_SHT"] = temp;
  jsonDoc["Work_mode"] = "IIC";

  // Serialize the JSON data to a string
  serializeJson(jsonDoc, jsonString);

  static uint8_t message[200];
  jsonString.getBytes(message, 200);

  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    //LMIC_setTxData2(1, data, sizeof(data) - 1, 0);
    LMIC_setTxData2(1, message, sizeof(message) - 1, 0);
    Serial.println(F("Sending uplink packet..."));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println(F("Starting..."));

  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Set static session parameters.
  LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);

  // Disable link check validation
  LMIC_setLinkCheckMode(0);

  // TTN uses SF9 for its RX2 window.
  LMIC.dn2Dr = DR_SF9;

  // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF12, 14);

  // Start job
  do_send(&sendjob);
}

void loop() {
  os_runloop_once();
}

uint8_t readReg(uint8_t reg, const void* pBuf, size_t size) {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;
  Wire.beginTransmission(address);
  Wire.write(&reg, 1);
  if (Wire.endTransmission() != 0) {
    return 0;
  }
  delay(20);
  Wire.requestFrom((int)address, (int)size);
  for (uint16_t i = 0; i < size; i++) {
    _pBuf[i] = Wire.read();
  }
  return size;
}