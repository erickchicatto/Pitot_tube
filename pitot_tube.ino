#include <Wire.h>
#include <LiquidCrystal.h>

/*LCD object*/
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

/*Prototype functions */
byte fetch_pressure(unsigned int *P_dat, unsigned int *T_dat);


/*Sutherlands Ecuations variables, obtain the viscosity*/
double T0_ = 524.07; /*default value for temp*/
double U0_ = 0.01827; /*default value for viscosity*/

/*Reynolds Ecuations variables */
double densidad_aire = 1.20; // kg/m^3

/*Diametro circular y rectangular*/
double diametro_circular = 0; // cm 
double perimetro_rectangular = 0; // cm 

/*Boton*/
const int inputPin = 4;
int value = 0 ;


/*integerValue by Serial*/
unsigned int integerValue = 0;
char incomingByte;


void setup() {
  Serial.begin(9600);
  Wire.begin(21,22);

  pinMode(inputPin,INPUT);
  pinMode(2,OUTPUT);

  delay(5000);
   /*Turn on the lcd*/
  lcd.begin(16, 2);
  /*Print a message to the LCD */
  lcd.print("Presiona el boton ");  
   
}

void loop() {

  byte _status;
  unsigned int P_dat;
  unsigned int T_dat;
  double PR;
  double TR;
  double V;
  double VV; 
 
  while(1){
    _status = fetch_pressure(&P_dat,&T_dat);

    switch(_status){
        case 0 : Serial.println("Read_MR");
        break;

        case 1: Serial.println("Read_DF2");
        break;

        case 2: Serial.println("Read_DF3");
        break;
      }


    /*Print sensors values */
     PR = (double)((P_dat-819.15)/(14744.7)) ;
     PR = (PR - 0.49060678) ;
     PR = abs(PR);
     
     V = ((PR*13789.5144)/1.225);
     VV = (sqrt((V)));

     TR = (double)((T_dat*0.09770395701));
     TR = TR-50;

   Serial.print("raw Pressure:");
   Serial.println(P_dat);
   Serial.print("pressure psi:");
   Serial.println(PR,10);
   Serial.print(" ");
   Serial.print("raw Temp:");
   Serial.println(T_dat);
   Serial.print("temp:");
   Serial.println(TR);
   Serial.print("speed m/s :");
   Serial.println(VV,5);

    /*Sutherlands ecuation*/
    double val_A = (0.555*T0) + T_dat ;
    double T = ((T0_ + 273.15)*(1.8));
    double val_B = ((0.555*T)+T0);
    double val_to2 = sqrt(pow(T/T0_,3));
    /*Viscosity*/
    double EC = U0_*(val_A/val_B)*(val_to2);
    
    /*Print resultd to lcd*/
    delay(1000); 

    /*Use boton with led*/
    value = digitalRead(inputPin);
     
    if(value==HIGH){
      if(Serial.available()>0){
      integerValue=0;
      incomingByte = Serial.read();
      if(incomingByte=='\n') break;
      if(incomingByte==-1)continue;
      delay(300);
      
      diametro_circular =  ((incomingByte-48)+integerValue);
      Serial.println("valor del diametro circular ");
      Serial.println(diametro_circular);
      delay(300);
      digitalWrite(2, HIGH); 
      /*Reynolds Ecuation*/
      double reynolds_ecuation_circular = ((densidad_aire*VV*diametro_circular)/(EC))*10;
      lcd.setCursor(0, 1);
      lcd.print(reynolds_ecuation_circular);
      delay(1000);
       }
      }
    else if(value==LOW){
      
      if(Serial.available()>0){
         
      digitalWrite(2, LOW); 
      double reynolds_ecuation_rectangular = ((densidad_aire*VV*perimetro_rectangular)/(EC))*10;
      lcd.setCursor(0, 1);
      lcd.print(reynolds_ecuation_rectangular);
      delay(1000);
        }
    }

  }
}

/*Function to modify bits in registers*/

byte fetch_pressure(unsigned int *p_P_dat, unsigned int *p_T_dat){

      byte address, Press_H, Press_L, _status;
      unsigned int P_dat;
      unsigned int T_dat;
  
      address = 0x28;
      
      Wire.beginTransmission(address);
      Wire.endTransmission();
      delay(100);


      Wire.requestFrom((int)address, (int) 4);//Request 4 bytes need 4 bytes are read
      Press_H = Wire.read();
      Press_L = Wire.read();
      byte Temp_H = Wire.read();
      byte  Temp_L = Wire.read();
      Wire.endTransmission();

      /*Enmascaramiento de bits */
      /*Use the addres ? to shift registers?*/
       _status = (Press_H >> 6) & 0x03;
      
      Press_H = Press_H & 0x3f;
      P_dat = (((unsigned int)Press_H) << 8) | Press_L;
      *p_P_dat = P_dat;

      Temp_L = (Temp_L >> 5);
      T_dat = (((unsigned int)Temp_H) << 3) | Temp_L;
      *p_T_dat = T_dat;
      
      return (_status);
    
  }
