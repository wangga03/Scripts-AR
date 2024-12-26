#include "secrets.h"
#include <Firebase.h>
#include <Servo.h>
#include <DHT.h>

// Pin konfigurasi untuk BTS7960
const int RPWM = 10;  
const int R_EN = 12;  
const int LPWM = 11;  
const int L_EN = 13; 

// Pin untuk sensor dan servo
const int sensorInduktif = 6;  
const int sensorIR = 4;       
const int servoPin1 = 3;       
const int servoPin2 = 5;      

// Pin dan tipe sensor DHT
#define DHTPIN 9       
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

Servo servo1;
Servo servo2;

// Variabel status
bool conveyorRunning = false;
int motorSpeed = 200;  
int speed = 0;
int pwmVal;
const float maxTemperature = 34.0;  

// Variabel untuk menghitung benda
int totalBenda = 0;      
int bendaLogam = 0;      
int bendaNonLogam = 0;   
int prevStateIR = HIGH;  
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

  pinMode(RPWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(L_EN, OUTPUT);

  pinMode(sensorInduktif, INPUT);  
  pinMode(sensorIR, INPUT);       


  motorStop();


  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(140); 
  servo2.write(90);  


  dht.begin();

  Serial.println("Ketik 'START' untuk memulai conveyor.");
  Serial.println("Ketik 'STOP' untuk menghentikan conveyor.");
  Serial.println("Ketik angka (0-255) untuk mengatur PWM motor.");


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

}

void loop() {


  counter++;

  Serial.print("Counter : ");
  Serial.println(counter);

  stateButton = fb.getBool("ButtonState/state");
  power = fb.getFloat("TubesIoT/power/value"); 


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
    // digitalWrite(R_EN, LOW);   
    // digitalWrite(L_EN, HIGH);  
    // analogWrite(RPWM, 0);      
    // analogWrite(LPWM, power);  
    // Serial.println("Conveyor mulai berjalan mundur.");
  } else if (stateButton == 0) {
    conveyorRunning = false;
    digitalWrite(R_EN, LOW);  
    digitalWrite(L_EN, LOW);  
    analogWrite(RPWM, 0);   
    analogWrite(LPWM, 0);     
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
    if (temperature > maxTemperature) {
      conveyorRunning = false;
      motorStop();
      // Serial.println("Peringatan! Suhu terlalu tinggi. Conveyor dihentikan.");
    }
  }

  if (conveyorRunning) {
    motorBackward(speed);  
    int sensorLogam = digitalRead(sensorInduktif);

    if (sensorLogam == 0) {  // Logam terdeteksi
      // Serial.println("Logam terdeteksi! Posisi servo berubah.");
      bendaLogam++;     
      totalBenda++;     
      servo1.write(90);   
      servo2.write(45);  
      delay(5000);       
      servo1.write(140);  
      servo2.write(90);   
      // Serial.println("Servo kembali ke posisi awal.");
    }


    int sensorIRValue = digitalRead(sensorIR);
    if (sensorIRValue == LOW && prevStateIR == HIGH) {
      totalBenda++;            
      if (sensorLogam != 0) {  
        bendaNonLogam++;

      }
    }

    prevStateIR = sensorIRValue;  

    // Menampilkan jumlah benda
    // Serial.print("Total Benda: ");
    // Serial.println(totalBenda);
    // Serial.print("Benda Logam: ");
    // Serial.println(bendaLogam);
    // Serial.print("Benda Non-Logam: ");
    // Serial.println(bendaNonLogam);
    sendData(temperature, bendaLogam, bendaNonLogam, totalBenda); 


  } else {
    delay(500);  
  }
}

void sendData(float temperature, int induktif, int IR, int Jumlah) {
  fb.setInt("TubesIoT/induktif", induktif);
  fb.setInt("TubesIoT/IR", IR);
  fb.setInt("TubesIoT/SUM", Jumlah);
  fb.setFloat("TubesIoT/temperature", temperature);
}

void motorBackward(int speed) {
  digitalWrite(R_EN, LOW);   
  digitalWrite(L_EN, HIGH);  
  analogWrite(RPWM, 0);     
  analogWrite(LPWM, speed);  
}

void motorStop() {
  digitalWrite(R_EN, LOW);  
  digitalWrite(L_EN, LOW);  
  analogWrite(RPWM, 0);     
  analogWrite(LPWM, 0);    
}