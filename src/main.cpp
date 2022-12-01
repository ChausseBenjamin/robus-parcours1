#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LibRobus.h>
#include <math.h>
#include <string.h>

// définition des pins
#define BOUTONROUGE 2
#define BOUTONORANGE 3
#define BOUTONJAUNE 9
#define BOUTONVERT 10
#define BOUTOMAUVE 8
#define VIBRATOR_PIN 43

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
// constante des différentes couleurs des skittles
#define GREEN 1
#define YELLOW 2
#define ORANGE 3
#define VIOLET 4
#define RED 0
#define COULEUR_RIEN_PANTOUTE 5

// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to groundé
// set to false if using a common cathode LED
#define commonAnode false

// Sorting servo data (SORT prefix defines the servo angles for components)
#define SERVO_SORT 0   // TODO: vérifier si le servo est le bon
#define SORT_TARGET 0  // TODO: vérifier si l'angle du servo est le bon
#define SORT_TRASH 180 // TODO: vérifier si l'angle du servo est le bon
#define SORT_SENSOR 90 // TODO: vérifier si l'angle du servo est le bon

// function prototypes
int choix_de_couleur_boutons();

// function prototypes
int choix_de_couleur_boutons();

// our RGB -> eye-recognized gamma color
byte gammatable[256];
// initialise des pointeurs de moyenne des couleurs

int moyenneRouge = 0;
int moyenneVert = 0;
int moyenneBleu = 0;

int totalRouge = 0;
int totalVert = 0;
int totalBleu = 0;

int ArrayMoyenne[3] = {0, 0, 0};
// valeurs RGB des différentes couleurs de skittles
static const int SKITTLEROUGE[3] = {146, 134, 140};
static const int SKITTLEVERT[3] = {138, 187, 155};
static const int SKITTLEJAUNE[3] = {275, 242, 171};
static const int SKITTLEORANGE[3] = {233, 158, 152};
static const int SKITTLEVIOLET[3] = {115, 129, 135};
static const int RIEN_PANTOUTE[3] = {143, 164, 164};
int SkittleLu[3]; // tableau taille 3 pour les valeurs RGB du skittle lu

// void turnoff();

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// void color(int red, int blue, int green);
void lecture();
int determinerCouleur();
void sortSkittle(int target);

void setup()
{
    pinMode(VIBRATOR_PIN,OUTPUT);
  Serial.begin(9600);
  BoardInit();
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
  Serial.println("Color View Test!");

  if (tcs.begin())
  {
    Serial.println("Found sensor");
    tcs.setInterrupt(false); // turn on LED
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections");
    while (1)
      ; // halt!
  }

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i = 0; i < 256; i++)
  {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode)
    {
      gammatable[i] = 255 - x;
    }
    else
    {
      gammatable[i] = x;
    }
    // Serial.println(gammatable[i]);
  }
}

// fonction qui lit la couleur d'un skittle avec une moyenne de cinq lectures
void Lecture()
{

  uint16_t red = 0, green = 0, blue = 0, clear = 0;

  // boucle pour dix lectures
  /*for (int i = 0; i < 10; i++) {
      tcs.getRawData(&red, &green, &blue, &clear); //on lit
      totalRouge += red;
      //Serial.print("Total rouge courant : ");
      //Serial.println(totalRouge);
      totalVert += green;
      //Serial.print("Total vert courant : ");
      //Serial.println(totalVert);
      totalBleu += blue;
      //Serial.print("Total bleu courant : ");
      //Serial.println(totalBleu);

      delay(20);
  }*/
  tcs.getRawData(&red, &green, &blue, &clear); // on lit
  totalRouge = red;
  Serial.print("Total rouge courant : ");
  Serial.println(totalRouge);
  totalVert = green;
  Serial.print("Total vert courant : ");
  Serial.println(totalVert);
  totalBleu = blue;
  Serial.print("Total bleu courant : ");
  Serial.println(totalBleu);

  // delay(20);

  ArrayMoyenne[0] = totalRouge / 10;
  ArrayMoyenne[1] = totalVert / 10;
  ArrayMoyenne[2] = totalBleu / 10;
}

