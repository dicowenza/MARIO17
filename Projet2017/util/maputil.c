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
	unsigned int old_width=getWidth(file);// on sauvegarde l'ancienne largeur pour plus tard
	unsigned int new_width=width;
	lseek(file,0,SEEK_SET);//On se met au début du fichier pour remplacer l'ancienne valeur de la largeur
	write(file,&new_width,sizeof(unsigned int));//on remplace l'ancienne valeur

	/*On doit ensutie regarder si la nouvelle largeur est plus petite que l'ancienne , si 
			c'est le cas on doit supprimer les objets de la carte qui 
				n'apparaitront pas à l'écran
							/* main  qui doit tester si l'argument est uen fonction valide et doit l'éxécuter */

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
	    read(file,&filenameSize,sizeof(unsigned int));
	    write(temp,&filenameSize,sizeof(unsigned int));
	    for(int j=0;j<filenameSize;j++){
	      char car=0;
	      read(file,&car,sizeof(char));
	      write(temp,&car,sizeof(char));
	    }
	    write(temp,'\0',sizeof(char));
	    unsigned int frames , solidity , destructible,collectible,generator;
	    read(file,&frames,sizeof(unsigned int));
	    read(file,&solidity,sizeof(unsigned int));
	    read(file,&destructible,sizeof(unsigned int));
	    read(file,&collectible,sizeof(unsigned int));
	    read(file,&generator,sizeof(unsigned int));
	    write(temp,&frames,sizeof(unsigned int));
	     write(temp,&solidity,sizeof(unsigned int));
	      write(temp,&destructible,sizeof(unsigned int));
	       write(temp,&collectible,sizeof(unsigned int));
	        write(temp,&generator,sizeof(unsigned int));
	  }
	  	close(temp);
		rename("../maps/save2.map","../maps/saved.map");
	}

}
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
