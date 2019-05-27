int divisionTemps = 64;
int divideTime = 4;
int nbElements = divisionTemps / divideTime;

GrilleRythme vide, sequenceLineaire, testRythme, interpolateRythme;

color [] randomColors = {
  color(255, 0, 0), 
  color(150, 25, 0), 
  color(55, 148, 22), 
  color(0, 150, 50), 
  color(20, 0, 255), 
  color(255, 255, 0), 
  color(0, 255, 255), 
  color(255, 0, 0), 
  color(0, 100, 255)
};

import java.util.Comparator;
import java.util.Collections;
import java.util.Comparator;


ArrayList<Bloc> rythme, rythmeCopie;

void setup() {

  size(500, 500);
  
  long l = 6;
  randomSeed(6);
  println((int)random(5));

  float hS = height/8;

  color[] values = new color[nbElements];

  for (int i = 0; i < nbElements; i++) {
    values[i] = randomColors[(int)random(randomColors.length)];
  }

  rythme = new ArrayList<Bloc>();
  ArrayList<PlageDisponibilite> plages = new ArrayList<PlageDisponibilite>();

  rythme = calculRythme(divideTime, values);
  rythmeCopie = rythme;

  vide = new GrilleRythme(0, 0, width, hS, divisionTemps, nbElements, rythme, plages, 'x', values);
  sequenceLineaire = new GrilleRythme(0, hS, width, hS, divisionTemps, nbElements, rythme, plages, 'b', values);
  testRythme = new GrilleRythme(0, hS*2, width, hS, divisionTemps, nbElements, rythme, plages, 'a', values);
  interpolateRythme = new GrilleRythme(0, hS*3, width, hS, divisionTemps, nbElements, rythmeCopie, plages, 'a', values);
  
  
}


ArrayList<Bloc> calculRythme(int segments, color[] _values) {

  ArrayList<Bloc> blocs = new ArrayList<Bloc>();
  ArrayList<PlageDisponibilite> plages;
  
  ArrayList<Integer> rdInds = new ArrayList<Integer>(); 
  ArrayList<Integer> rdSizes = new ArrayList<Integer>(); 
  
  int[] fixedRdInds = {1,32,54,60,0,57,58,49,59,62,5,52,3,53,55,14};
  int[] fixedRdSizes = {1,17,1,2,1,1,1,3,1,1,4,1,1,1,1,1};
  
  boolean fixedMode = false;
  
  color[] values;

  blocs = new ArrayList<Bloc>();
  plages = new ArrayList<PlageDisponibilite>();

  plages.add(new PlageDisponibilite(0, segments-1));

  values = _values;

  // Creation du rythme

  int indexBloc = 0;

  //int tailleRestante = segments;

  int algorithmeFails = 0;
  int failLimite = 2000;
  

  while (indexBloc < nbElements && failLimite > algorithmeFails) { // on souhaite avoir un rythme total

    int randomPlageIndex = (int)random(plages.size());
    PlageDisponibilite randomPlage = plages.get(randomPlageIndex);

    int rdInd = (int)random(randomPlage.ind, randomPlage.inf - 1);
    int rdSize = (int)random(1, randomPlage.inf - rdInd);
    int rdInf = rdInd + rdSize;
    
    
    plages.remove( randomPlage );

    PlageDisponibilite precedente, suivante;

    boolean ajoutPlagePrecedente = false;
    boolean ajoutPlageSuivante = false;

    if (plages.size() == 0 && indexBloc == 0) {

      precedente = new PlageDisponibilite(0, rdInd);
      suivante = new PlageDisponibilite(rdInf, divisionTemps-1);

      if (rdInd > 0)plages.add(precedente);
      if (rdInf < divisionTemps - 1)plages.add(suivante);

    } else {

      int voisinPrecedentIndex = voisinPrecedant(blocs, randomPlage.ind);
      int voisinSuivantIndex = voisinSuivant(blocs, randomPlage.inf);
      
      println("vsi");
      println(voisinSuivantIndex);
      
      
      

      int indexPrecedent = voisinPrecedentIndex != -1 ? blocs.get(voisinPrecedentIndex).inf : 0;
      int indexSuivant = voisinSuivantIndex != -1 ? blocs.get(voisinSuivantIndex).ind : divisionTemps-1;
      
      println("");

      precedente = new PlageDisponibilite(indexPrecedent, rdInd);
      suivante = new PlageDisponibilite(rdInf, indexSuivant);

      if (rdInd > 0 && indexPrecedent != rdInd) {
        ajoutPlagePrecedente = true;
        plages.add(precedente);
      }

      if (rdInf < divisionTemps - 1 && indexSuivant != rdInf) {
        ajoutPlageSuivante = true;
        plages.add(suivante);
      }
    }

    printPlages(plages);

    //tailleRestante -= rdSize;

    int disponibles = segmentsDisponibles(plages);
    int nombreMinimalNecessaireDePlaces = (nbElements - indexBloc - 1);

    if (disponibles < nombreMinimalNecessaireDePlaces) {
      // PLUS ASSEZ D ESPACE DISPONIBLE !!!!
      println("plus assez despace . index Bloc | " + indexBloc + " plages size() " + plages.size() + " disponibles " + disponibles + " segmentsDisponibles(plages) " + segmentsDisponibles(plages) + " nbMP " + nombreMinimalNecessaireDePlaces);
      // on doit supprimer les disponibilites precedemment crées, recreer l'ancienne plage supprimée

      if (ajoutPlagePrecedente) {
        plages.remove(precedente);
      }

      if (ajoutPlageSuivante) {
        plages.remove(suivante);
      }

      plages.add( randomPlage );
    } else {

      // on peut ajouter un bloc et incrementer   
      println("rd Ind " + rdInd + " rdS Size " + rdSize);

      if(fixedMode){
       rdInd =  fixedRdInds[indexBloc];
       rdInf = rdInd + fixedRdSizes[indexBloc];
        
      }

      rdInds.add(rdInd);
      rdSizes.add(rdSize);

      //color c = randomColors[(int)random(randomColors.length)];
      blocs.add( new Bloc(rdInd, rdInf, values[indexBloc]));
      indexBloc++;
      //println("creation bloc " + indexBloc  );
    }
  }
  
  println("rdInds ");

  for(int i = 0 ; i < rdInds.size(); i++){
    
    String s = rdInds.get(i) + "";
    if(i != rdInds.size()-1) s += ",";
    print(s);
     
  }
  println(" ");
  println("rdSizes ");
  
  for(int i = 0 ; i < rdSizes.size(); i++){
    
    String s = rdSizes.get(i) + "";
    if(i != rdSizes.size()-1) s += ",";
    print(s);
     
  }
  
  println(" ");

  //printBlocs(blocs);

  Collections.sort(blocs, new Comparator<Bloc>() {
    public int compare(Bloc s1, Bloc s2) {
      return s1.iind.compareTo(s2.iind);
    }
  }
  );

  int c = 0;

  for (Bloc b : blocs) {

    b.setColor(values[c]);
    c++;
  }

  println("||||||||");


  printBlocs(blocs);
  println(blocs.size());



  return blocs;
}

