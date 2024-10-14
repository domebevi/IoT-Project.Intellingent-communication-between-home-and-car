#include<Wire.h>
#include <SoftwareSerial.h>
const int MPU = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int Pitch, Roll;

//DEFINIZIONE PINs
#define pnpPIN 3
#define BT_TX_PIN 10
#define BT_RX_PIN 11

//VARIABILI DI SISTEMA
int received; //var ricezione bt
int l = 0;  //var tempo duty cycle
int incidente = 0;  //var incidente avvenuto
bool corri = false;

SoftwareSerial bt =  SoftwareSerial(BT_RX_PIN, BT_TX_PIN);  //inizializzazione seriale bt


void setup()
{
  pinMode(pnpPIN, OUTPUT);
  Serial.begin(9600);
  init_MPU();
  bt.begin(9600);
  digitalWrite(pnpPIN, HIGH);
}

void loop() {

  if (corri == true)
  {
    //RILEVAZIONI SENSORI
    digitalWrite(pnpPIN, HIGH);   //transistor ON

    while (bt.available() > 0)  //ricezione bt
      received = bt.read();

    FunctionsMPU();   //rilevazione dati giroscopio

    Roll = FunctionsPitchRoll(AcX, AcY, AcZ);   //calcolo angolo Roll
    Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //calcolo angolo Pitch

    if ((abs(Roll) > 30 or abs(Pitch) > 30) and incidente == 0)    //ribaltamento rilevato
    {
      incidente = 1;
      Serial.println("incidente");
    }

    //DUTY CYCLE
    l++;

    if (l == 8 and incidente == 1)    //invio rilevazione incidente
    {
      bt.print("i");
      incidente = 0;
    }

    if (l == 11)    //invio temperatura
      bt.print((int)(Tmp / 340.00 + 32.53)+1);

    if (l == 15)    //invio segnale di sync duty cycle
      bt.print("0");

    if (l == 17)    //spegnimento transistor e attesa
    {
      digitalWrite(pnpPIN, LOW);
      delay(4000);
      l = 0;
    }
  }
  
  else
  {
    while (bt.available() > 0)    //ricezione bt
      received = bt.read();
    if ((char)received == '1')
      corri = true;
  }
  delay(400);
}




//Funzione per l'inizializzazione del MPU-6050
void init_MPU() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);     //accendi MPU-6050
  Wire.endTransmission(true);
  delay(1000);
}

//Funzione per il calcolo degli angoli Pitch e Roll
double FunctionsPitchRoll(double A, double B, double C) {
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B * B) + (C * C);
  DatoB = sqrt(DatoB);

  Value = atan2(DatoA, DatoB);
  Value = Value * 180 / 3.14;

  return (int)Value;
}

//Funzione per l'acquisizione degli assi X,Y,Z e temperatura del MPU6050
void FunctionsMPU()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); //richiesta 14 registri
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
