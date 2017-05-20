// Name        : Minotaure.ino
// Description : Robot sumo pour le tournois national de robotique de Nimes 2017
// Author      : (c) Arthur & Jean-Paul GERST
// Date        : 2017

#include ".\BinaryInput.h"

/*
 * Definition des entres / sorties
 */
#define pinDirGauche        A0  // port du capteur de Distance infrarouge Gauche
#define pinDirAvGauche      A1  // port du capteur de Distance infrarouge Avant Gauche
#define pinDirAvDroit       A2  // port du capteur de Distance infrarouge Avant Droit
// SDA  A4
// SCL  A5
//#define pinDirArriere     A6  // port du capteur de Distance infrarouge Arriere
#define pinDirDroit         A7  // port du capteur de Distance infrarouge Droit
#define pinLed              13  // port de la LED interne de la carte
#define pinIrAvGauche       8   // port du detecteur Infrarouge CNY70 Avant Gauche 
#define pinIrAvDroit        4   // port du detecteur Infrarouge CNY70 Avant Droit
#define pinIrArGauche       6   // port du detecteur Infrarouge CNY70 Arriere Gauche 
#define pinIrArDroit        5   // port du detecteur Infrarouge CNY70 Avant Gauche 
#define pinboutonStart      12  // port du Bouton poussoir permettant le demarrage
#define pinMoteurInGauche1  3
#define pinMoteurInGauche2  9
#define pinMoteurInDroit1   10
#define pinMoteurInDroit2   11

/*
 * Definition des constantes
 */
#define SEUIL_DIR_AV        180  // Seuil de detection des capteurs analogiques de distance infrarouge.
#define SEUIL_DIR_GAUCHE    220  // Seuil de detection des capteurs analogiques de distance infrarouge.
#define SEUIL_DIR_DROIT     220  // Seuil de detection des capteurs analogiques de distance infrarouge.
#define VITESSE             100 
#define VITESSE_RECULE     -100
#define VITESSE_POUSSE      200
#define VITESSE_ROTATION     60
#define TEMPO_AVANCE        500
#define TEMPO_RECULE        500
#define TEMPO_TOURNE        400

/*
 * Definition des differents modes dans lequel est le robot
 */
#define MODE_UNKNOW   0     // Mode inconu ou non défini
#define MODE_BOOT     1     // Le robot vient de demarrer, un relachement du bouton start le fera passer en mode WAIT
#define MODE_WAIT     2     // Le robot attends 5s. Si le bouton est réappuié / relaché, cela remet le conteur à zero
#define MODE_TEST     3     // Le robot lance la procedure de checkup !! un appuit sur le bouton start n'a aucun effect direct, si ce n'est allumé en vert un des neoPixel
#define MODE_CHERCHE  4     // Le robot tourne sur lui-même pour rechercher l'adversaire
#define MODE_AVANCE   5     // Le robot se deplace un peu sur la piste, et relancera un SEARCH
#define MODE_ALIGN    6     // Le robot adverse est detecté, un alignement de ce dernier avec les capteurs avants est a faire
#define MODE_POUSSE   7     // Le robot adverse est en face, il faut pousser
#define MODE_ESQUIVE  8     // Le robot ne se deplace pas comme imagine. Il faut probablement esquiver
#define MODE_RECULE   9
#define MODE_TOURNE_DROITE  10
#define MODE_TOURNE_GAUCHE  11
#define MODE_CHERCHE_DROITE 13     // Le robot tourne sur lui-même pour rechercher l'adversaire
#define MODE_CHERCHE_GAUCHE 14     // Le robot tourne sur lui-même pour rechercher l'adversaire

/*
 * Definition des différentes variable de capteur
 */
BinaryInput irAvGauche(pinIrAvGauche);
BinaryInput irArGauche(pinIrArGauche);
BinaryInput irAvDroit(pinIrAvDroit);
BinaryInput irArDroit(pinIrArDroit);
BinaryInput binAvGauche(-1);
BinaryInput binAvDroit(-1);
BinaryInput binDroit(-1);
BinaryInput binGauche(-1);

