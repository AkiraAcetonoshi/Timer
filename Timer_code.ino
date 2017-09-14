/*
  Ce programme permet de créer un minuteur à l'aide de trois boutons, un afficheur 7 Segments SH5461AS et un anneau de Leds Adafruit neopixel
  Ce programme a été écrit par Matthieu CHATRY, Flavien, Yannick, Maÿlis et David LE GALL et est publié dans le domaine public
  Retrouvez le tutoriel complet à cette adresse : http://wikifab.org/wiki/Timer_:_Un_minuteur_%C3%A0_base_d%27Arduino
*/



#include <SevSeg.h>                    //Bibliothèque permettant de piloter l'afficheur 7 segments à télécharger ici https://github.com/DeanIsMe/SevSeg  
#include <Adafruit_NeoPixel.h>         //Bibliothèque permettant de piloter l'anneau de leds Adafruit Neopixel à télécharger ici https://github.com/adafruit/Adafruit_NeoPixel

//Connexions électroniques et variables des boutons
const int BOUTON_MOINS = 2;            // Le bouton MOINS est relié à la broche 2
const int BOUTON_PLUS = 3;             // Le bouton PLUS est relié à la broche 3
const int BOUTON_START = 4;            // Le bouton START est relié à la broche 4
int etatBoutonMoins = 0;               // variable d'état du bouton MOINS
int etatBoutonPlus = 0;                // variable d'état du bouton PLUS
int etatBoutonStart = 0;               // variable d'état du bouton START


// Anneau de Led
const int NEOPIXEL = 5;                   // La broche de commande (Connexion "Data") de l'anneau Neopixel relié à la broche 0
const int NOMBRE_NEOPIXEL = 24;           // nombre de LEDs Neopixels connectées, ici on utilise un anneau de 24 Leds : https://www.adafruit.com/product/2862
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NOMBRE_NEOPIXEL, NEOPIXEL, NEO_GRB + NEO_KHZ800); ///On déclare un objet strip qui représente l'anneau de Leds

//Afficheur 7 segments
const byte numDigits = 4;                      // Nombre de digits de l'afficheur 7 segments, ici il y en a 4 ( on utilise un afficheur référencé SH5461AS )
const byte digitPins[] = {13, 10, 9, A5};      // Branchement des broches contrôlant les chiffres, celles ou on mettra une résistance de 220 Ohm
const byte segmentPins[] = {12, 8, A3, A1, A0, 11, A4, A2}; // Branchements des broches controlant les segments
SevSeg sevseg;                                 // On déclare un objet sevseg qui représente l'afficheur 7 segments


//Variables relatives au temps
int selecteurTemps = 0;                   // Sélection du temps
unsigned long temps = 0;                  // Temps que l'on a sélectionné
unsigned long instantStart = 0;           // Instant ou l'on a appuyé sur START
long tempsEcoule = 0;                     // Temps écoulé depuis que l'on a appuyé sur START
char* tempsAffiche[] = {"  1", "  3", "  5", " 10", " 15", " 20", " 30", " 45", " 1h", "1h30", " 2h"}; //les différents temps affichés dans le menu, à faire correspondre avec les temps internes ci dessous
int tempsEnSecondes[] = {60, 180, 300, 600, 900, 1200, 1800, 2700, 3600, 5400, 7200}; // les différents temps proposés dans le menu ( variable interne en secondes, n'est pas affichée)


void setup() {
  // On déclare chaque Bouton comme étant une entrée
  pinMode(BOUTON_MOINS, INPUT);
  pinMode(BOUTON_PLUS, INPUT);
  pinMode(BOUTON_START, INPUT);

  // On initialise l'anneau de Leds
  strip.begin();
  strip.show();

  //On initialise l'afficheur 7 Segments
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(100);      //On règle l'intensité lumineuse de l'afficheur 7 Segments
}

