int velocity = 100;//velocity of MIDI notes, must be between 0 and 127
//higher velocity usually makes MIDI instruments louder

int noteON = 145;//144 = 10010000 in binary, note on command
int noteOFF = 129;//128 = 10000000 in binary, note off command

const int notesDansPattern = 16;
int pattern[notesDansPattern];
int noteDepart = 0x1E; // F#-0 (0x1E)
int noteFin = 0x5A; // F#-5 (0x5A)
int pentatonicMinorScales [5] = {0, 3, 5, 7, 10};


int beatResult[notesDansPattern*2]; // Beat Interpolated Result ( beat starting and ending point are in same array )

#include <TimedAction.h>

#include "KIASM_XSeq_Beat.h"
#include "KIASM_XSeq_RythmInterpolate.h"


int timeUnitOn = 50; //
int timeUnitOff = 50; // divideTime
int timeUnit = timeUnitOn + timeUnitOff; // 100ms <> 120/5 = 100/5 + 20/5  24 // 500ms <> 120bpm
int sequenceIndex = 0;

int delaySequence = 0;
int delaySequenceMax = 10;

unsigned long time;

boolean isPlaying = false;
boolean releaseNote = false;

// on détermine le nombre maximum de notes en faisant les divisions minimales du rythme
// 1 2 4 8 16 32 64 128

//int divideTime = 128;

// on a plusieurs modes :
// le mode linéaire crée des notes sur TOUTE la sequence
// interpolation
// on crée un calque de rythme
// ex : pour 16 temps, on a divisé la séquence en 4 notes
// on a une valeur de 0 à 1 qui correspond à la morphologie du rythme : 0 = rythme pas du tout pris en compte / 1 = rythme totalement pris en compte
// ex rythme
//   |1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16
int divisionTemps = 64;
int divideTime = 4;
int nbElements = divisionTemps / divideTime;


//Beat beats[nbElements];
Beat* beats = NULL;

boolean debug = false;

void playNoteInSequence() {

/*
  int note = pattern[sequenceIndex];

  if (delaySequence == 0 && !releaseNote) {
    MIDImessage(noteON, note, velocity);//turn note on
    releaseNote = true;
  }

  if (delaySequence > delaySequenceMax) {

    MIDImessage(noteOFF, note, velocity);//turn note off

    sequenceIndex++;
    sequenceIndex %= notesDansPattern;

    delaySequence = 0;
    releaseNote = false;
  }
*/
}

void releaseNoteInSequence() {
/*
  int note = pattern[sequenceIndex];
  MIDImessage(noteOFF, note, velocity);//turn note off
 */ 
}

int momentumIndex = 0;
int momentumSequenceIndex = 0;

void sequenceMomentum(){

  
  //if(millis() < 10000 && debug)Serial.println("");
  //if(millis() < 10000 && debug)Serial.print("sequenceMomentum ");
  //if(millis() < 10000 && debug)Serial.print(momentumIndex % divisionTemps);


  for(int i = 0; i < nbElements; i++){

    if(beatResult[i*2 + 1] == momentumIndex % divisionTemps){

      int note = pattern[momentumSequenceIndex % nbElements];
      // ON STOPPE LA NOTE
      MIDImessage(noteOFF, note, velocity);//turn note off
      momentumSequenceIndex++;

      //if(millis() < 10000 && debug)Serial.println("");
      //if(millis() < 10000 && debug)Serial.print("Release ");
      //if(millis() < 10000 && debug)Serial.print(note);
    }

    if(beatResult[i*2] == momentumIndex % divisionTemps){

      // ON JOUE LA NOTE
      int note = pattern[momentumSequenceIndex % nbElements];
      MIDImessage(noteON, note, velocity);//turn note on

      //if(millis() < 10000 && debug)Serial.println("");
      //if(millis() < 10000 && debug)Serial.print("Play ");
      //if(millis() < 10000 && debug)Serial.print(note);

      //momentumSequenceIndex++;
      
    }
    
  }

  momentumIndex++;
}

TimedAction playSequenceAction = TimedAction(timeUnit, playNoteInSequence);
TimedAction releaseSequenceAction = TimedAction(timeUnit, releaseNoteInSequence);

TimedAction momentum = TimedAction(timeUnit, sequenceMomentum);


// Get control Input, potars etc
void getControlInput() {
  Serial.println(digitalRead(3));
}

TimedAction getControlInputAction = TimedAction(20, getControlInput);



