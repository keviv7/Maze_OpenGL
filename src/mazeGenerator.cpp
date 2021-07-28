
#include <iostream>
using namespace std;
#include <vector>
#include <random>
#include <utility>
#include <algorithm>

random_device rd; 
mt19937 gen(rd()); 
vector<char> dir{'U','D','L','R'};
char endBorder;
int startX, startY, endX, endY,arrSize;
bool** arr;

int getRandomInteger(int min,int max){
	uniform_int_distribution<int> getRandom(min,max);
	return getRandom(gen);
}

void allocatearr(){
	arr = new bool*[arrSize];
    
    for(int i = 0; i < arrSize; ++i){
        arr[i] = new bool[arrSize];
    }

    for(int i=0;i<arrSize;i++) for(int j=0;j<arrSize;j++) arr[i][j]=true;
}

void setStartPosition(){
    startX = getRandomInteger(1, arrSize - 2);
    startY = getRandomInteger(1, arrSize - 2);

    while (arr[startY][startX])
    {
        startX = getRandomInteger(1, arrSize - 2);
        startY = getRandomInteger(1, arrSize - 2);
    }
}

void setExit(){

    bool foundExit = false;

    while (!foundExit)
    {
        int exitIndex = getRandomInteger(1, arrSize - 2);
        char genDirection= dir[getRandomInteger(0,3)];


        if(genDirection=='U'){
        	if (!arr[1][exitIndex]){

                endX = exitIndex;
                endY = 1;
                endBorder = genDirection;
                arr[0][exitIndex] = false;
                foundExit = true;
            }
        }
        else if(genDirection=='D'){
        	if (!arr[arrSize-2][exitIndex]){

                endX = exitIndex;
                endY = arrSize-2;
                endBorder = genDirection;
                arr[arrSize-1][exitIndex] = false;
                foundExit = true;
            }
        }
        else if(genDirection=='L'){
        	if (!arr[exitIndex][1]){

                endX = 1;
                endY = exitIndex;
                endBorder = genDirection;
                arr[exitIndex][0] = false;
                foundExit = true;
            }
        }
        else{
        	if (!arr[exitIndex][arrSize-2]){

                endX = arrSize-2;
                endY = exitIndex;
                endBorder = genDirection;
                arr[exitIndex][arrSize-1] = false;
                foundExit = true;
            }
        }

       
    }
}

void dfs(int x, int y){

    if (x >= arrSize - 1 || x < 1 || y < 1 || y >= arrSize - 1 || !arr[x][y]) return;

    vector<pair<int,int>> nextVertex{{0,1},{1,0},{-1,0},{0,-1}};

    int times=getRandomInteger(1,24);
    for(int i=1;i<=times;i++) next_permutation(nextVertex.begin(),nextVertex.end());

    //Count visited neighbours
    int count = 0;

    for (int i = 0; i < (int)nextVertex.size(); i++){
    	int xx=x+nextVertex[i].first;
    	int yy=y+nextVertex[i].second;

    	count+=(arr[xx][yy]==false);
    }

    if (count > 1) return;

    arr[x][y] = false;

    
    for (int i = 0; i < (int)nextVertex.size(); i++){
    	int xx=x+nextVertex[i].first;
    	int yy=y+nextVertex[i].second;

    	dfs(xx,yy);
    }
}

bool** generateMaze(int size){
	arrSize=size+2;

	allocatearr();

    int x, y;

    x = getRandomInteger(3, arrSize - 3);
    y = getRandomInteger(3, arrSize - 3);

    dfs(x,y);

    setExit();
    setStartPosition();

    return arr;
}


int getStartX()
{
    return startX;
}

int getStartY()
{
    return startY;
}

int getEndX()
{
    return endX;
}

int getEndY()
{
    return endY;
}

char getEndBorder()
{
    return endBorder;
}
