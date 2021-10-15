/*

Etäisyyden mittaaminen ultraäänianturilla HC-SR04.
 
 6.6.2019 -Jaakko Kaski-
 
 */

// Määritellään kytkentänavat ultraäänitutkalle HC-SR04
 const int GNDPin = 11; // ultraäänianturin maa-napa
 const int echoPin = 10; // Echo Pin (kaiku, eli vastaanotinpuoli)
 const int trigPin = 9; // Trigger Pin (ultraääni-lähtetin)
 const int VccPin = 8; // Anturin käyttöjännite

// Muuttujamäärittelyt. Huomaa, että desimaalierotin on piste!
float maximumRange = 100.0; // Maksimietäisyys (cm) jota tällä on tarkoitus mitata.
float minimumRange = 1.2; // Minimietäisyys (cm). Lyhyellä etäisyydellä sivusuuntainen kulkumatka tulee merkittäväksi
unsigned long duration = 0; // Lähetetyn uä-pulssin kulkuaika mikrosekunteina
float distance = 0.0; // Äänen kulkuaika kohteeseen ja takaisin; etäisyys (cm)
unsigned long aika = 0; // Aikaleima (ms), tyyppinä "pitkä, merkitön" muoto, koska INT-tyyppisenä numeroavaruus tulee n. puolessa minuutissa täyteen.
int SisaanTunniste = 0; // Käyttöjännitteen asettamiseen liittyvä tunniste. Reagoidaan kun mennään ohjelmaan
                        // eka kerran sisään.
bool yli=false; bool ali=false; // Muuttujat kuvastamaan ollaanko menty ylä- tai alaraja-arvon yli
bool onkoSuuntaYlospain;      //Auttaa määrittämään onko Arduino menossa ylös- vai alaspäin
float distanceMax = -1000000; //Alustus
float distanceMin = 1000000;  //Alustus
int toisto;       //Monta kertaa Arduino on käynyt sekä ylhäällä että alhaalla
float m = 0.058;  //Arduinon massa kilogrammoina
// Alustetaan kytkentänavat ja sarjayhteys
void setup() {
 Serial.begin (19200); // Tämä täytyy valita myös Serial Monitorista samaksi

 // Ultraäänianturin napojen määrittely:
 pinMode(GNDPin, OUTPUT); // Maadoitus; tämäkin on output-napa joka antaa 0V:n jännitteen
 pinMode(echoPin, INPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(VccPin, OUTPUT); // Käyttöjännite

// Asetetaan syöttöjännite (5V UNO-BOARDILLA) ja maa-arvot (0V):
    digitalWrite(VccPin, HIGH);
    delayMicroseconds(2); 
    digitalWrite(GNDPin, LOW); 
    delayMicroseconds(2); 
    
    while(Serial.available() != 0)
     {
        // Odotellaan että yhteys käynnistyy jos tässä sattuu olemaan viivettä. 0 tarkoittaa että yhteys on.
     } 
  Serial.print("Etaisyysmittaus alkaa, tulos cm-muodossa. Havainnointi serial plotterilla.");
}
float laskePotentiaaliEnergia(float korkeus, bool onkoSuuntaYlospain)
{
  float  potentiaaliEnergia;
  if (onkoSuuntaYlospain)
  {
    potentiaaliEnergia = m * 9.81 * korkeus * 0.01;
  }
  else
  {
    potentiaaliEnergia = m * 9.81 * korkeus * 0.6 * 0.01;
  }
  return potentiaaliEnergia;
}

void loop() {
  
// trigPin/echoPin kierros määrittää etäisyyden kohteeseen, josta lähtenyt äänipulssi heijastuu takaisin
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trigPin, LOW);

 duration = pulseIn(echoPin, HIGH); // Äänen kulkuaika mikrosekunteina saadaan kaiusta

 // Aikaleima (ms)
 aika = millis(); // Aikaleima luetaan heti kun etäisyys on mitattu ja juuri mennään lukemaan kiihtyvyydet

 //Lasketaan etäisyys (cm) perustuen äänen nopeuteen ilmassa. 
 //Voit selvittää fysiikasta mistä jakaja tulee... "etäisyysLaskelma.xlsx"
 distance =(duration * 0.016854 + 1.0595);

float distanceTulos;

if (distance > 30)
{
  yli = true;
    if (distance > distanceMax) 
    {
      distanceMax = distance;
    }
}
else if (distance < 10)
{
  ali = true;
  if (distance < distanceMin) 
    {
      distanceMin = distance;
    }
}
if (yli && ali)
{
  toisto++;
  distanceTulos = distanceMax - distanceMin;
  float potentiaaliEnergia = laskePotentiaaliEnergia(distanceTulos, onkoSuuntaYlospain);
  yli = ali = false;
  distanceMin = 10000000;
  distanceMax = -10000000;
   Serial.print(toisto);
   Serial.print(";");
   Serial.print(distanceTulos);
   Serial.print(";");
   Serial.println(potentiaaliEnergia);
}
else if (yli)
{
  //Serial.println("Suunta on alaspain");
  onkoSuuntaYlospain = false;  
}
else if (ali)
{
  //Serial.println("Suunta on ylospain");
  onkoSuuntaYlospain = true;  
}
/*
 Serial.print("EtaisyysMax on ");
 Serial.println(distanceMax);
 Serial.print("EtaisyysMin on: ");
 Serial.println(distanceMin);
*/
 delay(100);
}
