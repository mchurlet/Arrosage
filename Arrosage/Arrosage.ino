////****************************************************
// Arrosage calibré asservi à la mesure d'humidité du sol
//****************************************************
/* Création Maurice Churlet - Usage libre - 18 08 2019
  Arduino, 1 interrupteur, 1 potentiomètre 10 k, trois led avec resistances 220 homs
  Un switch pour la raz du compteur d'arrosage
  Deux sondes (humidité et réserve vide)
  1 pompe 12v/3A  avec relai 5 et un bac d'eau.
  un panneau solaire 12v + régulateur + batterie
  Alimentation 12 v du circuit et de la pompe par la batterie
  Sonde dans la terre fabriquées avec des équerres métal jaune soudées sur fil.
  Intérupteur
  Placer les sondes dans la terre espacée de 20 cm eniron.

  L'interrupeur permet de faire un reset du calibrage
  Le potentiomètre permet de régler le temps d'arrosage de 0 à 20s
  Calibrer lorsque l'humidité de la terre est correcte entre la sonde.
  Le calibrage se fait sur 1 heure

  Lorque le montage est calibrée, la led d'hmidité clignotte de 0 à 6 fois fois chaque seconde en fonction de l'humidité.
  La led de pompe on clignote autant de fois qu'il y a eu d'arrosage depuis le dernier calibrage.
  Lors de l'arosage, la pompe est actionnée de 0 à 20s toute les 15 mn jusqu'à ce que l'humidité soit correcte.
*/

// Pins
int PinIA_Humidite = A0;     //Broche Analogique de mesure d'humidité
int PinIA_Pot = A1; //Potentiomètre

int PinIN_Test = 6; // Broche de commande mode test interrupteur
int PinON_Pompe = 7;  //Commande du relai de pompe
int PinON_Humidite = 8;  // Diode humidité
int PinIN_razA = 12;      //RAZ Arrosage
int PinON_DiodePompe = 9; // Diode Pompe
int PinON_Vide = 10;  // Diode réserve vide
int PinIN_Vide = 11;  // Signal capteur plus d'eau

// Variables

/* Pour mise au point
int calt = 3; //Temps en s avant calibrage (mettre à 15 pour test)
int attente = 1
int ini = 1;// temps avant initialisation complète
*/

int calt = 3600; //Temps en s avant calibrage (mettre à 15 pour test)
int attente = 15 * 60;//15 * 60;Faire varier pour mise au point
int ini = 10;// temps avant initialisation complète

int tempo = 0; // Règle le tempo d'allimentation de la pompe
int tempoLoop; // pour boucle de temporisation
int tTest = 1;
int hsol;  //Humidite su sol, mesure analogique
int hRef = 0; //Référence humidité cible
int hsolCal; //Humidité calibrée
int tArrosage = 1; //Réglage potentiomètre
int secheresse; //déclenchement arrosage
bool memNiveau = false; //
int mem_hsol = hsol; //Pour test changement de valeur et affichage
int maxhsol = 0;
int minhsol = 1024;
int nbArrosage = 0;
int ctrAffichage = 0;
int ctrCalibrage = 0;
int coef = 1; //Coéficient de précision

const int clignotement = 250;

// RAZ
void reset() {
  maxhsol = 0;
  minhsol = 1024;
  nbArrosage = 0;// reset nombre d'arrosages
  hRef = 0;// reset du calibrage.
  ctrCalibrage = 0;
  Serial.println("================================");
  Serial.println("Reset");
}

void calibrage() {
  hRef = (maxhsol + minhsol) / 2;
  coef =  (maxhsol - minhsol) / 6;
  if (coef == 0){
    coef = 1;
  }
  Serial.println("================================");
  Serial.println("Calibrage");
}


int ledNiveau() { // Fais clignoter la led de niveau d'humidité et calcul du niveau d'humidité
  int n = 3 - hsolCal;
  if (n > 7) {
    n = 7;
  }

  if (hRef != 0) {
    //Affichage du niveau d'humidité.
    for (int i = 0; i <  n ; i++) {
      digitalWrite(PinON_Humidite, HIGH); // Led
      delay(clignotement);
      digitalWrite(PinON_Humidite, LOW); // Led
      delay(clignotement);
    }

    //Affichage du nombre d'arrosages
    if  (digitalRead(PinIN_Vide) == LOW) {
      for (int i = 0; i <  nbArrosage ; i++) {
        digitalWrite(PinON_Vide, LOW); // Led
        delay(clignotement);
        digitalWrite(PinON_Vide, HIGH); // Led
        delay(clignotement);
      }
    }
  }
  return n;
}