void draw() {

  background(200);
  vide.display();
  sequenceLineaire.display();
  testRythme.display();
  //interpolateRythme.display();
  interpolateRythme.interpolateRythme(rythme,divideTime,map(mouseX,0,width,0,1));
  //println("ee");

  //void repositionnerBlocs(ArrayList<Bloc> rythmeReference, int divideTime, float ratio) {

  //println("eee");
}




class GrilleRythme {

  float x;
  float y;
  float w;
  float h;
  int segments;
  char mode;
  ArrayList<Bloc> blocs;

  Rythme rythme;
  Sequence sequence;
  //Sequence in
  Rythme interpolated;
  
  color[] _values;


  GrilleRythme(float _posX, float _posY, float _w, float _h, int _segments, int nbEl, ArrayList<Bloc> _blocs, ArrayList<PlageDisponibilite> plages, char _mode, color[] values) {
    x = _posX;
    y = _posY;
    w = _w;
    h = _h;
    segments = _segments;
    mode = _mode;
    blocs = _blocs;

    if (mode == 'a') {
      //rythme = new Rythme(_segments, nbEl, values);
      rythme = new Rythme(_segments, nbEl, values, blocs);
    }

    if (mode == 'b') {
      sequence = new Sequence(nbEl, values);
    }
    
    _values = values;
    
  }

  void display() {

    displayGrid();

    if (mode == 'a' || mode == 'c') {
      displayRythme();
    }

    if (mode == 'b') {
      displaySequence();
    }
  }

  void displayGrid() {

    for (int i = 0; i < segments; i++) {
      float currX = x + ( i * w / segments );
      pushStyle();
      noFill();
      rect(currX, y, w/segments, h);
      popStyle();
    }
  }

  void displaySequence() {

    int i = 0;

    //println(sequence.values.length);

    for (color c : sequence.values) {

      float currX = x + ( i * w / segments * divideTime);

      pushStyle();
      fill(c);
      rect(currX, y, w/segments*divideTime, h);
      popStyle();

      i++;
    }
  }

  void displayRythme() {

    int i = 0;

    for (Bloc bloc : rythme.blocs) {

      //color c = sequence.values[i];

      float currX = x + ( bloc.ind * w / segments  );

      pushStyle();
      stroke(255);
      fill(bloc.c);
      rect(currX, y, w/segments * (bloc.inf-bloc.ind), h);
      popStyle();

      i++;
    }
  }

  void interpolateRythme(ArrayList<Bloc> b, int d, float f) {
    
    
    int[] blocsVals = rythme.repositionnerBlocs(b, d, f);
    
     
     for(int i = 0 ; i < blocsVals.length ; i+=2){
       
       int ind = blocsVals[i];
       int inf = blocsVals[i+1];
       
       
       float currX = ind * w / segments;
       float currXInf = inf * w / segments;
       
       float currW = currXInf - currX;
       
       pushStyle();
      stroke(55);
      fill(_values[i/2]);
      rect(currX, y, currW, h);
      popStyle();
       
       
     }
     

  }
  