// détermine la couleur du skittle
int determinerCouleur()
{
  int closestColor = COULEUR_RIEN_PANTOUTE;
  int cntRouge = 0;
  int cntVert = 0;
  int cntJaune = 0;
  int cntOrange = 0;
  int cntViolet = 0;
  int cntPantoute = 0;

  // on déclare des tableaux pour les différences de valeurs RGB
  int deltaSkittleRouge[3];
  int deltaSkittleVert[3];
  int deltaSkittleJaune[3];
  int deltaSkittleOrange[3];
  int deltaSkittleViolet[3];
  int deltaRien_Pantoute[3];

  for (int i = 0; i < 10; i++)
  {
    /* code */

    Lecture();

    // trouvons la différence entre les valeurs RGB des différents skittles et les valeurs RGB lues
    // différence avec skittle Rouge
    deltaSkittleRouge[0] = (SKITTLEROUGE[0] - totalRouge);
    deltaSkittleRouge[0] = abs(deltaSkittleRouge[0]);
    deltaSkittleRouge[1] = (SKITTLEROUGE[1] - totalVert);
    deltaSkittleRouge[1] = abs(deltaSkittleRouge[1]);
    deltaSkittleRouge[2] = (SKITTLEROUGE[2] - totalBleu);
    deltaSkittleRouge[2] = abs(deltaSkittleRouge[2]);
    // différence avec skittle Vert
    deltaSkittleVert[0] = SKITTLEVERT[0] - totalRouge;
    deltaSkittleVert[0] = abs(deltaSkittleVert[0]);
    deltaSkittleVert[1] = SKITTLEVERT[1] - totalVert;
    deltaSkittleVert[1] = abs(deltaSkittleVert[1]);
    deltaSkittleVert[2] = SKITTLEVERT[2] - totalBleu;
    deltaSkittleVert[2] = abs(deltaSkittleVert[2]);
    // différence avec skittle Jaune
    deltaSkittleJaune[0] = SKITTLEJAUNE[0] - totalRouge;
    deltaSkittleJaune[0] = abs(deltaSkittleJaune[0]);
    deltaSkittleJaune[1] = SKITTLEJAUNE[1] - totalVert;
    deltaSkittleJaune[1] = abs(deltaSkittleJaune[1]);
    deltaSkittleJaune[2] = SKITTLEJAUNE[2] - totalBleu;
    deltaSkittleJaune[2] = abs(deltaSkittleJaune[2]);
    // différence avec skittle Orange
    deltaSkittleOrange[0] = SKITTLEORANGE[0] - totalRouge;
    deltaSkittleOrange[0] = abs(deltaSkittleOrange[0]);
    deltaSkittleOrange[1] = SKITTLEORANGE[1] - totalVert;
    deltaSkittleOrange[1] = abs(deltaSkittleOrange[1]);
    deltaSkittleOrange[2] = SKITTLEORANGE[2] - totalBleu;
    deltaSkittleOrange[2] = abs(deltaSkittleOrange[2]);
    // différence avec skittle Violet
    deltaSkittleViolet[0] = SKITTLEVIOLET[0] - totalRouge;
    deltaSkittleViolet[0] = abs(deltaSkittleViolet[0]);
    deltaSkittleViolet[1] = SKITTLEVIOLET[1] - totalVert;
    deltaSkittleViolet[1] = abs(deltaSkittleViolet[1]);
    deltaSkittleViolet[2] = SKITTLEVIOLET[2] - totalBleu;
    deltaSkittleViolet[2] = abs(deltaSkittleViolet[2]);
    // différence avec rien pantoute
    deltaRien_Pantoute[0] = RIEN_PANTOUTE[0] - totalRouge;
    deltaRien_Pantoute[0] = abs(deltaRien_Pantoute[0]);
    deltaRien_Pantoute[1] = RIEN_PANTOUTE[1] - totalVert;
    deltaRien_Pantoute[1] = abs(deltaRien_Pantoute[1]);
    deltaRien_Pantoute[2] = RIEN_PANTOUTE[2] - totalBleu;
    deltaRien_Pantoute[2] = abs(deltaRien_Pantoute[2]);

    // sommes des différences
    int sommeDeltaRouge = 0;
    int sommeDeltaVert = 0;
    int sommeDeltaJaune = 0;
    int sommeDeltaOrange = 0;
    int sommeDeltaViolet = 0;
    int sommeDeltaRien_Pantoute = 0;

    for (int i = 0; i < 3; i++)
    {
      sommeDeltaRouge += deltaSkittleRouge[i];
      sommeDeltaVert += deltaSkittleVert[i];
      sommeDeltaJaune += deltaSkittleJaune[i];
      sommeDeltaOrange += deltaSkittleOrange[i];
      sommeDeltaViolet += deltaSkittleViolet[i];
      sommeDeltaRien_Pantoute += deltaRien_Pantoute[i];
    }
    /*Serial.print("Somme delta Rouge");
    Serial.println(sommeDeltaRouge);
    Serial.print("Somme delta Vert");
    Serial.println(sommeDeltaVert);
    Serial.print("Somme delta Jaune");
    Serial.println(sommeDeltaJaune);
    Serial.print("Somme delta Orange");
    Serial.println(sommeDeltaOrange);
    Serial.print("Somme delta Violet");
    Serial.println(sommeDeltaViolet);
    Serial.print("Somme delta Rien Pantoute");
    Serial.println(sommeDeltaRien_Pantoute);*/

    if (sommeDeltaRouge <= sommeDeltaVert && sommeDeltaRouge <= sommeDeltaJaune && sommeDeltaRouge <= sommeDeltaOrange && sommeDeltaRouge <= sommeDeltaViolet && sommeDeltaRouge <= sommeDeltaRien_Pantoute)
    {
      Serial.println("Rouge");
      cntRouge++;
      // return RED;
    }
    else if (sommeDeltaVert <= sommeDeltaJaune && sommeDeltaVert <= sommeDeltaOrange && sommeDeltaVert <= sommeDeltaViolet && sommeDeltaVert <= sommeDeltaRien_Pantoute)
    {
      Serial.println("Vert");
      // return GREEN;
      cntVert++;
    }
    else if (sommeDeltaJaune <= sommeDeltaOrange && sommeDeltaJaune <= sommeDeltaViolet && sommeDeltaJaune <= sommeDeltaRien_Pantoute)
    {
      Serial.println("Jaune");
      cntJaune++;
      // return YELLOW;
    }
    else if (sommeDeltaOrange <= sommeDeltaViolet && sommeDeltaOrange <= sommeDeltaRien_Pantoute)
    {
      Serial.println("Orange");
      cntOrange++;
      // return ORANGE;
    }
    else if (sommeDeltaViolet <= sommeDeltaRien_Pantoute)
    {
      Serial.println("Violet");
      cntViolet++;
      // return VIOLET;
    }
    else
    {
      Serial.println("Rien pantoute");
      cntPantoute++;
      // return COULEUR_RIEN_PANTOUTE;
    }
  }

  // Determine quelle couleur a ete la plus sélectionnee
  if (cntRouge > cntVert && cntRouge > cntJaune && cntRouge > cntOrange && cntRouge > cntViolet && cntRouge > cntPantoute)
  {
    closestColor = RED;
  }
  else if (cntVert > cntJaune && cntVert > cntOrange && cntVert > cntViolet && cntVert > cntPantoute)
  {

    closestColor = GREEN;
  }
  else if (cntJaune > cntOrange && cntJaune > cntViolet && cntJaune > cntPantoute)
  {

    closestColor = YELLOW;
  }
  else if (cntOrange > cntViolet && cntOrange > cntPantoute)
  {

    closestColor = ORANGE;
  }
  else if (cntViolet > cntPantoute)
  {

    closestColor = VIOLET;
  }
  return closestColor;
}

