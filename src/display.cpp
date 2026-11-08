
#include <inttypes.h>
#include "main.h"
#include "display.h"
#include "text.h"

#include "defines.h"

//extern uint16_t currentexpoarray[5][513];

extern uint8_t kanalsettingarray[5][4][4];

extern  uint8_t                 curr_model; // aktuelles modell
extern  uint8_t                 speichermodel;
extern  uint8_t                 curr_funktion; // aktueller kanal
extern  uint8_t                 curr_aktion;
extern  uint8_t                 curr_wert;
extern  uint8_t                 curr_modus;

extern  uint8_t                 curr_setting; // aktuelles Setting fuer Modell
extern  uint8_t                 curr_screen; // aktueller screen
extern  uint8_t                 last_screen; // letzter screen

extern  uint8_t                 curr_page; // aktuelle page
extern  uint8_t                 curr_col; // aktuelle colonne

extern  uint8_t                 curr_cursorzeile; // aktuelle zeile des cursors
extern  uint8_t                 curr_cursorspalte; // aktuelle colonne des cursors
extern  uint8_t                 last_cursorzeile; // letzte zeile des cursors
extern  uint8_t                 last_cursorspalte; // letzte colonne des cursors

extern uint8_t                  curr_pfeil;

extern uint8_t                   curr_steuerstatus;

extern uint16_t                  blink_cursorpos;
extern uint8_t                   blinkstatus;
extern uint8_t                   calibstatus;

extern uint8_t adresse;
extern uint16_t paketcounter;
extern uint16_t adressmatch;


#define BLINKPFEILUP    0
#define BLINKPFEILDOWN    1

extern uint8_t                      calibstatus;


#define cursortab0 2
#define cursortab1 26
#define cursortab2 46
#define cursortab3 54
#define cursortab4 66
#define cursortab5 78
#define cursortab6 90
#define cursortab7 100
extern   uint8_t cursortab[10] = {cursortab0,cursortab1,cursortab2,cursortab3,cursortab4,cursortab5,cursortab6,cursortab7,cursortab0,cursortab0};
//extern  uint16_t  cursorpos[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer cursor (hex). geladen aus progmem

extern uint16_t   stopsekunde;
extern uint16_t   stopminute;
extern uint16_t   throttlecounter;
extern uint16_t    throttlesekunden;

extern uint16_t blinkcounter;

#define itemtab0  10
#define itemtab1  34
#define itemtab2  50
#define itemtab3  62
#define itemtab4  74
#define itemtab5  88
#define itemtab6  110
#define itemtab7  118

extern  uint8_t itemtab[10] = {itemtab0,itemtab1,itemtab2,itemtab3,itemtab4,itemtab5,itemtab6,itemtab7,itemtab0,itemtab0};

//extern  uint16_t  posregister[8][8]; // Aktueller screen: werte fuer page und daraufliegende col fuer Menueintraege (hex). geladen aus progmem

#define  taby0    16
#define  taby1    24
#define  taby2    32
#define  taby3    40
#define  taby4    48
#define  taby5    56
#define  taby6    56
#define  taby7    56

extern  uint8_t taby[8] = {taby0,taby1,taby2,taby3,taby4,taby5,taby6,taby7};

#define menu0  24
#define menu1  40
#define menu2  56

#define menuh 20

//extern float UBatt;
//extern uint16_t batterieanzeige;
extern Signal data;





uint8_t charh = 0;
uint8_t balkenh = 50;
uint8_t balkenb = 5;
uint8_t balkenvh = 40;
 uint8_t balkenvb = 5;
 uint8_t balkenhh = 3;
 uint8_t balkenhb = 40;

//char menubuffer[20];
//char titelbuffer[20];


U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void initDisplay()
{
  u8g2.begin();
  uint8_t c = 0;
}

 void oled_fill(uint8_t x,uint8_t y,uint8_t l)
{
   //u8g2.setDrawColor(0);
   u8g2.drawBox(x,y-charh,l,charh+4);
   //u8g2.setDrawColor(1);
   u8g2.sendBuffer();
}



