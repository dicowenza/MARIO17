#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "function.h"

int main(int argc,char ** argv){

	int file=open(argv[1],O_RDWR);

	if (file==-1)
	    bigError("Erreur lors de l'ouverture du fichier ");

	char * commande=argv[2];// nom de la fonction à exécuter

	if(strcmp(commande,"--getwidth")==0){
	  printf("la largeur est %u\n",getWidth(file));
	}
	else if(strcmp(commande,"--getheight")==0){
	  printf("la hauteur est %u\n",getHeight(file));
	}
	else if(strcmp(commande,"--getobjects")==0){
	  printf("le nombre d'objet est %u\n",getNbObject(file));
	}
	else if(strcmp(commande,"--getinfo")==0){
	   printf("la largeur est %u\n",getWidth(file));
	   printf("la hauteur est %u\n",getHeight(file));
	   printf("le nombre d'objet est %u\n",getNbObject(file));
	}
	if(strcmp(commande,"--setwidth")==0){
	    if(argc!=4)
	       fprintf(stderr," 4 paramètres nécessaire pour setWidth");
	    
	    unsigned int a=atoi(argv[3]);
	    if(a < 16 || a > 1024){
			printf("La largeur doit etre comprise entre 16 et 1024\n");
			exit(1);
		}
	    setWidth(file,a);
	  }

	if(strcmp(commande,"--setheight")==0){
	    if(argc!=4)
	       fprintf(stderr," 4 paramètres nécessaire pour setheight");
	 
	    unsigned int a=atoi(argv[3]);
	    if(a < 12 || a > 20){
			printf("La hauteur doit etre comprise entre 12 et 20\n");
			exit(1);
		}
	    setHeight(file,a);
	  }

	if(strcmp(commande,"--pruneobjects")==0){
	    if(argc!=3){
		fprintf(stderr," 3 paramètres nécessaire pour pruneobjects");
	    }
	    pruneObjects(file);
	  }
		
	if(strcmp(commande,"--setobjects")==0)
	   replace_objects(argc,argv,file);
		  
	return EXIT_SUCCESS;

}
