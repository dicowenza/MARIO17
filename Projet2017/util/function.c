#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"
#include "../include/map.h"
//print un message d'erreur
void bigError(char * message)
{
  fputs("ERROR: ", stderr);
  fputs(message, stderr);
  fputs("\n", stderr);
  exit(EXIT_FAILURE);
}

bool comparer(Object obj1,Object obj2){
  if(strcmp(obj1->filename,obj2->filename)==0){
      if(obj1->frames==obj2->frames&&obj1->solidity==obj2->solidity
          &&obj1->destructible==obj2->destructible&&obj1->collectible==obj2->collectible
          &&obj1->generator==obj2->generator)
          return true;
  }
  else 
      return false;
}

void copyWrite(int src,int dst,int count,void * buf){
  read(src,&buf,count);
  write(dst,&buf,count);
}

void copyEndFile(int src,int dst,unsigned int frames ,unsigned int solidity,unsigned int destructible , unsigned int collectible , unsigned int generator ){
  copyWrite(src,dst,sizeof(unsigned int),frames);
  copyWrite(src,dst,sizeof(unsigned int),solidity);
  copyWrite(src,dst,sizeof(unsigned int),destructible);
  copyWrite(src,dst,sizeof(unsigned int),collectible);
  copyWrite(src,dst,sizeof(unsigned int),generator);
}


/*Fonctions getteurs des données de la carte */
unsigned int getWidth(int file){
  /* on se place au début du fichier car la première valeur du fichier est la largeur
     et elle est de type unsigned int */
  unsigned int largeur;
  lseek(file,0,SEEK_SET);
  read(file,&largeur,sizeof(unsigned int )); // on lit la premiere valeur écrite dans le fichier (largeur ) et on la stocke dans largeur
  return largeur;
}



/*Idem pour la fonction getHeight sauf qu'il faut d'abord lire la largeur et ensuite la hauteur ! */
unsigned int getHeight(int file){
  unsigned int hauteur;
  lseek(file,sizeof(unsigned int),SEEK_SET);//on se positionne à la deuxieme valeur du fichier
  read(file,&hauteur,sizeof(unsigned int));
  return hauteur;
}



/*Meme chose pour la fonction getNbObject il faut lire largeur et hauteur avant d'avoir le nombre d'objet*/
unsigned int getNbObject(int file){
  
  unsigned int nbObjet;
  lseek(file,2*sizeof(unsigned int),SEEK_SET);//on se positionne à la deuxieme valeur du fichier
  read(file,&nbObjet,sizeof(unsigned int));
  return nbObjet;
}


void setWidth(int file,unsigned int width){
  unsigned int old_width=getWidth(file);// on sauvegarde l'ancienne largeur pour plus tard
  unsigned int new_width=width;
  /*Dans le cas ou new_width>old_width || new_width<old_width on crée un fichier */
  /* On va créer un fichier temporaire pour recopier le fichier actuel dans le fichier temporaire avec les  nouvelles dimensions et en ne recopiant que les objets aux bonnes coordonnées*/
  /*On commence par réecrire les dimensions de la carte */
  int temp=open("../maps/save2.map",O_CREAT | O_TRUNC | O_RDONLY| O_WRONLY , 0666);
  write(temp,&new_width,sizeof(unsigned int));
  unsigned int height=getHeight(file);
  write(temp,&height,sizeof(unsigned int));
  unsigned int nbObject=getNbObject(file);// le curseur de file est à la quatrieme valeur
  write(temp,&nbObject,sizeof(unsigned int));
  unsigned int val;
  /*Suivant la nouvelle largeur on va copier le fichier save pour certaines coordonnées voir toutes dans le deuxième cas */
  if (new_width< old_width) {
    for(int i=0;i<old_width;i++){
      for(int j=0;j<height;j++){
	      read(file,&val,sizeof(unsigned int));
	      if(i<new_width){ //on recopie si on est plus petit que la nouvelle largeur
	        write(temp,&val,sizeof(unsigned int));
	      }
      }
    }
  }
  else{
    for(int i=0;i<new_width;i++){
      for(int j=0;j<height;j++){
	      if(i<old_width){
	          read(file,&val,sizeof(unsigned int));
	          write(temp,&val,sizeof(unsigned int));
	      }
	      else{/*si on dépasse la largeur old_width il faut définir tout les nouveaux objets en MAP_OBJECT_NONE */
	          unsigned int a=MAP_OBJECT_NONE;
            write(temp,&a,sizeof(unsigned int));
        }
      }
    }
  }

  for(int i=0;i<nbObject;i++){
    unsigned int filenameSize=0;

    read(file,&filenameSize,sizeof(unsigned int));
    write(temp,&filenameSize,sizeof(unsigned int));
    for(int j=0;j<filenameSize;j++){
      char car=0;
      read(file,&car,sizeof(char));
      write(temp,&car,sizeof(char));
    }
    write(temp,'\0',sizeof(char));
    unsigned int frames , solidity , destructible,collectible,generator;
    copyEndFile(file,temp,frames,solidity,destructible,collectible,generator);

  }
  close(temp);
  rename("../maps/save2.map","../maps/saved.map");
}
void setHeight(int file,unsigned int height){
  /*Comme pour width on va créer un deuxieme fichier save */
  int temp=open("../maps/save2.map",O_CREAT | O_TRUNC | O_RDONLY| O_WRONLY , 0666);
  unsigned int old_height=getHeight(file);
  unsigned int new_height=height;
  unsigned int width,nbObject,val;
  width=getWidth(file);
  nbObject=getNbObject(file);
  write(temp,&width,sizeof(unsigned int));
  write(temp,&new_height,sizeof(unsigned int));
  write(temp,&nbObject,sizeof(unsigned int));

  if(old_height>new_height){
    /*on "efface" les objets en haut de la carte c'est a dire coordoonée 0 à old_width-new_height*/
    for(int i=0;i<width;i++){
      for(int j=0;j<old_height;j++){
        read(file,&val,sizeof(unsigned int));
        if(j>=old_height-new_height){
          write(temp,&val,sizeof(unsigned int));
        }
      }
    }
  }
  else{
    for(int i=0;i<width;i++){
      for(int j=0;j<new_height;j++){
        if(j<new_height-old_height){
          val=MAP_OBJECT_NONE;
          write(temp,&val,sizeof(unsigned int));
        }
        else{
          read(file,&val,sizeof(unsigned int));
          write(temp,&val,sizeof(unsigned int));
        }

      }
    }
  }/* puis on recopie les objets et lerus caractéristiques */
    for(int i=0;i<nbObject;i++){
      unsigned int filenameSize=0;
      read(file,&filenameSize,sizeof(unsigned int));
      write(temp,&filenameSize,sizeof(unsigned int));
      for(int j=0;j<filenameSize;j++){
        char car=0;
        read(file,&car,sizeof(char));
        write(temp,&car,sizeof(char));

      }
      write(temp,'\0',sizeof(char));
      unsigned int frames , solidity , destructible,collectible,generator;
      copyEndFile(file,temp,frames,solidity,destructible,collectible,generator);

    }
    close(temp);
    rename("../maps/save2.map","../maps/saved.map");
  }
  
  
  void pruneObject(int file,Object objectToremove){
 
  }


  void addObject(int file,Object * ObjectToadd){

  }