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

void reader(int fd, void * buf, int count)
{
   int r = read(fd, buf, count);

   if (r != count)
      invalidMap();
}

void writer(int fd, void * buf, int count)
{
   int r = write(fd, buf, count);

   if (r != count)
      bigError("writing to map file failed");
}

void map_save (char *filename)
{
  // On commence par créer un fichier de sauvegarde
  int save=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0664);
  if(save==-1)
    bigError("probleme d'ouverture de filename");
  
  //on recupere quelques valeurs connues les dimensions de la carte
  unsigned int width=map_width();
  unsigned int height=map_height();
  unsigned int nb_objet=map_objects();


  //on commence la sauvegarde
  writer(save,&width,sizeof(unsigned int));
  writer(save,&height,sizeof(unsigned int));
  writer(save,&nb_objet,sizeof(unsigned int));

  //on remplit l'air de jeu
  for(int x=0;x<width;++x){
    for(int y=0;y<height;++y){
      unsigned int celltype=map_get(x,y);
      writer(save,&celltype,sizeof(unsigned int));
    }
  }

  //on rajoute les objets du decor
  for(int i=0;i<nb_objet;++i){
    char * fname=map_get_name(i);
    unsigned int fnameSize=strlen(fname);
    writer(save,&fnameSize,sizeof(unsigned int));
    for(int j=0;j<fnameSize;++j){
      unsigned int a=fname[j];
      writer(save,&a,sizeof(unsigned int));
    }

    unsigned int frames=map_get_frames(i);
    unsigned int solidity=map_get_solidity(i);
    unsigned int destructible =map_is_destructible(i);
    unsigned int collectible=map_is_collectible(i);
    unsigned int generator=map_is_generator(i);

    writer(save,&frames,sizeof(unsigned int));
    writer(save,&solidity,sizeof(unsigned int));
    writer(save,&destructible,sizeof(unsigned int));
    writer(save,&collectible,sizeof(unsigned int));
    writer(save,&generator,sizeof(unsigned int));
  }

  close(save);

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
 printf("%u est la largeur",&nb_objet);
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture de la largeur");
 }
 lecture = read(chargement,&hauteur,sizeof(unsigned int));
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture de la hauteur");
 }
 lecture = read(chargement,&nb_objet,sizeof(unsigned int));
  printf("%u est la largeur",nb_objet);
 if (lecture < 0 ){
	fprintf(stderr,"Erreur lors de la lecture du nombre d'objet");
 }

  
}

#endif
