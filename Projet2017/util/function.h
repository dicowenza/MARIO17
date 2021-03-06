#ifndef FUNCTION_H
#define FUNCTION_H


void bigError(char * message);
void copyEndFile(int src,int dst,unsigned int frames ,unsigned int solidity,unsigned int destructible , unsigned int collectible , unsigned int generator );
void copyWrite(int src,int dst,int count,void * buf);


unsigned int getWidth(int file);
unsigned int getHeight(int file);
unsigned int getNbObject(int file);

void setWidth(int file,unsigned int width);
void setHeight(int file,unsigned int height);

void replace_objects(int argc, char* argv[],int file);
void pruneObjects(int file);
#endif