void sortSkittle(int target)
{
  // Vérifier quelle est la bonne fonction pour le controlleur
  // delay(150);
  SERVO_SetAngle(0, 85);
  int color = determinerCouleur();
  Serial.print("Color is : ");
  Serial.println(color);
  if (color == RED)
  {
    Serial.println("\nSkittle is Red\n\n");
  }
  if (color == GREEN)
  {
    Serial.println("\nSkittle is Green\n\n");
  }
  if (color == YELLOW)
  {
    Serial.println("\nSkittle is Yellow\n\n");
  }
  if (color == ORANGE)
  {
    Serial.println("\nSkittle is Orange\n\n");
  }
  if (color == VIOLET)
  {
    Serial.println("\nSkittle is Violet\n\n");
  }

  delay(200);
  digitalWrite(VIBRATOR_PIN, HIGH);
  if (color == target)
  {
    SERVO_SetAngle(0, 162);
  }
  else
  {
    SERVO_SetAngle(0, 0);
  }
  delay(500);
  digitalWrite(VIBRATOR_PIN, LOW);
  SERVO_SetAngle(0, 85);
}

void monter_recipient()
{
  delay(10);
  SERVO_SetAngle(1, 130);
  delay(1000);
  digitalWrite(VIBRATOR_PIN,HIGH);
  delay(2000);
  digitalWrite(VIBRATOR_PIN,LOW);
  delay(4000);
  SERVO_SetAngle(1, 0);
}
void loop() 
{
  delay(1500);
  Serial.println("Entered in loop function");
  int choix_bouton = COULEUR_RIEN_PANTOUTE;
  while (choix_bouton == COULEUR_RIEN_PANTOUTE)
  //while(true)
  {
    choix_bouton = choix_de_couleur_boutons();
    //Serial.println(choix_bouton);
  }
  // SERVO_SetAngle(0, 85);
  monter_recipient();
  do
  {
    sortSkittle(choix_bouton);
  } while (determinerCouleur() != COULEUR_RIEN_PANTOUTE);
  while (1)
  {
  };
  
}

int choix_de_couleur_boutons()
{
  int val = 0;
  int state = LOW;

  val = digitalRead(BOUTONROUGE);

  if (val == 1)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.print("BOUTON ROUGE");
      return RED;
    }
  }

  val = digitalRead(BOUTONORANGE);
  if (val == 1)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.print("BOUTON ORANGE");
      return ORANGE;
    }
  }

  val = digitalRead(BOUTONJAUNE);
  if (val == 1)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.print("BOUTON JAUNE");
      return YELLOW;
    }
  }

  val = digitalRead(BOUTONVERT);
  if (val == 1)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.print("BOUTON VERT");
      return GREEN;
    }
  }

  val = digitalRead(BOUTOMAUVE);
  if (val == 1)
  {
    state = !state;
    if (state == HIGH)
    {
      Serial.print("BOUTON MAUVE");
      return VIOLET;
    }
  }
  return COULEUR_RIEN_PANTOUTE;

  delay(20);


}