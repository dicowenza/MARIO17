#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"

	//print un message d'erreur
void bigError(char * message)
{
   fputs("ERROR: ", stderr);
   fputs(message, stderr);
   fputs("\n", stderr);
   exit(EXIT_FAILURE);
}

void copyWrite(int src,int dst,int count,void * buf){
	read(src,buf,count);
	write(dst,buf,count);
}
	
void copyEndFile(int src,int dst,unsigned int frames ,unsigned int solidity,unsigned int destructible , unsigned int collectible , unsigned int generator ){
	read(src,&frames,sizeof(unsigned int));
	    read(src,&solidity,sizeof(unsigned int));
	    read(src,&destructible,sizeof(unsigned int));
	    read(src,&collectible,sizeof(unsigned int));
	    read(src,&generator,sizeof(unsigned int));
	    write(dst,&frames,sizeof(unsigned int));
	     write(dst,&solidity,sizeof(unsigned int));
	      write(dst,&destructible,sizeof(unsigned int));
	       write(dst,&collectible,sizeof(unsigned int));
	        write(dst,&generator,sizeof(unsigned int));
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
	/* on se place au début du fichier car la deuxième valeur du fichier est la hauteur
				et elle est de type unsigned int */
	unsigned int hauteur;	
	lseek(file,0,SEEK_SET);
	read(file,&hauteur,sizeof(unsigned int )); // on lit la premiere valeur écrite dans le fichier (largeur) et on la stocke dans hauteur , cette valeur est " sautée  "
	read(file,&hauteur,sizeof(unsigned int));//on lit la deuxieme valeur écrite dans le fichier (hauteur ) et on la stocke dans hauteur
	return hauteur;
}

	/*Meme chose pour la fonction getNbObject il faut lire largeur et hauteur avant d'avoir le nombre d'objet*/
unsigned int getNbObject(int file){
	/* on se place au début du fichier car la deuxième valeur du fichier est la hauteur
				et elle est de type unsigned int */
	unsigned int nbObjet;	
	lseek(file,0,SEEK_SET);
	read(file,&nbObjet,sizeof(unsigned int )); // on lit la premiere valeur écrite dans le fichier( largeur ) et on la stocke dans nbObjet, cette valeur est " sautée  "
	read(file,&nbObjet,sizeof(unsigned int));// on lit la deuxieme valeur écrite dans le fichier (hauteur ) et on la stocke dans nbObjet, cette valeur est " sautée  "
	read(file,&nbObjet,sizeof(unsigned int));// on lit la troisieme valeur écrite dans le fichier (nbObjet ) et on la stocke dans nbObjet
	return nbObjet;
}
void setWidth(int file,unsigned int width){
	unsigned int old_width=getWidth(file);// on sauvegarde l'ancienne largeur pour plus tard
	unsigned int new_width=width;
	lseek(file,0,SEEK_SET);//On se met au début du fichier pour remplacer l'ancienne valeur de la largeur
	write(file,&new_width,sizeof(unsigned int));//on remplace l'ancienne valeur dans le fichier save existant 

	/*On doit ensutie regarder si la nouvelle largeur est plus petite que l'ancienne , si 
			c'est le cas on doit supprimer les objets de la carte qui 
				n'apparaitront pas à l'écran*/

	if (new_width< old_width) {
	  /* On va créer un fichier temporaire pour recopier le fichier actuel dans le fichier temporaire avec les  nouvelles dimensions et en ne recopiant que les objets aux bonnes coordonnées !*/
	  /*On commence par réecrire les dimensions de la carte */
	  int temp=open("../maps/save2.map",O_CREAT | O_TRUNC | O_RDONLY| O_WRONLY , 0666);
	  write(temp,&new_width,sizeof(unsigned int));
	  unsigned int height=getHeight(file);
	  write(temp,&height,sizeof(unsigned int));
	  unsigned int nbObject=getNbObject(file);// le curseur de file est à la quatrieme valeur
	  write(temp,&nbObject,sizeof(unsigned int));
	  unsigned int val;
	  /*On recopie le type d'objet qui sont dans les nouvelles dimensions de la carte */
	  for(int i=0;i<old_width;i++){
	    //on recopie si on est plus petit que la nouvelle largeur
	    for(int j=0;j<height;j++){
	      read(file,&val,sizeof(unsigned int));
	      if(i<new_width){
		write(temp,&val,sizeof(unsigned int));
	      }
	    } 
	  }
	   
	  for(int i=0;i<nbObject;i++){
	    unsigned int filenameSize=0;
	    copyWrite(file,temp,sizeof(unsigned int ),filenameSize);
	    for(int j=0;j<filenameSize;j++){
	      char car=0;
		    copyWrite(file,temp,sizeof(char),car);
	      
	    }
	    write(temp,'\0',sizeof(char));
	    unsigned int frames , solidity , destructible,collectible,generator;
	    copyEndFile(file,temp,frames,solidity,destructible,collectible,generator);
	  }
	  	close(temp);
		rename("../maps/save2.map","../maps/saved.map");
	}

}