  void drawInterpoloate(ArrayList<Bloc> b){
    
  }
  
  
}

class Sequence {

  color[] values;

  Sequence(int nbElements, color[] _values) {

    //println(nbElements);
    values = _values;
  }
}

class Rythme {



  ArrayList<Bloc> blocs;
  //ArrayList<PlageDisponibilite> plages;
  color[] values;

  //Rythme(int segments){
  //Rythme(int segments, int nbElements, color[] _values, ArrayList<Bloc> blocs, ArrayList<PlageDisponibilite> plages) {
  Rythme(int segments, int nbElements, color[] _values, ArrayList<Bloc> _blocs) {

    //blocs = new ArrayList<Bloc>();
    //plages = new ArrayList<PlageDisponibilite>();

    blocs = _blocs;
  }

  int[] repositionnerBlocs(ArrayList<Bloc> rythmeReference, int divideTime, float ratio) {

    int nbBlocs = rythmeReference.size();
    
    int[] _newBlocs = new int[nbBlocs * 2];
    
    //blocs = rythmeReference;

    //int[] distanceInd = new int[nbBlocs];
    //int[] distanceInf = new int[nbBlocs];

    for (int i = 0; i < nbBlocs; i++) {

      int distanceInd = (i * divideTime) - rythmeReference.get(i).ind;
      int distanceInf = (i * divideTime) - rythmeReference.get(i).inf; 

      //println("dind " + distanceInd);
      
      int a = rythmeReference.get(i).ind;
      int b = (i * divideTime) - rythmeReference.get(i).ind ;
      int c = rythmeReference.get(i).inf;
      int d = ((i+1) * divideTime) - rythmeReference.get(i).inf;
      
      //println(i + " " + a + " " + b );
      
      
      
      _newBlocs[i*2] = a + round(b * ratio);
      _newBlocs[i*2 + 1] = c + round(d * ratio);
      
      
      //println(" bb " + blocs.get(i).ind + " bbb " + blocs.get(i).inf);
      
      
    }
    
    return _newBlocs;
  }
}

static void printBlocs(ArrayList<Bloc> blocs) {

  int i = 0;

  for (Bloc b : blocs) {

    //println("(i " + i+") b.ind | " + b.ind + " b.inf | " + b.inf);

    i++;
  }
}

void printPlages(ArrayList<PlageDisponibilite> plages) {

  int i = 0;

  for (PlageDisponibilite b : plages) {

    //println("(i " + i+") p.ind | " + b.ind + " p.inf | " + b.inf);

    i++;
  }
}

class PlageDisponibilite {

  int ind;
  int inf;

  PlageDisponibilite(int _ind, int _inf) {
    ind = _ind;
    inf = _inf;
  }
}




class Bloc {

  int ind;
  int inf;
  color c;
  Integer iind;
  Integer iinf;

  Bloc(int indexDebut, int indexFin, color _c) {
    ind = indexDebut;
    inf = indexFin;
    iind = new Integer(ind);
    iinf = new Integer(inf);

    c = _c;
  }

  void setColor(color cc) {
    c=cc;
  }

  void display(float _x, float _y, float _w, float _h) {
    pushStyle();
    fill(c);
    rect(_x, _y, _w, _h);
    popStyle();
  }
}


static boolean espaceLibre(ArrayList<Bloc> blocs, int ind, int inf) {

  for (Bloc bloc : blocs) {
    if (bloc.ind < ind && ind < bloc.inf) {
      return false;
    }
  }

  return true;
}

static int voisinPrecedant(ArrayList<Bloc> blocs, int ind) {

  int distanceMin = 99999999;
  int i = 0;
  int indexVoisin = -1;

  for (Bloc bloc : blocs) {

    int distance = ind - bloc.inf; // inf doit etre avant donc distance doit etre > 0
    //println("Recherche precedent distance " + distance + " bloc inf " + bloc.inf + " ind " + ind);

    if ( distance >= 0 && distance < distanceMin) {
      distanceMin = distance;
      indexVoisin = i;
    }

    i++;
  }

  return indexVoisin;
}

static int voisinSuivant(ArrayList<Bloc> blocs, int inf) {

  int distanceMin = 99999999;
  int i = 0;
  int indexVoisin = -1;

  for (Bloc bloc : blocs) {

    int distance = bloc.ind - inf; // inf doit etre avant donc distance doit etre > 0
    println("Recherche suivant distance " + distance + " bloc ind " + bloc.ind + " inf " + inf);

    

    if ( distance >= 0 && distance < distanceMin) {
      distanceMin = distance;
      indexVoisin = i;
    }

    i++;
  }

  return indexVoisin;
}

static int segmentsDisponibles(ArrayList<PlageDisponibilite> disponibilites ) {

  int total = 0;

  for (PlageDisponibilite disponibilite : disponibilites) {

    total += disponibilite.inf - disponibilite.ind;
  }

  return total;
}

static void debugDispos(ArrayList<PlageDisponibilite> disponibilites) {

  for (PlageDisponibilite dispo : disponibilites) {

    println(" dispo ind " + dispo.ind + " dispo inf " + dispo.inf);
  }
}
