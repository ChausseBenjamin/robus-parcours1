#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include<LibRobus.h>
#include <math.h>
#include <string.h>

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
//constante des différentes couleurs des skittles
#define GREEN 30
#define YELLOW 22
#define ORANGE 24
#define PURPLE 28
#define BLUE 23
#define RED 21
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to groundé
// set to false if using a common cathode LED
#define commonAnode false

// Sorting servo data (SORT prefix defines the servo angles for components)
#define SERVO_SORT  0   // TODO: vérifier si le servo est le bon
#define SORT_TARGET 0   // TODO: vérifier si l'angle du servo est le bon
#define SORT_TRASH  180 // TODO: vérifier si l'angle du servo est le bon
#define SORT_SENSOR 90  // TODO: vérifier si l'angle du servo est le bon

// our RGB -> eye-recognized gamma color
byte gammatable[256];
//initialise des pointeurs de moyenne des couleurs
int *moyenneRouge = NULL;
int *moyenneVert = NULL;
int *moyenneBleu = NULL;
//valeurs RGB des différentes couleurs de skittles
static const int SKITTLEROUGE[3]={255,0,0 };
static const int SKITTLEVERT[3]={0,255,0};
static const int SKITTLEJAUNE[3]={0,0,255};
static const int SKITTLEORANGE[3]={255,69,0};
static const int SKITTLEVIOLET[3]={128,0,0};
int SkittleLu[3]; //tableau taille 3 pour les valeurs RGB du skittle lu

//void turnoff();

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//void color(int red, int blue, int green);
void lecture();
int determinerCouleur();
void sortSkittle(int target);

void setup() {
    Serial.begin(9600);
    BoardInit();
    MOTOR_SetSpeed(0,0);
    MOTOR_SetSpeed(1,0);
    Serial.println("Color View Test!");


    if (tcs.begin()) {
        Serial.println("Found sensor");
        tcs.setInterrupt(false);      // turn on LED
    } else {
        Serial.println("No TCS34725 found ... check your connections");
        while (1); // halt!
    }

    // use these three pins to drive an LED
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(YELLOW, OUTPUT);

    // thanks PhilB for this gamma table!
    // it helps convert RGB colors to what humans see
    for (int i = 0; i < 256; i++) {
        float x = i;
        x /= 255;
        x = pow(x, 2.5);
        x *= 255;

        if (commonAnode) {
            gammatable[i] = 255 - x;
        } else {
            gammatable[i] = x;
        }
        //Serial.println(gammatable[i]);
    }
}


void loop() {
     MOTOR_SetSpeed(0,0);
   MOTOR_SetSpeed(1,0);
    uint16_t clear, red, green, blue;

    delay(150);  // takes 50ms to read

    tcs.getRawData(&red, &green, &blue, &clear);

    Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);

    // Figure out some basic hex code for visualization

    //uint32_t sum = clear;
    float r, g, b;
    r = red;// r /= sum;
    g = green;// g /= sum;
    b = blue; //b /= sum;
    // r *= 256; g *= 256; b *= 256;
    Serial.print("\t");
    Serial.print((int)r, HEX);
    Serial.print((int)g, HEX);
    Serial.print((int)b, HEX);
    Serial.println();
    //color(red,blue,green);
    void turnoff();
    //Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );

   /* analogWrite(redpin, gammatable[(int)r]);
    analogWrite(greenpin, gammatable[(int)g]);
    analogWrite(bluepin, gammatable[(int)b]);*/
}


//fonction qui lit la couleur d'un skittle avec une moyenne de cinq lectures
void Lecture() {

    uint16_t  red = 0, green = 0, blue = 0, clear = 0;
    int totalRouge = 0;
    int totalVert = 0;
    int totalBleu = 0;
    //boucle pour cinq lectures
    for (int i = 0; i < 5; i++) {
        tcs.getRawData(&red, &green, &blue, &clear); //on lit
        totalRouge += red;
        Serial.print(totalRouge);
        totalVert += green;
        Serial.print(totalVert);
        totalBleu += blue;
        Serial.print(totalBleu);

        delay(20);
    }
    *moyenneRouge = totalRouge/5;
    *moyenneVert = totalVert/5;
    *moyenneBleu = totalBleu/5;

}

