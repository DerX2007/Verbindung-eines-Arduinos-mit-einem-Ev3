/*
    Der Arduino tauscht die Sensordaten über I2C mit dem Ev3 aus.
    Der Arduino ist der Slave.
    Der Ev3 ist der Master.
   Ports:
    Belegt:
    13 Pieper
    12 Joystickbutton
    11 reciver
    10 Bewegungsmelder
    9  Display DIN
    8  Display Clk
    7  Display CS
    6 leer 
    5 knopfdrehgeber
    4 encoder b
    3 DHT 11
    2 encoder a
    1 leer
    0 leer

    A0 X Achse Joystick
    A1 Y Achse Joystick0
    A2 photoresistor
    A3 Wasserstandssensor
    A4 I2C
    A5 I2C

*/

#include <Wire.h>    //Biblothek für i2c
#include "IRremote.h"//Biblothek für Irfernbedienung
#include "LedControl.h"
#include <dht11.h>
#define DHT11PIN 3

//Variablen bestimmen
//BEWEGUNGSMELDER/////////////////////////////////
int bewegung = 10;
int bewegungsstatus = 0;
//JOYSTICK////////////////////////////////////////
int JoystickButton = 12;
int XAchse = A0;
int YAchse = A1;
int PositionX = 0;
int PositionY = 0;
int ButtonLesen = 0;
int vor = 0;
int hinten = 0;
int rechts = 0;
int links = 0;
int knopf = 0;
int knopfzwei = 0;
//drehgebermodul/////////////////////////////////////
int buttondreh = 7;
unsigned int lastReportedPos = 1;
static boolean rotating = false;
const int encoderPinA = 2;
const int encoderPinB = 4;
int Pos, oldPos ;
volatile int encoderPos = 90; 
boolean A_set = false;
boolean B_set = false;
//FERNBEDIENUNG/////////////////////////////////////
int emiter = 0;
int receiver = 11;
IRrecv irrecv(receiver);
decode_results results;
//Wasserstandssensor/////////////////////////////////
boolean wasseraus = false;
int wasserstandsrohwert = 0;
// I2C //////////////////////////////////////////////
int a = 0;
int richtwert = 83;

//DHT 11
dht11 DHT11;
//DHT 11



// Display
LedControl lc=LedControl(9,8,7,1);  // Pins: DIN,CLK,CS,  Display 
unsigned long delayTime=200;  // Delay between Frames


byte invader1a[] =
{
   B00011000,  // First frame of invader #1
   B00111100,
   B01111110,
   B11011011,
   B11111111,
   B00100100,
   B01011010,
   B10100101
};

byte invader1b[] =
{
  B00011000, // Second frame of invader #1
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01011010,
  B01000010
};

byte invader1c[] =
{
  B00100100, // First frame of invader #2
  B00100100,
  B01111110,
  B11011011,
  B11111111,
  B11111111,
  B10100101,
  B00100100
};

byte invader1d[] =
{
  B00100100, // Second frame of invader #2
  B10100101,
  B11111111,
  B11011011,
  B11111111,
  B01111110,
  B00100100,
  B01000010
};
//  Take values in Arrays and Display them
void sinvader1a()
{
  for (int i = 0; i < 8; i++)  
  {
    lc.setRow(0,i,invader1a[i]);
  }
}

void sinvader1b()
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(0,i,invader1b[i]);
  }
}

void sinvader2a()
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(1,i,invader1c[i]);
  }
}

void sinvader2b()
{
  for (int i = 0; i < 8; i++)
  {
    lc.setRow(1,i,invader1d[i]);
  }
}


//Buzzer
const int buzzer = 9; //buzzer to arduino pin 9



 
 



 
  



void setup()
{
  //i2c
  Wire.begin(111);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);

