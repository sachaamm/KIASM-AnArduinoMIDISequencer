

static int voisinPrecedent(Beat* beats, int nbOfBeats, int ind) {

  int distanceMin = 9999;
  int i = 0;
  int indexVoisin = -1;

  int e = 0;

  //Serial.println(" ");
  //Serial.println(" VPI ");

  
  while (e < nbOfBeats) {

    int distance = ind - beats[e].stop;
    //Serial.print(ind);
    //Serial.print(" ");
    //Serial.print(distance);
    //Serial.print(" | ");

    if ( distance >= 0 && distance < distanceMin) {
      distanceMin = distance;
      indexVoisin = e;
    }

    e++;
  }

  //Serial.println(" ");

  return indexVoisin;
}

static int voisinSuivant(Beat* beats, int nbOfBeats, int inf) {

  int distanceMin = 9999;
  int i = 0;
  int indexVoisin = -1;

  int e = 0;

  //Serial.println(" ");
  //Serial.println(" Voisin suivant index ");
  //Serial.println(nbOfBeats);
    
  while (e < nbOfBeats) {

    int distance = beats[e].start - inf;
    //Serial.print(e);
    //Serial.print(" ");
    //Serial.print(beats[e].start);
    //Serial.print(" ");
    //Serial.print(inf);
    //Serial.print(" ");
    //Serial.print(distance);
    //Serial.print(" | ");
    

    if ( distance >= 0 && distance < distanceMin) {
      distanceMin = distance;
      indexVoisin = e;
      //Serial.println("ok ! ");
    }

    e++;
  }

  //Serial.println(" ");

  return indexVoisin;
}

static int segmentsDisponibles(PlageDisponibilite* plages, int nbOfPlages) {

  int total = 0;

  int e = 0;

  //Serial.println("Segments disponibles : ");
  
  while (e < nbOfPlages) {

    int diff = plages[e].stop - plages[e].start;
    total += diff;


    //Serial.print(" stt ");
    //Serial.print(plages[e].start);
    //Serial.print(" stp ");
    //Serial.print(plages[e].stop);
    //Serial.print(" (");
    //Serial.print(diff);
    //Serial.print(") | ");
    
    

    e++;
  }

  //Serial.println(" ");

  return total;

}

static void supprimerDisponibilite(PlageDisponibilite* plages, int indexToRemove, int nbPlages){

  for (int c = indexToRemove; c < nbPlages; c++) {

      plages[c] = plages[c + 1];

    }
    
}

int sort_ascBeat(const void *cmp1, const void *cmp2)//https://arduino.stackexchange.com/questions/38177/how-to-sort-elements-of-array-in-arduino-code
{
  
  Beat b = *((Beat *)cmp1);
  Beat a = *((Beat *)cmp2);

  return a.start > b.start ? -1 : (a.start < b.start ? 1 : 0);

}



