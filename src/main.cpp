#include <Arduino.h>

// put function declarations here:
#include "Arduino.h"
#include "SoftwareSerial.h"
#include <SPI.h> // incluye librería interfaz SPI
#include <Wire.h> // Incluye librería para módulo I2C
#include <LCD.h> // Incluye librería h para el LCD
#include <LiquidCrystal_I2C.h> // Incluye librería NewLiquidCrystal para I2C
#include <RTClib.h> // incluye librería para el manejo del módulo RTC

RTC_DS3231 rtc; // crea objeto del tipo RTC_DS3231

#define NOTE_F7 2794
#define NOTE_DS8 4978
#define NOTE_AS7 3729

#define BUZZER_PASIVO 5

int Estado = 1; 


int melodia[] = { NOTE_DS8, NOTE_DS8, NOTE_DS8 };
int duraciones[] = { 1, 1, 1 };
int melodia1[] = { NOTE_F7 };
int duraciones1[] = { 1 };
int melodia2[] = { NOTE_AS7, NOTE_AS7 };
int duraciones2[] = { 1, 1 };

#define RELE 6

// Declaración del encoder rotativo
int pinEnt = 4;
int B = 3;
int A = 2;

int ANTERIOR = 4;
int POSICION = 1;
int posicionHora = 0;
int anteriorHora = 0;
int posicionMinuto = 0;
int anteriorMinuto = 0;
int horaInicial = 0;
int minutoInicial = 0;


