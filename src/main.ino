/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/* ****************************************************************************
Inclure les librairies de functions que vous voulez utiliser
**************************************************************************** */

#include <LibRobus.h> // Essentielle pour utiliser RobUS
#include <stdio.h>

/* ****************************************************************************
Variables globales et defines
**************************************************************************** */
// -> defines...
// L'ensemble des fonctions y ont acces
#define FUNNEL_MOTOR 43 // Digital IO
#define SHAKE_TIME 3000 // how long the motor should shake the funnel


/* ****************************************************************************
Vos propres fonctions sont creees ici
**************************************************************************** */

// Avancer() est la fonction qui permet au robot d’avancer tout droit 
// Tourner() est la fonction qui permet au robot de tourner vers la gauche ou la droite

void Avancer(float distance) // distance : Distance à parcourir tout droit (cm)
{
  int pulsesParCM = 3200/11.969; //Calcul du nombre de pulses nécessaires pour parcourir un centimètre
  int32_t nbPulseDistance = distance*pulsesParCM; //Nombre de pulses équivalents à la distance à parcourir
  int nbPulseVouluParIntervalle = 500; //Nombre de pulses voulus par intervalle 
  /* Note : Pour avoir un nombre de pulses par intervalle qui fait du sens avec la vitesse initiale des moteurs, 
  on va faire un printf du nbPulseRealGauche à la première itération. Comme ça, on ne va pas starter avec une
  combinaison nbPulseRealGauche - nbPulseVouluParIntervalle trop divergente */ 
  int intervalleTemps = 100; //Intervalle de temps entre chaque vérification (en ms)
  int nbVerif = 0; //Nombre de fois que la vérification a été effectuée
  int pulseRealTotalGauche = 0;
  int pulseRealTotalDroit = 0;
  float speedLeft = 0.3; //vitesse initiale donnée au moteur gauche
  float speedRight = 0.3; //vitesse initiale donnée au moteur droit
 
  while (pulseRealTotalGauche <= nbPulseDistance || pulseRealTotalGauche <= nbPulseDistance)
  {
    delay (intervalleTemps);
    nbVerif++;
    int nbPulseRealGauche = ENCODER_Read(LEFT); //Pulses lus par l’encodeur gauche
    int nbPulseRealDroit = ENCODER_Read(RIGHT); //Pulses lus par l’encodeur droit
    Serial.print(nbPulseRealGauche);
    Serial.print(nbPulseRealDroit);
      //Serial.print("Gauche : %d Droite : %d", nbPulseRealGauche, nbPulseRealDroit);
    pulseRealTotalGauche += nbPulseRealGauche; 
    pulseRealTotalDroit += nbPulseRealDroit; 
    int erreurGauche = nbPulseVouluParIntervalle - nbPulseRealGauche; //Différence entre le nombre de pulses voulus et ceux lus par l’encodeur gauche
    int erreurDroit = nbPulseVouluParIntervalle - nbPulseRealDroit; //Différence entre le nombre de pulses voulus et ceux lus par l’encodeur droit
    int pulseVouluTotal = nbPulseVouluParIntervalle * nbVerif;
    //la fonction ENCODER_ReadReset()  permet de lire le nombre de pulses à cette itération en lisant l’encodeur puis reset le remet à 0 pour la prochaine itération
    int erreurGaucheTotal = pulseVouluTotal - pulseRealTotalGauche;
    int erreurDroitTotal = pulseVouluTotal - pulseRealTotalGauche; 
    float Kp = 0.0002; //Correction sur la vitesse qui ne prend pas en compte l’erreur sur la distance parcourue depuis le début
    float Ki = 0.00002; //Correction sur la vitesse qui prend en compte l’erreur sur la distance parcourue depuis le début
    float correctionGauche = erreurGauche * Kp + erreurGaucheTotal * Ki;
    float correctionDroit = erreurDroit * Kp + erreurDroitTotal * Ki;
    speedLeft += correctionGauche; //ajuste la nouvelle vitesse (speedLeft provient de l’itération précédente et est ajustée avec la correction
    speedRight += correctionDroit;
    MOTOR_SetSpeed(0, speedLeft); //ajustement de la vitesse du moteur gauche
    MOTOR_SetSpeed(1, speedRight);//ajustement de la vitesse du moteur droit
    pulseRealTotalGauche += ENCODER_ReadReset(LEFT);
    pulseRealTotalDroit += ENCODER_ReadReset(RIGHT);
  }
}

// active un moteur de vibration pendant 3 secondes pour éviter que l'entonnoir ne se bloque
void shakeFunnel(int ms){
  // send a digital signal to the funnel motor
  digitalWrite(FUNNEL_MOTOR, HIGH);
  delay(ms);
  digitalWrite(FUNNEL_MOTOR, LOW);
}
 



/* ****************************************************************************
Fonctions d'initialisation (setup)
**************************************************************************** */
// -> Se fait appeler au debut du programme
// -> Se fait appeler seulement un fois
// -> Generalement on y initilise les varibbles globales

void setup(){
  BoardInit();
  // sets FUNNEL_MOTOR as an output in digital mode set to LOW
  pinMode(FUNNEL_MOTOR, OUTPUT); // XXX: Très important pour être capable de stopper le moteur de vibration
  digitalWrite(FUNNEL_MOTOR, LOW);
}

/* ****************************************************************************
Fonctions de boucle infini (loop())
**************************************************************************** */
// -> Se fait appeler perpetuellement suite au "setup"

  void loop() {
  // SOFT_TIMER_Update(); // A decommenter pour utiliser des compteurs logiciels
  delay(3000);// Delais pour décharger le CPU
  digitalWrite(FUNNEL_MOTOR, HIGH);
  delay(3000);
  digitalWrite(FUNNEL_MOTOR, LOW);


  //Test pour commit via VS Code
  //test
  //test ann-sosdfsdf 2222222

  //comment
  /*


  */
}