void oled_setInt(uint8_t x,uint8_t y, uint16_t data)
{
   u8g2.setCursor(x,y);
   u8g2.print(data);
   u8g2.sendBuffer();

}

void oled_delete(uint8_t x,uint8_t y,uint8_t l)
{
   u8g2.setDrawColor(0);
   u8g2.drawBox(x,y-charh,l,charh+4);
   u8g2.setDrawColor(1);
   //u8g2.sendBuffer();
}

void oled_frame(uint8_t x,uint8_t y,uint8_t l)
{
   //u8g2.setDrawColor(0);
   u8g2.drawFrame(x,y-charh,l,charh+4);
   //u8g2.setDrawColor(1);
   u8g2.sendBuffer();
}
void oled_vertikalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h)
{
   u8g2.drawFrame(x,y,b,h);


}

void oled_vertikalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert)
{
  //Serial.print(h);
  //Serial.print("\t");
  //Serial.print(wert);
  
  //Serial.print("\n");
  
  
  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);
  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  u8g2.drawHLine(x,y+h-wert,b);
  u8g2.drawHLine(x,y+h-wert-1,b);
  u8g2.drawHLine(x,y+h-wert+1,b);

}

void oled_batteriebalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint16_t wert)
{
 uint8_t anzeige = map(wert-30,0,12,0,h); // Bereich 3-4.2V, 1.2V
 uint8_t min = map(2,0,12,0,h);
  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);
  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  //u8g2.drawHLine(x,y+h-wert,b);
  //u8g2.drawHLine(x,y+h-wert-1,b);
  //u8g2.drawHLine(x,y+h-wert+1,b);
  u8g2.drawBox(x+1,y+h-anzeige,b-2,anzeige);
  u8g2.setDrawColor(0);
  u8g2.drawHLine(x,y+h-min,b);
  u8g2.drawHLine(x,y+h-min-1,b);
  u8g2.setDrawColor(1);

       // Batt
      //sprintf(buf1, "%1.1f", UBatt);



}

void oled_setBatterieWert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,float wert)
{
      u8g2.setCursor(x,y);
      u8g2.setDrawColor(0);
      u8g2.print(wert,1);
      u8g2.setDrawColor(1);
}

void oled_horizontalbalken(uint8_t x,uint8_t y, uint8_t b, uint8_t h)
{
   u8g2.drawFrame(x,y,b,h);
}
void oled_horizontalbalken_setwert(uint8_t x,uint8_t y, uint8_t b, uint8_t h,uint8_t wert)
{
  //Serial.print(b);
  //Serial.print("\t");
  //Serial.print(wert);
  
  //Serial.print("\n");
  

  u8g2.setDrawColor(0);
  u8g2.drawBox(x+1,y+1,b-2,h-2);

 

  u8g2.setDrawColor(1);
  //u8g2.drawBox(x+7,y+1,b-2,h-2);
  //return;
  u8g2.drawVLine(x+b-wert,y,h);
  //u8g2.drawHLine(x+b-wert-1,y,h);
  //u8g2.drawHLine(x+b-wert+1,y,h);

}

void resetRegister(void)
{
   uint8_t i=0,k=0;
   for(i=0;i<8;i++)
   {
      for (k=0;k<8;k++)
      {
         //posregister[i][k]=0xFFFF;
      }
   }
}


void setHomeScreen()
{
   u8g2.clear();
   u8g2.setFont(u8g2_font_t0_18_mr);  
   u8g2.setCursor(TAB0, 14);
   //u8g2.print(F("nRF24 T"));
   u8g2.print(ModelTable[curr_model]);
   u8g2.setFont(u8g2_font_t0_18_mr);  
  // oled_vertikalbalken(BATTX,BATTY,BATTB,BATTH);
   u8g2.sendBuffer();
   curr_cursorspalte = 0;
   curr_cursorzeile = 0;

}

