#include <Arduino.h>

#include "bsp_api.h"

#include "hal_data.h"

// von RC_nRF_FS_every

#include "main.h"

#include <SPI.h>
#include <U8g2lib.h>
#include "display.h"

#include <nRF24L01.h>
#include <RF24.h>

#include <elapsedMillis.h>
#include "defines.h"

#include "MCP23S17.h"
const uint8_t chipSelect = 8;

#define ANZLOKALLOKS 6 // anz loks bei lokalem Betrieb
#define ANZLOKALPOTS 4

#define IMPULSTASK 1
#define PAUSETASK 2
#define RICHTUNGSTART 0

#define ANZWWEICHEN 16
elapsedMillis sinceweiche;
elapsedMillis sincelastweiche[ANZWWEICHEN];

#define SPI_MCP_CS 8
#define SPI_SR_CS 22

/*
// PIN-Nummern regulaer:
#define SET_A_A  6   // 1
#define SET_A_B  3   // 1
#define OUT_A_A  5   // 0

#define SET_A_C  7   // 1
#define SET_A_D  2   // 1
#define OUT_A_B  4   // 0

#define SET_B_A  14   // 1
#define SET_B_B  11   // 1
#define OUT_B_A  13   // 0

#define SET_B_C  15   // 1
#define SET_B_D  10   // 1
#define OUT_B_B  12   // 0
*/

// PIN-Nummern Bank vertauscht:

#define SET_A_A 9  // 1
#define SET_A_B 12 // 1
#define OUT_A_A 10 // 0

#define SET_A_C 8  // 1
#define SET_A_D 13 // 1
#define OUT_A_B 11 // 0

#define SET_B_A 6 // 1
#define SET_B_B 3 // 1
#define OUT_B_A 5 // 0

#define SET_B_C 7 // 1
#define SET_B_D 2 // 1
#define OUT_B_B 4 // 0

#define SET_A_A_BIT 1 // 1
#define SET_A_B_BIT 4 // 1

#define SET_A_C_BIT 0 // 1
#define SET_A_D_BIT 5 // 1

#define SET_B_A_BIT 6 // 1
#define SET_B_B_BIT 3 // 1

#define SET_B_C_BIT 7 // 1
#define SET_B_D_BIT 2 // 1

MCP23S17 mcp1(&SPI, SPI_MCP_CS, 0); // A2=GND, A1=GND, A0=GND
MCP23S17 mcp2(&SPI, SPI_MCP_CS, 2); // A2=GND, A1=GND, A0=VDD

uint8_t regA = 0x0;
uint8_t regB = 0;

#define ANZWEICHENGRUPPEN 1
#define GRUPPE_0 0
#define GRUPPE_1 1
uint8_t weichenaddressarray[2][4] = {{2, 2, 2, 1}, {2, 2, 1, 1}};
uint8_t weichenposition[2] = {}; // 0: gerade 1: ablenkung

volatile uint8_t tastencodeA = 0;
volatile uint8_t tastencodeB = 0;
volatile uint8_t tastencodeC = 0;
volatile uint8_t tastencodeD = 0;

volatile uint8_t weichentastencodeC = 0;
volatile uint8_t weichentastencodeD = 0;
volatile uint8_t oldweichentastencodeC = 0;
volatile uint8_t oldweichentastencodeD = 0;

// mcp2
volatile uint8_t weichentastencodeE = 0;
volatile uint8_t weichentastencodeF = 0;
volatile uint8_t oldweichentastencodeE = 0;
volatile uint8_t oldweichentastencodeF = 0;

// tasten-wait
volatile uint8_t weichentastendelay = 0;

#define RINGBUFFER_SIZE 32
#define RINGBUFFER_MASK (RINGBUFFER_SIZE - 1)

typedef struct
{
  uint8_t weiche;
  uint8_t richtung;
} weichendata;

