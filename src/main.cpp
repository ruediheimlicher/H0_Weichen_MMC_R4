#include <Arduino.h>
#include "bsp_api.h"
// von RC_nRF_FS_every

#include "main.h"

#include <SPI.h>
#include <U8g2lib.h>
#include "display.h"

#include <nRF24L01.h>
#include <RF24.h>

#include <elapsedMillis.h>
#include "defines.h"

const uint64_t pipeOut = 0xABCDABCD71LL; // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır

extern "C"

    uint16_t loopcounter0 = 0;
uint16_t loopcounter1 = 0;
uint16_t blinkcounter = 0;
uint16_t impulscounter = 0;
uint16_t paketcounter = 0;

uint8_t savestatus = 0;

uint8_t adresse = 0;
uint16_t adressmatch = 0;
uint16_t speed = 0;

uint8_t taskstatus = 0;

Signal data;

#define TEST 0

#define CE_PIN 9
#define CSN_PIN 10

#define ADRESS_EN_PIN D2 // Adresse auf Bus geben
#define OUT_EN_PIN A2

#define OUT_PIN A0

#define OSZIA_PIN A6
#define OSZIB_PIN A7

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

#define LOOPLED A1

unsigned char char_x = 0;
unsigned char char_y = 0;
uint8_t curr_model = 0; // aktuelles modell

uint8_t curr_modus = 0; // Modell oder Sim oder Calib

uint8_t curr_screen = 0; // aktueller screen
uint8_t last_screen = 0; // letzter screen

uint8_t curr_page = 7; // aktuelle page
uint8_t curr_col = 0;  // aktuelle colonne

uint8_t curr_cursorzeile = 0;  // aktuelle zeile des cursors
uint8_t curr_cursorspalte = 0; // aktuelle colonne des cursors
uint8_t last_cursorzeile = 0;  // letzte zeile des cursors
uint8_t last_cursorspalte = 0; // letzte colonne des cursors

// put function declarations here:
void OSZIA_HI(void)
{
  digitalWrite(OSZIA_PIN, 1);
}

void OSZIA_LO(void)
{
  digitalWrite(OSZIA_PIN, 0);
}

void OSZIA_TOG(void)
{
  digitalWrite(OSZIA_PIN, !digitalRead(OSZIA_PIN));
}

void OSZIB_HI(void)
{
  digitalWrite(OSZIB_PIN, 1);
}

void OSZIB_LO(void)
{
  digitalWrite(OSZIB_PIN, 0);
}

void OSZIB_TOG(void)
{
  digitalWrite(OSZIB_PIN, !digitalRead(OSZIB_PIN));
}

void setup()
{
  pinMode(OSZIA_PIN, OUTPUT);
  pinMode(OSZIB_PIN, OUTPUT);

  digitalWrite(OSZIA_PIN, 1);
  digitalWrite(OSZIB_PIN, 1);
  pinMode(ADRESS_EN_PIN, OUTPUT); // Enable Adress-wert
  digitalWrite(ADRESS_EN_PIN, 0); // OFF

  pinMode(OUT_EN_PIN, OUTPUT); // Enable ADC-wert
  digitalWrite(OUT_EN_PIN, 0); // OFF

  pinMode(LOOPLED, OUTPUT);
  // pinMode(OUT_PIN, OUTPUT);
  analogWriteResolution(12);
  initDisplay();
  setHomeScreen();

  u8g2.sendBuffer();

  // Pins D3 bis D6 als Eingang mit internem Pull-up definieren
  for (int pin = 3; pin <= 6; pin++)
  {
    pinMode(pin, INPUT);
  }

  /*
//                Configure the NRF24 module  | NRF24 modül konfigürasyonu
  radio.begin();
  radio.openWritingPipe(pipeOut);
  //radio.setChannel(100);
  radio.setChannel(124);
  radio.setAutoAck(false);
  //radio.setDataRate(RF24_250KBPS);    // The lowest data rate value for more stable communication  | Daha kararlı iletişim için en düşük veri hızı.
  radio.setDataRate(RF24_2MBPS); // Set the speed of the transmission to the quickest available


  radio.setPALevel(RF24_PA_MAX);      // Output power is set for maximum range  |  Çıkış gücü maksimum menzil için ayarlanıyor.

  radio.setPALevel(RF24_PA_MIN);
  radio.setPALevel(RF24_PA_MAX);

  radio.stopListening();              // Start the radio comunication for Transmitter | Verici için sinyal iletişimini başlatır.
  if (radio.failureDetected)
  {
     radio.failureDetected = false;
     delay(250);
     Serial.println("Radio failure detected, restarting radio");
  }
  else
  {
     Serial.println("Radio OK");
  }
  ResetData();

charh = u8g2.getMaxCharHeight();
  */
}

