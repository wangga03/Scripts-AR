#include "secrets.h"
#include <Firebase.h>
#include <Servo.h>
#include <DHT.h>

// Pin konfigurasi untuk BTS7960
const int RPWM = 10;  // Kecepatan maju
const int R_EN = 12;  // Mengaktifkan arah maju
const int LPWM = 11;  // Kecepatan mundur
const int L_EN = 13;  // Mengaktifkan arah mundur

// Pin untuk sensor dan servo
const int sensorInduktif = 6;  // Pin sensor proximity logam
const int sensorIR = 4;        // Pin sensor inframerah
const int servoPin1 = 3;       // Servo 1
const int servoPin2 = 5;       // Servo 2

// Pin dan tipe sensor DHT
#define DHTPIN 9       // Pin DHT11
#define DHTTYPE DHT11  // Jenis sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

Servo servo1;
Servo servo2;

// Variabel status
bool conveyorRunning = false;
int motorSpeed = 200;  // Kecepatan default (0-255)
int speed = 0;
int pwmVal;
const float maxTemperature = 34.0;  // Suhu maksimum sebelum conveyor berhenti

// Variabel untuk menghitung benda
int totalBenda = 0;      // Total semua benda
int bendaLogam = 0;      // Total benda logam
int bendaNonLogam = 0;   // Total benda non-logam
int prevStateIR = HIGH;  // Status sebelumnya untuk sensor IR
float temperatureData = 0.;


bool stateButton;
float power;
int counter = 0;

/* Use the following instance for Test Mode (No Authentication) */
Firebase fb(REFERENCE_URL);

/* Use the following instance for Locked Mode (With Authentication) */
// Firebase fb(REFERENCE_URL, AUTH_TOKEN);


int timer_firebase_start;