// Configuración del display LCD
const int en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x27;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void unPitido() {
  for (int i = 0; i < 1; i++) {
    int duracion = 100 / duraciones1[i];
    tone(BUZZER_PASIVO, melodia1[i], duracion);
    int pausa = duracion * 1.30;
    delay(pausa);
    noTone(BUZZER_PASIVO);
  }
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
bool sonido =true;
bool clear =true;
void encoder() {
  
  static unsigned long ultimaInterrupcion = 0;

  unsigned long tiempoInterrupcion = millis();

  if (tiempoInterrupcion - ultimaInterrupcion > 100) {
    if (digitalRead(B) == HIGH) {
      sonido = true;
      POSICION++;
      posicionHora++;
      posicionMinuto++;
     
    } else {
    //  clear = true;
      sonido = true;
      POSICION--;
      posicionHora--;
      posicionMinuto--;
    
    }

    POSICION = min(100, max(0, POSICION));
    ultimaInterrupcion = tiempoInterrupcion;

    posicionHora = min(37, max(0,posicionHora));
    ultimaInterrupcion = tiempoInterrupcion;

    posicionMinuto = min(61, max(0,posicionMinuto));
    ultimaInterrupcion = tiempoInterrupcion;
  }
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void setup() {

  
  Serial.begin(9600);

  pinMode(BUZZER_PASIVO, OUTPUT);
  pinMode(RELE, OUTPUT);
  pinMode(A, INPUT); // A como entrada
  pinMode(B, INPUT); // B como entrada
  pinMode(pinEnt, INPUT); // pinEnt como entrada

  attachInterrupt(digitalPinToInterrupt(A), encoder, LOW); // Interrupción sobre pin A

  lcd.begin(20, 4); // Setea display 20 caracteres, 4 renglones
  lcd.clear(); // Borra contenido del display
  
  if (!rtc.begin()) { // Si falla la inicialización del módulo RTC
    lcd.print("Modulo RTC NO ENCONTRADO !");
    while (1); // Bucle infinito
  }
  rtc.adjust(DateTime(__DATE__, __TIME__)); // Ajusta el reloj
}

void dosPitidos() {
  for (int i = 0; i < 2; i++) {
    int duracion = 100 / duraciones2[i];
    tone(BUZZER_PASIVO, melodia2[i], duracion);
    int pausa = duracion * 1.30;
    delay(pausa);
    noTone(BUZZER_PASIVO);
  }
}

void tresPitidos() {
  for (int i = 0; i < 3; i++) {
    int duracion = 100 / duraciones[i];
    tone(BUZZER_PASIVO, melodia[i], duracion);
    int pausa = duracion * 1.30;
    delay(pausa);
    noTone(BUZZER_PASIVO);
  }
}
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// 

void releVeinteCeroCuatro() { 

  
  lcd.clear();

 while (true) {
  
  lcd.setCursor(6, 0);
  lcd.print("FP 20/04 ");
  
  // Declaramos las variables de control
  bool evento_inicio = true; // Variable de control para inicio de evento con valor true
  bool evento_fin = true;    // Variable de control para finalización de evento con valor true  
  
  DateTime fecha = rtc.now(); // Obtener la hora actual del RTC

  // Mostrar la hora actual en el LCD (esto es solo para depuración)
  Serial.print("Hora actual: ");
  Serial.print(fecha.hour());
  Serial.print(":");
  Serial.println(fecha.minute());

  // Evento de inicio del fotoperíodo
  if (fecha.hour() == 13 && fecha.minute() == 20 && evento_inicio == true) { 
     DateTime fecha = rtc.now(); // Obtener la hora actual del RTC 
    digitalWrite(RELE, HIGH);  // Activa el módulo relé
    Serial.print("rele prendido");
    lcd.setCursor(9, 2);
    lcd.print("on ");
    evento_inicio = false;     // Cambiar a falso para evitar que se active más de una vez
  }
    else{
       // Evento de apagado del fotoperíodo
   if (fecha.hour() == 13 && fecha.minute() == 21 && evento_fin == true) { 
     DateTime fecha = rtc.now(); // Obtener la hora actual del RTC 
     Serial.print("rele apagado");
    digitalWrite(RELE, LOW);   // Desactiva el módulo relé
    lcd.setCursor(9, 2);
    lcd.print("off");
    evento_fin = false;        // Cambiar a falso para evitar que se apague más de una vez
   }
    else{
      if (fecha.hour() == 20 && fecha.minute() == 0) {  
    evento_inicio = true;     // Restablecer la variable de control de inicio
    evento_fin = true;        // Restablecer la variable de control de fin
     }
    }
   }
  }
 }

 ////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////////////////////////////

void fotoPeriodoVeinteCeroCuatro() {
 
  lcd.clear();

  while (true) {
    if (POSICION > 2) { 
      POSICION = 1; 
    } else { 
      if (POSICION < 1) { 
        POSICION = 2; 
      }
    }
    
    switch (POSICION) {
      case 1:
        lcd.setCursor(0, 0);
        lcd.print(" 20 Horas prendido  ");
        lcd.setCursor(0, 1);
        lcd.print(" 04 Horas apagado   ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print(" >INICIAR           ");
        break;
        
      case 2:
        lcd.setCursor(0, 0);
        lcd.print(" 20 Horas prendido  ");
        lcd.setCursor(0, 1);
        lcd.print(" 04 Horas apagado   ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("            >APAGAR ");
        break;
    }
 
    if (POSICION == 1 && digitalRead(pinEnt) == LOW) {
      dosPitidos();
      releVeinteCeroCuatro(); 
    }
   else{
    if(POSICION == 2 && digitalRead(pinEnt) == LOW) {
       tresPitidos();
       return; // Salir de la función 'fotoPeriodoVeinteCeroCuatro'
     }
    }
   }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

if (Estado == 1) {
  
  if (POSICION != ANTERIOR) {
   ANTERIOR = POSICION;
}
   if (POSICION > 5) {
     POSICION = 1;
   } else if (POSICION < 1) {
     POSICION = 5;
   }

switch (POSICION) {
     case 1:
       lcd.setCursor(0, 0);
       lcd.print("----FOTOPERIODOS----");
       lcd.setCursor(0, 1);
       lcd.print(" > 20HS.ON-04HS.OFF ");
       lcd.setCursor(0, 2);
       lcd.print("   18HS.ON-06HS.OFF ");
       lcd.setCursor(0, 3);
       lcd.print("   12HS.ON-12HS.OFF ");
       if (sonido ==true) {unPitido(); sonido =false;}
       if (digitalRead(pinEnt) == LOW) {dosPitidos(); Estado=2;delay(500);}
      
       break;

     case 2:
       lcd.setCursor(0, 0);
       lcd.print("----FOTOPERIODOS----");
       lcd.setCursor(0, 1);
       lcd.print("   20HS.ON-04HS.OFF ");
       lcd.setCursor(0, 2);
       lcd.print(" > 18HS.ON-06HS.OFF ");
       lcd.setCursor(0, 3);
       lcd.print("   12HS.ON-12HS.OFF ");
       if (sonido ==true) {unPitido(); sonido =false;}
      
       break;

     case 3:
       lcd.setCursor(0, 0);
       lcd.print("----FOTOPERIODOS----");
       lcd.setCursor(0, 1);
       lcd.print("   20HS.ON-04HS.OFF ");
       lcd.setCursor(0, 2);
       lcd.print("   18HS.ON-06HS.OFF ");
       lcd.setCursor(0, 3);
       lcd.print(" > 12HS.ON-12HS.OFF ");
       if (sonido ==true) {unPitido(); sonido =false;}
      
       break;

     case 4:
       lcd.setCursor(0, 0);
       lcd.print("   20HS.ON-04HS.OFF ");
       lcd.setCursor(0, 1);
       lcd.print("   18HS.ON-06HS.OFF ");
       lcd.setCursor(0, 2);
       lcd.print("   12HS.ON-12HS.OFF ");
       lcd.setCursor(0, 3);
       lcd.print(" > 36HS.ON-12HS.OFF ");
       if (sonido ==true) {unPitido(); sonido =false;}
      
       break;

       case 5:
       lcd.setCursor(0, 0);
       lcd.print("   18HS.ON-06HS.OFF ");
       lcd.setCursor(0, 1);
       lcd.print("   12HS.ON-12HS.OFF ");
       lcd.setCursor(0, 2);
       lcd.print("   36HS.ON-12HS.OFF ");
       lcd.setCursor(0, 3);
       lcd.print(" >  CUSTOMIZABLE    ");
       if (sonido ==true) {unPitido(); sonido =false;}
      
       break;
   }

 } 
 else if(Estado ==2){
   

  if (POSICION != ANTERIOR) {
     ANTERIOR = POSICION;
  }
     if (POSICION > 2) {
       POSICION = 1;
     } else if (POSICION < 1) {
       POSICION = 2;
     }

     switch (POSICION) {

      case 1:
        lcd.setCursor(0, 0);
        lcd.print(" 20HS PRENDIDO      ");
        lcd.setCursor(0, 1);
        lcd.print(" 04HS APAGADO       ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("  INICIAR           ");
        if (sonido ==true) {unPitido(); sonido =false;}
        if (digitalRead(pinEnt) == LOW) {Estado = 3; delay(500);}
        
        break;
 
      case 2:
        lcd.setCursor(0, 0);
        lcd.print(" 20HS PRENDIDO      ");
        lcd.setCursor(0, 1);
        lcd.print(" 04HS APAGADO       ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("  VOLVER            ");
        if (sonido ==true) {unPitido(); sonido =false;}
        if (digitalRead(pinEnt) == LOW)  {POSICION = 1; Estado =1;delay(500);} 

        break;
       
    }

 }

 else if(Estado ==3){
 // int horaInicial = 0;

  if (posicionHora != anteriorHora) {
    anteriorHora = posicionHora;
 }
    if (posicionHora > 24) {
      lcd.setCursor(1, 3);
      lcd.print(" ");
      posicionHora = 1;
    } else if (posicionHora < 1) {
      posicionHora = 24;}

                lcd.setCursor(0, 0);
                lcd.print("   ELIGE LA HORA    ");
                lcd.setCursor(0, 1);
                lcd.print("   PARA INICIAR     ");
                lcd.setCursor(0, 2);
                lcd.print("                    ");
                lcd.setCursor(0, 3);
                lcd.print(posicionHora);
                lcd.setCursor(2, 3);
                lcd.print("                  ");
                if (digitalRead(pinEnt) == LOW){horaInicial = posicionHora;Estado=4;delay(500);}
              
 }
 else if(Estado == 4){
  //int minutoInicial = 0;

  if (posicionMinuto != anteriorMinuto) {
    anteriorMinuto = posicionMinuto;
 }
    if (posicionMinuto > 60) {
      posicionMinuto = 1;

                  lcd.setCursor(1, 3);
                  lcd.print(" ");
    } else if (posicionMinuto < 1) {
      posicionMinuto = 60;
      }
                  lcd.setCursor(0, 3);
                  lcd.print(posicionMinuto);
                  lcd.setCursor(2, 3);
                  lcd.print("                  ");
                  lcd.setCursor(0, 0);
                  lcd.print(" ELIGE LOS MINUTOS  ");
                  lcd.setCursor(0, 1);
                  lcd.print("    PARA INICIAR    ");
                  lcd.setCursor(0, 2);
                  lcd.print("                    ");
                  if (digitalRead(pinEnt) == LOW){minutoInicial = posicionMinuto;Estado = 5;delay(500);}
 }

 else if(Estado == 5) {
bool evento_inicio = true; 
bool evento_fin = true;

lcd.setCursor(0, 0);
lcd.print("    FOTOPERIODO     ");
lcd.setCursor(0, 1);
lcd.print(" 20HS-ON / 04HS-OFF ");
lcd.setCursor(0, 2);
lcd.print("      ACTIVADO!     ");
lcd.setCursor(0, 3);
lcd.print("                    ");

  DateTime fecha = rtc.now();
  int horaFinal = (horaInicial + 20) % 24;  // Maneja el overflow de la hora

  if (fecha.hour() == horaInicial && fecha.minute() == minutoInicial && evento_inicio == true) {
      digitalWrite(RELE, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    LA LUZ ESTA     ");
      lcd.setCursor(0, 1);
      lcd.print("     PRENDIDA!      ");
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print(" Se apaga en 20hs   ");
      evento_inicio = false;
  }

  if (fecha.hour() == horaFinal && fecha.minute() == minutoInicial && evento_fin == true) {
      digitalWrite(RELE, LOW);
      lcd.setCursor(9, 2);
      lcd.print("off");
      evento_fin = false;
  }

  if (fecha.hour() == (horaInicial + 10) % 24 && fecha.minute() == minutoInicial) {
      evento_inicio = true;
      evento_fin = true;
  }
}
/////////////////////////////////  llave de cierre del void loop      ////////////////////////
}