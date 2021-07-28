#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP

int getRandomInteger(int min,int max);
bool** allocateArray();
void setStartPosition();
void setExit();
void dfs(int x, int y);
bool** generateMaze(int size);

int getStartX();
int getStartY();
int getEndX();
int getEndY();
char getEndBorder();

#endif