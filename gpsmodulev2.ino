#include <SoftwareSerial.h>

SoftwareSerial sim808(2, 3);  // RX, TX

unsigned long donguzaman = 0;
unsigned long serialzaman = 0;

String phone_no = "+905010847207";  // Telefon numarası
String msggelen;
String msggiden;
String msg;
String latitude, longitude, tarih, enlem, boylam, yil, gun, ay, saat, dakika, rakim, hiz, zaman;

#define bir 1
#define onn 10
#define yuz 100
#define ikiyuz 200

int signalStrength = 0;



void checkSignalStrength() {
  sim808.println("AT+CSQ");
  delay(1000);

  if (sim808.find("+CSQ: ")) {
    signalStrength = sim808.parseInt();
    delay(500);
    if (signalStrength == 0) {
      Serial.println("Sinyal yok veya çok zayıf.");
    } else if (signalStrength >= 2 && signalStrength <= 30) {
      Serial.print("Sinyal gücü: ");
      Serial.println(signalStrength);
    } else {
      Serial.println("Geçersiz sinyal gücü değeri.");
    }
  } else {
    Serial.println("Sinyal gücü alınamadı.");
  }
}


void deneme() {
  checkSignalStrength();
  delay(500);
  sim808.println("AT+CMGF=1");  // SMS gönderme formatını metin moduna ayarla
  delay(1000);

  while (signalStrength < 2) {
    checkSignalStrength();
    delay(1000);
  }
  Serial.println("SMS Gönderiyorum");
  sim808.print("AT+CMGS=\"");
  sim808.print("+905438737595");
  sim808.println("\"");

  delay(1000);

  sim808.print("konum");
  delay(100);
  sim808.println((char)26);  // CTRL+Z karakteri (SMS gönderme komutu için)
  delay(1000);
  Serial.print("sim 808 SMS gelen : ");
  // Cevabı oku
  while (sim808.available()) {
    char c = sim808.read();
    Serial.write(c);
  }
  Serial.println();



  if (sim808.find("OK")) {
    Serial.println("SMS başarıyla gönderildi.");
  } else {
    Serial.println("SMS gönderirken bir hata oluştu. yeniden deniyorum");
    delay(1000);
    gpskonum();
  }
}


