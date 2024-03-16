/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <RCSwitch.h>

byte sck = 18;
byte miso = 16;
byte mosi = 19;
byte csn = 17;
byte gdo0 = 15;
byte gdo2 = 14;

// Signal to loop() whether the RF chip was set up properly so we don't
// unnecessarily wait for stuff or try to transmit things without it being
// properly set up.
bool rf_ok = false;

// Used to decode signals
RCSwitch mySwitch = RCSwitch();

void setup()
{
  // Start the Serial Monitor
  Serial.begin(9600);

  // Wait a bit so the monitor captures the init sequence as well
  delay(2000);

  pinMode(gdo0, OUTPUT);
  pinMode(gdo2, INPUT);

  Serial.println("Set up RF");
  // Configures SPI Pins for cc1100. Could only get this to work with the
  // default set up for Pico on Platform.io. The ELECHOUSE lib doesn't really
  // configure this for anything other than ESP32 and seems to rely on the
  // default config...  Default pin configuration for Pico on Platform.io is here:
  // https://github.com/arduino/ArduinoCore-mbed/blob/8e0a044330e88bde781eb4c509875ce94980659f/variants/RASPBERRY_PI_PICO/pins_arduino.h
  ELECHOUSE_cc1101.addSpiPin(sck, miso, mosi, csn, 0);
  // Switches between modules. from 0 to 5. So a maximum of 6.
  ELECHOUSE_cc1101.setModul(0);

  // Sets up the SPI connection and reads the chip version from a status
  // register. If that version is > 0, this returns true, otherwise it returns
  // false.
  Serial.println("Check RF status");
  if (ELECHOUSE_cc1101.getCC1101())
  {
    rf_ok = true;
    Serial.println("Connection OK");
  }
  else
  {
    Serial.println("Connection Error");
  }

  // As long as we use the default pins we should be good to go.
  // ELECHOUSE_cc1101.setSpiPin(sck, miso, mosi, csn);

  // Init the pins, SPI,the chip and reset all registers
  ELECHOUSE_cc1101.Init();

  // Set the rx bandwidth. Can be any value between 58.03 and 812.50. Default
  // is 812.50. Flipper says it used 650, but 200 works just fine.
  ELECHOUSE_cc1101.setRxBW(200);

  // Here you can set your basic frequency. The lib calculates the frequency
  // automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ
  // and 779-928MHZ. Read More info from datasheet.
  ELECHOUSE_cc1101.setMHZ(433.92);

  // The following things are not necessary to set up, I'll just keep them
  // around for documentation purposes or in case we need to adjust something.
  // Set the GPIO pins for reading/writing data from resp. to the chip. Once
  // set up, the CC1101 can act like any low cost RF chip that just sends data
  // over a single PIN.
  // ELECHOUSE_cc1101.setGDO(gdo0, gdo2);
  // ELECHOUSE_cc1101.setGDO0(gdo0);

  // Set config for internal transmission mode.
  // ELECHOUSE_cc1101.setCCMode(1);

  // Set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
  // Default should be 0 = 2-FSK.
  // ELECHOUSE_cc1101.setModulation(2);

  // Combined sync-word qualifier mode.
  //   0 = No preamble/sync.
  //   1 = 16 sync word bits detected.
  //   2 = 16/16 sync word bits detected.
  //   3 = 30/32 sync word bits detected.
  //   4 = No preamble/sync, carrier-sense above threshold.
  //   5 = 15/16 + carrier-sense above threshold.
  //   6 = 16/16 + carrier-sense above threshold.
  //   7 = 30/32 + carrier-sense above threshold.
  // ELECHOUSE_cc1101.setSyncMode(0);

  // 1 = CRC calculation in TX and CRC check in RX enabled.
  // 0 = CRC disabled for TX and RX.
  // ELECHOUSE_cc1101.setCrc(0);

  mySwitch.enableReceive(gdo2);

  ELECHOUSE_cc1101.SetRx();
}

void loop()
{
  if (!rf_ok)
  {
    Serial.println("RF not ok");
    delay(1000);
  }
  else
  {
    if (mySwitch.available())
    {

      Serial.print("Received ");
      Serial.print(mySwitch.getReceivedValue(), HEX);
      Serial.print(" / ");
      Serial.print(mySwitch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(mySwitch.getReceivedProtocol());

      mySwitch.resetAvailable();
    }
  }
}