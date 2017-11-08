#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN
static char * solidite[]={"air", "semi-solid","solid","liquid"};

void map_new (unsigned width, unsigned height)
{
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  /*char * recuperer_ligne(int fichier){
	char ligne[1024]; // on crée un tableau de 1024 car il y a au maximum 1024 caractère par ligne
	int i=0;
	int lecture:
	while(i<1024 && (lecture=read(fd,buffer+i,1))>0){ // tant qu'on a pas lu toute la ligne et qu'on peut encore lire 
		if (buffer[i]== '\n'){// si on arrive à la "fin" d'une ligne ( saut de ligne ) 
			buffer[i]='0'; // fin de ligne
			break;//on sort de la boucle , ça ne sert à rien de lire vu qu'il n'y a plus rien
		}
		i+=n;
	}*/
	


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
  // Pièces
  map_object_add ("images/coin.png", 20, MAP_OBJECT_COLLECTIBLE);
  // Fleurs
  map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);

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
  int largeur=map_width(); // largeur de la map
  unsigned int hauteur=map_height(); //hauteur de la map
  unsigned int nb_objet=map_objects();
  int wr;
  
  // La même chose en 3 lignes
  //printf("%u\t",largeur);
  //printf("%u\t",hauteur);
  //printf("%u\n",nb_objet); // on saute une ligne


  //Une version avec des writes , 
  wr=write(sauvegarde,&largeur,sizeof(unsigned int));
  if(wr<0){
	fprintf(stderr,"Erreur lors de l'écriture de la largeur");
  }
  wr=write(sauvegarde,&hauteur,sizeof(unsigned int));
  if(wr<0){
	fprintf(stderr,"Erreur lors de l'écriture de la hauteur");
  }
   wr=write(sauvegarde,&nb_objet,sizeof(unsigned int));
  if(wr<0){
	fprintf(stderr,"Erreur lors de l'écriture du nombre d objet");
  }
  // On revient ensuite à la ligne 
  printf("\n");
  

  /*On écrit ensuite la liste des objets disponibles pour la carte avec leurs caractéristiques :
 nom , frames , si ils sont solides , destructibles , collectables ...  Pour gérer la solidité des objets nous avons rajouté un tableaux en début de fichier
	il pourrait être judicieux et beucoup plus simple de créer également des tableaux pour les autres paramètres afin de raccourcir le code ci-dessous */
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
			printf("%d\t",x);// sauvegarde de la coordonnée x
			printf("%d\t",y);// sauvegarde de la coordonnée y
			printf("%d",type); // sauvegarde du type d'objet
			printf("\n");
		}
	}
  }
  close(sauvegarde);
  dup2(sauvegarde_stdout,1); // on rétablie la sortie standard
  printf("La carte a été sauvegardée \n ");
  
  

  
}

void map_load (char *filename)
{
 //On commence par ouvrir le fichier en lecture seule
 int chargement=open(filename,O_RDONLY);
 if (chargement ==-1){
 	fprintf(stderr,"Erreur lors de l'ouverture du fichier %s",filename);
 }
 
 //Il faut d'abord récupérer les dimensions de la carte ainsi que les différents objets afin d'appeller la fonction map_allocate(largeur , hauteur )
 unsigned int largeur , hauteur , nb_objet ;
 int lecture;
 lecture = read(chargement,&largeur,sizeof(unsigned int));
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture de la largeur");
 }
 lecture = read(chargement,&hauteur,sizeof(unsigned int));
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture de la hauteur");
 }
 lecture = read(chargement,&nb_objet,sizeof(unsigned int));
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture du nombre d'objet");
 }
  
 //On alloue ensuite la carte suivant les dimensions
 map_allocate(largeur,hauteur);

 //On charge ensuite les différents objets de la carte avec la fonction map_object_begin();
 map_object_begin(nb_objet);
 
 //il faut donc récupérer le chemin de l'image , son nombre de frame , ses caractéristiques ( solidité , destructible , generateur , collectable .... )
		// a l'aide de la fonction strtok qui permet de séparer une chaine de caractère à l'aide de séparateur , \t par exemple 
 char * ligne;
 for(int i=0;i<nb_objet;i++){
 	ligne=getline(chargement,ligne); // permeet de récupérer une ligne de code du fichier chargement et de la stocker dans la variable ligne 
	char * nom=strtok(ligne,"\t");// on " casse la chaîne " pour séparer chaque donnée grâce aux tabulations
	char * frames=strtok(NULL,"\t"); 
	char * solidit=strtok(NULL,"\t");
	char * destruction= strtok(NULL,"\t");
	char * collectable=strtok(NULL,"\t");
	char * generator=strtok(NULL,"\t");

	int flags;

	// il faut vérifier que la chaine de caractère récupérer precedemment correspond à une chaine de caractère valide !
	flags|=solidit;

	if(strcmp(destruction,"destructible")){
		flags |= MAP_OBJECT_DESTRUCTIBLE;// on rajoute a flags le parametre MAP_OBJECT_DESTRUCTIBLE
	}
	if(strcmp(collectable,"collectible")){
		flags |= MAP_OBJECT_COLLECTIBLE;// on rajoute a flags le parametre MAP_OBJECT_COLLECTIBLE
	}
	if(strcmp(destruction,"generator")){
		flags |= MAP_OBJECT_GENERATOR;// on rajoute a flags le parametre MAP_OBJECT_GENERATOR
	}

	// a la fin on aura lu tout ce qu'il faut pour pouvoir ajouter l'objet dans la carte !
	int frame=atoi(frames); // ne pas oublier de convertir la chaine de caractère en entier !
	map_object_add(nom,frame,flags);
 }
 map_object_end();

 //Il ne reste plus qu'à créer les objets sur la carte


	
 

 

 

  
}

#endif
