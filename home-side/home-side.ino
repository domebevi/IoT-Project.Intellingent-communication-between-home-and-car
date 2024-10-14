#include <SoftwareSerial.h>
#define BT_TX_PIN 10
#define BT_RX_PIN 11

//PIN SENSORI
#define fotoPIN A0
#define tempPIN A1
#define pirPIN 7

//PIN ATTUATORI
#define luxPIN 13
#define hotPIN 12
#define garagePIN 9

//PIN TRANSISTOR
#define pnpPIN 3

//VARIABILI DI SISTEMA
int lux = 0;  //var luce
int hot = 0;  //var caldaia
int pir = 0;   //var sensore IR
int garage = 0;   //var garage

bool corri = false;

int received;   //var ricezione bt
int l = 0;  //var tempo duty cycle

SoftwareSerial bt =  SoftwareSerial(BT_RX_PIN, BT_TX_PIN);    //inizializzaione seriale bt

void setup()
{
  pinMode(fotoPIN, INPUT);
  pinMode(tempPIN, INPUT);
  pinMode(luxPIN, OUTPUT);
  pinMode(hotPIN, OUTPUT);
  pinMode(garagePIN, OUTPUT);
  pinMode(pnpPIN, OUTPUT);
  pinMode(pirPIN, INPUT);
  Serial.begin(9600);
  digitalWrite(pnpPIN, HIGH);
  bt.begin(9600);
}

void loop()
{
  if (corri == true)
  {
    //RILEVAMENTO SENSORI
    digitalWrite(pnpPIN, HIGH);   //transistor ON

    while (bt.available() > 0)    //ricezione bt
      received = bt.read();

    lux = analogRead(fotoPIN);    //rilevamento luce

    hot = analogRead(tempPIN);    //rilevamento temperatura e conversione in gradi
    float temp = ((hot * 0.00455) - 0.5) / 0.01;

    if (pir == 0) //rilevamento movimento IR
      pir = digitalRead(pirPIN);


    //CONTROLLO ATTUATORI

    //luci
    if (char(received) == 'l')
      digitalWrite(luxPIN, HIGH);
    if (char(received) == 'b')
      digitalWrite(luxPIN, LOW);
    //caldaia
    if (char(received) == 'c')
      digitalWrite(hotPIN, HIGH);
    if (char(received) == 'f')
      digitalWrite(hotPIN, LOW);
    //garage
    if (char(received) == 'u')
      digitalWrite(garagePIN, HIGH);
    if (char(received) == 'd')
      digitalWrite(garagePIN, LOW);

    //DUTY CYCLE
    l++;

    if ((l == 8) and (pir == 1))  //invio rilevazione movimento
    {
      bt.print("a");
      pir = 0;
    }

    if (l == 11)    //invio temperatura
      bt.print((int)temp);

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
