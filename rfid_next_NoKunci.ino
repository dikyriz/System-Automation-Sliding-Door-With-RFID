/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 *
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 *
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_1_PIN        10         // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 2
#define SS_2_PIN        8          // Configurable, take a unused pin, only HIGH/LOW required, must be diffrent to SS 1
#define NR_OF_READERS   2


byte ssPins[] = {SS_1_PIN, SS_2_PIN};

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

int Motor_IN3 = 5; //lawan arah jarum jam
int Motor_IN4 = 6; //searah jarum jam
int enB = 3; //pengatur kecepatan
byte state;

/**
 * Initialize.
 */
void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  pinMode(Motor_IN3, OUTPUT);
  pinMode(Motor_IN4, OUTPUT);
  pinMode(enB, OUTPUT);

//  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();

  SPI.begin();        // Init SPI bus

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
  state = 0; //kondisi awal-terkunci

  lcd.setCursor(0,0);
  lcd.print("Pintu Tertutup");
  lcd.setCursor(0,1);
  lcd.print("Scan Kartu");
}

void loop() {
  switch (state)
  {case 0: //standby-terkunci
           motorberhenti();
//           kondisiterkunci();
           cekcard1();
           
           break;
           
   case 1: //terbuka
           motorberhenti();
           cekcard2();
           
           break;
           
  }
}


void pintuterbuka(){
  analogWrite(enB, 255);
  digitalWrite(Motor_IN3, LOW);
  digitalWrite(Motor_IN4, HIGH);
  //delay(3700);
  delay(3000);
}

void pintutertutup(){
  analogWrite(enB, 255);
  digitalWrite(Motor_IN3, HIGH);
  digitalWrite(Motor_IN4, LOW);
  //delay(3900);
  //delay(3000);
  delay(3600);
}

void motorberhenti(){
  digitalWrite(Motor_IN3, HIGH);
  digitalWrite(Motor_IN4, HIGH);
}

void cekcard1(){
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.println(reader);
      // Show some details of the PICC (that is: the tag/card)
//      Serial.print(F(": Card UID:"));
      dump_byte_array1(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
//      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
//      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}

void dump_byte_array1(byte *buffer, byte bufferSize) {
  String tag = "";
  for (byte i = 0; i < bufferSize; i++) {
    tag.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    tag.concat(String(buffer[i], HEX));
  }
  tag.toUpperCase();

  //Checking the card
  if (tag.substring(1) == "4B EC 45 0B") //change here the UID of the card/cards that you want to give access
  {
    clearlcd();
    lcd.setCursor(0,0);
    lcd.print("ID Benar");
    lcd.setCursor(0,1);
    lcd.print("Pintu Terbuka");
//      kondisiterbuka();
    pintuterbuka();
    state = 1;
  }
  else
  {
    clearlcd();
    lcd.setCursor(0,0);
    lcd.print("ID Salah");
  //  kondisiterkunci();
    motorberhenti();
  }
  
}

void cekcard2(){
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.println(reader);
      // Show some details of the PICC (that is: the tag/card)
//      Serial.print(F(": Card UID:"));
      dump_byte_array2(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
//      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
//      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}

void dump_byte_array2(byte *buffer, byte bufferSize) {
  String tag = "";
  for (byte i = 0; i < bufferSize; i++) {
    tag.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    tag.concat(String(buffer[i], HEX));
  }
  tag.toUpperCase();

  //Checking the card
  if (tag.substring(1) == "4B EC 45 0B") //change here the UID of the card/cards that you want to give access
  {
    clearlcd();
    lcd.setCursor(0,0);
    lcd.print("ID Benar");
    lcd.setCursor(0,1);
    lcd.print("Pintu Tertutup");
    pintutertutup();
    motorberhenti();
//    kondisiterkunci();
    state = 0;
  }
  else
  {
    clearlcd();
    lcd.setCursor(0,0);
    lcd.print("Kartu Salah");
//    kondisiterbuka();
    motorberhenti();
  }
  
}

void clearlcd(){
  lcd.setCursor(0,0);
    for(int x = 0; x<16; ++x){
      lcd.write(' ');
    }
    lcd.setCursor(0,1);
    for(int y = 0; y<16; ++y){
      lcd.write(' ');
    }
}