//Buzzer
 pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output


  Serial.begin(9600); //start Serieler Monitor
  //Drehgeber///////////////////////////////////////
  pinMode(encoderPinA, INPUT);//input auf pin  für
  pinMode(encoderPinB, INPUT);//input auf pin  für
  attachInterrupt(0, doEncoderA, CHANGE); // interupt a für Drehgeber
  attachInterrupt(1, doEncoderB, CHANGE); // interupt b für Drehgeber
  pinMode (buttondreh , INPUT);     //Drehgeberknopf
  digitalWrite(encoderPinA, HIGH);  // Drehgeber wiederstand an
  digitalWrite(encoderPinB, HIGH);  // Drehgeber wiederstand an
  //Joystick/////////////////////////////////////////
  pinMode(JoystickButton, INPUT);//input auf Pin  für Joystickknopf
  //bewegungsmelder//////////////////////////////////
  pinMode(bewegung, INPUT);//input auf Pin  für Bewegungsmelder
  //irfernbedienung//////////////////////////////////
  irrecv.enableIRIn();//Ir input



//display
 lc.shutdown(0,false);  // Wake up displays
  //lc.shutdown(1,false);
  lc.setIntensity(0,5);  // Set intensity levels
  //lc.setIntensity(1,5);
  lc.clearDisplay(0);  // Clear Displays
  //lc.clearDisplay(1);


}
int val, flag = 0, index = 0;

void loop()
{

//Buzzer

 tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec/


//Photoresistor
int val = analogRead(A2);  
Serial.println("Analog Value :");
Serial.println(val);  
delay(10);

//Matrix
  sinvader1a(); //display
   delay(10);
   
sinvader1b();// Display
    delay(10);
    




//DHT11/////////////////////////////////
  int chk = DHT11.read(DHT11PIN);

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Temperature (C): ");
  Serial.println((float)DHT11.temperature, 2);

  delay(2000);
  
  //Drehgeber///////////////////////////////////
  rotating = true;  // reset the debouncer

  if (lastReportedPos != encoderPos)
  {
    if (encoderPos < 1 )
    {
      encoderPos ++ ;
    }
    if (encoderPos > 100)
    {
      encoderPos = 0 ;
    }
    if (encoderPos > 100)
    {
      encoderPos -- ;
    }
    lastReportedPos = encoderPos;
  }

  //Joistick////////////////////////////////////////
  PositionX = analogRead(XAchse);
  PositionY = analogRead(YAchse);
  ButtonLesen = digitalRead(12);

  //reset i2c Varriablen
  rechts = 0;
  links = 0;
  vor = 0;
  hinten = 0;
  knopf = 0;
  if (ButtonLesen == 1)//Knopf gedrückt
  {
    knopf = 1;
  }



  if (PositionX > 600)//nach vorne
  {
    vor = 1;
  }

  if (PositionX < 400)//nach hinten
  {
    hinten = 1;
  }

  if (PositionY > 600)//nach rechts
  {
    rechts = 1;
  }

  if (PositionY < 400)//nach links
  {
    links = 1;
  }


  //IR ////////////////////////////////////////////////////////////////////////////////////////
  if (irrecv.decode(&results))//wenn ir empänger was merkt

  {
    translateIR();//geh zum interupt
    irrecv.resume();//und scan weiter
  }
  //Wasserstandssensor/////////////////////////////////////////////////////////////////////////
  wasserstandsrohwert = analogRead(2);
  if (wasserstandsrohwert > 400)
  {
    wasseraus = true;
  }
  else
  {
    wasseraus = false;
  }


  //wenn man will, dass man die Varriablen sieht (Der Arduino ist aber deutlich langsamer wenn er das schreiben muss):
/*
  Serial.println ("wert Bewegungsmelder =");
  Serial.print(bewegungsstatus);
  Serial.println ("wert Joistichknopf   =");
  Serial.print(knopf);
  Serial.println ("wert joistick links  =");
  Serial.print(links);
  Serial.println ("wert joistick rechts =");
  Serial.print(rechts);
  Serial.println ("wert joistick hinten =");
  Serial.print(hinten);
  Serial.println ("wert joistick vor    =");
  Serial.print(vor);
  Serial.println ("wert ir Sendung=");
  Serial.print(emiter);
  Serial.println(encoderencoderPos);
  Serial.print("neu ab hier //////////////////////////////////");
*/
}
//ENDE LOOP TEIL //////////////////////////////////////////////////////////////////////////////////

