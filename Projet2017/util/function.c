#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


	//print un message d'erreur
void bigError(char * message)
{
   fputs("ERROR: ", stderr);
   fputs(message, stderr);
   fputs("\n", stderr);
   exit(EXIT_FAILURE);
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
