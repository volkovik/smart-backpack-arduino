/*
   RFID RC522 подключение к микроконтроллеру
   ------------------------------------
               MFRC522      Arduino    
               Reader/PCD   Nano v3    
   Signal      Pin          Pin        
   ------------------------------------
   RST/Reset   RST          D9         
   SPI SS      SDA(SS)      D10        
   SPI MOSI    MOSI         D11        
   SPI MISO    MISO         D12        
   SPI SCK     SCK          D13        
*/

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN 9
#define SS_PIN 10

#define RX_PIN 7
#define TX_PIN 6


MFRC522 mfrc522(SS_PIN, RST_PIN); // Экземпляр класса для работы с RFID RC522
SoftwareSerial Bluetooth(RX_PIN, TX_PIN); // Второй Serial порт для Bluetooth

String currentID;
String lastID = "";
int countOfNoCardPresent = 0;


void setup() {
    // Инициализация
    Serial.begin(9600);
    Bluetooth.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
}


void loop() {
    // Если не была показана новая карта
    if (!mfrc522.PICC_IsNewCardPresent()) {
        // Если у нас есть ID прошлой карты
        if (lastID != "") {
            countOfNoCardPresent++;

            // Если счётчик ровняется 2-ум, то сейчас к ридеру не приложена карта
            if (countOfNoCardPresent == 2) {
                // Очищаем прошлый ID. Чистый ID считается индикатором того, что
                // у нас не была приложена карта
                lastID = "";
                delay(1000);
            }
        }
        
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    
    // Получаем ID метки
    currentID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        currentID += String(mfrc522.uid.uidByte[i], HEX);
    }
    currentID.toUpperCase();
    
    // Проверяем, приложена ли всё ещё та же карта
    if (currentID == lastID) {
        countOfNoCardPresent = 0; // Сбиваем счётчик, чтобы не присылать повторно ID
        return;
    } else {
        lastID = currentID;
    }

    Serial.println(currentID);
    Bluetooth.println(currentID); // Передаём по Bluetooth ID метки
}