typedef struct
{
  weichendata data[RINGBUFFER_SIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
} H0_RingBuffer; // ringbuffer fuer weichenposition

void rb_init(H0_RingBuffer *rb)
{
  rb->head = 0;
  rb->tail = 0;
}

bool rb_is_empty(H0_RingBuffer *rb)
{
  return rb->head == rb->tail;
}

bool rb_is_full(H0_RingBuffer *rb)
{
  return ((rb->head + 1) & RINGBUFFER_MASK) == rb->tail;
}

bool rb_push(H0_RingBuffer *rb, weichendata value)
{
  uint16_t next = (rb->head + 1) & RINGBUFFER_MASK;

  if (next == rb->tail)
    return false; // voll

  rb->data[rb->head] = value;
  rb->head = next;

  return true;
}

int rb_pop(H0_RingBuffer *rb, weichendata *value)
{
  if (rb_is_empty(rb))
    return -1; // leer

  *value = rb->data[rb->tail];
  rb->tail = (rb->tail + 1) & RINGBUFFER_MASK;

  return 0;
}

uint16_t rb_count(H0_RingBuffer *rb)
{
  if (rb->head >= rb->tail)
    return rb->head - rb->tail;

  return RINGBUFFER_SIZE - rb->tail + rb->head;
}

uint8_t weichenxor0 = 0;
uint8_t weichenxor1 = 0;
uint8_t weichenxor2 = 0;
uint8_t weichenxor3 = 0;
uint8_t weichenxor4 = 0;

uint8_t weichennummer = 0; // 0..7
uint8_t weichenrichtung = 0;

uint8_t tastenstatusA = 0;
uint8_t tastenstatusB = 0;

volatile uint8_t diptastenadresseA = 0; // dipschalter wird von links gelesen, bit 0 ist ganz links
volatile uint8_t diptastenadresseB = 0;
volatile uint8_t diptastenadresseC = 0; // dipschalter wird von links gelesen, bit 0 ist ganz links
volatile uint8_t diptastenadresseD = 0;

uint8_t lokalstatus = 0;
#define LOKALRICHTUNGBIT0 0
#define LOKALRICHTUNGBIT1 1
#define LOKALRICHTUNGBIT2 2
#define LOKALRICHTUNGBIT3 3
elapsedMillis sincelocalrichtung;

// Utilities
elapsedMillis sinceringbuffer;

elapsedMillis sincewegbuffer;

elapsedMillis sinceemitter;

elapsedMillis sincemcp;

volatile uint8_t weichenstatus = 0;
volatile uint16_t weichencounter = 0;

uint8_t weichenXORcounterA = 0;
uint8_t weichenXORcounterB = 0;
uint8_t weichenXORcounterC = 0;
uint8_t weichenXORcounterD = 0;

volatile uint8_t weichenbuffer[4] = {};

#define WEICHESTART 0
#define WEICHERUN 1
#define WEICHEDELAY 2
#define WEICHEREADY 3

#define WEICHEOFF 5
#define ABLENKUNG 6
#define GERADE 7
#define WEICHENIMPULSDAUER 0xAF
#define MAXWEICHENCOUNTER 50

// end Weichen decleration

H0_RingBuffer weichenringbuffer = {0};

const uint64_t pipeOut = 0xABCDABCD71LL; // NOTE: The address in the Transmitter and Receiver code must be the same "0xABCDABCD71LL" | Verici ve Alıcı kodundaki adres aynı olmalıdır

extern "C"

    uint16_t loopcounter0 = 0;
uint16_t loopcounter1 = 0;
uint16_t blinkcounter = 0;
uint16_t impulscounter = 0;
uint16_t paketcounter = 0;

uint8_t savestatus = 0;

uint8_t taskstatus = 0;

uint8_t adresse = 0;
uint16_t adressmatch = 0;
uint16_t speed = 0;

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
void poptask(void)
{
  weichendata w;
  int erfolg = rb_pop(&weichenringbuffer, &w);
  if (erfolg == 0) // Weiche vorhanden
  {
    u8g2.setCursor(64, 48);
    u8g2.print(w.weiche);
    u8g2.print(w.richtung);
  }
  else
  {
    u8g2.setCursor(64, 48);
    u8g2.print("X");
  }
  u8g2.sendBuffer();
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

  pinMode(SPI_MCP_CS, OUTPUT);
  digitalWrite(SPI_MCP_CS, HIGH);

  mcp1.begin();
#define CHECK 15
#define BLINK 0
  // mcp1.pinMode(CHECK, OUTPUT);
  // mcp1.pinMode(BLINK, OUTPUT);
  //  11001100
  //  mcp1.gpioPort(0xFFFF); // alle HI
  mcp1.writePort(0, 0xCC); // alle LO, Gerade
  mcp1.writePort(1, 0x33); // alle LO, Gerade

  mcp2.begin();
  mcp2.writePort(0, 0xCC); // alle LO, Gerade
  mcp2.writePort(1, 0x33); // alle LO, Gerade

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

  for (int i = 0; i <= 3; i++)
  {
    if (digitalRead(3 + i) == HIGH)
    {
      // adresse |= (1 << i);
    }
  }

  // adresse = (R_PORT1->PIDR >> 3) & 0x0F;
  uint32_t pins = R_PORT1->PIDR;

  adresse = ((pins >> 4) & 1) << 0;  // D3
  adresse |= ((pins >> 3) & 1) << 1; // D4
  adresse |= ((pins >> 2) & 1) << 2; // D5
  adresse |= ((pins >> 6) & 1) << 3; // D6

  if (adresse == 0)
  {
    taskstatus &= ~(1 << MCP_START);
  }
  if (adresse == 7) // Vorbereitung
  {
    //
    // analogWrite(DAC, speed);
    // analogWrite(DAC,  2000);
  }
  if (adresse == 6)
  {
    if (!(taskstatus & (1 << ADRESSE_START)))
    {
      OSZIA_LO();
    }
    taskstatus |= (1 << ADRESSE_START);
    analogWrite(DAC, speed);
    digitalWrite(OUT_EN_PIN, 1);
    digitalWrite(ADRESS_EN_PIN, 1);
    adressmatch++;
    OSZIA_HI();
    //}
  }

  if ((adresse == 7)) // & (taskstatus & (1 << ADRESSE_START)))
  {

    paketcounter++;
    digitalWrite(OUT_EN_PIN, 0);
    digitalWrite(ADRESS_EN_PIN, 0);
    // analogWrite(DAC, 0);

    taskstatus &= ~(1 << ADRESSE_START);
  }

  if (adresse == 9)
  {
    if (!(taskstatus & (1 << MCP_START)))
    {
      taskstatus |= (1 << MCP_START);
      weichentastencodeC = mcp1.readPort(0);
      weichentastencodeD = mcp1.readPort(1);
      weichentastencodeE = mcp2.readPort(0);
      weichentastencodeF = mcp2.readPort(1);
    }
    weichenposition[GRUPPE_0] = 0;
    weichentastencodeC = mcp1.readPort(0);
    weichenxor0 = weichentastencodeC ^ oldweichentastencodeC;

    if (weichenxor0)
    {
      weichenXORcounterA++;
      // Weiche 2 Position 0
      if ((weichentastencodeC & (1 << SET_A_A_BIT)) == 0) // 6   Taste A  gedrueckt , weiche2
      {
        // u8g2.setCursor(8, 48);
        // u8g2.print("C A_A");
        if (sincelastweiche[2] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("C A_A");
          mcp1.writePort(OUT_A_A, HIGH);          // GPA7
          weichenposition[GRUPPE_0] &= ~(1 << 2); // bit fuer weiche loeschen
          weichendata wC = {.weiche = 2, .richtung = 1};
          rb_push(&weichenringbuffer, wC);
          uint8_t eepromdata = (wC.weiche << 4) | wC.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wC.weiche,(eepromdata));
          sincelastweiche[2] = 0;
          u8g2.setCursor(8, 48);
          // u8g2.print("C            ");
        }
      }
      else
      {
        // u8g2.setCursor(8, 48);
        // u8g2.print("C            ");
      }

      if ((weichentastencodeC & (1 << SET_A_B_BIT)) == 0) //  3    Taste B gedrueckt weiche2
      {
        // u8g2.setCursor(8, 48);
        // u8g2.print("C A_B");
        if (sincelastweiche[2] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("C A_B");
          mcp1.writePort(OUT_A_A, LOW);          //
          weichenposition[GRUPPE_0] |= (1 << 2); // bit fur weiche setzen
          weichendata wC = {.weiche = 2, .richtung = 0};
          rb_push(&weichenringbuffer, wC);
          uint8_t eepromdata = (wC.weiche << 4) | wC.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wC.weiche,(eepromdata));
          sincelastweiche[2] = 0;
          // u8g2.setCursor(8, 48);
          // u8g2.print("C            ");
        }
      }
      else
      {
        // u8g2.setCursor(8, 48);
        // u8g2.print("C            ");
      }

      // Weiche B Position 1
      if ((weichentastencodeC & (1 << SET_A_C_BIT)) == 0) //  7    Taste 3 gedrueckt , weiche1
      {
        if (sincelastweiche[3] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("C A_C");
          mcp1.writePort(OUT_A_B, HIGH); // GPA7
          weichenposition[GRUPPE_0] &= ~(1 << 3);
          weichendata wC = {.weiche = 3, .richtung = 1};
          uint8_t eepromdata = (wC.weiche << 4) | wC.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wC.weiche,(eepromdata));
          rb_push(&weichenringbuffer, wC);
          sincelastweiche[3] = 0;
          u8g2.setCursor(8, 48);
          // u8g2.print("C            ");
        }
      }

      if ((weichentastencodeC & (1 << SET_A_D_BIT)) == 0) // 2    Taste 2 gedrueckt weiche1
      {
        if (sincelastweiche[3] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("C A_D");

          mcp1.writePort(OUT_A_B, LOW); //
          weichenposition[GRUPPE_0] |= (1 << 3);
          weichendata wC = {.weiche = 3, .richtung = 0};
          uint8_t eepromdata = (wC.weiche << 4) | wC.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wC.weiche,(eepromdata));
          rb_push(&weichenringbuffer, wC);
          sincelastweiche[3] = 0;
        }
      }
      oldweichentastencodeC = weichentastencodeC;
    } // if weichenxor0

    // weichendata wD;                                   // data in ringbuffer
    weichentastencodeD = mcp1.readPort(1) & 0xFE; // Blink exkl.
    weichenxor1 = weichentastencodeD ^ oldweichentastencodeD;
    if (weichenxor1) // neue Daten
    {
      weichenXORcounterB++;
      // Weiche 0 Position 2
      if ((weichentastencodeD & (1 << SET_B_A_BIT)) == 0) // Taste 6 gedrueckt
      {
        if (sincelastweiche[0] > 100)
        {
          weichenposition[GRUPPE_0] &= ~(1 << 0); // bit fuer weiche loeschen
          u8g2.setCursor(8, 48);
          u8g2.print("D B_A");
          mcp1.writePort(OUT_B_A, HIGH); // GPB7
          weichendata wD = {.weiche = 0, .richtung = 1};
          rb_push(&weichenringbuffer, wD);
          uint8_t eepromdata = (wD.weiche << 4) | wD.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wD.weiche,(eepromdata));
          sincelastweiche[0] = 0;
          // taste = 'G';
        }
      }

      if ((weichentastencodeD & (1 << SET_B_B_BIT)) == 0) // Taste  gedrueckt
      {
        if (sincelastweiche[0] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("D B_B");
          weichenposition[GRUPPE_0] |= (1 << 0); // bit fur weiche setzen
          mcp1.writePort(OUT_B_A, LOW);          //
          weichendata wD = {.weiche = 0, .richtung = 0};
          rb_push(&weichenringbuffer, wD);
          uint8_t eepromdata = (wD.weiche << 4) | wD.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wD.weiche,(eepromdata));
          sincelastweiche[0] = 0;
          // taste = 'A';
        }
      }

      // Weiche 1 Position 1
      if ((weichentastencodeD & (1 << SET_B_C_BIT)) == 0) // Taste 3 gedrueckt , weiche3
      {
        if (sincelastweiche[1] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("D B_C");
          weichenposition[GRUPPE_0] &= ~(1 << 1); // bit fuer weiche loeschen
          mcp1.writePort(OUT_B_B, HIGH);          // GPA7
          weichendata wD = {.weiche = 1, .richtung = 1};
          rb_push(&weichenringbuffer, wD);
          uint8_t eepromdata = (wD.weiche << 4) | wD.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wD.weiche,(eepromdata));
          sincelastweiche[1] = 0;
        }
      }

      if ((weichentastencodeD & (1 << SET_B_D_BIT)) == 0) // Taste 2 gedrueckt weiche3
      {
        if (sincelastweiche[1] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("D B_D");
          weichenposition[GRUPPE_0] |= (1 << 1); // bit fur weiche setzen
          mcp1.writePort(OUT_B_B, LOW);          //
          weichendata wD = {.weiche = 1, .richtung = 0};
          rb_push(&weichenringbuffer, wD);
          uint8_t eepromdata = (wD.weiche << 4) | wD.richtung;
          // EEPROM.update(EEPROM_WEICHESTART + wD.weiche,(eepromdata));
          sincelastweiche[1] = 0;
        }
      }

      oldweichentastencodeD = weichentastencodeD;
      // mcp1.gpioDigitalWrite(OUT_B_A, 1);

    } // if(weichentastencodeD ^

    // weichendata wE;                            // data in ringbuffer
    weichentastencodeE = mcp2.readPort(0); //& 0x7F;

    weichenxor2 = weichentastencodeE ^ oldweichentastencodeE;
    if (weichenxor2) // neue Daten
    {
      weichenXORcounterC++;
      // Weiche 6 Position 4
      if (((weichentastencodeE & (1 << SET_A_A_BIT)) == 0)) // Taste 3 gedrueckt
      {
        if (sincelastweiche[6] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("E A_A");
          mcp2.writePort(OUT_A_A, HIGH);   //
          weichenposition[GRUPPE_0] &= ~(1 << 6); // bit fuer weiche loeschen

          weichendata wE = {.weiche = 6, .richtung = 1};
          rb_push(&weichenringbuffer, wE);
          uint8_t eepromdata = (wE.weiche << 4) | wE.richtung;
          //EEPROM.update(EEPROM_WEICHESTART + wE.weiche, (eepromdata));
          sincelastweiche[6] = 0;
        }
      }

      if ((weichentastencodeE & (1 << SET_A_B_BIT)) == 0) // Taste 2 gedrueckt
      {
        if (sincelastweiche[6] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("E A_B");
          mcp2.writePort(OUT_A_A, LOW);   //
          weichenposition[GRUPPE_0] |= (1 << 6); // bit fur weiche setzen

          weichendata wE = {.weiche = 6, .richtung = 0};
          rb_push(&weichenringbuffer, wE);
          uint8_t eepromdata = (wE.weiche << 4) | wE.richtung;
          //EEPROM.update(EEPROM_WEICHESTART + wE.weiche, (eepromdata));
          sincelastweiche[6] = 0;
        }
      }

      // Weiche 7 Position 5
      if ((weichentastencodeE & (1 << SET_A_C_BIT)) == 0) // Taste 5 gedrueckt , weiche1
      {
        if (sincelastweiche[7] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("E A_C");
          mcp2.writePort(OUT_A_B, HIGH);  // GPA7
          weichenposition[GRUPPE_0] |= (1 << 7); // bit fur weiche setzen

          weichendata wE = {.weiche = 7, .richtung = 1};
          rb_push(&weichenringbuffer, wE);
          uint8_t eepromdata = (wE.weiche << 4) | wE.richtung;
          //EEPROM.update(EEPROM_WEICHESTART + wE.weiche, (eepromdata));
          sincelastweiche[7] = 0;
        }
      }

      if ((weichentastencodeE & (1 << SET_A_D_BIT)) == 0) // Taste 5 gedrueckt weiche1
      {
        if (sincelastweiche[7] > 100)
        {
          u8g2.setCursor(8, 48);
          u8g2.print("E A_D");
          mcp2.writePort(OUT_A_B, LOW);   //
          weichenposition[GRUPPE_0] |= (1 << 7); // bit fur weiche setzen

          weichendata wE = {.weiche = 7, .richtung = 0};
          rb_push(&weichenringbuffer, wE);
          uint8_t eepromdata = (wE.weiche << 4) | wE.richtung;
          //EPROM.update(EEPROM_WEICHESTART + wE.weiche, (eepromdata));
          sincelastweiche[7] = 0;
        }
      }

      oldweichentastencodeE = weichentastencodeE;
    } // if(weichentastencodeDE ^

    u8g2.sendBuffer();

    if (rb_count(&weichenringbuffer))
    {
      poptask();
    }

  } // adresse == 0

  loopcounter0++;

  if (loopcounter0 >= BLINKRATE)
  {

    // digitalWrite(LOOPLED, !digitalRead(LOOPLED));
    loopcounter0 = 0;

    impulscounter += 1;

    // Display 0.96
    loopcounter1++;
    uint8_t charindex = loopcounter1 & 0x7F;
    // u8g2.setDrawColor(0);

    // speed >>= 4;
    // Serial.print("speed: ");
    // Serial.println(speed);
    // analogWrite(DAC, (speed));

    if (loopcounter1 > 10)
    {
      if (DISPLAY)
      {

        u8g2.setCursor(0, 12);
        u8g2.print(weichentastencodeC);

        u8g2.setCursor(50, 12);
        u8g2.print(weichentastencodeD);

        u8g2.setCursor(0, 30);
        u8g2.print(weichentastencodeE);

        u8g2.setCursor(50, 30);
        u8g2.print(weichentastencodeF);

        u8g2.setCursor(100, 12);
        u8g2.print(speed);
      }

      blinkcounter += 255;
      speed = blinkcounter & 0xFFF;
      if (blinkcounter > 4000)
      {
        blinkcounter = 0;
      }
      // OSZIB_LO();
      if (curr_screen == 0)
      {
        // updateHomeScreen();
        // u8g2.sendBuffer();
      }
      // OSZIB_HI();

      // Serial.print("\t");
      loopcounter1 = 0;

      digitalWrite(LOOPLED, !digitalRead(LOOPLED));
      // Serial.print("Byte-Wert: ");
      // for (int b = 7; b >= 0; b--)
      {
        // Serial.print((adresse >> b) & 1);
      }
      // Serial.println();
      u8g2.sendBuffer();
    }
  } // if loopcounter
}
