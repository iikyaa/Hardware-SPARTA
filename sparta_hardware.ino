// Library LCD I2C 20x4
#include <LiquidCrystal_I2C.h>
// Batas ambang sensor ultrasonik untuk mendeteksi kendaraan
#define treshold 7.2
// Library servo dan koneksi WiFi + Firebase
#include <ESP32Servo.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Konfigurasi koneksi WiFi
#define WIFI_SSID "kyy"
#define WIFI_PASSWORD "asdfghjkl27"

// Konfigurasi Firebase
#define FIREBASE_HOST "https://sparta-60d0f-default-rtdb.firebaseio.com/"  // URL database Firebase
#define FIREBASE_AUTH "AIzaSyB8PktXa1nlULc0QSG_ANBBlEyC_6qo28Q"            // Secret key Firebase

// Objek konfigurasi Firebase
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;
// Waktu pengiriman data Firebase
unsigned long previousMillis = 0;
const long interval = 2000;  // Kirim data setiap 2 detik

// Variabel untuk input sensor & tombol
int ir1;
int ir2;
int led1 = 5;
int led2 = 17;
int sig;
int button1, button2;
int tersedia; // slot tersedia

// PIN HCSR04 (sensor ultrasonik)
const int trigPin1 = 32;
const int echoPin1 = 33;
const int trigPin2 = 25;
const int echoPin2 = 26;
const int trigPin3 = 27;
const int echoPin3 = 14;
const int trigPin4 = 12;
const int echoPin4 = 13;
// bool initalizeSpot = false;

// LCD konfigurasi
int lcdColumns = 20;
int lcdRows = 4;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Status slot parkir
bool slot1;
bool slot2;
bool slot3;
bool slot4;

// Variabel jarak HCSR04
#define SOUND_SPEED 0.034
long duration;
float distanceCm1;
float distanceCm2;
float distanceCm3;
float distanceCm4;

// Servo motor gate
static const int servo1Pin = 4;
static const int servo2Pin = 16;
Servo servo1;
Servo servo2;
// Variabel hitung status
int Total = 4;
int p1, p2, p3, p4;
char names[4][4];

// Fungsi pengecekan jarak kendaraan dari sensor ultrasoni
void checkavailable() {
  // HCSR04 1
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);
  distanceCm1 = duration * SOUND_SPEED / 2;
  // HCSR04 2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration = pulseIn(echoPin2, HIGH);
  distanceCm2 = duration * SOUND_SPEED / 2;
  // HCSR04 3
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  duration = pulseIn(echoPin3, HIGH);
  distanceCm3 = duration * SOUND_SPEED / 2;
  // HCSR04 4
  digitalWrite(trigPin4, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin4, LOW);
  duration = pulseIn(echoPin4, HIGH);
  distanceCm4 = duration * SOUND_SPEED / 2;

  // SLOT 1 BAWAH
  if (distanceCm1 < treshold) {
    slot1 = true;
    p1 = 1;
  } else {
    slot1 = false;
    p1 = 0;
  }
  // SLOT 2 BAWAH
  if (distanceCm2 < treshold) {
    slot2 = true;
    p2 = 1;
  } else {
    slot2 = false;
    p2 = 0;
  }
  // SLOT 1 ATAS
  if (distanceCm3 < treshold) {
    slot3 = true;
    p3 = 1;
  } else {
    slot3 = false;
    p3 = 0;
  }
  // SLOT 2 ATAS
  if (distanceCm4 < treshold) {
    slot4 = true;
    p4 = 1;
  } else {
    slot4 = false;
    p4 = 0;
  }

  // tersedia = Total - p1 - p2 - p3 - p4;
  // initalizeSpot = true;
}

// Fungsi menampilkan slot parkir yang kosong
void checkspot() {
  if (p1 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silahkan Parkir di");
    lcd.setCursor(0, 1);
    lcd.print("p1");
    delay(3000);
    lcd.clear();

    return;
  }
  if (p2 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silahkan Parkir di");
    lcd.setCursor(0, 1);
    lcd.print("p2");
    delay(3000);
    lcd.clear();

    return;
  }
  if (p3 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silahkan Parkir di");
    lcd.setCursor(0, 1);
    lcd.print("p3");
    delay(3000);
    lcd.clear();

    return;
  }
  if (p4 == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Silahkan Parkir di");
    lcd.setCursor(0, 1);
    lcd.print("p4");
    delay(3000);
    lcd.clear();

    return;
  }
}