void updateHomeScreen()
{
 
      char buf0[4];
      u8g2.setCursor(90,14);
      u8g2.print(blinkcounter);
      // Yaw
      //u8g2.setCursor(4,30);
      //u8g2.print(data.yaw);
      sprintf(buf0, "%3d", adresse);
      u8g2.drawStr(TAB0,30,buf0);
      
      // Pitch
     // u8g2.setCursor(36,30);
     // u8g2.print(data.pitch);
      //sprintf(buf0, "%3d", paketcounter);
      //u8g2.drawStr(32,30,buf0);
      u8g2.setCursor(36,30);
      u8g2.print(paketcounter);

      // Roll
      //u8g2.setCursor(4,46);
      //u8g2.print(data.roll);
      sprintf(buf0, "%3d", data.roll);
      u8g2.drawStr(TAB0,48,buf0);
      
        sprintf(buf0, "%3d", adressmatch);
      u8g2.drawStr(32,48,buf0);
     
      uint8_t p = curr_model;
 
   //u8g2.setCursor(4,58);
   //u8g2.print(la);
   //u8g2.setCursor(24,58);
   //u8g2.print(lb);


   u8g2.setCursor(44,56);
   //u8g2.print(kanalsettingarray[0][0][2]);
}

void setMenuScreen()
{
   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   charh = u8g2.getMaxCharHeight()-1;

   char_x = 24;
   char_y = 45;
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);

   // Modus
   u8g2.setFont(u8g2_font_t0_15_mr); 
   u8g2.drawStr(112,40,"Modus");

   
   u8g2.setFont(u8g2_font_t0_15_mr); 
   u8g2.drawStr(char_x,char_y +charh,SettingTable[0]);
   

   u8g2.setFontDirection(0);

   // Pfeil Modus

   u8g2.setFont(u8g2_font_unifont_t_symbols);
   u8g2.drawGlyph(100,55, 0x23F5);
   u8g2.setFont(u8g2_font_t0_15_mr);  
   
   updateMenuScreen();
   

}


void updateMenuScreen() // Liste der Modelle
{
   uint8_t z =  curr_cursorzeile;
   
   char_y = 2;
   uint8_t i = 0;
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 36;
   while (char_y < 64)
   {
      if(i==curr_model)
      {
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,64,18);
         //u8g2.setDrawColor(1);
         //u8g2.drawStr(char_x+2,char_y + charh, ModelTable[i]);
         //u8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW1, 50, 1, 1, ModelTable[i]);

      }
      else
      {
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,64,18);
         //u8g2.setDrawColor(1);
         //u8g2.drawStr(char_x+2,char_y + charh, ModelTable[i]);
         //8g2.drawButtonUTF8(char_x,char_y, U8G2_BTN_BW0, 50, 1, 1, ModelTable[i]);

      }
      u8g2.setDrawColor(1);
      u8g2.drawStr(char_x+2,char_y + charh-1, ModelTable[i]);
      //u8g2.drawFrame(char_x,char_y,50,24);
      //u8g2.drawStr(char_x+2,char_y,ModelTable[i]);
      char_y += menuh;
      i++;
   }
}

void setModellScreen() // Auswahl Funktion fuer ausgewaehltes Modell
{
   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_x = 36;
   char_y = 45;
   //u8g2.drawFrame(char_y,char_y,64,18);
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y + charh, ModelTable[curr_model]);
   u8g2.setFontDirection(0);
   char_y = taby[3];
   updateModellScreen();
   //u8g2.drawStr(2,char_y,SettingTable[1]);

}

void updateModellScreen(void)
{
   char_y = 4;
   uint8_t i = 0;
   u8g2.setFont(u8g2_font_t0_14_mr);  
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 48;
   while (char_y < 64)
   {
       u8g2.setDrawColor(1);
      u8g2.drawStr(char_x+2,char_y + charh, FunktionTable[i]);
      if(i==curr_funktion)
      {
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,64,14);
         
      }
      else
      {
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,64,14);
         u8g2.setDrawColor(1);
      }
   
      char_y += 16;
      i++;
   }
   u8g2.setFont(u8g2_font_t0_15_mr);  

}

