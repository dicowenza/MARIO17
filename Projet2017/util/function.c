#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"
#include "../include/map.h"

//affichage d'un message d'erreur

void bigError(char * message)
{
  fputs("ERROR: ", stderr);
  fputs(message, stderr);
  fputs("\n", stderr);
  exit(EXIT_FAILURE);
}


void copyWrite(int src,int dst,int count,void * buf){
  read(src,&buf,count);
  write(dst,&buf,count);
}


void copyEndFile(int src,int dst,unsigned int frames ,unsigned int solidity,unsigned int destructible , unsigned int collectible , unsigned int generator ){
  copyWrite(src,dst,sizeof(unsigned int),&frames);
  copyWrite(src,dst,sizeof(unsigned int),&solidity);
  copyWrite(src,dst,sizeof(unsigned int),&destructible);
  copyWrite(src,dst,sizeof(unsigned int),&collectible);
  copyWrite(src,dst,sizeof(unsigned int),&generator);
}


unsigned int getWidth(int file){
  /* on se place au début du fichier car la première valeur du fichier est la largeur et elle est de type unsigned int */
  unsigned int largeur;
  lseek(file,0,SEEK_SET);
  read(file,&largeur,sizeof(unsigned int )); // on lit la premiere valeur écrite dans le fichier (largeur ) et on la stocke dans largeur
  return largeur;
}


unsigned int getHeight(int file){
  unsigned int hauteur;
  lseek(file,sizeof(unsigned int),SEEK_SET);//on se positionne à la deuxieme valeur du fichier
  read(file,&hauteur,sizeof(unsigned int));
  return hauteur;
}


unsigned int getNbObject(int file){

  unsigned int nbObjet;
  lseek(file,2*sizeof(unsigned int),SEEK_SET);//on se positionne à la troisieme valeur du fichier
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
  if(temp==-1)
    bigError("Problème de creation de la carte temporaire pour setwitdh");
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
	      if(i<old_width){// on recopie toutes les anciennes valeurs
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
    unsigned int frames=0 , solidity=0, destructible=0,collectible=0,generator=0;
    copyEndFile(file,temp,frames,solidity,destructible,collectible,generator);

  }
  close(temp);
  rename("../maps/save2.map","../maps/saved.map");
}


void setHeight(int file,unsigned int height){
  /*Comme pour width on va créer un deuxieme fichier save */
  int temp=open("../maps/save2.map",O_CREAT | O_TRUNC | O_RDONLY| O_WRONLY , 0666);
  if(temp==-1)
    bigError("Problème de creation de la carte temporaire pour setwitdh");
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
  }/* puis on recopie les objets et leurs caractéristiques */
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
      unsigned int frames=0, solidity=0 , destructible=0,collectible=0,generator=0;
      copyEndFile(file,temp,frames,solidity,destructible,collectible,generator);

    }
    close(temp);
    rename("../maps/save2.map","../maps/saved.map");
}


