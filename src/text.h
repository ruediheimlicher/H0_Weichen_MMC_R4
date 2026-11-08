//
//  text.h
//  DOG_LCD
//
//  Created by Ruedi Heimlicher on 22.11.2013.
//
//

#ifndef DOG_LCD_text_h
#define DOG_LCD_text_h


// Homescreen
 const char titel0[]  = "RC-22 Home";
 const char titel1[]  = "ON-Zeit:";
 const char titel2[]  = "Stoppuhr";
 const char titel3[]  = "Motorzeit";
 const char titel4[]  = "Menu";
 const char titel5[]  = "Set";
 const char titel6[]  = "Akku\0";
 const char titel7[]  = "D\0";

 const char *TitelTable[]  = {titel0, titel1, titel2, titel3, titel4, titel5, titel6, titel7};

// Modelle
const char model0[]  = "MMC ";
const char model1[]  = "Trottel";
const char model2[]  = "Heimi25";
const char model3[]  = "Motor A     ";
const char model4[]  = "Motor B     ";
const char model5[]  = "AA\0        ";
const char model6[]  = "BB\0        ";
const char model7[]  = "CC\0        ";

const char *ModelTable[]  = {model0, model1, model2, model3, model4, model5, model6, model7};





// Settingscreen
const char menutitel[]  = "MODELL";
const char model[]  = "Fkt:";
const char setting[]  = "Fkt";
const char kanal[]  = "Kanal";

const char mix[]  = "Mix";
const char zuteilung[]  = "Zuteilung";
const char ausgang[]  = "Ausgang";


const char *SettingTable[]  = {menutitel, model, setting, kanal,  mix, zuteilung,ausgang};


// Funktionscreen
//const char kanaltitel[]  = "Kan:";
//const char richtung[]  = "Ri:";
//const char funktion[]  = "Fkt:";
const char level[]  = "Level";
const char expo[]  = "Expo";
const char richtung[]  = "Ri:";

//const char seitea[]  = "A:";
//const char seiteb[]  = "B:";
const char typ[]  = "Typ:";

const char *AktionTable[]  = { level, expo, richtung, typ};




// Sichern
const char frage[]  = "Aenderungen sichern";
const char sichern[]  = "SICHERN";
const char abbrechen[]  = "Ignorieren";

const char* SichernTable[]  = {frage,abbrechen,sichern};

// Funktion

const char funktion0[]  = "YAW   ";
const char funktion1[]  = "PITCH ";
const char funktion2[]  = "ROLL  ";
const char funktion3[]  = "THROTT";
const char funktion4[]  = "QuerL";
const char funktion5[]  = "QuerR";
const char funktion6[]  = "Lande";
const char funktion7[]  = "Aux  ";

const char *FunktionTable[]  = {funktion0, funktion1, funktion2, funktion3, funktion4, funktion5, funktion6, funktion7};




#endif