//détermine la couleur du skittle
int determinerCouleur() {
    Lecture();
    int baseColors[5] = {RED, GREEN, YELLOW, ORANGE, PURPLE};
    int deltas[5] = {0};
    int averages[3] = { *moyenneRouge, *moyenneVert, *moyenneBleu };
    int colorTable[5][3] = {0};
    for (int i=0;i<3;i++) {
      colorTable[0][i] = SKITTLEROUGE[i]; colorTable[1][i] = SKITTLEVERT[i];
      colorTable[2][i] = SKITTLEJAUNE[i]; colorTable[3][i] = SKITTLEORANGE[i];
      colorTable[4][i] = SKITTLEVIOLET[i];
    }

    //on déclare des tableaux pour les différences de valeurs RGB
    int deltaSkittleRouge[3];
    int deltaSkittleVert[3];
    int deltaSkittleJaune[3];
    int deltaSkittleOrange[3];
    int deltaSkittleViolet[3];

    //trouvons la différence entre les valeurs RGB des différents skittles et les valeurs RGB lues

    int diff;
    for (int i=0;i<5;i++){
      for (int j=0;j<3;j++){
        diff = 0;
        diff += averages[i] - colorTable[i][j];
        diff = abs(diff);
      }
      deltas[i] = diff;
    }

    int color = 0; // no color is set
    int lastDelta = 255*3; // max delta possible
    for (int i = 0; i < 5; i++) {
      color = (deltas[i] < lastDelta) ? baseColors[i] : color;
    };
    return color;

    // //différence avec skittle Rouge
    // deltaSkittleRouge[0] = abs(SKITTLEROUGE[0]- *moyenneRouge);
    // deltaSkittleRouge[1] = abs(SKITTLEROUGE[1]- *moyenneVert);
    // deltaSkittleRouge[2] = abs(SKITTLEROUGE[2]- *moyenneBleu);
    // //différence avec skittle Vert
    // deltaSkittleVert[0] = abs(SKITTLEVERT[0]- *moyenneRouge);
    // deltaSkittleVert[1] = abs(SKITTLEVERT[1]- *moyenneVert);
    // deltaSkittleVert[2] = abs(SKITTLEVERT[2]- *moyenneBleu);
    // //différence avec skittle Jaune
    // deltaSkittleJaune[0] = abs(deltaSkittleJaune[0]- *moyenneRouge);
    // deltaSkittleJaune[1] = abs(deltaSkittleJaune[1]- *moyenneVert);
    // deltaSkittleJaune[2] = abs(deltaSkittleJaune[2]- *moyenneBleu);
    // //différence avec skittle Orange
    // deltaSkittleOrange[0] = abs(SKITTLEORANGE[0]- *moyenneRouge);
    // deltaSkittleOrange[1] = abs(SKITTLEORANGE[1]- *moyenneVert);
    // deltaSkittleOrange[2] = abs(SKITTLEORANGE[2]- *moyenneBleu);
    // //différence avec skittle Violet
    // deltaSkittleViolet[0] = abs(SKITTLEVIOLET[0]- *moyenneRouge);
    // deltaSkittleViolet[1] = abs(SKITTLEVIOLET[1]- *moyenneVert);
    // deltaSkittleViolet[2] = abs(SKITTLEVIOLET[2]- *moyenneBleu);

  //   //sommes des différences
  //   int sommeDeltaRouge = 0, sommeDeltaVert = 0, sommeDeltaJaune = 0, sommeDeltaOrange = 0, sommeDeltaViolet = 0;
  //   for (int i = 0; i < 3; i++) {
  //       sommeDeltaRouge += deltaSkittleRouge[i];
  //       sommeDeltaVert += deltaSkittleVert[i];
  //       sommeDeltaJaune += deltaSkittleJaune[i];
  //       sommeDeltaOrange += deltaSkittleOrange[i];
  //       sommeDeltaViolet += deltaSkittleViolet[i];
  //   }
  //  // int PlusPetiteSomme = 0;
  //  int deltaColors[5] = {sommeDeltaRouge,
  //                        sommeDeltaVert,
  //                        sommeDeltaJaune,
  //                        sommeDeltaOrange,
  //                        sommeDeltaViolet};

    // if (sommeDeltaRouge <= sommeDeltaVert && sommeDeltaRouge <= sommeDeltaJaune && sommeDeltaRouge <= sommeDeltaOrange && sommeDeltaRouge <= sommeDeltaViolet){
    //     Serial.println("Rouge");
    //     return RED;
    // }
    // else if (sommeDeltaVert <= sommeDeltaJaune && sommeDeltaVert <= sommeDeltaOrange && sommeDeltaVert <= sommeDeltaViolet) {
    //     Serial.println("Vert");
    //     return GREEN;}
    // else if (sommeDeltaJaune <= sommeDeltaOrange && sommeDeltaJaune <= sommeDeltaViolet) {
    //     Serial.println("Jaune");
    //     return YELLOW;
    // }
    // else if (sommeDeltaOrange <= sommeDeltaViolet) {
    //     Serial.println("Orange");
    //     return ORANGE;
    // }
    // else {
    //     Serial.println("Violet");
    //     return PURPLE;
    // }
}

void sortSkittle(int target){
  // Vérifier quelle est la bonne fonction pour le controlleur
  SERVO_SetAngle(SERVO_SORT, SORT_SENSOR);
  int color = determinerCouleur();
  char colorName[10];
  switch (color) {
  case GREEN:
    strcpy(colorName, "Vert");
    break;
  case ORANGE:
    strcpy(colorName, "Orange");
    break;
  case PURPLE:
    strcpy(colorName, "Violet");
    break;
  case RED:
    strcpy(colorName, "Rouge");
    break;
  case YELLOW:
    strcpy(colorName, "Jaune");
    break;
  default:
    strcpy(colorName, "INCONNUE");
    break;
  }
  Serial.print("Skittle is: ");
  Serial.println(colorName);
  if(color == target){
    SERVO_SetAngle(SERVO_SORT, SORT_TARGET);
  } else {
    SERVO_SetAngle(SERVO_SORT, SORT_TRASH);
  }
  SERVO_SetAngle(SERVO_SORT, SORT_SENSOR);
}



   /* void turnoff()
    {
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,LOW);
        digitalWrite(BLUE,LOW);
        digitalWrite(YELLOW,LOW);

    }*/



