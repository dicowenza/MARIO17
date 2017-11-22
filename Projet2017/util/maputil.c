#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"
							/*Fonctions setteurs des données de la carte */

void main(int argc,char ** argv){
	int file=open(argv[1],O_RDONLY);
	if (file==-1){
		bigError("Erreur lors de l'ouverture du fichier ");
	}
	char * c=argv[2];
	
	if(strcmp(c,"--getwidth")==0){
	  printf("la largeur est %u\n",getWidth(file));
	}
	else if(strcmp(c,"--getheight")==0){
	  printf("la hauteur est %u\n",getHeight(file));
	}
	else if(strcmp(c,"--getobjects")==0){
	  printf("le nombre d'objet est %u\n",getNbObject(file));
	}
	else if(strcmp(c,"--getinfo")==0){
	   printf("la largeur est %u\n",getWidth(file));
	   printf("la hauteur est %u\n",getHeight(file));
	    printf("le nombre d'objet est %u\n",getNbObject(file));
	   
	}
	if(strcmp(c,"--setWidth")==0){
	    unsigned int a=atoi(argv[3]);
	    setWidth(file,a);
	  }
	  
	    
}