void replace_objects( int argc, char* argv[],int file){
  /*on se place à la fin du fichier ( là où il y a les objets et leurs caractéristiques )*/
          unsigned int nbObjet=getNbObject(file);
          unsigned int object_add=(argc-3)/6;
          lseek(file,0,SEEK_END);
		;
          for( int i =0; i<object_add*6 ;i=i+6){
		
            unsigned int newsize=strlen(argv[3+i]);

            write(file, &newsize,sizeof(unsigned int));
 
            char * c=argv[3+i];

            for(int j=0;j<newsize;j++){
                write(file,&c[j],sizeof(char));    
            }
            unsigned int nframes = atoi(argv[4 +i]);
            write(file, &nframes, sizeof(unsigned int));

            

            char * ctab =argv[5+i];
            unsigned int nsolidity;
            if(strcmp(ctab,"air")==0){
              nsolidity=0;
            }
            if(strcmp(ctab,"semi-solid")==0){
              nsolidity=1;
            }
            if(strcmp(ctab,"solid")==0){
              nsolidity=2;
            }
            if(strcmp(ctab,"liquid")==0){
              nsolidity=3;
            }
            write(file, &nsolidity, sizeof(unsigned int));
           
            
            ctab =argv[6+i];
            unsigned int ndestructible;

            if(strcmp(ctab,"not-destructible")==0){
              ndestructible=0;
            }

            if(strcmp(ctab,"destructible")==0){
              ndestructible=1;
            }
            write(file, &ndestructible, sizeof(unsigned int));
            
            
            ctab=argv[7 +i];
	    unsigned int ncollectible;
            if(strcmp(ctab,"not-collectible")==0){
              ncollectible=0;
            }
            if(strcmp(ctab,"collectible")==0){
              ncollectible=1;
            }
            write(file, &ncollectible, sizeof(unsigned int));
           
            
            ctab=argv[8+i];
            unsigned int ngenerator;
            if(strcmp(ctab,"not-generator")==0){
              ngenerator=0;
            }
            if(strcmp(ctab,"generator")==0){
              ngenerator=1;
            }
           write(file, &ngenerator, sizeof(unsigned int));

           nbObjet++;//incrementation du nombre d'objet à chaque ajout
          }
          lseek(file,sizeof(unsigned int)*2,SEEK_SET);
          write(file,&nbObjet,sizeof(unsigned int));



}
void pruneObjects(int file){

  /* on recopie tout le fichier du début jusqu'au chemin et caracteristiques de chaques objets */

	int temp=open("../maps/save2.map",O_CREAT | O_WRONLY | O_TRUNC, 0666 );
	if(temp==-1)
    	  bigError("Problème de creation de la carte temporaire pour setwitdh");
  	lseek(temp,0,SEEK_SET);
	unsigned int width=getWidth(file);
	unsigned int height=getHeight(file);
	unsigned int nbObject=getNbObject(file);

  /* On initialise un compteur pour savoir exactement combien d'objets différents sont utilisés */
	unsigned int new_nb=0;

	write(temp,&width,sizeof(unsigned int));
	write(temp,&height,sizeof(unsigned int));
	write(temp,&nbObject,sizeof(unsigned int));
	int tab[nbObject];

  /*on initialise un tableau , chaque case correspond à une image ! ( l'ordre est définie dans le fichier objets.txt et apparait comme tel dans la liste d'édition de la carte)*/
	for(int i=0;i<nbObject;i++){
		tab[i]=0;// on initialise chaque case à 0 au cas ou ....
         }

	//les pixels a copier
          unsigned int pixeltype=0;
          for(unsigned int x=0;x<width;++x){
            for(unsigned int y=0;y<height;++y){
              read(file,&pixeltype,sizeof(unsigned int));
              write(temp,&pixeltype,sizeof(unsigned int));
	      if(pixeltype!=MAP_OBJECT_NONE){
		 tab[pixeltype]=tab[pixeltype]+1;/*si un type d'objet est utilisé on incrémente la case qui correspond à l'objet dans le tableau*/
		}
             }
           }

	//on copie les objets de la carte

          unsigned int filenameSize,frames,solidity,destructible,collectible,generator;
          char filename;
          for(int i=0;i<nbObject;++i){
              read(file, &filenameSize, sizeof(unsigned int));
	      if(tab[i]!=0){/*si l'objet n'est pas utilisé on n'écrit pas la taille de la chaine de caractère sinon on augmente le nouveaux nombre d'objets qui sont utilisés */
		 new_nb+=1;
              	 write(temp, &filenameSize, sizeof(unsigned int));
                }
              for (unsigned int j = 0; j < filenameSize; j++){
                  read(file, &filename, sizeof(char));
		  if(tab[i]!=0){/*si l'objet n'est pas utilisé on n'écrit pas  la chaine de caractère */
                     write(temp, &filename, sizeof(char));
		    }
                }
              read(file,&frames,sizeof(unsigned int));
	      read(file,&solidity,sizeof(unsigned int));
	      read(file,&destructible,sizeof(unsigned int));
	      read(file,&collectible,sizeof(unsigned int));
	      read(file,&generator,sizeof(unsigned int));
	      if(tab[i]!=0){/*si l'objet n'est pas utilisé on n'écrit pas ses caractèristiques*/
                 write(temp,&frames,sizeof(unsigned int));
                 write(temp,&solidity,sizeof(unsigned int));
                 write(temp,&destructible,sizeof(unsigned int));
                 write(temp,&collectible,sizeof(unsigned int));
                 write(temp,&generator,sizeof(unsigned int));
	      }
           }
              /* Pour finir il faut modifier le nombre d'objet à utiliser ( 3ème paramètre / valeurs inscrites dans le fichier saved.map)*/
	   lseek(temp,sizeof(unsigned int)*2,SEEK_SET);
	   write(temp,&new_nb,sizeof(unsigned int));
	   close(temp);
           rename("../maps/save2.map","../maps/saved.map");


}
