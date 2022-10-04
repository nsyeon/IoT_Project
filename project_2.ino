#include <dht.h>
#include <DHT11.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#define   SS_PIN  10
#define   RST_PIN 9

int vResistor = A0;

DHT11 dht11(vResistor);

int RFID_ID;  // RFID의 ID

SoftwareSerial mySerial(2, 3); //RX,TX

MFRC522 mfrc522(SS_PIN, RST_PIN);

String ssid = "iptime_YSKIM";
String PASSWORD = "";
String host = "220.67.2.21";

void connectWifi()
{
    String join = "AT+CWJAP=\"" + ssid + "\",\"" + PASSWORD + "\"";

    Serial.println("Connect Wifi...");
    mySerial.println(join);
    delay(10000);
    if (mySerial.find("OK"))
    {
        Serial.print("WIFI connect\n");
    }
    else
    {
        Serial.println("connect timeout\n");
    }
    delay(1000);
}

void httpclient(String char_input)
{
    delay(100);
    Serial.println("connect TCP...");
    mySerial.println("AT+CIPSTART=\"TCP\",\"" + host + "\",8787");
    delay(500);
    if (Serial.find("ERROR"))
        return;

    Serial.println("Send data...");
    String url = char_input;
    String cmd = "GET /process.php?data=" + url + " HTTP/1.0\r\n\r\n";
    mySerial.print("AT+CIPSEND=");
    mySerial.println(cmd.length());
    Serial.print("AT+CIPSEND=");
    Serial.println(cmd.length());
    if (mySerial.find(">"))
    {
        Serial.print(">");
    }
    else
    {
        mySerial.println("AT+CIPCLOSE");
        Serial.println("connect timeout");
        delay(1000);
        return;
    }
    delay(500);
    mySerial.println(cmd);
    Serial.println(cmd);
    delay(100);

    if (Serial.find("ERROR2"))
        return;

    mySerial.println("AT+CIPCLOSE");
    delay(100);
}

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    mySerial.begin(9600);
    mfrc522.PCD_Init();
    connectWifi();
    delay(500);
}

void loop()
{
    // ★★★★★★★★★★
    if (!mfrc522.PICC_IsNewCardPresent())
      return;

    if (!mfrc522.PICC_ReadCardSerial())
      return;
    
    RFID_ID = mfrc522.uid.uidByte[0];
    String str_output = String(RFID_ID);
    
    delay(1000);
    httpclient(str_output);
    delay(1000); //Serial.find("+IPD");
    while (mySerial.available())
    {
        char response = mySerial.read();
        Serial.write(response);
        if (response == '\r')
            Serial.print('\n');
    }

    Serial.println("\n==================================\n");

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(2000);
}