void setup() {
  //  Set MIDI baud rate:
  Serial.begin(9600);

  pinMode(13,OUTPUT);

  randomizePattern(noteDepart, noteFin, notesDansPattern); // from F#-0 (0x1E) to F#-5 (0x5A)

  beats = malloc(sizeof(Beat) * nbElements);

  momentum = TimedAction(timeUnit, sequenceMomentum);
  
  randomSeed(6);


  

  int* notes;


  calculRythme( beats, divisionTemps, nbElements, notes);
  

  Serial.println("-----**");
  Serial.println("Beats ");
  
  for (int i = 0 ; i < nbElements ; i++){
    Serial.println(beats[i].start);

  }
  

  interpolationRythme(beats, nbElements, divideTime,0);


  Serial.println("----------");
  Serial.println("Beats after interpolate");
  Serial.println(beatResult[0]);
  Serial.println(beatResult[1]);

  
  
/*
  Serial.println("done");

  Serial.println(beats[0].start);

  Serial.println(beats[1].start);
  Serial.println(beats[2].start);
  Serial.println(beats[3].start);
  Serial.println(beats[4].start);


int a [3] = {5,1,2};
test(a);
*/

//Serial.println(a[0]);


}

float prevRatioVal = 0;

void loop() {

  momentum.check();


  float ratioVal = (float)analogRead(A0)/(float)1024;

  if(ratioVal > 0.5){
    digitalWrite(13,HIGH);
  }else{
    digitalWrite(13,LOW);
    
  }

   if(ratioVal != prevRatioVal){

    interpolationRythme(beats, nbElements, divideTime,ratioVal);
    prevRatioVal = ratioVal;

   }

}

//send MIDI message
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  if(!debug)Serial.write(command);//send note on or note off command
  if(!debug)Serial.write(MIDInote);//send pitch data
  if(!debug)Serial.write(MIDIvelocity);//send velocity data
}





void randomizePattern( int from, int to, int nombreDeNotes) {

  int octaves = ((to - from) - (to - from) % 12) / 12;

  int octavePart = (to - from) % 12;


  int notePossibleIndex = 0;

  int derniereOctave = from + octaves * 12;

  int nbNotesDansGamme = sizeof(pentatonicMinorScales) / sizeof(pentatonicMinorScales[0]);

  boolean debugPatternConstruction = true;
  boolean debugNotesPossibles = true;

  int tailleTableau = 0;

  for (int i = 0; i < octaves; i++) {

    int octaveActuelle = (int)from + (i * 12);

    for (int j = 0; j < nbNotesDansGamme; j++) {

      int noteActuelle = octaveActuelle + pentatonicMinorScales[j];
      tailleTableau++;

    }

  }

  if (octavePart != 0) {

    // AJOUT DE LA DERNIERE OCTAVE PARTIELLE SI IL Y EN A UNE
    for (int i = 0 ; i < nbNotesDansGamme; i++) {

      int noteActuelle = derniereOctave + 12 + pentatonicMinorScales[i];

      if (noteActuelle <= to) {
        tailleTableau++;
      }

    }

  }

  int notesPossibles[tailleTableau];

  for (int i = 0; i < octaves; i++) {

    int octaveActuelle = from + (i * 12);

    for (int j = 0; j < nbNotesDansGamme; j++) {

      int noteActuelle = octaveActuelle + pentatonicMinorScales[j];

      notesPossibles[notePossibleIndex] = noteActuelle;
      notePossibleIndex++;

    }

  }

  if (octavePart != 0) {

    // AJOUT DE LA DERNIERE OCTAVE PARTIELLE SI IL Y EN A UNE
    for (int i = 0 ; i < nbNotesDansGamme; i++) {

      int noteActuelle = derniereOctave + 12 + pentatonicMinorScales[i];

      if (noteActuelle <= to) {
        notesPossibles[notePossibleIndex] = noteActuelle;
        notePossibleIndex++;
      }

    }
  }

  int  nouveauPattern [nombreDeNotes];

  pattern[0] = from;
  Serial.println("DEBUG PATTERN");
  Serial.println(from);

  for (int i = 0 ; i < nombreDeNotes - 2; i++) {

    int randomNoteIndex = (int)random(notePossibleIndex - 1);
    int noteActuelle = notesPossibles[randomNoteIndex];
    pattern[i + 1] = noteActuelle;

    Serial.println(noteActuelle);

  }

  Serial.println(from + 7);
  
  pattern[nombreDeNotes - 1] = from + 7; // ON FINIT SUR LA DOMINANTE

  Serial.println("+++++++++++");
}

