#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

#define MAP_MAX_OBJECTS 500
#define MAP_OBJECT_MAX_FRAMES 500
#define MAP_OBJECT_MAX_FILENAME_SIZE 500

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

void reader(int load, void * buf, int count)
{
   int r = read(load, buf, count);

   if (r != count)
      invalidMap();
}

void writer(int load, void * buf, int count)
{
   int r = write(load, buf, count);

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


  //on commence la sauvegarde des dimensions de la carte
  writer(save,&width,sizeof(unsigned int));
  writer(save,&height,sizeof(unsigned int));
  writer(save,&nb_objet,sizeof(unsigned int));

  //on recupere l'air de jeu
  for(int x=0;x<width;++x){
    for(int y=0;y<height;++y){
      unsigned int pixeltype=map_get(x,y);
      writer(save,&pixeltype,sizeof(unsigned int));
    }
  }

  //on recupere les objets de la carte
  for(int i=0;i<nb_objet;++i){ 

    char * filename = map_get_name(i);
    unsigned int filenameSize = strlen(filename);
    writer(save, &filenameSize, sizeof(unsigned int));

    for (int j = 0; j < filenameSize; j++)
    {
       int8_t c = filename[j];
       writer(save, &c, sizeof(int8_t));
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
  printf("sauvegarde reussi !\n");
}

void map_load (char *filename)
{
  int load = open(filename, O_RDONLY);
     if (load == -1)
        bigError("unable to open map file for reading");
  
     unsigned int width     = 0;
     unsigned int height    = 0;
     unsigned int nbObjects = 0;
     reader(load, &width    , sizeof(unsigned int));
     reader(load, &height   , sizeof(unsigned int));
     reader(load, &nbObjects, sizeof(unsigned int));
  
     if ( width < MIN_WIDTH || width > MAX_WIDTH || height < MIN_HEIGHT || height > MAX_HEIGHT|| nbObjects < 0 || nbObjects > MAP_MAX_OBJECTS )
      invalidMap();       
     
     map_allocate(width, height);
  
     for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++)
        {
           unsigned int pixel = MAP_OBJECT_NONE;
           reader(load, &pixel, sizeof(unsigned int));
  
           if (pixel != MAP_OBJECT_NONE && (pixel < 0 || pixel > nbObjects - 1))
              invalidMap();
           
           map_set(x, y, pixel);
        }
      }
     
     map_object_begin(nbObjects);
     
     for (int i = 0; i < nbObjects; i++)
     {
              unsigned int filenameSize = 0;
              reader(load, &filenameSize, sizeof(unsigned int));
        
              if (filenameSize < 1 || filenameSize > MAP_OBJECT_MAX_FILENAME_SIZE)
                 invalidMap();
        
              char * filename = malloc(filenameSize + 1);
        
              for (int j = 0; j < filenameSize; j++)
              {
                 int8_t c = 0;
                 reader(load, &c, sizeof(int8_t));
                 filename[j] = c;
              }
        
              filename[filenameSize] = '\0';
        
              
              unsigned int frames       = 0;
              unsigned int  solidity     = 0;
              unsigned int  destructible = 0;
              unsigned int  collectible  = 0;
              unsigned int  generator    = 0;
              reader(load, &frames      , sizeof(unsigned int));
              reader(load, &solidity    , sizeof(unsigned int));
              reader(load, &destructible, sizeof(unsigned int));
              reader(load, &collectible , sizeof(unsigned int));
              reader(load, &generator   , sizeof(unsigned int)); 
            
              
              

  
              if( frames < 1 || frames > MAP_OBJECT_MAX_FRAMES
                  || (solidity != MAP_OBJECT_AIR && solidity != MAP_OBJECT_SEMI_SOLID && solidity != MAP_OBJECT_SOLID)
                  || (destructible != 0 && destructible != 1)
                  || (collectible  != 0 && collectible  != 1)
                  || (generator    != 0 && generator    != 1)
                )
                invalidMap();
               
              unsigned int flags = 0;
              flags = solidity;
              if (destructible)
                  flags = MAP_OBJECT_DESTRUCTIBLE;
              if (collectible)
                  flags = MAP_OBJECT_COLLECTIBLE;
              if (generator)
                  flags = MAP_OBJECT_GENERATOR;
              map_object_add(filename, frames, flags);
              free(filename);
     }
  
     map_object_end();
     
     close(load);
  }
#endif
