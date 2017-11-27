#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"
							/*Fonctions setteurs des données de la carte */

void main(int argc,char ** argv){
	int file=open(argv[1],O_RDWR);
	if (file==-1){
		bigError("Erreur lors de l'ouverture du fichier ");
	}
	char * c=argv[2];// nom de la fonction à exécuter

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
	if(strcmp(c,"--setwidth")==0){
	    if(argc!=4){
		fprintf(stderr," 4 paramètres nécessaire pour setWidth");
	    }
	    unsigned int a=atoi(argv[3]);
	    setWidth(file,a);
	  }

	if(strcmp(c,"--setheight")==0){
	    if(argc!=4){
		fprintf(stderr," 4 paramètres nécessaire pour setWidth");
	    }
	    unsigned int a=atoi(argv[3]);
	    setHeight(file,a);
	  }
	if(strcmp(c,"--pruneobjects")==0){
	    if(argc!=3){
		fprintf(stderr," 3 paramètres nécessaire pour pruneobjects");
	    }
	    pruneObjects(file);
	  }
		if(strcmp(c,"--setobjects")==0){

		   replace_objects(argc,argv,file);
		  }

}
