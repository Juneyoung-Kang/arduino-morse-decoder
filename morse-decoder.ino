// Morse Decoder
// (C) 2018 Juneyoung KANG

#include <Wire.h>
#include <LiquidCrystal_I2C.h> 

#define SLEUTEL 10
#define BUZZER 11

LiquidCrystal_I2C lcd(0x3F,16,2);

void setup() {
  pinMode(SLEUTEL, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();     
}

float StreepDuur = 200.0;
boolean PrevS = false;
long tStartTeken, tStartPauze;
boolean S;
String kar = "";
void loop() {
  S = !digitalRead(SLEUTEL);

  if (S) {
    if (S != PrevS) {
      tStartTeken = millis();
      DecodeerPauze(tStartPauze);
    }
    digitalWrite(BUZZER, HIGH);
  }
  else {
    if (S != PrevS) {
      tStartPauze = millis();
      Decodeer(tStartTeken);
    }
    digitalWrite(BUZZER, LOW);
  }

  if (abs(millis() - tStartPauze) > StreepDuur * 10) {
    DecodeerPauze(tStartPauze);
  }

  PrevS = S;
}

void Decodeer(long starttijd) {
  char teken = '?';
  long tijd = abs(millis() - starttijd); // Tijdsduur van het geseinde teken (een punt of een streep)
  float PuntDuur = StreepDuur / 3.0;

  if (tijd <= 2) return; // Denderonderdrukking

  if (tijd <= PuntDuur) teken = '.';
  else if (tijd > StreepDuur) teken = '-';
  else if ((tijd > (StreepDuur + PuntDuur) / 1.9) && tijd <= StreepDuur) teken = '-';
  else teken = '.';

  if (teken == '-') {
    if (tijd > StreepDuur) StreepDuur++;
    if (tijd < StreepDuur) StreepDuur--;
  }
  else if (teken == '.') {
    if (tijd > StreepDuur / 3.0) StreepDuur++;
    if (tijd < StreepDuur / 3.0) StreepDuur--;
  }
  kar += teken;
  //Serial.println(teken);
}

void DecodeerPauze(long starttijd) {
  if (kar == "") return;

  char teken = '?';
  long tijd = abs(millis() - starttijd);
  if (tijd > StreepDuur - StreepDuur / 40) {
    DecodeerKar();
    //Serial.print();
  }
  if (tijd > StreepDuur * 2) {
    DecodeerKar();
    Print(" "); Serial.print(" ");
  }
  //else DecodeerKar();
  //Serial.println(teken);
}

void DecodeerKar() {
  static String letters[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
    ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "E"
  };
  int i = 0;
  while (letters[i] != "E") {
    if (letters[i] == kar) {
      Print((char)('A' + i));
      Serial.print((char)('A' + i));
      break;
    }
    i++;
  }
  if (letters[i] == "E") {
    Print(kar);
    Serial.print(kar);
  }
  kar = "";
}

int y = 0, x = 0;
void Print(String &s) {
  for (int i = 0; i < s.length(); i++) {
    upd(); lcd.print(s[i]);
  }
}
void Print(char s) {
  upd();
  lcd.print(s);
}
void Print(char *s) {
  for (int i = 0; i < strlen(s); i++) {
    upd();
    lcd.print(s[i]);
  }
}

void upd() {
  lcd.setCursor(x, y);
  x++; if (x >= 16) {
    x = 0;
    y++;
  }
  if (y >= 2) {
    lcd.clear();
    x=y = 0;    
  }
}
