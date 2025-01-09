

#include <Arduino.h>   
#include <SPI.h>    
#include <SD.h> 
#include "RTClib.h"    
#include <Wire.h>  
#include <stdio.h> 
#include <stdlib.h> 
#include <SoftwareSerial.h> 
#include <ChainableLED.h>  
#include <Adafruit_Sensor.h>   
#include <Adafruit_BME280.h>   
#include <BME280I2C.h>  


RTC_DS3231 rtc; 
ChainableLED leds(5, 6, 1); 
File dataFile;  
BME280I2C bme;  
#define PUSHPIN_V 2
#define PUSHPIN_R 3



typedef struct variables{
    unsigned long log_interval = 100;   // Intervalle entre les mesures en millisecondes (10 minutes)
    unsigned short file_max_size = 2000;    // Taille maximale du fichier en octets (2 ko)
    unsigned short timeout = 10;    
    int currentRevision = 0;    
    volatile unsigned short start_time = 0;
    bool lumin = 1; 
bool temp_air = 1; 
    bool hygr = 1;  
    bool pressure = 1; 
    int pressure_min = 850; 
int pressure_max = 1080;

    int hygr_min = 0;
int hygr_max = 50;
    int min_temp_air = -10;
int max_temp_air = 60;
    int lumin_low = 255;
int lumin_high = 768;
}variables;

variables utile; 

typedef struct {
  String date;   
  String humidity;
String pressure;
String temp;   
String light;   
} capteurs;

capteurs values; 


void Rtc();
void light();
void initialisation_interruption();
void RedButton();
void GreenButton();
void M_standard();
void M_config();
void M_eco();
void M_maintenance();
String generateFileName(int revision);
int findCurrentRevision();
void verif();
void config();
void temperature();
void humidity();
void pression();


void setup(){
  Serial.begin(9600);  
  Wire.begin(); 
  verif(); 
  pinMode(PUSHPIN_R, INPUT); 
  pinMode(PUSHPIN_V, INPUT); 
  initialisation_interruption();    
  utile.currentRevision = findCurrentRevision();   
  if (digitalRead(PUSHPIN_R) == LOW or Mode == 1) {
  M_config();}
  else {M_standard();}}


