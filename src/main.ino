#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- CONFIGURACIÓ ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define DHTPIN 2
#define MODEPIN 5
#define RECOPIN 6
#define DHTTYPE DHT11

#define MODO1 33 // Mode corredor
#define MODO2 44 // Mode ciclisme
#define VISTA1 55 // Vista temperatura
#define VISTA2 66 // Vista recomanació

DHT dht(DHTPIN, DHTTYPE);

int modo = MODO1;
int vista = VISTA1;
float h = 0.0;
float t = 0.0;

// Timer per a la lectura de sensor
unsigned long tempsAnterior = 0;
const long intervalSensor = 2000; 

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  pinMode(MODEPIN, INPUT_PULLUP);
  pinMode(RECOPIN, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Iniciant...");
  delay(1000);
  lcd.clear();
}


// Funció de les recomanacions
void recomendarCorrer() {
  lcd.setCursor(0, 0);
  lcd.print("RUNNING: "); lcd.print(t, 0); lcd.print(" C ");
  lcd.setCursor(0, 1);
  if (h > 90) { lcd.print("Pot ploure!"); return; }
  
  if (t < 5) lcd.print("Termica+Guants  ");
  else if (t >= 5 && t < 12) lcd.print("Llarg o xaqueta");
  else if (t >= 12 && t < 20) lcd.print("Curt + Short   ");
  else lcd.print("Vas a suar jaja ");
}

void recomendarBici() {
  lcd.setCursor(0, 0);
  lcd.print("BICI: "); lcd.print(t, 0); lcd.print(" C     ");
  lcd.setCursor(0, 1);
  if (h > 90) { lcd.print("Pot plore! "); return; }

  if (t < 5) lcd.print("Un fred que pela");
  else if (t >= 5 && t < 10) lcd.print("Invierno+Botin  ");
  else if (t >= 10 && t < 14) lcd.print("Ca.vientos/Llarg");
  else if (t >= 14 && t < 18) lcd.print("Curt + Manguitos");
  else if (t >= 18 && t < 24) lcd.print("Curt 100% ");
  else lcd.print("Corto Total     ");
}

void loop() {
  
  // Canvi de modes (carrera/bicicleta)
  if (digitalRead(MODEPIN) == LOW) {
    // Cambiamos el modo
    if (modo == MODO1) modo = MODO2;
    else modo = MODO1;

    // Visualització del canvi
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CANVI MODE A:");
    lcd.setCursor(0, 1);
    if (modo == MODO1) lcd.print(">> CORREDOR <<");
    else lcd.print(">> CICLISTA <<");
    
    delay(1500); // Espera per a lectura
    lcd.clear();
    
    tempsAnterior = 0; 
  }

  // Canvi de vista (clima/recomanació)
  if (digitalRead(RECOPIN) == LOW) {
    // Canvi de vista
    if (vista == VISTA1) vista = VISTA2;
    else vista = VISTA1;

    // Feedback visual
    lcd.clear();
    if (vista == VISTA1) {
      lcd.setCursor(0, 0); lcd.print("   MONITOR DE   ");
      lcd.setCursor(0, 1); lcd.print("     CLIMA      ");
    } else {
      lcd.setCursor(0, 0); lcd.print("  RECOMADADOR   ");
      lcd.setCursor(0, 1); 
      if (modo == MODO1) lcd.print("(Mode: Runner)");
      else lcd.print("(Mode: Bici)");
    }

    delay(1500);
    lcd.clear();
    
    tempsAnterior = 0;
  }

  // LÓGICA PRINCIPAL
  if (millis() - tempsAnterior >= intervalSensor || tempsAnterior == 0) {
    tempsAnterior = millis();
    
    h = dht.readHumidity();
    t = dht.readTemperature();

    // Protecció error sensor
    if (isnan(h) || isnan(t)) {
      lcd.setCursor(0, 0); lcd.print("Error Sensor");
      return; 
    }

    // Segons la vista seleccionada...
    if (vista == VISTA1) {
      // PANTALLA DE DATES
      lcd.setCursor(0, 0);
      lcd.print("Temp: "); lcd.print(t, 1); lcd.print(" C ");
      lcd.setCursor(0, 1);
      lcd.print("Hum:  "); lcd.print(h, 0); lcd.print(" % ");
    } 
    else if (vista == VISTA2) {
      // PANTALLA DE RECOMANACIÓ
      if (modo == MODO1) recomendarCorrer();
      else recomendarBici();
    }
  }
}