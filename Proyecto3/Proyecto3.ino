//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);


extern uint8_t fondo[];
//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  pinMode (PF_1, INPUT);//Right
  pinMode (PA_5, INPUT);//Left
  pinMode (PA_7, INPUT);//Shoot
  pinMode (PA_6, INPUT);//Reset
  pinMode (PE_3, INPUT);//Start

  pinMode (PF_2, OUTPUT);//5V
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);

//***************************************************************************************************************************************
// Start Screen
//***************************************************************************************************************************************
  FillRect(0, 0, 319, 239, 0x0000);
  LCD_Bitmap(0, 0, 320, 240, fondo);
  
  String text1 = "Defend from";
  LCD_Print(text1, 20, 100, 2, 0xffff, 0x0000);
  String text2 = "Invaders";
  LCD_Print(text2, 60, 150, 2, 0xffff, 0x0000);
  
  //LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

  delay(1500);
  FillRect(0, 0, 320, 240, 0x0000);
    
  //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
  
  for(int x = 0; x <319; x++){
    LCD_Bitmap(x, 180, 16, 5, tile);
    x += 15;
  }
  String text = "Press Start";
  LCD_Print(text, 75, 10, 2, 0xffff, 0x0000);
  
}
//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
int W = 1;
int counter = 0;

int V1 = 0;
int X1 = 1;
int Y1 =0;
int bounce1 = 0;
int V2 = 0;
int X2 = 50;
int Y2 =0;
int bounce2 = 0;
int V3 = 0;
int X3 = 100;
int Y3 =0;
int bounce3 = 0;

int animJ = 0;
int XJ = 144;
int RIGHT;
int LEFT;
int SHOOT;

int VB = 0;
int showB = 0;
int XB = 0;
int YB = 200;

