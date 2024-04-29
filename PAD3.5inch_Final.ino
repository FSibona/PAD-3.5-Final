#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV mylcd(ILI9486, A3, A2, A1, A0, A4); //model,cs,cd,wr,rd,reset

//define some colour values
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define AQUA 0x5D1C
#define DARK_CYAN 0x03EF
#define GRAY 0x7BEF
#define NAVY 0x000F
#define OLIVE 0x7BE0


//Variables de caudalimetro
#include <PinChangeInt.h>
#define PinCLK_pin11 11
#define PinCLK_pin10 10

const int measureInterval = 100;
volatile int pulseConter;
volatile int pulseConter2;
const float factorK = 7.5;// YF-S201
float volume = 0;
float volume2 = 0;
float precio = 0;
float precio2 = 0;
long t0 = 0;
int button;
int button2;
int reset;
int k = 0;
uint8_t rot = 1;


void ISRCountPulse()
{
  pulseConter++;
}

void ISRCountPulse2()
{
  pulseConter2++;
}

float GetFrequency()
{
  pulseConter = 0;

  interrupts();
  delay(measureInterval);
  noInterrupts();

  return (float)pulseConter * 1000 / measureInterval;
}

float GetFrequency2()
{
  pulseConter2 = 0;

  interrupts();
  delay(measureInterval);
  noInterrupts();

  return (float)pulseConter2 * 1000 / measureInterval;
}

void SumVolume(float dV)
{
  volume += dV / 60 * (millis() - t0) / 1000.0;
  t0 = millis();
}


void setup()
{
  mylcd.Init_LCD();
  mylcd.Set_Rotation(rot);
  mylcd.Fill_Screen(DARK_CYAN);
  mylcd.Set_Text_Mode(0);
//  mylcd.Set_Text_colour(CYAN);
//  mylcd.Set_Text_Size(6);
//  mylcd.Print_String("#TOMAAGUA", 2, 5);


  Serial.begin(9600);
  pinMode(PinCLK_pin10, INPUT); // Set normal 10 pin to Input
  pinMode(PinCLK_pin11, INPUT); // Set normal 11 pin to Input
  attachPinChangeInterrupt(PinCLK_pin11, ISRCountPulse, RISING); // interrupt connected to pin 11
  attachPinChangeInterrupt(PinCLK_pin10, ISRCountPulse2, RISING); // interrupt connected to pin 10

  t0 = millis();

  pinMode(0, INPUT_PULLUP);  //Reset
  pinMode(12, INPUT_PULLUP); // Pulsador de SURTIDOR GRANDE
  pinMode(13, INPUT_PULLUP); // Pulsador de SURTIDOR CHICO

  volume = 0;
  volume2 = 0;
  precio = 0;
  precio2 = 0;
}

void loop()
{
  reset = digitalRead(0);
  button = digitalRead(12);
  button2 = digitalRead(13);


  if ( reset == LOW)
  {
    volume = 0;
    precio = 0;
    volume2 = 0;
    precio2 = 0;

      //Rectangulos
    mylcd.Set_Draw_color(DARK_CYAN);
    mylcd.Fill_Rectangle(0, 70, 500, 150); 
    mylcd.Fill_Rectangle(0, 240, 500, 350);
  }



    if (button == LOW)
    {
      float frequency = GetFrequency();
      // calcular caudal L/min
      float dV = frequency / 0.8;
      volume += dV / 60 * (millis() - t0) / 1000;
      t0 = millis();
      precio = volume * 5;

    }
    if (button2 == LOW)
    {
      float frequency2 = GetFrequency2();
      float dV2 = frequency2 / 1.54;
      volume2 += dV2 / 60 * (millis() - t0) / 1000;
      t0 = millis();
      precio2 = volume2 * 5;

    }

    mylcd.Set_Text_colour(CYAN);
    mylcd.Set_Text_Size(4);
    mylcd.Set_Text_Back_colour(DARK_CYAN);

    mylcd.Print_String("Surtidor 2", 30, 30);
    mylcd.Print_String("Surtidor 3", 30, 180);
    mylcd.Set_Text_Size(3);
    mylcd.Print_String("Lts", 195, 110);
    mylcd.Print_String("Lts", 195, 260);
    mylcd.Set_Text_Size(5);
    mylcd.Print_String("$", 270, 100);
    mylcd.Print_String("$", 270, 250);


    //Numeros
    mylcd.Set_Text_colour(WHITE);
    mylcd.Set_Text_Size(5);
    mylcd.Print_Number_Float(volume, 1, 70, 100, '.', 0, ' ');
    mylcd.Print_Number_Float(precio, 1, 350, 100, '.', 0, ' ');
    mylcd.Print_Number_Float(volume2 , 1, 70, 250, '.', 0, ' ');
    mylcd.Print_Number_Float(precio2 , 1, 350, 250, '.', 0, ' ');

    //Subrayado
    mylcd.Fill_Rect(30, 62, 237, 4, GRAY);
    mylcd.Fill_Rect(30, 212, 237, 4, GRAY);

  
  //Rectangulos
  //  mylcd.Set_Draw_color(WHITE);
  //  mylcd.Draw_Round_Rectangle(5, 80, 190, 150, 5);
  //  mylcd.Draw_Round_Rectangle(300, 80, 470, 150, 5);
  //  mylcd.Draw_Round_Rectangle(5, 230, 190, 300, 5);
  //  mylcd.Draw_Round_Rectangle(300, 230, 470, 300, 5);

  //  mylcd.Set_Text_Mode(0);
  //  //display 1 times string
  //  //mylcd.Fill_Screen(0x0000);
  //  mylcd.Set_Text_colour(CYAN);
  //  mylcd.Set_Text_Back_colour(BLACK);
  //  mylcd.Set_Text_Size(4);
  //  mylcd.Print_String("Surtidor 2", 30, 30);
  //  mylcd.Print_String("Surtidor 3", 30, 180);
  //  mylcd.Set_Text_Size(3);
  //  mylcd.Print_String("Lts", 195, 110);
  //  mylcd.Print_String("Lts", 195, 260);
  //  mylcd.Set_Text_Size(5);
  //  mylcd.Print_String("$", 270, 100);
  //  mylcd.Print_String("$", 270, 250);
  //
  //
  //  //Numeros
  //  mylcd.Set_Text_colour(GREEN);
  //  mylcd.Set_Text_Size(5);
  //  mylcd.Print_Number_Float(volume, 2, 70, 100, '.', 0, ' ');
  //  mylcd.Print_Number_Float(precio, 2,350, 100, '.', 0, ' ');
  //  mylcd.Print_Number_Float(volume2 , 2, 70, 250, '.', 0, ' ');
  //  mylcd.Print_Number_Float(precio2 , 2, 350, 250, '.', 0, ' ');
  //
  //  //Rectangulos
  //  mylcd.Draw_Round_Rectangle(5, 80, 190, 150, 5);
  //  mylcd.Draw_Round_Rectangle(300, 80, 470, 150, 5);
  //  mylcd.Draw_Round_Rectangle(5, 230, 190, 300, 5);
  //  mylcd.Draw_Round_Rectangle(300, 230, 470, 300, 5);


}