/*
 * Gestion de l'ensemble des capteurs & de leur mise à jour
 */
class Sensors {
  private:
    unsigned long _previousMillis = 0;
    unsigned long _duree = 50;
    int counteur = 100;

  public:
    int dirAvDroit;
    int dirAvGauche;
    int dirDroit;
    int dirGauche;
    int dirArriere;
    int8_t temp;
    int compteur = 0;
    int angle = 0;
    BinaryInput boutonStart = BinaryInput(pinboutonStart, INPUT_PULLUP);

    /*
     * Initialisation des capteurs
     */
    void setup() {
    }

   /*
    * Mise à jour de l'ensemble des capteurs
    */
    void update(unsigned long currentMillis) {
      int compteur_parasitage;
      int nb_test_parasitage = 5;
      int seuil_parasitage = 3;
      int i;

      /*
       * Lecture des capteurs binaires      
       */
      irAvGauche.read(true);
      irAvDroit.read(true);
      irArGauche.read(true);
      irArDroit.read(true);
      boutonStart.read(true);

      /*
       * Mise à jour des capteurs de distance
       * 
       * En raison de parasitage incroyable sur les capteurs, je fais plusieurs lectures, un nombre impaire 
       * et je prends la valeur majoritaire
       */
      // Lecture du capteur de distance avant gauche
      for (i=0, compteur_parasitage=0; i<nb_test_parasitage; i++) {
        dirAvGauche = analogRead(pinDirAvGauche);    
        if (dirAvGauche > SEUIL_DIR_AV)
          compteur_parasitage++;
      }
      if (compteur_parasitage >= seuil_parasitage)
        binAvGauche.set(HIGH);
      else
        binAvGauche.set(LOW);

      // Lecture du capteur de distance avant droit
      for (i=0, compteur_parasitage=0; i<nb_test_parasitage; i++) {
        dirAvDroit = analogRead(pinDirAvDroit);
        if (dirAvDroit > SEUIL_DIR_AV)
          compteur_parasitage++;
      }

      if (compteur_parasitage >= seuil_parasitage)
        binAvDroit.set(HIGH);
      else
        binAvDroit.set(LOW);

      // Lecture du capteur de distance droit
      for (i=0, compteur_parasitage=0; i<nb_test_parasitage; i++) {
        dirDroit = analogRead(pinDirDroit);
        if (dirDroit < SEUIL_DIR_DROIT)
          compteur_parasitage++;
      }

      if (compteur_parasitage >= seuil_parasitage)
        binDroit.set(HIGH);
      else
        binDroit.set(LOW);

      // Lecture du capteur de distance gauche
      for (i=0, compteur_parasitage=0; i<nb_test_parasitage; i++) {
        dirGauche = analogRead(pinDirGauche);      
        if (dirGauche < SEUIL_DIR_GAUCHE)
          compteur_parasitage++;
      }
      
      if (compteur_parasitage >= seuil_parasitage)
        binGauche.set(HIGH);
      else
        binGauche.set(LOW);

      _previousMillis = currentMillis;
    }
};
Sensors sensors;


class StateMachine {
  public:
    int mode;     // Mode actuel
    int nextMode = MODE_UNKNOW;   // Prochain mode,
    int previousMode = MODE_UNKNOW;
    int newMode = 2;              // S'agit d'une entree dans ce mode ? 2 car il faut faire deux updates avant de supprimer la valeur new...
    unsigned long modeStartTime;  // Quand le mode actuel a t il commencé 
    unsigned long modeDuration;   // Combien de temps le mode actuel doit durer. Zero signifie sans limite de temps.
    unsigned long cycleDuration;  // Temps de cycle. Tous les combiens en fait un update ?
    unsigned long previousMillis; // Heure du cycle precedent, pour calculer le cycle actuel
    unsigned long previousMicro;  // Heure du cycle precedent, pour calculer le cycle actuel
    