//i2c interupt wurde aufgerufen
void sendData()
{
  a ++ ; //(dazu da, dass der Arduino immer denn nächste Wert sendet)
  if (a == 1)
  {
    Wire.write(emiter);
   
  }
  if (a == 2)
  {
    Wire.write(encoderPos);
   
  }
  if (a == 3)
  {
    Wire.write(vor);
   
  }
  if (a == 4)
  {
    Wire.write(hinten);
   
  }
  if (a == 5)
  {
    Wire.write(rechts);
  
  }
  if (a == 6)
  {
    Wire.write(links);
   
  }
  if (a == 7)
  {
    Wire.write(digitalRead(12));
   
  }
  if (a == 8)
  {
    Wire.write(digitalRead(bewegung));
   
  }
  if (a == 9)
  {
    Wire.write(wasseraus);
   
  }
  if (a == 10)
  {
    Wire.write(a);
   
  }
  if (a == 11)
  {
    Wire.write(89);

  }
  if (a == 12)
  {
    Wire.write(richtwert);
  
    a = 0;
  }
}



void doEncoderA()
{
  if ( rotating ) delay (1);  // wait a little until the bouncing is done
  if ( digitalRead(encoderPinA) != A_set ) { // debounce once more
    A_set = !A_set;

    if ( A_set && !B_set )
      encoderPos ++;
    rotating = false;  // no more debouncing until loop() hits again
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB() {
  if ( rotating ) delay (1);
  if ( digitalRead(encoderPinB) != B_set ) {
    B_set = !B_set;

    if ( B_set && !A_set )
      encoderPos --;

    rotating = false;
  }
}




void receiveData(int byteCount)
{
  while (Wire.available() > 0)
  {

  }

}
void translateIR()

{

  switch (results.value)

  {
    case 0xFFA25D: //POWER");
      emiter = 1; break;
    case 0xFFE21D: //FUNC/STOP");
      emiter = 2; break;
    case 0xFF629D: //VOL+");
      encoderPos ++ ;
      emiter = 3; break;
    case 0xFF22DD: //FAST BACK");
      emiter = 4; break;
    case 0xFF02FD: //PAUSE");
      emiter = 5; break;
    case 0xFFC23D: //FAST FORWARD");
      emiter = 6; break;
    case 0xFFE01F: //DOWN");
      emiter = 7; break;
    case 0xFFA857: //VOL-");
      encoderPos -- ;
      emiter = 8; break;
    case 0xFF906F: //UP");
      emiter = 9; break;
    case 0xFF9867: //EQ");
      emiter = 10; break;
    case 0xFFB04F: //ST/REPT");
      emiter = 11; break;
    case 0xFF6897: //0");
      emiter = 12; break;
    case 0xFF30CF: //"1");
      emiter = 13; break;
    case 0xFF18E7: //2");
      emiter = 14; break;
    case 0xFF7A85: //3");
      emiter = 15; break;
    case 0xFF10EF: //4");
      emiter = 16; break;
    case 0xFF38C7: //5");
      emiter = 17; break;
    case 0xFF5AA5: //6");
      emiter = 18; break;
    case 0xFF42BD: //7");
      emiter = 19; break;
    case 0xFF4AB5: //8");
      emiter = 20; break;
    case 0xFF52AD: //9");
      emiter = 21; break;
      //   case 0xFFFFFFFF: Serial.println(" REPEAT");
      //     emiter = 22; break;
      Serial.print(emiter);
      // default:
      //   Serial.println(" other button   ");
      //   emiter = 23;


  }
}
