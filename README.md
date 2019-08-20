# Arrosage-v2
 Arrosage automatique

Arrosage calibré asservi à la mesure d'humidité du sol
Création Maurice Churlet - Usage libre - 18/08/2019 - Retour d'expérience souhaité ici.
  Arduino, 1 interrupteur, 1 potentiomètre 10 k, trois led avec resistances 220 homs
  Deux switch pour la raz du compteur d'arrosage et le reset
  Deux sondes (humidité et réserve vide)
  1 pompe 12v/3A  avec relai 5 et un bac d'eau.
  un panneau solaire 12v + régulateur + batterie
  Alimentation 12 v du circuit et de la pompe par la batterie
  Sonde dans la terre fabriquées avec des équerres métal jaune soudées sur fil.
    Placer les sondes dans la terre espacée de 20 cm environ.
  L'interrupeur reset permet de faire un reset du calibrage
  Le potentiomètre permet de régler le temps d'arrosage de 0 à 20s
  Calibrer lorsque l'humidité de la terre est correcte entre la sonde.
  Le calibrage se fait sur 1 heure

  Lorsque le montage est calibrée, la led d'humidité clignote de 0 à 6 fois fois chaque seconde en fonction de l'humidité.
  La led de pompe on clignote autant de fois qu'il y a eu d'arrosages depuis le dernier calibrage.
  Lors de l’arrosage, la pompe est actionnée de 0 à 20s toute les 15 mn jusqu'à ce que l'humidité soit correcte.
