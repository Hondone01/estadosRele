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

int B = 3;
int A = 2;
int pinEnt = 4;


int Estado = 1;

int melodia[] = { NOTE_DS8, NOTE_DS8, NOTE_DS8 };
int duraciones[] = { 1, 1, 1 };
int melodia1[] = { NOTE_F7 };
int duraciones1[] = { 1 };
int melodia2[] = { NOTE_AS7, NOTE_AS7 };
int duraciones2[] = { 1, 1 };

#define RELE 6


volatile int ANTERIOR = 4;
int POSICION = 1;
int posicionHora = 0;
int anteriorHora = 0;
int posicionMinuto = 0;
int anteriorMinuto = 0;
int horaInicial;
int minutoInicial = 0;

int horaFinal;

//funcionalidad con millis para mostrar cartel emergente y cancelar el fotoperiodo seleccionado
unsigned long tiempoInicio = 0;

// Configuración del display LCD
const int en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;
const int i2c_addr = 0x27;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
bool sonido = true;
//bool clear = true;

void encoder() {
    static unsigned long ultimaInterrupcion = 0;

    unsigned long tiempoInterrupcion = millis();

    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
        if (digitalRead(B) == HIGH) {
            sonido = true;
            POSICION++;
            posicionHora++;
            posicionMinuto++;
        } else {
            // clear = true;
            sonido = true;
            POSICION--;
            posicionHora--;
            posicionMinuto--;
        }

        POSICION = min(100, max(0, POSICION));
       // ultimaInterrupcion = tiempoInterrupcion;

        posicionHora = min(37, max(-1, posicionHora));
       // ultimaInterrupcion = tiempoInterrupcion;

        posicionMinuto = min(61, max(0, posicionMinuto));
        ultimaInterrupcion = tiempoInterrupcion;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void setup() {

  // Declaración del encoder rotativo
  
    Serial.begin(9600);
 // static unsigned long tiempo1 = 0;
  

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

void unPitido() {
    for (int i = 0; i < 1; i++) {
        int duracion = 100 / duraciones1[i];
        tone(BUZZER_PASIVO, melodia1[i], duracion);
        int pausa = duracion * 1.30;
        delay(pausa);
        noTone(BUZZER_PASIVO);
    }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
                if (digitalRead(pinEnt) == LOW) {
                    dosPitidos();
                    Estado = 2;
                    delay(500);
                }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
                break;
        }

    } else if (Estado == 2) {

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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
                if (digitalRead(pinEnt) == LOW) {
                    Estado = 3;
                    delay(500);
                }
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
                if (sonido == true) {
                    unPitido();
                    sonido = false;
                }
                if (digitalRead(pinEnt) == LOW) {
                    POSICION = 1;
                    Estado = 1;
                    delay(500);
                }
                break;

        }

    } else if (Estado == 3) {
        DateTime fecha = rtc.now();
        
        if (posicionHora != anteriorHora) {
            anteriorHora = posicionHora;
        }
        if (posicionHora > 23) {
            posicionHora = 0;
        } else if (posicionHora < 0) {
            posicionHora = 23;
        } 

       // int horaInicial = 0;
        lcd.setCursor(0, 0);
        lcd.print("Hora actual:");
        lcd.setCursor(12,0);
        lcd.print(" ");
        if(fecha.hour() < 10){
          lcd.setCursor(13, 0);
          lcd.print("0");
          lcd.setCursor(14, 0);
          lcd.print(fecha.hour());
        } else if(fecha.hour() >= 10){
          lcd.setCursor(13,0);
          lcd.print(fecha.hour());
        }
        lcd.setCursor(15,0);
        lcd.print(":");
        if(fecha.minute() < 10){
          lcd.setCursor(16, 0);
          lcd.print("0");
          lcd.setCursor(17, 0);
          lcd.print(fecha.minute());
        } else if(fecha.minute() >= 10){
          lcd.setCursor(16,0);
          lcd.print(fecha.minute());
        }
        lcd.setCursor(0, 1);
        lcd.print("   ELIGE LA HORA    ");
        lcd.setCursor(0, 2);
        lcd.print("   PARA INICIAR     ");
        
        if(posicionHora < 10){
          lcd.setCursor(0, 3);
          lcd.print("0");
          lcd.setCursor(1, 3);
          lcd.print(posicionHora);
          lcd.setCursor(4,3);
          lcd.print("formato 24 hs!");
        } else if(posicionHora >= 10){
          lcd.setCursor(0,3);
          lcd.print(posicionHora);
          lcd.setCursor(4,3);
          lcd.print("formato 24 hs!");
        }   
        lcd.setCursor(2, 3);
        lcd.print("  ");
        if (digitalRead(pinEnt) == LOW) {
            horaInicial = posicionHora;
            Estado = 4;
            delay(500);
        }

    } else if (Estado == 4) {
        DateTime fecha = rtc.now();
        if (posicionMinuto != anteriorMinuto) {
            anteriorMinuto = posicionMinuto;
        }
        if (posicionMinuto > 59) {
            posicionMinuto = 1;
        } else if (posicionMinuto < 1) {
            posicionMinuto = 59;
        } 

        //int minutoInicial = 0;
        lcd.setCursor(0, 0);
        lcd.print("Hora actual:");
        lcd.setCursor(12,0);
        lcd.print(" ");
        if(fecha.hour() < 10){
          lcd.setCursor(13, 0);
          lcd.print("0");
          lcd.setCursor(14, 0);
          lcd.print(fecha.hour());
        } else if(fecha.hour() >= 10){
          lcd.setCursor(13,0);
          lcd.print(fecha.hour());
        }
        lcd.setCursor(15,0);
        lcd.print(":");
        if(fecha.minute() < 10){
          lcd.setCursor(16, 0);
          lcd.print("0");
          lcd.setCursor(17, 0);
          lcd.print(fecha.minute());
        } else if(fecha.minute() >= 10){
          lcd.setCursor(16,0);
          lcd.print(fecha.minute());
        }

          lcd.setCursor(0, 1);
          lcd.print(" ELIGE LOS MINUTOS  ");
          lcd.setCursor(0, 2);
          lcd.print("    PARA INICIAR    ");
        if(posicionMinuto < 10){
          lcd.setCursor(0, 3);
          lcd.print("0");
          lcd.setCursor(1, 3);
          lcd.print(posicionMinuto);
          lcd.setCursor(4,3);
          lcd.print("formato 24 hs!");
        } else if(posicionMinuto >= 10){
          lcd.setCursor(0,3);
          lcd.print(posicionMinuto);
          lcd.setCursor(4,3);
          lcd.print("formato 24 hs!");
        }
        lcd.setCursor(2, 3);
        lcd.print("  ");   
        if (digitalRead(pinEnt) == LOW) {
            minutoInicial = posicionMinuto;
            Estado = 5;
            delay(500);
        }

    } else if (Estado == 5) {
      // tiempo = 0;
     

       if (millis() - tiempoInicio > 30000){
        tiempoInicio = millis();
        Estado = 8;
       }

        lcd.setCursor(0, 0);
        lcd.print(" 20HS-ON / 04HS-OFF ");
        lcd.setCursor(0, 1);
        lcd.print("      ACTIVADO!     ");
        lcd.setCursor(0, 2);
        lcd.print("Inicia: ");
        if(horaInicial < 10){
          lcd.setCursor(8, 2);
          lcd.print("0");
          lcd.setCursor(9, 2);
          lcd.print(horaInicial);
        } else if(horaInicial >= 10){
          lcd.setCursor(8,2);
          lcd.print(horaInicial);
        }   
        lcd.setCursor(10,2);
        lcd.print(":");
        if(minutoInicial < 10){
          lcd.setCursor(11, 2);
          lcd.print("0");
          lcd.setCursor(12, 2);
          lcd.print(minutoInicial);
          lcd.setCursor(13,2);
          lcd.print("hs ");
        } else if(minutoInicial >= 10){
          lcd.setCursor(11,2);
          lcd.print(minutoInicial);
          lcd.setCursor(13,2);
          lcd.print("hs ");
        }   
        lcd.setCursor(0, 3);
        lcd.print("Termina: ");
        if(horaFinal < 10){
          lcd.setCursor(9, 3);
          lcd.print("0");
          lcd.setCursor(10, 3);
          lcd.print(horaFinal);
        } else if(horaFinal >= 10){
          lcd.setCursor(9,3);
          lcd.print(horaFinal);
        }   
        lcd.setCursor(11,3);
        lcd.print(":");
        if(minutoInicial < 10){
          lcd.setCursor(12, 3);
          lcd.print("0");
          lcd.setCursor(13, 3);
          lcd.print(minutoInicial);
          lcd.setCursor(14,3);
          lcd.print("hs  ");
        } else if(minutoInicial >= 10){
          lcd.setCursor(12,3);
          lcd.print(minutoInicial);
          lcd.setCursor(14,3);
          lcd.print("hs  ");
        }   

        DateTime fecha = rtc.now();
        switch (horaInicial) {

          case 0:
          horaFinal= 20;
          break;

          case 1:
          horaFinal= 21;
          break;

          case 2:
          horaFinal= 22;
          break;

          case 3:
          horaFinal= 23;
          break;
          
          case 4:
          horaFinal= 0;
          break;

          case 5:
          horaFinal= 1;
          break;

          case 6:
          horaFinal= 2;
          break;

          case 7:
          horaFinal= 3;
          break;

          case 8:
          horaFinal= 4;
          break;

          case 9:
          horaFinal= 5;
          break;

          case 10:
          horaFinal= 6;
          break;

          case 11:
          horaFinal= 7;
          break;

          case 12:
          horaFinal= 8;
          break;

          case 13:
          horaFinal= 9;
          break;

          case 14:
          horaFinal= 10;
          break;

          case 15:
          horaFinal= 11;
          break;

          case 16:
          horaFinal= 12;
          break;

          case 17:
          horaFinal= 13;
          break;

          case 18:
          horaFinal= 14;
          break;

          case 19:
          horaFinal= 15;
          break;

          case 20:
          horaFinal= 16;
          break;

          case 21:
          horaFinal= 17;
          break;

          case 22:
          horaFinal= 18;
          break;

          case 23:
          horaFinal= 19;
          break;
          
        }
       
        if (fecha.hour() == horaInicial && fecha.minute() == minutoInicial){lcd.clear(); Estado = 6;} 
        if (millis() - tiempoInicio > 3000 && digitalRead(pinEnt) == LOW ){
          tiempoInicio = millis();
         // posicionEstadoAnterior = 5;
          POSICION = 1;
          Estado = 0;
          
         }
    }

    else if (Estado == 6) {
        DateTime fecha = rtc.now();
       // int horaFinal = (horaInicial + 20) % 24;  // Maneja el overflow de la hora
       // bool evento_fin = true;

        digitalWrite(RELE, HIGH);
        lcd.setCursor(0, 0);
        lcd.print("    LA LUZ ESTA     ");
        lcd.setCursor(0, 1);
        lcd.print("     PRENDIDA!      ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("Se apaga:");
        if(horaFinal < 10){
          lcd.setCursor(10, 3);
          lcd.print("0");
          lcd.setCursor(11, 3);
          lcd.print(horaFinal);
        } else if(horaFinal >= 10){
          lcd.setCursor(10,3);
          lcd.print(horaFinal);
        }   
        lcd.setCursor(12,3);
        lcd.print(":");
        if(minutoInicial < 10){
          lcd.setCursor(13, 3);
          lcd.print("0");
          lcd.setCursor(14, 3);
          lcd.print(minutoInicial);
          lcd.setCursor(15,3);
          lcd.print("hs");
        } else if(minutoInicial >= 10){
          lcd.setCursor(13,3);
          lcd.print(minutoInicial);
          lcd.setCursor(15,3);
          lcd.print("hs");
        }   

        if (fecha.hour() == horaFinal && fecha.minute() == minutoInicial) {
            lcd.clear();
            Estado = 7;
        }
    } else if (Estado == 7) {
        DateTime fecha = rtc.now();
        digitalWrite(RELE, LOW);

        lcd.setCursor(0, 0);
        lcd.print("    LA LUZ ESTA     ");
        lcd.setCursor(0, 1);
        lcd.print("     APAGADA!       ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("Se prende:");
        if(horaInicial < 10){
          lcd.setCursor(11, 3);
          lcd.print("0");
          lcd.setCursor(12, 3);
          lcd.print(horaInicial);
        } else if(horaInicial >= 10){
          lcd.setCursor(11,3);
          lcd.print(horaInicial);
        }   
        lcd.setCursor(13,3);
        lcd.print(":");
        if(minutoInicial < 10){
          lcd.setCursor(14, 3);
          lcd.print("0");
          lcd.setCursor(15, 3);
          lcd.print(minutoInicial);
          lcd.setCursor(16,3);
          lcd.print("hs");
        } else if(minutoInicial >= 10){
          lcd.setCursor(14,3);
          lcd.print(minutoInicial);
          lcd.setCursor(16,3);
          lcd.print("hs");
        }   
           
        if (fecha.hour() == horaInicial && fecha.minute() == minutoInicial) {
        lcd.clear();
        Estado = 6;
        }
    } else if (Estado == 8) {

        lcd.setCursor(0, 0);
        lcd.print(" Presiona durante 3 ");
        lcd.setCursor(0, 1);
        lcd.print(" segundos el boton  ");
        lcd.setCursor(0, 2);
        lcd.print(" para cancelar e ir ");
        lcd.setCursor(0, 3);
        lcd.print(" al menu principal  ");

        if (millis() - tiempoInicio > 3000){
          tiempoInicio = millis();
          lcd.clear();
          Estado = 5;
         }
    } else if (Estado == 0) {
      lcd.setCursor(0, 0);
      lcd.print("----FOTOPERIODOS----");
      lcd.setCursor(0, 1);
      lcd.print(" > 20HS.ON-04HS.OFF ");
      lcd.setCursor(0, 2);
      lcd.print("   18HS.ON-06HS.OFF ");
      lcd.setCursor(0, 3);
      lcd.print("   12HS.ON-12HS.OFF ");
      delay(500);
      Estado = 1;
    }
}
    
