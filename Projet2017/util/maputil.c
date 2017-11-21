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
	

							/*Fonctions setteurs des données de la carte */
void setWidth(int file,unsigned int width){
	unsigned int oLargeur=getWidth(file);// on sauvegarde l'ancienne largeur pour plus tard
	unsigned int nLargeur=width;
	lseek(file,0,SEEK_SET);//On se met au début du fichier pour remplacer l'ancienne valeur de la largeur
	write(file,&nLargeur,sizeof(unsigned int));//on remplace l'ancienne valeur

	/*On doit ensutie regarder si la nouvelle largeur est plus petite que l'ancienne , si 
			c'est le cas on doit supprimer les objets de la carte qui 
				n'apparaitront pas à l'écran
							/* main  qui doit tester si l'argument est uen fonction valide et doit l'éxécuter */

	if (nLargeur< oLargeur) {

	/* On va créer un fichier temporaire pour recopier le fichier actuel dans le fichier temporaire avec les  nouvelles dimensions et en ne recopiant que les objets aux bonnes coordonnées !*/

	/*On commence par réecrire les dimensions de la carte */
	int temp=("temp.txt",O_CREAT | O_TRUNC | O_RDONLY| O_WRONLY , 0666);
	write(temp,&nLargeur,sizeof(unsigned int));
	unsigned int hauteur=getHeight(file);
	write(temp,&hauteur,sizeof(unsigned int));
	unsigned int val=getNbObject(file);// le curseur de file est à la quatrieme valeur
	write(temp,&val,sizeof(unsigned int));

	/*On recopie le type d'objet qui sont dans les nouvelles dimensions de la carte */
	for(int i=0;i<oLargeur;i++){
		if(i<=nLargeur){//on recopie si on est plus petit que la nouvelle largeur
			for(int j=0;j<hauteur;i++){
				read(file,&val,sizeof(unsigned int));
				write(temp,&val,sizeof(unsigned int));
			}
		}
	}
	}

	/*Il faut ensuite recopié la fin du fichier file c'est à dire les différents objets 
			de la carte avec leurs caractèristiques*/

}
void main(int argc,char ** argv){
	int file=open(argv[1],O_RDONLY);
	if (file==-1){
		bigError("Erreur lors de l'ouverture du fichier ");
	}
	//Cas du getHeight
	printf("la hauteur est %u\n",getHeight(file));
	//Cas du getWidth
	printf("la largeur est %u\n",getWidth(file));
	//Cas du getNbObejct
	printf("le nombre d'objet est %u\n",getNbObject(file));

	//Cas du getInfo
	printf("la largeur est %u\n",getWidth(file));
	printf("la hauteur est %u\n",getHeight(file));
	printf("le nombre d'objet est %u\n",getNbObject(file));

}
	
	