    void setup() {
      setMode(MODE_BOOT);      
    }
    
    void setMode(int _newMode, int _nextMode = MODE_UNKNOW, unsigned long _modeDuration = 0) {
      previousMode = mode;
      mode = _newMode;
      modeStartTime = millis();
      newMode = 2;
      modeDuration = _modeDuration;
      nextMode = _nextMode;
    }

    void update(unsigned long currentMillis) {
      previousMillis = currentMillis;
      unsigned long currentMicro = micros(); 
      cycleDuration = currentMicro - previousMicro;

      // Toutes les valeurs differentes de zero sont vraies.
      if (newMode > 0)
        newMode--;
      if ((modeDuration != 0) && (((modeStartTime + modeDuration) < currentMillis))) {
        setMode(nextMode);
      }
      previousMicro = currentMicro;
    }
};
StateMachine stateMachine;

/*
 * Gestion des moteurs
 */
class Moteurs {
public:
  int currentGauche = 0;
  int currentDroit = 0;
  
  void setup() {
    pinMode(pinMoteurInGauche1, OUTPUT);
    pinMode(pinMoteurInGauche2, OUTPUT);
    pinMode(pinMoteurInDroit1,  OUTPUT);
    pinMode(pinMoteurInDroit2,  OUTPUT);
    off();    
  }

  /*
   * Etteinds tous les moteurs
   */
  void off() {
      digitalWrite(pinMoteurInGauche1, LOW);
      digitalWrite(pinMoteurInGauche2, LOW);
      digitalWrite(pinMoteurInDroit1, LOW);
      digitalWrite(pinMoteurInDroit2, LOW);    
  }


  /*
   * Gestion de la consigne moteurs - Permet de controler les moteurs, leur vitesse, leur sens.
   * 
   * int gauche - consigne moteur gauche
   * int droit - consigne moteur droit 
   * 
   * Les consignes sont des valeurs allant de -255 à 255. Les valeurs négative font tourner le moteur en marche arrière
   * 
   * Pour une raison que j'ignore, les valeurs PWM semblent inversees.
   * 0 fait tourner le moteur tres vite, et 255 pas du tout. 
   * 
   * Il y a donc un filtre pour convertir les consignes allant de 0 à 255 en PWM allant de 255 à 0
   */
  void consigne(int gauche, int droit)
  {  
    if (gauche < 0) {
      digitalWrite(pinMoteurInGauche1, HIGH);
      analogWrite(pinMoteurInGauche2, 255 - abs(gauche));
    } else {
      digitalWrite(pinMoteurInGauche2, HIGH);
      analogWrite(pinMoteurInGauche1, 255 - gauche);
    }
  
    if (droit < 0) {
      digitalWrite(pinMoteurInDroit2, HIGH);
      analogWrite(pinMoteurInDroit1, 255 - abs(droit));
    } else {
      digitalWrite(pinMoteurInDroit1, HIGH);
      analogWrite(pinMoteurInDroit2, 255 - droit);
    }  
  }
};
Moteurs moteurs;

// Gestion du bord du pleateu
void resteSurLePlateau() 
{
  // Si les deux capteurs de gauche detectent le bord, tourner à droite
  if (irAvGauche.current && irArGauche.current) {
    stateMachine.setMode(MODE_TOURNE_DROITE, MODE_AVANCE, TEMPO_TOURNE);
  } 
  // Si les deux capteurs de droite detectent le bord, tourner à gauche
  else if (irAvDroit.current && irArDroit.current) {
    stateMachine.setMode(MODE_TOURNE_GAUCHE, MODE_AVANCE, TEMPO_TOURNE);
  } 
  // Si les deux capteurs avant detectent le bord, reculer
  else if (irAvGauche.current || irAvDroit.current) {
    moteurs.consigne(VITESSE_RECULE,VITESSE_RECULE);
    stateMachine.setMode(MODE_RECULE, MODE_CHERCHE, TEMPO_RECULE);
  } 
  // Si les deux capteurs arrières detectent le bord, avancer
  else if (irArGauche.current || irArDroit.current) {
    moteurs.consigne(VITESSE,VITESSE);
    stateMachine.setMode(MODE_AVANCE, MODE_CHERCHE, TEMPO_AVANCE);
  }
}