void mesure() {

  // RAZ
  if  (digitalRead(PinIN_Test) == LOW) {
    reset();
  }

  //Reset nb arrosages
  if  (digitalRead(PinIN_razA) == LOW) {
    nbArrosage = 0;
  }
  
  tArrosage = (analogRead(PinIA_Pot)) / 50;

  //Clignotement rapide en attente de calibrage
  if  (hRef == 0) {
    digitalWrite(PinON_DiodePompe, LOW);
    delay(clignotement / 3);
    digitalWrite(PinON_DiodePompe, HIGH); 
    digitalWrite(PinON_Vide, LOW); // Led
    delay(clignotement / 3);
    digitalWrite(PinON_Vide, HIGH);
    digitalWrite(PinON_Humidite, LOW);
    delay(clignotement / 3);
    digitalWrite(PinON_Humidite, HIGH);
  }

  // Relevé d'humidité
  hsol = analogRead(PinIA_Humidite); // Lit la tension analogique

  // Pour calibrage
  if (ctrCalibrage > ini) {
    if (ctrCalibrage == ini+1) {
      Serial.println("================================");
      Serial.println("Intitialisation terminée");
      Serial.println("Calibrage en préparation...");
    }
    if (hsol < 700 && maxhsol < hsol) {
      maxhsol = hsol;
    }
    if (minhsol > hsol) {
      minhsol = hsol;
    }
  }

  if (hsol > 800) {// Sonde débranchée
    reset();
    ctrCalibrage = 0;
  }

  if (ctrCalibrage == calt && hRef == 0 ) {
    calibrage();
  }

  hsol = hsol ;// Humidité calculée

  hsolCal = (hsol - hRef) / coef; //Calcul de l'humidité
  secheresse = ledNiveau();

  if (secheresse < 0 && hRef != 0 ) {// allumage de la diode déclenchement d'arrosage
    digitalWrite(PinON_DiodePompe, LOW);
  } else {
    digitalWrite(PinON_DiodePompe, HIGH);
  }

  // Affichage si la mesure ou pot change
  if (hsol != mem_hsol || ctrAffichage++ == 10) {
    ctrAffichage = 0;
    mem_hsol = hsol;
    Serial.println("================================");
    Serial.print("Humidité : ");
    Serial.print(hsol); // afficher la mesure
    Serial.print(" (Ref: ");
    Serial.print(hRef);
    Serial.println(")");
    Serial.print(minhsol);
    Serial.print(" : ");
    Serial.println(maxhsol);
    Serial.print("Humidité calibré : ");
    Serial.println(hsolCal); // afficher la mesure calibrée
    Serial.print ("Potentiomètre : ");
    Serial.println(analogRead(PinIA_Pot));
    Serial.print ("temps d'arrosage : ");
    Serial.println(tArrosage);
    Serial.print("Coéf : ");
    Serial.println(coef);
    Serial.print("Secheresse : ");
    Serial.println(secheresse);
    Serial.print("Arrosages : ");
    Serial.println(nbArrosage);
    Serial.print("Boucle : ");
    Serial.println(tempoLoop);
    Serial.print("Avant calibrage : ");
    Serial.println(calt - ctrCalibrage);
    Serial.println();
  }
}

//=======================================================================
void setup() { // Initialisation
  Serial.begin(9600);  //Connection série à 9600 baud
  pinMode(PinIA_Humidite, INPUT_PULLUP);       //pin A0 en entrée analogique
  pinMode(PinIA_Pot, INPUT);       //pin A1

  pinMode(PinIN_Vide, INPUT_PULLUP);
  pinMode(PinIN_razA, INPUT_PULLUP);
  pinMode(PinIN_Test, INPUT_PULLUP);  //pin de réglage de mode Test
  
  pinMode(PinON_Vide, OUTPUT);
  pinMode(PinON_Pompe, OUTPUT);   //Commande pompe
  pinMode(PinON_Humidite, OUTPUT); // Led humidité capteu 
  pinMode(PinON_DiodePompe, OUTPUT); 
}

void loop() { //boucle principale
  
  mesure();// mesure de l'humidité avec calculs et affichages

  //Incrément du compteur de calibrage
  if (ctrCalibrage < calt) {
    ctrCalibrage++;
  }

  //Réserve d'eau vide !
  if  (digitalRead(PinIN_Vide) == HIGH && hRef != 0) {
    Serial.println("Reserve d'eau vide");
    digitalWrite(PinON_Vide, LOW);
  } else {
    digitalWrite(PinON_Vide, HIGH);
  }

  // Arrosage
  if (secheresse < 0 && hRef != 0)
  {
    // Reglage d'un tempo normal pour arrosage et rapide pour les tests
   if  (digitalRead(PinIN_Test) == HIGH) {
      if (tempo != tArrosage) {
        tempo = tArrosage;
        Serial.println("Tempo sur " + String(tArrosage) + "s");
      }
    }
  else {
    if (tempo != tTest) {
      tempo = tTest;
      Serial.println("Tempo sur " + String(tTest) + "s");
    }
  }
    
    if  (digitalRead(PinIN_Test) == HIGH && digitalRead(PinIN_Vide) == LOW) {
      Serial.println("Arrosage On");
      nbArrosage++;//Incrémentation du nombre d'arrosages
      digitalWrite(PinON_Pompe, LOW);   // Pompe actionnée en mode réel
    }

    tempoLoop = tempo;// Marche ou arrêt de la pompe
    while (tempoLoop > 0) {
      if  (digitalRead(PinIN_Test) == LOW|| digitalRead(PinIN_Vide) == HIGH || secheresse >= 0 ) {
        // Sortie si passage en mode test ou humidité ok
        tempoLoop = 0;
      } else {
        delay(1000);
        mesure();
        tempoLoop--;
      }
    }

    if  (digitalRead(PinIN_Test) == HIGH){
      Serial.println("Arrosage Off");
    }

    digitalWrite(PinON_Pompe, HIGH);   // Pompe Arrêt
    tempoLoop = attente;
    while (tempoLoop > 0) {
      if  (digitalRead(PinIN_Test) == LOW || digitalRead(PinIN_Vide) == HIGH ) {
        tempoLoop = 0;
      } else {
        delay(1000);
        mesure();
        tempoLoop--;
      }
    }
  }

  delay(1000); //delai entre 2 mesures 1s
}