void setFunktionScreen() // Auswahl Aktion
{
   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_x = 18;
   char_y = 45;
   //u8g2.drawFrame(char_y,char_y,64,18);
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y + charh,FunktionTable[curr_funktion]);
   u8g2.setFontDirection(0);
   updateFunktionScreen();
}// setFunktionScreen

void updateFunktionScreen()
{
   char_y = 4;
   uint8_t i = 0;
   //u8g2.setFont(u8g2_font_t0_14_mr);  
   u8g2.setFont(u8g2_font_t0_15_mr);  
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 36;

   uint8_t feldx = 110;
   uint8_t feldyO = 6;
   uint8_t feldb = 16;
   uint8_t feldh = 16;
   uint8_t fkt = 0;
   
   u8g2.setDrawColor(1);
   u8g2.drawStr(char_x+2,char_y + charh, AktionTable[0]);
   u8g2.drawStr(char_x+2,char_y + charh + 36, AktionTable[1]);
   
   u8g2.setFont(u8g2_font_unifont_t_symbols);
   
   u8g2.drawGlyph(86,char_y + 6, 0x23F6);
   u8g2.drawGlyph(86,char_y + 22, 0x23F7);

   u8g2.drawGlyph(86,char_y + 6 + 36, 0x23F6);
   u8g2.drawGlyph(86,char_y + 22 + 36, 0x23F7);
      
   u8g2.setFont(u8g2_font_t0_15_mr);  
   //Serial.print(" curr_funktion: ");
   //Serial.println(curr_funktion);
   
   uint8_t level = kanalsettingarray[curr_model][curr_funktion][1];

   uint8_t expo = kanalsettingarray[curr_model][curr_funktion][2];
   //Serial.print("updatefunktionscreen level: ");
   //Serial.print(level);
   //Serial.print(" expo: ");
   //Serial.println(expo);
   uint8_t levelO = (level & 0xF0) >> 4;
   uint8_t levelU = (level & 0x0F);
   uint8_t expoO = (expo & 0xF0) >> 4;
   uint8_t expoU = expo & 0x0F;
   u8g2.setCursor(itemtab[6], char_y + 6);
   u8g2.print(levelO);
   u8g2.setCursor(itemtab[6], char_y + 22);
   u8g2.print(levelU);

   u8g2.setCursor(itemtab[6], char_y + 36 + 6);
   u8g2.print(expoO);
   u8g2.setCursor(itemtab[6], char_y + 36 + 22);
   u8g2.print(expoU);

   while (char_y < 64)
   {
      

      if(i==curr_aktion)
      {
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x,char_y,48,16);
         
      }
      else
      {
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x,char_y,48,16);
         u8g2.setDrawColor(1);
      }
       switch (curr_cursorspalte)
      {
         case 0:
         {

         }break;
         case 1: // Level, expo up, down
         {

         }break;
      }// switch curr_cursorspalte

      char_y += 36;
      fkt++;
      i++;
   }

   u8g2.setFont(u8g2_font_t0_15_mr);  

}// updateFunktionScreen

void setAktionScreen()
{
   u8g2.clear();
   
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_x = 18;
   char_y = 45;
   u8g2.setFont(u8g2_font_t0_15_mr);
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y + charh,FunktionTable[curr_funktion]); // "YAW"   
   u8g2.setFontDirection(0);
   
   updateAktionScreen();
}