int score=0;
int RESET;
int START;
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  digitalWrite(PF_2, HIGH); //5V
  START = digitalRead(PE_3);
  
  if (START==HIGH){
    W=0;
  }
  while(W==0){
    delay(15);
    counter++;

    for(int x = 0; x <319; x++){
    LCD_Bitmap(x, 180, 16, 5, tile);
    x += 15;
    }
    
    String text3 = String(score);
    LCD_Print(text3, 0, 200, 2, 0xffff, 0x0000);
  
    String text5 = String(counter);
    LCD_Print(text5, 250, 200, 2, 0xffff, 0x0000);
    
//***************************************************************************************************************************************
// Alien 1
//***************************************************************************************************************************************
    if(V1==0){
      int anim1 = (X1/35)%2;
      
      if(counter >= 30){
        if(bounce1==0){
          LCD_Sprite(X1,Y1,16,16,ALIEN,2,anim1,0,1);
          V_line( X1 -1, Y1, 16, 0x0000);
          X1++; 
        }
        if (bounce1==1){
          LCD_Sprite(X1,Y1,16,16,ALIEN,2,anim1,0,1);
          V_line( X1 +16, Y1, 16, 0x0000);
          X1--;
        }
        if (X1==320-32){
          bounce1=1;
          FillRect(X1-1, Y1, 16, 16, 0x0000);
          Y1=Y1+20;
        }
        if (X1==0){
          bounce1=0;
          FillRect(X1+1, Y1, 16, 16, 0x0000);
          Y1=Y1+20;
        }
      }
    }

//***************************************************************************************************************************************
// Alien 2
//***************************************************************************************************************************************
    if(V2==0){
      int anim2 = (X2/35)%2;
      
      if(counter >= 450){
        if(bounce2==0){
          LCD_Sprite(X2,Y2,16,16,ALIEN,2,anim2,0,1);
          V_line( X2 -1, Y2, 16, 0x0000);
          X2++; 
        }
        if (bounce2==1){
          LCD_Sprite(X2,Y2,16,16,ALIEN,2,anim2,0,1);
          V_line( X2 +16, Y2, 16, 0x0000);
          X2--;
        }
        if (X2==320-32){
          bounce2=1;
          FillRect(X2-1, Y2, 16, 16, 0x0000);
          Y2=Y2+20;
        }
        if (X2==0){
          bounce2=0;
          FillRect(X2+1, Y2, 16, 16, 0x0000);
          Y2=Y2+20;
        }
      }
    }

//***************************************************************************************************************************************
// Alien 3
//***************************************************************************************************************************************
    if(V3==0){
      int anim3 = (X3/35)%2;
      
      if(counter >= 600){
        if(bounce3==0){
          LCD_Sprite(X3,Y3,16,16,ALIEN,2,anim3,0,1);
          V_line( X3 -1, Y3, 16, 0x0000);
          X3++; 
        }
        if (bounce3==1){
          LCD_Sprite(X3,Y3,16,16,ALIEN,2,anim3,0,1);
          V_line( X3 +16, Y3, 16, 0x0000);
          X3--;
        }
        if (X3==320-32){
          bounce3=1;
          FillRect(X3-1, Y3, 16, 16, 0x0000);
          Y3=Y3+20;
        }
        if (X3==0){
          bounce3=0;
          FillRect(X3+1, Y3, 16, 16, 0x0000);
          Y3=Y3+20;
        }
      }
    }

//***************************************************************************************************************************************
// Jugador
//***************************************************************************************************************************************
    
    LCD_Sprite(XJ, 200, 32, 32, SHIP,3,animJ,1, 0);
    RIGHT = digitalRead(PF_1);
    LEFT = digitalRead(PA_5);
    SHOOT = digitalRead(PA_7);
    
    if (RIGHT==HIGH){
      if (XJ<=320-32){
        V_line( XJ -1, 200, 32, 0x0000);
        XJ++;
        animJ=2;
      }
    }
    if (LEFT==HIGH){
      if (XJ>=0){
        V_line( XJ + 16, 200, 32, 0x0000);
        XJ--;
        animJ=1;
      }
    }
    if (SHOOT==HIGH){
      VB=1;
      animJ=0;
    }

//***************************************************************************************************************************************
// Bullet
//***************************************************************************************************************************************
    if (VB==1){
      YB=200;
      XB=XJ+8;
      showB = 1;
      VB=0;
    }
    if (showB==1){
      LCD_Sprite(XB, YB, 16, 16, BULLET,1,0,1, 0);
      H_line( XB,YB +16,16,0x0000);
      YB--;

      if (YB==0){
        YB=200;
        showB=0;
      }
      
      if ((XB>=X1-10 && XB<=X1+10) && (YB>=Y1 && YB<=Y1+16)){
        score=score+100;
        FillRect(X1-1,Y1,16, 16, 0x0000);
        YB=200;
        X1 = 1;
        Y1 =0;
        showB=0;
      }
      else if ((XB>=X2-10 && XB<=X2+10) && (YB>=Y2 && YB<=Y2+16)){
        score=score+100;
        FillRect(X2-1,Y2,16, 16, 0x0000);
        YB=200;
        X2 = 1;
        Y2 =0;
        showB=0;
      }
      else if ((XB>=X3-10 && XB<=X3+10) && (YB>=Y3 && YB<=Y3+16)){
        score=score+100;
        FillRect(X3-1,Y3,16, 16, 0x0000);
        YB=200;
        X3 = 1;
        Y3 =0;
        showB=0;
      }
    }
//***************************************************************************************************************************************
// Game Over
//***************************************************************************************************************************************

    if (Y1 > 180-16 || Y2 > 180-16 || Y3 > 180-16){
      W=1;
      FillRect(0, 0, 319, 239, 0x0000);
      String text1 = "Game Over";
      LCD_Print(text1, 20, 100, 2, 0xffff, 0x0000);
      String text2 = "Score: ";
      LCD_Print(text2, 0, 150, 2, 0xffff, 0x0000);
      String text3 = String(score);
      LCD_Print(text3, 100, 150, 2, 0xffff, 0x0000);
      String text4 = "Time: ";
      LCD_Print(text4, 0, 200, 2, 0xffff, 0x0000);
      String text5 = String(counter);
      LCD_Print(text5, 100, 200, 2, 0xffff, 0x0000);
    }

  }

//***************************************************************************************************************************************
// Restart
//***************************************************************************************************************************************
  RESET = digitalRead(PA_6);
  
  if (RESET == HIGH){
    W = 0;
    counter = 0;

    V1 = 0;
    X1 = 1;
    Y1 =0;
    bounce1 = 0;
    V2 = 0;
    X2 = 50;
    Y2 =0;
    bounce2 = 0;
    V3 = 0;
    X3 = 100;
    Y3 =0;
    bounce3 = 0;

    XJ = 144;

    VB = 0;
    showB = 0;
    XB = 0;
    YB = 200;

    score=0;
  }
}
//***************************************************************************************************************************************
// inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Enviar comandos a la LCD
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Definir ventana
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      
      //LCD_DATA(bitmap[k]);    
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