void loop()
{
  adresse = 0;
  /*
  for (int i = 0; i <= 3; i++)
  {
    if (digitalRead(3 + i) == HIGH)
    {
      adresse |= (1 << i);
    }
  }
*/
  adresse = (R_PORT1->PIDR >> 3) & 0x0F;

  if (adresse == 7) // Vorbereitung
  {
    // OSZIB_TOG();
    analogWrite(DAC, 0x8FF);
  }

  if (adresse == 8)
  {
    //if (!(taskstatus & (1 << ADRESSE_START)))
    {
       OSZIA_LO();
       digitalWrite(ADRESS_EN_PIN,1);
       digitalWrite(OUT_EN_PIN,1);
       /*
      // D2 P105
      R_PORT1->PODR |= (1 << 5); // P105 ADRESS_EN_PIN D2
      // 
      R_PORT0->PODR |= (1 << 5); // P001 OUT_EN_PIN A2
      */                    // digitalWrite(OUT_EN_PIN,1);
      taskstatus |= (1 << ADRESSE_START);
      taskstatus &= ~(1 << ADRESSE_END);
    }

   

    adressmatch++;
    
  }

  if (adresse == 9)
  {
    paketcounter++;
    //if ((taskstatus & (1 << ADRESSE_START)))
    {
       
       /*
      R_PORT0->PODR &= ~(1 << 5); // P001 OUT_EN_PIN A2 LOW
                                  // digitalWrite(OUT_EN_PIN,1);

      // D2 P105
      R_PORT1->PODR &= ~(1 << 5); // P105 ADRESS_EN_PIN D2 LOW
      */
       digitalWrite(ADRESS_EN_PIN,0);
       digitalWrite(OUT_EN_PIN,0);

      analogWrite(DAC, 0);
      OSZIA_HI();
    }
  }
  else
  {
    // digitalWrite(ADRESS_EN_PIN,0);
  }

  loopcounter0++;

  if (loopcounter0 >= BLINKRATE)
  {

     //digitalWrite(LOOPLED, !digitalRead(LOOPLED));
    loopcounter0 = 0;

    impulscounter += 1;

    // Display 0.96
    loopcounter1++;
    uint8_t charindex = loopcounter1 & 0x7F;
    // u8g2.setDrawColor(0);

    blinkcounter++;
    speed = blinkcounter;
    // speed >>= 4;
    // Serial.print("adresse: ");
    // Serial.println(adresse);
    // analogWrite(DAC, (speed));
    if (loopcounter1 > 10)
    {
      // OSZIB_LO();
      if (curr_screen == 0)
      {
        // updateHomeScreen();
        // u8g2.sendBuffer();
      }
      // OSZIB_HI();

      // Serial.print("\t");
      loopcounter1 = 0;

      //R_PORT0->PODR = ~(1 << 0);
      digitalWrite(LOOPLED, !digitalRead(LOOPLED));
      // Serial.print("Byte-Wert: ");
      for (int b = 7; b >= 0; b--)
      {
        // Serial.print((adresse >> b) & 1);
      }
      // Serial.println();
    }
  } // if loopcounter
}