void setup() {
  Serial.begin(115200);  // Starts the serial communication
  // Konfigurasi PIN
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  // Koneksi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nTerhubung ke WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Konfigurasi Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Inisialisasi Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  // Setup servo awal (posisi tertutup)
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(90);
  servo2.write(90);
  checkavailable();
  // Baca kondisi slot awal
  lcd.init();
  lcd.backlight();
  // Inisialisasi LCD
  lcd.setCursor(0, 0);
  lcd.print("     HELLO USER");
  lcd.setCursor(0, 1);
  lcd.print("YOUR DEVICE IS READY");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     TUGAS AKHIR");
  lcd.setCursor(0, 1);
  lcd.print("  SISTEM IoT PARKIR");
  delay(2000);
  lcd.clear();
}

void loop() {

  //untuk penghitungan
  // int tersedia = Total - p1-p2-p3-p4;
  // Serial.println(tersedia);
  // Baca sensor dan tombol, sensor, servo
  ir1 = analogRead(34);
  ir2 = analogRead(35);
  button1 = digitalRead(19);
  button2 = digitalRead(18);
  sig = digitalRead(23);
  // Deteksi kendaraan dari IR
  if (ir1 > 500 && ir2 > 500) {
    digitalWrite(23, LOW);
  } else {
    digitalWrite(23, HIGH);
  }
  // Jika tidak ada slot yang tersedia
  // Kontrol gerbang masuk (servo1)
  if (tersedia == 0) {
    servo1.write(90);  // Servo tetap pada posisi 90 derajat (slot penuh)
  } else {
    // Jika ada slot tersedia, servo dan LED2 bisa kembali ke kontrol normal
    if (sig == HIGH && button1 == LOW) {
      checkspot();
      servo1.write(0);
      tersedia = tersedia - 1;
      Serial.print("tersedia: ");
      Serial.println(tersedia);
    }
    if (sig == LOW && button1 == HIGH) {
      servo1.write(90);
    }
  }
  // Kontrol gerbang keluar (servo2)
  if (button2 == LOW) {
    servo2.write(180);
  } else {
    delay(7000);
    tersedia = tersedia + 1;
    servo2.write(90);
  }

  // LED indikator: merah jika penuh, hijau jika tersedia
  if (tersedia == 0) {
    digitalWrite(led2, HIGH);
    digitalWrite(led1, LOW);
  } else {
    digitalWrite(led2, LOW);
    digitalWrite(led1, HIGH);
  }
  checkavailable(); // Perbarui status slot
  // Tampilkan status slot di LCD
  lcd.setCursor(0, 0);
  lcd.print("SLOT 1: ");
  if (slot1 == true) {
    lcd.print("terisi");
  } else {
    lcd.print("kosong");
  }
  // lcd.print(slot1);
  lcd.setCursor(0, 1);
  lcd.print("SLOT 2: ");
  if (slot2 == true) {
    lcd.print("terisi");
  } else {
    lcd.print("kosong");
  }
  // lcd.print(slot2);
  lcd.setCursor(0, 2);
  lcd.print("SLOT 3: ");
  if (slot3 == true) {
    lcd.print("terisi");
  } else {
    lcd.print("kosong");
  }
  // lcd.print(slot3);
  lcd.setCursor(0, 3);
  lcd.print("SLOT 4: ");
  if (slot4 == true) {
    lcd.print("terisi");
  } else {
    lcd.print("kosong");
  }
  // lcd.print(slot4);

  // Kirim data ke Firebase setiap 2 detik
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // SLOT PARKIRAN 1
    if (Firebase.setBool(firebaseData, "/parkiran/p1", slot1)) {
      Serial.println("Slot 1 berhasil dikirim ke Firebase!");
    } else {
      Serial.print("Gagal mengirim slot 1: ");
      Serial.println(firebaseData.errorReason());
    }
    // SLOT PARKIRAN 2
    if (Firebase.setBool(firebaseData, "/parkiran/p2", slot2)) {
      Serial.println("Slot 2 berhasil dikirim ke Firebase!");
    } else {
      Serial.print("Gagal mengirim slot 2: ");
      Serial.println(firebaseData.errorReason());
    }
    // SLOT PARKIRAN 3
    if (Firebase.setBool(firebaseData, "/parkiran/p3", slot3)) {
      Serial.println("Slot 3 berhasil dikirim ke Firebase!");
    } else {
      Serial.print("Gagal mengirim slot 3: ");
      Serial.println(firebaseData.errorReason());
    }
    // SLOT PARKIRAN 4
    if (Firebase.setBool(firebaseData, "/parkiran/p4", slot4)) {
      Serial.println("Slot 4 berhasil dikirim ke Firebase!");
    } else {
      Serial.print("Gagal mengirim slot 4: ");
      Serial.println(firebaseData.errorReason());
    }
    // SLOT TERSEDIA PARKIRAN
    if (Firebase.setString(firebaseData, "/parkiran/tersedia", tersedia)) {
      Serial.println("Sisa slot parkir berhasil dikirim ke Firebase!");
    } else {
      Serial.print("Gagal mengirim slot 4: ");
      Serial.println(firebaseData.errorReason());
    }
  }
}