void updateAktionScreen()
{
   //Serial.print("updateAktionScreen: curr_screen: ");
   //Serial.println(curr_screen);
    //Serial.print("updateAktionScreen: curr_wert: ");
   //Serial.println(curr_wert);

   char_y = 4;
   uint8_t i = 0;
   u8g2.setFont(u8g2_font_t0_15_mr);  
   charh = u8g2.getMaxCharHeight()-1;
   char_x = 36;
   u8g2.drawStr(char_x,char_y + charh, AktionTable[curr_aktion]);

   switch (curr_aktion)
   {
      case 0: // LEVEL
      {
         uint8_t level = kanalsettingarray[curr_model][curr_funktion][1];
         uint8_t levelO = (level & 0xF0) >> 4;
         uint8_t levelU = (level & 0x0F);
         char_x += 8;
         char_y += 24;
         uint8_t dy = 20; // Abstand 2. Zeile
         u8g2.drawStr(char_x,char_y + charh,"UP");
        
         u8g2.setCursor(char_x + 48,char_y + charh);
         u8g2.print(levelO);

         u8g2.drawStr(char_x,char_y + charh + dy,"DOWN");
         u8g2.setCursor(char_x + 48 ,char_y + charh + dy);
         u8g2.print(levelU);

         for (uint8_t i=0;i<2;i++)
         {
            if(i==curr_wert)
            {
               u8g2.setDrawColor(1);
               u8g2.drawFrame(char_x-2,char_y + i*dy,38,16);
               if(curr_cursorspalte == 1) // Wert einstellen
               {
                  u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
               
               }
               else
               {
                  u8g2.setDrawColor(0);
                  u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
                  u8g2.setDrawColor(1);
               }

            }
            else
            {
               u8g2.setDrawColor(0);
               u8g2.drawFrame(char_x-2,char_y + i*dy,38,16);
               u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
               
               u8g2.setDrawColor(1);
            }
         }
      
      }break;

      case 1: // EXPO
      {
         uint8_t expo = kanalsettingarray[curr_model][curr_funktion][2];
         uint8_t expoO = (expo & 0xF0) >> 4;
         uint8_t expoU = (expo & 0x0F);
         char_x += 8;
         char_y += 24;
         uint8_t dy = 20; // Abstand 2. Zeile
         u8g2.drawStr(char_x,char_y + charh,"UP");
        
         u8g2.setCursor(char_x + 48,char_y + charh);
         u8g2.print(expoO);

         u8g2.drawStr(char_x,char_y + charh + dy,"DOWN");
         u8g2.setCursor(char_x + 48 ,char_y + charh + dy);
         u8g2.print(expoU);
         for (uint8_t i=0;i<2;i++)
         {
            if(i==curr_wert)
            {
               u8g2.setDrawColor(1);
               u8g2.drawFrame(char_x-2,char_y + i*dy,38,16);
               if(curr_cursorspalte == 1) // Wert einstellen
               {
                  u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
               
               }
               else
               {
                  u8g2.setDrawColor(0);
                  u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
                  u8g2.setDrawColor(1);
               }

            }
            else
            {
               u8g2.setDrawColor(0);
               u8g2.drawFrame(char_x-2,char_y + i*dy,38,16);
               u8g2.drawFrame(char_x + 44,char_y + i*dy-1,18,18);
               
               u8g2.setDrawColor(1);
            }
         }

      }break;


   }//switch curr_aktion

   u8g2.setFont(u8g2_font_t0_15_mr);  
} // updateAktionScreen

void setSaveScreen(void)
{

   u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_x = 18;
   char_y = 25;
   u8g2.setFont(u8g2_font_t0_15_mr);
   u8g2.drawStr(char_x,char_y + charh ,"Save Changes?");

}