void loop(){
  verif();  
  switch (Mode) {
      case 4:  
        Rtc();   
        light();  
        temperature();   
        humidity();   
        pression();   
        Serial.print(values.date);
        Serial.println(values.light);
... (459lignes restantes)
Réduire
final.ino
18 Ko
﻿
Samofff
samofff
 

#include <Arduino.h>   
#include <SPI.h>    
#include <SD.h> 
#include "RTClib.h"    
#include <Wire.h>  
#include <stdio.h> 
#include <stdlib.h> 
#include <SoftwareSerial.h> 
#include <ChainableLED.h>  
#include <Adafruit_Sensor.h>   
#include <Adafruit_BME280.h>   
#include <BME280I2C.h>  


RTC_DS3231 rtc; 
ChainableLED leds(5, 6, 1); 
File dataFile;  
BME280I2C bme;  
#define PUSHPIN_V 2
#define PUSHPIN_R 3



typedef struct variables{
    unsigned long log_interval = 100;   // Intervalle entre les mesures en millisecondes (10 minutes)
    unsigned short file_max_size = 2000;    // Taille maximale du fichier en octets (2 ko)
    unsigned short timeout = 10;    
    int currentRevision = 0;    
    volatile unsigned short start_time = 0;
    bool lumin = 1; 
	bool temp_air = 1; 
    bool hygr = 1;  
    bool pressure = 1; 
    int pressure_min = 850; 
	int pressure_max = 1080;

    int hygr_min = 0;
	int hygr_max = 50;
    int min_temp_air = -10;
	int max_temp_air = 60;
    int lumin_low = 255;
	int lumin_high = 768;
}variables;

variables utile; 

typedef struct {
  float date;   
  float humidity;
	float pressure;
	float temp;   
	float light;   
} capteurs;

capteurs values; 


void Rtc();
void light();
void initialisation_interruption();
void RedButton();
void GreenButton();
void M_standard();
void M_config();
void M_eco();
void M_maintenance();
String generateFileName(int revision);
int findCurrentRevision();
void verif();
void config();
void temperature();
void humidity();
void pression();


void setup(){
  Serial.begin(9600);  
  Wire.begin(); 
  verif(); 
  pinMode(PUSHPIN_R, INPUT); 
  pinMode(PUSHPIN_V, INPUT); 
  initialisation_interruption();    
  utile.currentRevision = findCurrentRevision();   
  if (digitalRead(PUSHPIN_R) == LOW or Mode == 1) {
  M_config();}
  else {M_standard();}}


void loop(){
  verif();  
  switch (Mode) {
      case 4:  
        Rtc();   
        light();  
        temperature();   
        humidity();   
        pression();   
        Serial.print(values.date);
        Serial.println(values.light);
        Serial.print(values.date);
        Serial.print(values.light);
        Serial.print(values.temp);
        Serial.print(values.humidity);
        Serial.print(values.pressure);
        Serial.println(F(" $GPGLL,4328.88815,N,00523.19621,E,122810.96,A,A*66"));
        break;

    case 3:   // Mode économique
        utile.log_interval = utile.log_interval*2;    //augmentation de l'intervalle entre mesures
        Rtc();
        light();
        temperature();
        humidity();
        pression();
        utile.log_interval = utile.log_interval / 2;  //diminution de l'intervalle entre mesures
        break;
	
    case 2: // Mode maintenance depuis le mode standard
        Rtc();
        light();
        temperature();
        humidity();
        pression();
        Serial.print(values.date);
        Serial.print(values.light);
        Serial.print(values.temp);
        Serial.print(values.humidity);
        Serial.print(values.pressure);
        Serial.println(F(" $GPGLL,4328.88815,N,00523.19621,E,122810.96,A,A*66"));
        break;
	
    case 1: // Mode configuration
        config();
        break;

    case 0: // Mode standard
        Rtc();
        light();
        temperature(); 
        humidity();
        pression();
		break;}

// Si l'on est en mode standard ou économique
if (Mode == 0 || Mode == 3) {
    dataFile = SD.open(generateFileName(utile.currentRevision), FILE_WRITE);
    
    if (dataFile) {
        if (dataFile.size() > utile.file_max_size) {
            dataFile.close(); 
            utile.currentRevision = utile.currentRevision + 1; 
            dataFile = SD.open(generateFileName(utile.currentRevision), FILE_WRITE); 
            
            Serial.print(F("Creation d'un nouveau fichier : "));
            Serial.println(generateFileName(utile.currentRevision));
        }

        if (dataFile) {
            dataFile.print(values.date);
            dataFile.print(values.light);
            dataFile.print(values.temp);
            dataFile.print(values.humidity);
            dataFile.print(values.pressure);
            dataFile.println(" $GPGLL,4328.88815,N,00523.19621,E,122810.96,A,A*66");
            dataFile.close(); 
        } else {
           
            Serial.println(F("Erreur : Impossible d'ouvrir le fichier pour écrire les données."));
            while (1); 
        }
    } else {
        Serial.println(F("Erreur : Impossible d'ouvrir le fichier initial."));
    }

    
    delay(utile.log_interval);
} }


// vérifier l'accès aux capteurs
void verif(){
    byte conteur = 0;
    while(!bme.begin()){    
        if (conteur >= utile.timeout) { 
            Serial.println(F("Échec"));
            while (1);}
        Serial.println(F("Erreur d'acces aux capteurs"));
        leds.setColorRGB(0, 255, 0, 0);
        delay(400);
        leds.setColorRGB(0, 0, 255, 0);
        delay(400);
        conteur++;}

    conteur = 0;
    while (!rtc.begin()) { 
        if (conteur >= utile.timeout) {
            Serial.println(F("Échec"));
            while (1);}
        Serial.println(F("Erreur d'acces a l'horloge"));
        leds.setColorRGB(0, 255, 0, 0);
        delay(400);
        leds.setColorRGB(0, 0, 0, 255);
        delay(400);
        conteur++;} 

    conteur = 0;
    while (!SD.begin(4)) { 
        if (conteur >= utile.timeout) {
            Serial.println(F("Échec"));
            while (1);}
        Serial.println(F("erreur d'acces a la SD"));
        leds.setColorRGB(0, 255, 0, 0);
        delay(300);
        leds.setColorRGB(0, 255, 255, 255);
        delay(600);
        conteur++;}

    switch (Mode) {
        case 4: 
            leds.setColorRGB(0, 255, 70, 0); 
            break;

        case 3: 
            leds.setColorRGB(0, 0, 0, 255); 
	        break;

        case 2: 
            leds.setColorRGB(0, 255, 70, 0);
	        break;

        case 1:
            leds.setColorRGB(0, 255, 255, 0); 
            break;

        case 0: 
            leds.setColorRGB(0, 0, 255, 0);
	        break;}}


// Mode configuration
void config(){
    unsigned long time = millis();  
    variables base;

    Serial.println(F("Que souhaitez vous modifier :"));
    Serial.println(F("1-Intervalle des prises"));
    Serial.println(F("2-Taille des fichiers"));
    Serial.println(F("3-timeout"));
    Serial.println(F("4-reset"));
    Serial.println(F("5-autres paramètres"));

    
    while (!Serial.available() && millis() - time < 30000) {
        delay(500);}
    if (millis()-time > 30000){
        Serial.println(F("30 secondes d'inactivité, retour au mode standard"));
        Mode = 0;}  
    

    String read = "";
    String read1 = "";
    short resultat;
    short commande;
    read1 = Serial.readString();
    commande = read1.toInt();
    switch (commande){
        case 1:
            Serial.println(F("Entrer la valeur du nouvelle intervalle en sec"));
            while (!Serial.available()) delay(500);
		        read = Serial.readString();
                resultat = read.toInt();
                resultat = resultat *1000;
                utile.log_interval = resultat;
                Serial.print(F("nouvelle intervalle : "));
                Serial.println(utile.log_interval);
                break;

        case 2:
            Serial.println(F("Entrer la valeur de la taille du fichier en ko"));
            while (!Serial.available()) delay(500);
                read = Serial.readString();
                resultat = read.toInt();
                resultat = resultat *1000;
                utile.file_max_size = resultat;
                Serial.print(F("nouvelle taille : "));
                Serial.println(utile.file_max_size);
                break;

        case 3 :
            Serial.println(F("Entrer la valeur du time out en sec"));
            while (!Serial.available()) delay(500);
                read = Serial.readString();
                resultat = read.toInt();
                resultat = resultat *1024;
                utile.timeout = resultat;
                Serial.print(F("nouveau timeout : "));
                Serial.println(utile.timeout);
                break;

        case 4:
            Serial.println(F("Reinitialisation des valeurs par défaut"));
            utile.log_interval = base.log_interval;
			utile.file_max_size = base.file_max_size;
			utile.timeout = base.timeout;
			utile.lumin = base.lumin;
			utile.lumin_low = base.lumin_low;
			utile.lumin_high = base.lumin_high;
			utile.temp_air = base.temp_air;
			utile.min_temp_air = base.min_temp_air;
			utile.max_temp_air = base.max_temp_air;
			utile.hygr = base.hygr;
			utile.hygr_min = base.hygr_min;
			utile.hygr_max = base.hygr_max;
			utile.pressure = base.pressure;
			utile.pressure_min = base.pressure_min;
			utile.pressure_max = base.pressure_max;
			Serial.println(F("Default values restored"));
            break;

        case 5:
            Serial.println(F("LUMIN. Choices : \n1) True\n2) False"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			if (read == "1") utile.lumin=1;
			else if (read == "2") utile.lumin = 0;
            Serial.println(utile.lumin);
            Serial.println(F("LUMIN_LOW (D=255)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.lumin_low = read.toInt();
            Serial.println(utile.lumin_low);
            Serial.println(F("LUMIN_HIGH (D=665)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.lumin_high = read.toInt();
            Serial.println(utile.lumin_high);
            Serial.println(F("TEMP_AIR . Choices : \n1) True\n2) False"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			if(read == "1") utile.temp_air = 1;
			else if (read == "2") utile.temp_air = 0;
            Serial.println(utile.temp_air);
            Serial.println(F("MIN_TEMP_AIR (D=-10)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.min_temp_air = read.toInt();
            Serial.println(utile.min_temp_air);
            Serial.println(F("MAX_TEMP_AIR (D=60)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.max_temp_air = read.toInt();
            Serial.println(utile.max_temp_air);
            Serial.println(F("HYGR. Choices : \n1) True\n2) False"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			if(read == "1") utile.hygr = 1;
			else if (read == "2") utile.hygr = 0;
            Serial.println(utile.hygr);
            Serial.println(F("HYGR_MINT (D=0)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.hygr_min = read.toInt();
            Serial.println(utile.hygr_min);
            Serial.println(F("HYGR_MAXT (D=50)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.hygr_max = read.toInt();
            Serial.println(utile.hygr_max);
            Serial.println(F("PRESSURE: \n1) True\n2) False"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			if(read == "1") utile.pressure = 1;
			else if (read == "2") utile.pressure = 0;
            Serial.println(utile.pressure);
            Serial.println(F("PRESSURE_MIN (D=850)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.pressure_min = read.toInt();
            Serial.println(utile.pressure_min);
            Serial.println(F("PRESSURE_MAX (D=1080)"));
			while (!Serial.available()) delay(500);
			read = Serial.readString();
			utile.pressure_max = read.toInt();
            Serial.println(utile.pressure_max);
            break;}
    delay(5000);}

//  standard
void M_standard() {
    Mode = 0;
    Serial.println(F("Mode standard"));
    leds.setColorRGB(0, 0, 255, 0);}

// mode économique
void M_eco(){
    Mode = 3;
    Serial.println(F("Mode eco"));
    leds.setColorRGB(0, 0, 0, 255);}

// mode maintenance
void M_maintenance(){
    Serial.println(F("Mode maintenance"));
    leds.setColorRGB(0, 255, 60, 0);}

// mode configuration
void M_config(){
    Mode =1;
    Serial.println(F("Mode configuration"));
    leds.setColorRGB(0, 255, 255, 0); }

// Fonction de l'horloge
void Rtc(){
    DateTime now = rtc.now();   
    now = now + TimeSpan(8, 3, 49, -30);    
    String timestamp = "";
    timestamp += now.day();
    timestamp += "/";
    timestamp += now.month();
    timestamp += "/";
    timestamp += now.year();
    timestamp += "  ";
    timestamp += now.hour();
    timestamp += ":";
    timestamp += now.minute();
    timestamp += ":";
    timestamp += now.second();
    timestamp += " ; ";
    values.date = timestamp;}

// capteur de luminosité
void light(){
    if (utile.lumin){
        int sensor = analogRead(A2);    
        if (sensor >= 290 && sensor <= 310) {   
            values.light = "light= NA  ; ";}
        else { 
            values.light = "light:" + String(sensor);}
        if (sensor<utile.lumin_low){    
            values.light += "L ; ";}
        else if (sensor>utile.lumin_high){  
            values.light += "H ; ";}
        else {values.light += " ; ";}}}

// capteur de température
void temperature(){
    if (utile.temp_air){
        float temps(NAN), hum(NAN), pres(NAN);
        // Déclaration des unités de mesures
        BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
        BME280::PresUnit presUnit(BME280::PresUnit_Pa);
        bme.read(pres, temps, hum, tempUnit, presUnit);
        values.temp = "temp:" + String(temps) + "°C";
        if (temps<utile.min_temp_air){
            values.temp += "L ; ";}
        else if (temps>utile.max_temp_air){
            values.temp += "H ; ";}
        else {
            values.temp += " ; ";}}}

// capteur d'humidité
void humidity(){
    if (utile.hygr){
        float temps(NAN), hum(NAN), pres(NAN);
        bme.read(pres, temps, hum);
        values.humidity = "hum:" + String(hum) + "%";
        if (hum<utile.hygr_min){
            values.humidity += "L ; ";}
        else if (temps>utile.hygr_max){
            values.humidity += "H ; ";}
        else {
            values.humidity += " ; ";}}}

// capteur de pression
void pression(){
    if (utile.pressure){
        float temps(NAN), hum(NAN), pres(NAN);
        bme.read(pres, temps, hum);
        values.pressure = "pres:" + String(pres) + "PA";
        if (pres<utile.pressure_min){
            values.pressure += "L ; ";}
        else if (pres>utile.pressure_max){
            values.pressure += "H ; ";}
        else {
            values.pressure += " ; ";}}}

// nom de fichier
String generateFileName(int revision){
    DateTime now = rtc.now(); 
    String filename;
    int ans = now.year() % 100;
    int mois = now.month() +1;
    int jour = now.day() -23;
    filename += String(ans) + String(mois) + String(jour) + "_" + String(revision, DEC) + ".txt";
    return filename;}

// numéro de révision
int findCurrentRevision() {
    int maxRevision = 0;
    while (SD.exists(generateFileName(maxRevision))) {
        maxRevision++;} 
    if (maxRevision>0 && (SD.open(generateFileName(maxRevision-1), FILE_WRITE)).size() < utile.file_max_size){
        maxRevision--;} 
    return maxRevision;}

// interruptions
void initialisation_interruption() {
    attachInterrupt(digitalPinToInterrupt(PUSHPIN_R), RedButton, CHANGE); 
    attachInterrupt(digitalPinToInterrupt(PUSHPIN_V), GreenButton, CHANGE);}

//bouton rouge
void RedButton(){
    if (utile.start_time == 0){ 
        utile.start_time = millis();}   
    else if (10 < millis() - utile.start_time){ 
        Serial.print(F("Red Button pressed :"));
        Serial.print(millis() - utile.start_time);
        Serial.println(F("ms"));
        if ( millis() - utile.start_time >= 5000){  
            utile.start_time = 0;
            switch(Mode){
                case 0:
                    Mode = 2;
                    M_maintenance();
                    break;
          
                case 2:
                    M_standard();
                    break;
          
                case 4:
                    M_eco();
                    break;

                case 3:
                    Mode = 4;
                    M_maintenance();
                    break;}}
        utile.start_time = 0;}}

//bouton vert
void GreenButton(){
    if (utile.start_time == 0){
        utile.start_time = millis();}
    else if (10 < millis() - utile.start_time){
        Serial.print(F("Green Button pressed :"));
        Serial.print(millis() - utile.start_time);
        Serial.println(F("ms"));
        if ( millis() - utile.start_time >= 5000){
        utile.start_time = 0;
        switch(Mode){
            case 0:
                M_eco();
                break;
        
            case 3:
                M_standard();
                break;}}
        utile.start_time = 0;}}