void sendSMS(String number, String message,String hiz,String tarih,String saat) {
  checkSignalStrength();
  delay(500);
  sim808.println("AT+CMGF=1");  // SMS gönderme formatını metin moduna ayarla
  delay(1000);

  while (signalStrength < 2) {
    checkSignalStrength();
    delay(10000);
  }
  Serial.println("SMS Gönderiyorum");
  sim808.print("AT+CMGS=\"");
  sim808.print(number);
  sim808.println("\"");

  delay(1000);

  sim808.println(message);
  sim808.println(hiz);
  sim808.println(tarih);
  sim808.println(saat);


  Serial.println(number);
  Serial.println(message);

  delay(100);
  sim808.println((char)26);  // CTRL+Z karakteri (SMS gönderme komutu için)
  delay(5000);               // SMS gönderdikten sonra beklemek için



Serial.print("sim 808 SMS gelen : ");
  // Cevabı oku
  while (sim808.available()) {
    char c = sim808.read();
    Serial.write(c);
  }
  Serial.println();



  if (sim808.find("OK")) {
    Serial.println("SMS başarıyla gönderildi.");
  } else {
    Serial.println("SMS gönderirken bir hata oluştu. yeniden deniyorum");
    delay(1000);
    gpskonum();
  }
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void gpskonum() {
  Serial.println("GPS Kontrol Ediliyor");
  sim808.print("AT+CGNSINF\r\n");
  delay(1000);
  readsimPort();

  if (msggelen != "") {
    Serial.println("Konum sim808 GELEN:  ");
    Serial.println(msggelen);
  }

  if ((msggelen.indexOf("+CGNSINF: 1,1,") > -1)) {
    Serial.println("GPS Aktif Konum Bulundu :");
    delay(onn);
    tarih = getValue(msggelen, ',', 2);
    delay(onn);
    yil = tarih.substring(0, 4);
    delay(onn);
    gun = tarih.substring(6, 8);
    delay(onn);
    saat = tarih.substring(8, 10);
    delay(onn);

    enlem = getValue(msggelen, ',', 3);
    delay(onn);
    latitude = enlem.substring(0, 10);
    delay(onn);
    Serial.println("Enlem :" + latitude);
    delay(onn);

    boylam = getValue(msggelen, ',', 4);
    delay(onn);
    longitude = boylam.substring(0, 11);
    delay(onn);
    Serial.println("Boylam :" + longitude);
    delay(1000);

    rakim = getValue(msggelen, ',', 5);
    delay(onn);
    rakim = rakim.substring(0, 6);
    delay(onn);

    hiz = getValue(msggelen, ',', 6);
    delay(onn);
    hiz = hiz.substring(0, 5);
    delay(onn);

    msggelen = "";
    delay(onn);
    readsimPort();
    delay(1000);

    sendSMS(phone_no, "Konumunuz: https://www.google.com/maps/search/" + latitude + "," + longitude,hiz,tarih,saat);
    delay(2000);
  } else if ((msggelen.indexOf("+CGNSINF: 1,0,") > -1)) {
    sendSMS(phone_no, "Gps Açık,Konum bulunamadı lütfen bekleyin","","","");
    delay(onn);
  } else if ((msggelen.indexOf("+CGNSINF: 0,") > -1)) {
    sendSMS(phone_no, "GPS Kapalı, Acilacak :","","","");
    delay(onn);
  } else {
    msggelen = "";
    delay(onn);
  }
}

void verikontrol() {
  if (((msggelen.indexOf("konum") > -1) || (msggelen.indexOf("Konum") > -1) || (msggelen.indexOf("KONUM") > -1))) {
    Serial.println("Konum atilacak");
    delay(yuz);
    msggelen = "";
    delay(onn);
    return gpskonum();
  }
}

void setup() {
  Serial.begin(9600);
  sim808.begin(9600);  // SIM808 için bağlantı hızı 9600 baud olarak ayarlanmıştır
  delay(1000);
  Serial.println("SIM808 Test");

  // SIM kartı ve modül hazır olana kadar bekle
  while (!sim808.available()) {
    sim808.println("AT");
    delay(1000);
    Serial.println("Bağlanılıyor...");
  }

  Serial.println("Bağlantı başarılı!");

  setSMSC("+905598008000");
  checkSignalStrength();
  setsim808();  // Sadece ilk çalıştırmada kullanılır, sonrasında yorum satırına alınabilir.
  delay(5000);
 // deneme();
}

void loop() {
  donguzaman = millis();
  readsimPort();
  readSerialPort();

  if (msggelen != "") {
    Serial.println("sim808 GELEN:  ");
    Serial.println(msggelen);
    verikontrol();
    msggelen = "";
  }
  if (msggiden != "") {
    Serial.print("bizden giden :");
    Serial.println(msggiden);
    sim808.print(msggiden);
    msggiden = "";
  }
}


void setSMSC(String smscNumber) {
  sim808.print("AT+CSCA=\"");
  sim808.print(smscNumber);
  sim808.println("\"");
  delay(1000);

  if (sim808.find("OK")) {
    Serial.println("SMS Merkezi numarası başarıyla ayarlandı.");
  } else {
    Serial.println("SMS Merkezi numarası ayarlanırken bir hata oluştu.");
  }
}

void setsim808() {
  delay(onn);

  sim808.print("AT+CGPSPWR=1\r\n");
  delay(ikiyuz);  // GPS açık
  sim808.print("AT+CGNSSEQ=RMC\r\n");
  delay(ikiyuz);  // GPS açık

  sim808.print("AT+CMGF=1\r\n");
  delay(ikiyuz);  // SMS metin modunu ayarla
  sim808.print("AT+CNMi=2,2,0,0,0\r\n");
  delay(ikiyuz);  // Gelen mesajı okuma
  sim808.print("AT+CLIP=1\r\n");
  delay(ikiyuz);

  sim808.print("ATS0=0\r\n");
  delay(ikiyuz);  // Zil süresi

  sim808.print("AT&W\r\n");
  delay(ikiyuz);  // Profil kaydet
  Serial.println("Kullanıma Hazır.");
}

void readsimPort() {
  serialzaman = donguzaman;
  delay(bir);
  while ((donguzaman - serialzaman) < 200) {
    donguzaman = millis();
    while (sim808.available()) {
      donguzaman = millis();
      serialzaman = donguzaman;
      char c = sim808.read();
      msggelen += c;
    }
  }
  sim808.flush();
}

void readSerialPort() {
  while (Serial.available()) {
    delay(onn);
    if (Serial.available() > 0) {
      char d = Serial.read();  //gets one byte from serial buffer
      msggiden += d;           //makes the string readString
    }
  }
  Serial.flush();
}