void loop() {

  //on commence par lire l'etat des buttons et stocker les états dans les variables d'état
  etatBoutonMoins = digitalRead(BOUTON_MOINS);  
  etatBoutonPlus = digitalRead(BOUTON_PLUS);
  etatBoutonStart = digitalRead(BOUTON_START);
  
  // à ce stade on a trois possibilités : on peut appuyer sur le bouton PLUS, MOINS ou START, chaque possibilités est traitée en dessous


  //traitement bouton PLUS
  if (etatBoutonPlus == HIGH) {                           // Si on a appuyé sur PLUS (la variable d'état du bouton PLUS est à HIGH)
    if (selecteurTemps < (sizeof(tempsEnSecondes)) / 2 - 1) { // Et si le compteur n'est pas à son maximum (la taille du tableau)
      selecteurTemps = selecteurTemps + 1 ;                   // Alors on augmente la valeur du sélecteur
    }
    while (digitalRead(BOUTON_PLUS) == HIGH) {            // On bloque le programme en attendant que le bouton soit relaché, comme le programme tourne très vite, on incrémenterait la variable indéfiniment sinon
      sevseg.refreshDisplay();                            // On actualise l'afficheur 7 segments
    }
    temps = tempsEnSecondes[selecteurTemps];              // On met à jour la variable Temps en fonction de la nouvelle valeur du compteur
  }

  //traitement bouton MOINS
  if (etatBoutonMoins == HIGH) {                          // Si on a appuyé sur MOINS (la variable d'état du bouton MOINS est à HIGH)
    if (selecteurTemps > 0) {                             // Et si le compteur n'est pas à son minimum (zéro)
      selecteurTemps = selecteurTemps - 1 ;               // Alors on diminue la valeur du sélecteur
    }
    while (digitalRead(BOUTON_MOINS) == HIGH) {           // On bloque le programme en attendant que le bouton soit relaché, comme le programme tourne très vite, on décrémenterait la variable indéfiniment sinon
      sevseg.refreshDisplay();                            // On actualise l'afficheur 7 segments
    }
    temps = tempsEnSecondes[selecteurTemps];                           
  }

  //traitement bouton START
  if (etatBoutonStart == HIGH) {
    temps = tempsEnSecondes[selecteurTemps];                  // On met à jour la variable Temps en fonction de la nouvelle valeur du compteur
    instantStart = millis();                                  //On utilise la fonction millis qui renvoie le nombre de millisecondes depuis que le programme est lancé pour garder en mémoire l'instant ou l'on a appuyé sur start. Plus d'infos sur Millis ici : https://www.arduino.cc/en/Reference/Millis
    while (tempsEcoule != temps) {                            // Tant que le temps écoulé n'est pas celui que l'on a sélectionné
      tempsEcoule = 1 + (millis() - instantStart) / 1000;     //calcul du temps écoulé depuis l'appui sur start (+1 pour commencer chrono à 1)
      affichageLeds();                                        // On affiche les Leds correspondant au temps passé
      sevseg.setNumber(calcul_temps_renstant(tempsEcoule), 2);// On calcule le temps qu'il reste à décompter et on définit ce temps l'afficheur 7 Segments
      sevseg.refreshDisplay();                                // On rafraichit l'afficheur 7 Segments

      if (digitalRead(BOUTON_MOINS) == HIGH && digitalRead(BOUTON_PLUS) == HIGH) {  //Si on appuie sur les deux boutons en même temps
        break;                                               // La fonciton break nous permet de sortir de la boucle, c'est à dire d'arrêter de décompter le temps, plus d'infos sur break ici : https://www.arduino.cc/en/Reference/Break
      }

    }                                                        // Le temps écoulé est égal au temps sélectionné (La condition de la boucle while n'est plus vraie, donc on en sort)
    eteindreLeds();                                          // On remet la variable du temps écoulé à zéro
    tempsEcoule = 0;
  }
  sevseg.setChars(tempsAffiche[selecteurTemps]);
  sevseg.refreshDisplay(); // Must run repeatedly
}

// FONCTIONS ANNEXES //