void setModusScreen(void)
{
    u8g2.clear();
   resetRegister();
   blink_cursorpos=0xFFFF;
   char_y = 45;
   u8g2.setFont(u8g2_font_t0_15_mr);
   u8g2.setDrawColor(1);
   u8g2.setFontDirection(3);
   u8g2.drawStr(char_x,char_y + charh,"MODUS");
   // Modus
   u8g2.setFont(u8g2_font_t0_12_mr); 
   u8g2.drawStr(12,40,"Menu");
   u8g2.setFontDirection(0);
   
   // Pfeil Modus

   u8g2.setFont(u8g2_font_unifont_t_symbols);
   u8g2.drawGlyph(2,55, 0x23F4);
   u8g2.setFont(u8g2_font_t0_15_mr);  
   
   char_x = 48;
   char_y = 2;
   u8g2.drawStr(char_x+2,char_y + charh ,"MODELL");
   
   u8g2.drawStr(char_x+2,char_y + charh+18 ,"SIM");

   u8g2.drawStr(char_x+2,char_y + charh+36 ,"CALIB");

   updateModusScreen();

   
}

void updateModusScreen(void)
{
   char_x = 48;
   char_y = 4;
   u8g2.setFont(u8g2_font_t0_15_mr);  
   charh = u8g2.getMaxCharHeight()-1;
   switch (curr_modus)
   {
      case MODELL: //MODELL
      {
         //char_x = 14;
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x-2,char_y +18,56,18);
         u8g2.drawFrame(char_x-2,char_y +36,56,18);
         
         u8g2.setDrawColor(1);
         u8g2.drawFrame(char_x-2,char_y  ,56,18);
      }break;
      case SIM: // SIM
      {
         //char_y = 55;
         u8g2.setDrawColor(0);
         u8g2.drawFrame(char_x-2,char_y ,56,18);
         u8g2.drawFrame(char_x-2,char_y +36,56,18);
         u8g2.setDrawColor(1);
         
         u8g2.drawFrame(char_x-2,char_y+18 ,56,18);
      }break;

       case CALIB: // CALIB
      {
         if(calibstatus & (1<<CALIB_START))
         {
            if(blinkstatus)
            {
               u8g2.setDrawColor(0);
            }
            else
            {
               u8g2.setDrawColor(1);
            }
            u8g2.drawFrame(char_x-2,char_y+36 ,56,18);
            u8g2.setDrawColor(1);
         }
         else // keine aktion
         {
            u8g2.setDrawColor(0);
            u8g2.drawFrame(char_x-2,char_y ,56,18);
            u8g2.drawFrame(char_x-2,char_y +18,56,18);
            u8g2.setDrawColor(1);
            
            u8g2.drawFrame(char_x-2,char_y+36 ,56,18);
         }

      }break;

   }// switch curr_cursorspalte

}

void refreshScreen(void)
{

   switch (curr_screen) 
   {
      case 0:
      {
         char buf[6];
         sprintf(buf, "%2d:%2d",stopminute,stopsekunde);
         u8g2.drawStr(54,12,buf);

         u8g2.setCursor(64,24);
         u8g2.print(throttlecounter);

         //sprintf(buf,"%1.0F", throttlesekunden);

         u8g2.setCursor(64,48);
         u8g2.print(throttlesekunden);
         //u8g2.setFont(u8g2_font_inb24_mr);
         //u8g2.drawStr(64,42,buf);
         //u8g2.setFont(u8g2_font_t0_15_mr);

         u8g2.sendBuffer();

      }
      case 3: //FUNKTIONSCREEN
      {

         
         if(blinkstatus)
         {
            u8g2.setFontMode(0);
         }
         else
         {
            u8g2.setFontMode(1);   
         }
         u8g2.setFont(u8g2_font_unifont_t_symbols);
         uint8_t pos_y = (blink_cursorpos & 0xFF00)>>8;
         uint8_t pos_x = blink_cursorpos & 0x00FF;
         //u8g2.drawGlyph(pos_x,pos_y, 0x23F6);
         //u8g2.drawGlyph(pos_x,pos_x, 0x23F7);

         //u8g2.drawXBM( 10, 50, 8, 16, pfeil_l);
         //u8g2.drawXBM( 30, 50, 8, 16, pfeil_r);

         u8g2.setFont(u8g2_font_t0_15_mr);  
      }break;
   }
}

void setCalibScreen(void)
{



}  