/*
 * Configuration de l'application.
 * 
 * L'ensemble des objets sont initialises
 */
void setup()
{
  stateMachine.setup();
  sensors.setup();
  moteurs.setup();
}


int vitesseRotation = VITESSE_ROTATION;

/*
 * Boucle principale de l'application
 * 
 * On lit tous les capteurs, et en fonction du mode actuel en agit.
 */
void loop()
{
  unsigned long current = millis();
  
  sensors.update(current);
  stateMachine.update(current);
  resteSurLePlateau();
    
  switch(stateMachine.mode) 
  {
    case MODE_BOOT:
      if (stateMachine.newMode)
        moteurs.off();
      if (sensors.boutonStart.dropped) {
        if (binDroit.current)
          stateMachine.setMode(MODE_WAIT, MODE_CHERCHE_DROITE, 5000);
        else if (binGauche.current)
          stateMachine.setMode(MODE_WAIT, MODE_CHERCHE_GAUCHE, 5000);
        else
          stateMachine.setMode(MODE_WAIT, MODE_CHERCHE, 5000);
      }
      break;

    case MODE_WAIT:
      if (sensors.boutonStart.dropped)        
        stateMachine.setMode(MODE_BOOT);
      break;
      
    case MODE_AVANCE:
      if (stateMachine.newMode)
        moteurs.consigne(VITESSE, VITESSE);            
      break;

    case MODE_RECULE:
      if (stateMachine.newMode) 
        moteurs.consigne(VITESSE_RECULE, VITESSE_RECULE);        
      break;
    
    case MODE_TOURNE_DROITE:
      if (stateMachine.newMode)
        moteurs.consigne(VITESSE_ROTATION, -VITESSE_ROTATION);
      break;
      
    case MODE_TOURNE_GAUCHE:
      if (stateMachine.newMode)
        moteurs.consigne(-VITESSE_ROTATION, VITESSE_ROTATION);
      break;
      
    case MODE_CHERCHE:
      if (stateMachine.newMode) {
        vitesseRotation = VITESSE_ROTATION;
        moteurs.consigne(vitesseRotation, -vitesseRotation);
      }
      if (binAvGauche.current && binAvDroit.current)
        stateMachine.setMode(MODE_POUSSE);
      break;

    case MODE_CHERCHE_DROITE:
      if (stateMachine.newMode)
        moteurs.consigne(vitesseRotation, -vitesseRotation);
      else {
        vitesseRotation += 25;
        if (vitesseRotation > VITESSE_ROTATION)
          vitesseRotation = VITESSE_ROTATION;
      }
           
      if (binAvGauche.current && binAvDroit.current)
        stateMachine.setMode(MODE_POUSSE);
      break;

    case MODE_CHERCHE_GAUCHE:
      if (stateMachine.newMode)
        moteurs.consigne(-vitesseRotation, vitesseRotation);
      else {
        vitesseRotation += 25;
        if (vitesseRotation > VITESSE_ROTATION)
          vitesseRotation = VITESSE_ROTATION;
      }
      if (binAvGauche.current && binAvDroit.current)
        stateMachine.setMode(MODE_POUSSE);
      break;

    case MODE_POUSSE:
      if (stateMachine.newMode) {
        moteurs.consigne(VITESSE_POUSSE, VITESSE_POUSSE);
      }
      if (binAvGauche.dropped) {
        vitesseRotation = VITESSE_ROTATION / 2;
        stateMachine.setMode(MODE_CHERCHE_DROITE);
      }          
      
      if (binAvDroit.dropped) {
        vitesseRotation = VITESSE_ROTATION / 2;
        stateMachine.setMode(MODE_CHERCHE_GAUCHE);
      }
        
      break;
  }  
}


