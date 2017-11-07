#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height)
{
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (8);
  
    // Texture pour le sol
    map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
    // Mur
    map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
    // Gazon
    map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
    // Marbre
    map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
    // Herbe
    map_object_add ("images/herb.png", 1, MAP_OBJECT_AIR);
    // Petit plancher flottant
    map_object_add ("images/floor.png", 1, MAP_OBJECT_SEMI_SOLID);
    //fleur
    map_object_add("images/flower.png",1,MAP_OBJECT_AIR);
    //pièces
    map_object_add("images/coin.png",20,MAP_OBJECT_AIR|MAP_OBJECT_COLLECTIBLE);
  
    map_object_end ();
}

void map_save (char *filename)
{
  // On commence par créer un fichier de sauvegarde
    int sauvegarde=open(filename,O_CREAT | O_TRUNC | O_WRONLY , 0666);
    if (sauvegarde == -1){
	    fprintf(stderr,"Erreur lors de la création du fichier de sauvegarde ");
	    return NULL;
    }
  
  //On redirige ensuite la sortie standard directement dans le fichier sauvegarde ( lignes de 	codes moins compliqué qu'avec des writes etc... ) , il ne faut pas oublier de sauvegarder la sortie standard !
    int sauvegarde_stdout=dup(1);
    int redirection=dup2(sauvegarde,1);
    if (redirection ==-1){
	      fprintf(stderr,"Erreur lors de la redirection de stdout ");
    }
  

  //Il faut ensuite récupérer les dimensions de la carte , on choisit unsigned int car on a pas besoin des nombres négatifs du int 
    unsigned int largeur=map_width(); // largeur de la map
    unsigned int hauteur=map_height(); //hauteur de la map
    unsigned int nb_objet=map_objects();
  
  // La même chose en 3 lignes
    printf("%u\t",largeur);
    printf("%u\t",hauteur);
    printf("%u\t",nb_objet);
  char * info;
  // On sépare chaque écriture par une tabulation , sinon c'est le bazar !
      for(int i=0;i<nb_objet;i++){
	      printf("%s\t",map_get_name(i)); // on récupére le nom de l'objet
	      printf("%d\t",map_get_frames(i)); // on récupére le nombre de frame
	      printf("%s\t",solidite[map_get_solidity(i)]);
	      if (map_is_destructible(i)==1){
		      info="destructible";
		      printf("%s\t",info);
      	}
      	else{
	      	info="not-destructible";
		      printf("%s\t",info);
      	}
	      if (map_is_collectible(i)==1){
		      info="collectible";
		      printf("%s\t",info);
	      }
	      else{
		      info="not-collectible";
		      printf("%s\t",info);
	      }
	      if (map_is_generator(i)==1){
	      	info="generator";
		      printf("%s\t",info);
	      }
        else{
		      info="not-generator";
	      	printf("%s\n",info); // je met un \n pour revenir à la ligne
      	}
      }
  /* Il ne reste plus qu'à sauvegarder la position des objets sur la carte ainsi que le type d'objet ( fleurs pièces ....)  .
					On effectue un parcours en largeur*/

  int type;
  for(int y=0;y<hauteur;y++){
	  for(int x=0;x<largeur;x++){
		  if(type=map_get(x,y) != MAP_OBJECT_NONE){
			  printf("%d\t",x);// sauvegarde x
			  printf("%d\t",y);// sauvegarde y
			  printf("%d",type);
			  printf("\n");
		  }
	  }
  }
  fflush(stdout);
  close(sauvegarde);
  dup2(sauvegarde_stdout,1); // on rétablie la sortie standard
  printf("La carte a été sauvegardée \n ");
}


void map_load (char *filename)
{
  // TODO
  exit_with_error ("Map load is not yet implemented\n");
}

#endif
