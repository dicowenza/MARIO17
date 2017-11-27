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

void invalidMap()
{
   bigError("invalid map data");
}

void reader(int load, void * buf, int count)
{
   int r = read(load, buf, count);

   if (r != count)
      invalidMap();
}


void writer(int load, void * buf, int count)
{
   int r = write(load, buf, count);

   if (r != count)
      bigError("writing to map file failed");
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


void replace_objects( int argc, char* argv[]){

          unsigned int in = open(argv[1], O_RDONLY);
          if(in == -1)
            bigError("Fichier inexistant !\n");
          unsigned int out= open("../map/maptmp",O_CREAT | O_WRONLY | O_TRUNC, 0666 );
          if(in == -1)
            bigError("probleme de creatin de maptmp!\n");
  //les dimensions de la carte

          unsigned int width, height, nbObjet;

          reader(in, &width, sizeof(unsigned int));
          reader(in, &height, sizeof(unsigned int));
          reader(in, &nbObjet, sizeof(unsigned int));

          writer(out, &width, sizeof(unsigned int));
          writer(out, &height, sizeof(unsigned int));
          writer(out, &nbObjet, sizeof(unsigned int));
  //les pixels a copier
          unsigned int pixeltype=0;
          for(unsigned int x=0;x<width;++x){
            for(unsigned int y=0;y<height;++y){
              reader(in,&pixeltype,sizeof(unsigned int));
              writer(out,&pixeltype,sizeof(unsigned int));
            }
          }
  //on copie les objets de la carte
          unsigned int filenameSize;
          char filename;
          unsigned int frames;
          unsigned int solidity;
          unsigned int destructible;
          unsigned int collectible;
          unsigned int generator;

          for(int i=0;i<nbObjet;++i){

              reader(in, &filenameSize, sizeof(unsigned int));
              writer(out, &filenameSize, sizeof(unsigned int));

                for (unsigned int j = 0; j < filenameSize; j++){
                    reader(in, &filename, sizeof(char));
                    writer(out, &filename, sizeof(char));
                }

                reader(in,&frames,sizeof(unsigned int));
                writer(out,&frames,sizeof(unsigned int));
                reader(in,&solidity,sizeof(unsigned int));
                writer(out,&solidity,sizeof(unsigned int));
                reader(in,&destructible,sizeof(unsigned int));
                writer(out,&destructible,sizeof(unsigned int));
                reader(in,&collectible,sizeof(unsigned int));
                writer(out,&collectible,sizeof(unsigned int));
                reader(in,&generator,sizeof(unsigned int));
                writer(out,&generator,sizeof(unsigned int));
              }
  //ajouter nouvel objet


          unsigned int newsize=strlen(argv[3]);

          for( int i = 0; i < nbObjet * 6; i+=6){

            write(out, &newsize,sizeof(unsigned int));

            char * c=argv[3+i];

            for(int j=0;j<newsize;j++){
                writer(out,c[j],sizeof(char));
            }

            unsigned int frames = atoi(argv[4 + i]);
            unsigned int solidity =atoi(argv[5 + i]);
            unsigned int destructible =atoi(argv[6 + i]);
            unsigned int collectible =atoi(argv[7 + i]);
            unsigned int generator =atoi(argv[8 + i]);

            write(out, &frames, sizeof(unsigned int));
            write(out, &solidity, sizeof(unsigned int));
            write(out, &destructible, sizeof(unsigned int));
            write(out, &collectible, sizeof(unsigned int));
            write(out, &generator, sizeof(unsigned int));
          }
          close(in);
          close(out);
          remove(argv[1]);
          rename("../map/maptmp", argv[1]);
          exit(0);
}
void pruneObjects(int file){
	int temp=open("../maps/save2.map",O_CREAT | O_WRONLY | O_TRUNC, 0666 );
  lseek(temp,0,SEEK_SET);
	unsigned int width=getWidth(file);
	unsigned int height=getHeight(file);
	unsigned int nbObject=getNbObject(file);
	unsigned int new_nb=0;
	write(temp,&width,sizeof(unsigned int));
	write(temp,&height,sizeof(unsigned int));
	write(temp,&nbObject,sizeof(unsigned int));
	int tab[nbObject];
	for(int i=0;i<nbObject;i++){
		tab[i]=0;
}
	//les pixels a copier
          unsigned int pixeltype=0;
          for(unsigned int x=0;x<width;++x){
            for(unsigned int y=0;y<height;++y){
              read(file,&pixeltype,sizeof(unsigned int));
              write(temp,&pixeltype,sizeof(unsigned int));
		if(pixeltype!=MAP_OBJECT_NONE){
			tab[pixeltype]=tab[pixeltype]+1;

		}
            }
          }
	//on copie les objets de la carte
          unsigned int filenameSize,frames,solidity,destructible,collectible,generator;
          char filename;
          for(int i=0;i<nbObject;++i){
              read(file, &filenameSize, sizeof(unsigned int));
		if(tab[i]!=0){
		new_nb+=1;
    printf("%u\n",new_nb);
              write(temp, &filenameSize, sizeof(unsigned int));
                }
                for (unsigned int j = 0; j < filenameSize; j++){
                    read(file, &filename, sizeof(char));
		if(tab[i]!=0){
                    write(temp, &filename, sizeof(char));
		}
                }
                read(file,&frames,sizeof(unsigned int));
		read(file,&solidity,sizeof(unsigned int));
		read(file,&destructible,sizeof(unsigned int));
		read(file,&collectible,sizeof(unsigned int));
		read(file,&generator,sizeof(unsigned int));
		if(tab[i]!=0){
                write(temp,&frames,sizeof(unsigned int));
                write(temp,&solidity,sizeof(unsigned int));
                write(temp,&destructible,sizeof(unsigned int));
                write(temp,&collectible,sizeof(unsigned int));
                write(temp,&generator,sizeof(unsigned int));
		}
              }
	lseek(temp,sizeof(unsigned int)*2,SEEK_SET);
	write(temp,&new_nb,sizeof(unsigned int));
	close(temp);


}