void affichageLeds() {                           // Fonction permettant de piloter les Leds et leurs couleur en fonction du temps écoulé
  int nombre_led_a_allumer = (tempsEcoule * NOMBRE_NEOPIXEL) / temps; //il faut savoir combien de temps s'est écoulé en proportion du temps sélectionné pour savoir combien de led on doit afficher.
  int vert = 0;
  int rouge = 0;
  int bleu = 0;

  if ((tempsEcoule * 100 / temps) < 33) {       //si on est dans le premier tiers-temps
    vert = 255;                                 //Les Leds sont vertes
    rouge = 0;
    bleu = 0;
  }
  else if ((tempsEcoule * 100 / temps) < 66) {  //si on est dans le deuxième tiers-temps
    vert = 255;                                 //Les Leds sont oranges
    rouge = 220;
    bleu = 0;
  }
  else {                                        //sinon, c'est à dire qu'on est dans le troisième tiers-temps
    vert = 0;                                   //Les Leds sont rouges
    rouge = 255;
    bleu = 0;
  }
  for (int i = 0; i <= nombre_led_a_allumer; i++) { //On attribue la nouvelle couleur a chaque Led
    strip.setPixelColor(i, rouge, vert, bleu);
  }
  strip.show();                                 // On affiche les nouvelles couleurs
}

void eteindreLeds() {
  for (int i = 0; i <= NOMBRE_NEOPIXEL; i++) {     
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

int calcul_temps_renstant(int tempsActuel) {                  // Fonction permettant de transformer le temps en secondes en minutes et secondes.
  int tempsRestantEnMinutesEtSecondes=0;
  int minutesRestantes = (temps - tempsActuel) / 60;
  int secondesRestantes = (temps - tempsActuel) % 60; //ressource consultée pour le calcul https://www.arduino.cc/en/Reference/modulo
  return tempsRestantEnMinutesEtSecondes = minutesRestantes * 100 + secondesRestantes;
}





/* JOURNAL DES MODIFICATIONS

  MAJ David  01/09/17
  Passage du code tout en francais, commentaires et références.

  MAJ David  01/01/16
  J'ai remplacé l'écran par un afficheur 7 Segments

  MAJ Maÿlis 29/12/16
  Ajout du changement des couleurs selon le temps écoulé -> il faudrait améliorer le dégradé (qu'ils soit un peu moins "net"), qu'il soit plus progressif

  MAJ Flavien 22/12/16
  Modification de la fonction "calcul_temps". Utilisation de deux tableaux: l'un pour affecter le temps (en seconde), l'autre pour l'affichage dans le menu.
  -> Pour ajouter un temps au menu, ajouter son nombre de seconde dans le tableau "tempsEnSecondes" et le texte équivalent dans "tempsAffiche" à la même position.
  -> Affichage des temps dynamiquement directement dans le menu.
  Ajout d'une fonction "affichage_menu" pour afficher le menu dans son ensemble.
  Lors de l'appui sur "+", détection automatique (avec sizeof() ) de la taille du tableau "tempsEnSecondes" pour bloquer le compteur "selecteurTemps"
  ->Bug: la taille du tableau est doublé, seule solution pour le moment, diviser par deux (et le -1 car le compteur commence à 0).
  Le temps affiché est un décompte et fini par zéro.
  Lors du retour au menu, le temps par défaut est celui sélectionné auparavant.
  Ajout du texte "Temps restant:" lors du décompte du temps.
  Bugfix: Les LED ne clignotent plus, j'ai mis le "eteindreLeds()" en dehors de la boucle while.

  MAJ David 21/12/16
  intégration adafruit neopixel
  bugfix : jai changé long en unsigned long pour temps et timer, sinon ca buggait au dessus de 10s car la variable était pleine

  MAJ Yannick 16/12/16
  Temps affiché en minutes mais set en secondes pour tests
  Utiliser timer à la place de delay() cf: http://playground.arduino.cc/Code/AvoidDelay
  Pendant décompte si on appuie sur + et - cela reset
*/