static Beat* calculRythme( Beat* beats, int segments, const int nbOfBeats, int* notes) {
//static void calculRythme( int segments, int nbOfBeats, int* notes) {

  randomSeed(6.1);

  //static Beat beats[16];
  Beat arrBeats[nbOfBeats];

  int fixedRdInds [16] = {1,32,54,60,0,57,58,49,59,62,5,52,3,53,55,14};
  int fixedRdSizes [16] = {1,17,1,2,1,1,1,3,1,1,4,1,1,1,1,1};

  
  PlageDisponibilite* plages = malloc(sizeof(PlageDisponibilite) * segments);

  int indexPlage = 0;

  PlageDisponibilite plageEntiere;
  plageEntiere.start = 0;
  plageEntiere.stop = segments - 1;
  plages[0] = plageEntiere;

  indexPlage++;

  int indexBloc = 0;

  int fail = 0;
  

  while (indexBloc < nbOfBeats && fail < 3) {

    //Serial.println(indexBloc);
    
    int randomPlageIndex = (int)random(indexPlage);
    PlageDisponibilite randomPlage = plages[randomPlageIndex];

    int rdInd = (int)random(randomPlage.start, randomPlage.stop - 1);
    int rdSize = (int)random(1, randomPlage.stop - rdInd);
    int rdInf = rdInd + rdSize;

    //Serial.println(" ");
    //Serial.println("------");
    //Serial.print(" rdInd ");
    //Serial.print(rdInd);
    //Serial.print(" rdInf ");
    //Serial.print(rdInf);
    //Serial.print(" rdSize ");
    //Serial.print(rdSize);
    //Serial.println(" ");

   
    if(indexBloc > 0){
      //Serial.println("suppr disp");
      supprimerDisponibilite(plages, randomPlageIndex, indexPlage);
    }
    //if(indexBloc > 1){
      //Serial.println("indexplage -- ");
      indexPlage--;
    //}

    //Serial.println("indexPlage ");
    //Serial.println(indexPlage);

    PlageDisponibilite precedente, suivante;

    boolean ajoutPlagePrecedente = false;
    boolean ajoutPlageSuivante = false;

    if (indexPlage == 0 && indexBloc == 0) {

      precedente.start = 0;
      precedente.stop = rdInd;

      suivante.start = rdInf;
      suivante.stop = segments - 1;

      //Serial.println(" ajoutPremiere plage");
      

      if (rdInd > 0) {
        plages[indexPlage] = precedente;
        indexPlage++;
        //Serial.println("ajout plage precedente");
      }

      if (rdInf < segments - 1) {
        plages[indexPlage] = suivante;
        indexPlage++;
        //Serial.println("ajout plage suivante");
      }

      

    } else {

      //Serial.println("nb De Beats");
      //Serial.println(indexBloc);

      //Serial.println("randomPlage start");
      //Serial.println(randomPlage.start);
      //Serial.println("randomPlage stop");
      //Serial.println(randomPlage.stop);

      int iPlage = indexPlage;
      //if(indexBloc == 1)iPlage++;

      // il renvoie l'index mais doit renvoyer beats[index].start
      
      int voisinPrecedentIndex = voisinPrecedent(beats, indexBloc, randomPlage.start);
      int voisinSuivantIndex = voisinSuivant(beats, indexBloc, randomPlage.stop);

      int indexPrecedent = voisinPrecedentIndex != -1 ? beats[voisinPrecedentIndex].stop : 0;
      int indexSuivant = voisinSuivantIndex != -1 ? beats[voisinSuivantIndex].start : segments - 1;

      // precedente = new PlageDisponibilite(indexPrecedent, rdInd);
      precedente.start = indexPrecedent;
      precedente.stop = rdInd;

      // suivante = new PlageDisponibilite(rdInf, indexSuivant);
      suivante.start = rdInf;
      suivante.stop = indexSuivant;

      //Serial.print(" vpi ");
      //Serial.print(voisinPrecedentIndex);
      //Serial.print(" vsi ");
      //Serial.print(voisinSuivantIndex);

      //Serial.print(" ip ");
      //Serial.print(indexPrecedent);
      //Serial.print(" is ");
      //Serial.print(indexSuivant);


      if (rdInd > 0 && indexPrecedent != rdInd) {
        
        //Serial.print(" APP! ");
        ajoutPlagePrecedente = true;
        plages[indexPlage] = precedente;
        indexPlage++;
      }

      if (rdInf < segments - 1 && indexSuivant != rdInf) {

        //Serial.print(" APS! ");
        ajoutPlageSuivante = true;
        plages[indexPlage] = suivante;
        indexPlage++;
      }

      //Serial.println(" ");

    }

    int disponibles = segmentsDisponibles(plages, indexPlage);
    int nombreMinimalNecessaireDePlaces = (nbOfBeats - indexBloc - 1);

    //Serial.print(" d? ");
    //Serial.print(disponibles);
    //Serial.print(" NMin ");
    //Serial.print(nombreMinimalNecessaireDePlaces);

    if (disponibles < nombreMinimalNecessaireDePlaces) {
      // PLUS ASSEZ D ESPACE DISPONIBLE !!!!
      // on doit supprimer les disponibilites precedemment crées, recreer l'ancienne plage supprimée

      // A VERIFIER ! 
      if (ajoutPlagePrecedente) {
        int offsetPrecedente = ajoutPlageSuivante ? -2 : -1;
        supprimerDisponibilite(plages, indexPlage - offsetPrecedente, indexPlage - offsetPrecedente + 1); // on supprime precedente qui est remplacé par suivante
      }

      if (ajoutPlageSuivante) {
        supprimerDisponibilite(plages, indexPlage - 1, indexPlage);  // on supprime juste le dernier element
      }

      if(ajoutPlagePrecedente){
        //Serial.println(" app index plage -- ");
        indexPlage--;
      }
      if(ajoutPlageSuivante){
        //Serial.println(" aps index plage --");
        indexPlage--;
      }

      plages[indexPlage] = randomPlage;

      //Serial.println("pas assez de place");
      fail++;

    } else {

      Beat beatPart;
      beatPart.start = rdInd;
      beatPart.stop = rdInf;

      //Serial.println("");
      //Serial.println("add beat ");
      //Serial.print(rdInd);
      //Serial.print(" ");
      //Serial.print(rdInf);
      //Serial.println("");
      
      beats[indexBloc] = beatPart;
      arrBeats[indexBloc] = beatPart;
    
      indexBloc++;

    }

// JAI AUSSI PEUR QUE RANDOM PLAGE POINTE AILLEURS....
  }

  //Serial.println("-----");

  //for(int i = 0 ; i < nbOfBeats;i++){

    //Serial.println(beats[i].start);
  //}

  //Serial.println("-----");
  qsort(arrBeats, sizeof(arrBeats) / sizeof(arrBeats[0]), sizeof(arrBeats[0]), sort_ascBeat);
  // ON DOIT ENSUITE sort LES BEATS
  
  for(int i = 0 ; i < nbOfBeats;i++){
    beats[i] = arrBeats[i];
    Serial.println(beats[i].start);
  }

  return beats;
  //beatss = beats;

}


void interpolationRythme(Beat* rythmeReference, int nbBeats, int divideTime, float ratio) {

    //Serial.println("********");

    Beat b = (Beat )rythmeReference[0];
    
    //Serial.println(b.start);
    
    int nbBlocs = nbBeats;
    
    int _newBlocs [nbBlocs * 2];

    for (int i = 0; i < nbBlocs; i++) {

      int distanceInd = (i * divideTime) - rythmeReference[i].start;
      int distanceInf = (i * divideTime) - rythmeReference[i].stop; 
   
      int a = rythmeReference[i].start;
      int b = (i * divideTime) - rythmeReference[i].start ;
      int c = rythmeReference[i].stop;
      int d = ((i+1) * divideTime) - rythmeReference[i].stop;

      beatResult[i*2] = a + round(b * ratio);
      beatResult[i*2 + 1] = c + round(d * ratio);

    }
    
    //return _newBlocs;
  }



void test(int* arr){

  arr[0] = 0;
  
}