void setup() {
  Serial.begin(115200);

  // Mengatur pin sebagai output
  pinMode(RPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(L_EN, OUTPUT);

  pinMode(sensorInduktif, INPUT);  // Sensor proximity logam
  pinMode(sensorIR, INPUT);        // Sensor inframerah

  // Memastikan motor mati saat awal
  motorStop();

  // Inisialisasi servo
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  // Posisi awal servo
  servo1.write(90);  // Servo 1 di 90 derajat
  servo2.write(0);   // Servo 2 di 0 derajat

  // Inisialisasi sensor DHT11
  dht.begin();

  // Inisialisasi Serial Monitor
  Serial.begin(9600);
  Serial.println("BTS7960 Motor Controller");
  Serial.println("Ketik 'START' untuk memulai conveyor.");
  Serial.println("Ketik 'STOP' untuk menghentikan conveyor.");
  Serial.println("Ketik angka (0-255) untuk mengatur PWM motor.");

  /* Connect to WiFi */
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("-");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.println();
  /* ----- */
}

void loop() {


  counter++;

  Serial.print("Counter : ");
  Serial.println(counter);

  stateButton = fb.getBool("ButtonState/state");
  power = fb.getFloat("TubesIoT/power/value");  //Power dalam persen


  Serial.print("State Button:\t\t");
  Serial.println(stateButton);
  Serial.print("Power:\t\t");
  Serial.println(power);

  // Mengatur power

  float val = (power / 100);

  pwmVal = 200 * val;
  speed = 200 - pwmVal;

  Serial.print("Pwm Val : ");
  Serial.println(pwmVal);

  Serial.print("Speed : ");
  Serial.println(speed);
  Serial.print("val : ");
  Serial.println(val);



  if (stateButton == 1) {
    conveyorRunning = true;
    // digitalWrite(R_EN, LOW);   // Matikan arah maju
    // digitalWrite(L_EN, HIGH);  // Aktifkan arah mundur
    // analogWrite(RPWM, 0);      // Pastikan maju mati
    // analogWrite(LPWM, power);  // Atur kecepatan mundur
    // Serial.println("Conveyor mulai berjalan mundur.");
  } else if (stateButton == 0) {
    conveyorRunning = false;
    digitalWrite(R_EN, LOW);  // Matikan arah maju
    digitalWrite(L_EN, LOW);  // Matikan arah mundur
    analogWrite(RPWM, 0);     // Pastikan maju mati
    analogWrite(LPWM, 0);     // Pastikan mundur mati
    Serial.println("Conveyor berhenti.");
  }


  // ======== ( Baca Sensor ) =======

  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    // Serial.println("Gagal membaca data dari sensor DHT11!");
  } else {
    // Serial.print("Suhu saat ini: ");
    // Serial.print(temperature);
    // Serial.println("°C");

    // Jika suhu melebihi batas maksimum, hentikan conveyor
    if (temperature > maxTemperature) {
      conveyorRunning = false;
      motorStop();
      // Serial.println("Peringatan! Suhu terlalu tinggi. Conveyor dihentikan.");
    }
  }

  if (conveyorRunning) {
    motorBackward(speed);  // Conveyor berjalan mundur dengan kecepatan yang ditentukan
    int sensorLogam = digitalRead(sensorInduktif);

    if (sensorLogam == 0) {  // Logam terdeteksi
      // Serial.println("Logam terdeteksi! Posisi servo berubah.");
      bendaLogam++;      // Tambahkan jumlah benda logam
      totalBenda++;      // Tambahkan total benda
      servo1.write(0);   // Servo 1 bergerak ke 0 derajat
      servo2.write(90);  // Servo 2 bergerak ke 90 derajat
      delay(5000);       // Tunggu 5 detik
      servo1.write(90);  // Servo 1 kembali ke 90 derajat
      servo2.write(0);   // Servo 2 kembali ke 0 derajat
      // Serial.println("Servo kembali ke posisi awal.");
    }

    // Membaca sensor inframerah untuk mendeteksi non-logam
    int sensorIRValue = digitalRead(sensorIR);

    // Deteksi tepi turun pada sensor IR
    if (sensorIRValue == LOW && prevStateIR == HIGH) {
      totalBenda++;            // Tambahkan total benda
      if (sensorLogam != 0) {  // Jika logam tidak terdeteksi, maka ini benda non-logam
        bendaNonLogam++;
        // Serial.println("Non-logam terdeteksi!");
      }
    }

    prevStateIR = sensorIRValue;  // Perbarui status sebelumnya untuk sensor IR

    // Menampilkan jumlah benda
    // Serial.print("Total Benda: ");
    // Serial.println(totalBenda);
    // Serial.print("Benda Logam: ");
    // Serial.println(bendaLogam);
    // Serial.print("Benda Non-Logam: ");
    // Serial.println(bendaNonLogam);
    sendData(temperature, bendaLogam, bendaNonLogam, totalBenda);  // Fungsi untuk mengirim data ganti setiap parameter dengan variable yang sesuai
    // Membaca sensor proximity logam

  } else {
    // Conveyor dalam keadaan berhenti
    delay(500);  // Mengurangi output spam saat conveyor tidak berjalan
  }
}

void sendData(float temperature, int induktif, int IR, int Jumlah) {
  fb.setInt("TubesIoT/induktif", induktif);
  fb.setInt("TubesIoT/IR", IR);
  fb.setInt("TubesIoT/SUM", Jumlah);
  fb.setFloat("TubesIoT/temperature", temperature);
}

// Fungsi untuk menggerakkan motor mundur
void motorBackward(int speed) {
  digitalWrite(R_EN, LOW);   // Matikan arah maju
  digitalWrite(L_EN, HIGH);  // Aktifkan arah mundur
  analogWrite(RPWM, 0);      // Pastikan maju mati
  analogWrite(LPWM, speed);  // Atur kecepatan mundur
}

// Fungsi untuk menghentikan motor
void motorStop() {
  digitalWrite(R_EN, LOW);  // Matikan arah maju
  digitalWrite(L_EN, LOW);  // Matikan arah mundur
  analogWrite(RPWM, 0);     // Pastikan maju mati
  analogWrite(LPWM, 0);     // Pastikan mundur mati
}