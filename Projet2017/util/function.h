#ifndef FUNCTION_H
#define FUNCTION_H

//structure definissant un objet
typedef struct
{
   char * filename;
   unsigned int filenameSize;

   unsigned int frames;
   unsigned int solidity;
   unsigned int destructible;
   unsigned int collectible;
   unsigned int generator;
}Object;


void bigError(char * message);
bool comparer(Object obj1,Object obj2);
unsigned int getWidth(int file);
unsigned int getHeight(int file);
unsigned int getNbObject(int file);
void copyEndFile(int src,int dst,unsigned int frames ,unsigned int solidity,unsigned int destructible , unsigned int collectible , unsigned int generator );
void copyWrite(int src,int dst,int count,void * buf);
void setWidth(int file,unsigned int width);
void setHeight(int file,unsigned int height);

void pruneObject(int file,Object objectToremove);
void addObject(int file,Object * ObjectToadd);
#endif
