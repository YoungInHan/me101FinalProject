//Written by group 68
#include <iostream>
#include <cmath>
#include <cstdlib> 
#include <fstream>
#include <iomanip>

using namespace std;

const int MAX = 300;

void readCoordinates(ifstream & fin, int coordinates[][MAX], int & coordCount);
void arrayMap (int coordinates[][MAX], int map[][MAX], int coordCount);
void findMaxima(int coordinates[][MAX], int & xMin, int & yMin, 
				int & xMax, int & yMax, int coordCount);
void plot (int map[][MAX], int xMin,int yMin, int xMax, int yMax);
void fillBorders(int map[][MAX], int xMin, int yMin, int xMax, int yMax);
void shift (int coordinates[][MAX], int xMin, int yMin, int coordCount);
void clearNoise(int map[][MAX], int xMin, int yMin, int xMax, int yMax);

int main()
{

	//open and check for file
	ifstream fin("coordinates.txt");
	
	if(!fin)
	{
		cout << "No File";
		return EXIT_FAILURE;
	}

	//coordinates 2 x n array: stores x,y values in "list"
	int coordinates[2][MAX] = {0};
	
	//map n x n array: contains values according to solid and empty space
	int map[MAX][MAX] = {0};
	
	int coordCount = 0, xMax = 0, yMax = 0, xMin = 100, yMin = 100;
	
	//call functions in set order
	readCoordinates(fin,coordinates,coordCount);
	findMaxima(coordinates, xMin, yMin, xMax, yMax, coordCount);
	shift (coordinates,xMin,yMin,coordCount);
	findMaxima(coordinates, xMin, yMin, xMax, yMax, coordCount);
	arrayMap(coordinates,map,coordCount);
	fillBorders(map,xMin,yMin,xMax,yMax);
	clearNoise(map,xMin,yMin,xMax,yMin);
	plot (map,xMin,yMin,xMax,yMax);
	
	fin.close();
	return EXIT_SUCCESS;
}

//read in text file and add them to a coordinates array
void readCoordinates(ifstream & fin, int coordinates[][MAX], int & coordCount)
{
	int xCoord = 0, yCoord = 0;
	
	while(fin >> xCoord >> yCoord)
	{
		coordinates[0][coordCount] = xCoord;
		coordinates[1][coordCount] = yCoord;
		coordCount++;
	}
}

//adds the coordinates into a map array where the coordinate is placed in the
//respective indices
void arrayMap (int coordinates[][MAX], int map[][MAX], int coordCount)
{
	for (int index = 0; index < coordCount; index++)
		map[coordinates[1][index]][coordinates[0][index]] = 2;
}

//gets the minimum values and uses them to shift all coordinates such 
//that there are no negative numbers
void shift (int coordinates[][MAX], int xMin, int yMin, int coordCount)
{
	for (int index = 0; index < coordCount; index++)
	{
		if (xMin < 0)
			coordinates[0][index] += abs(xMin);
		if (yMin < 0)
			coordinates[1][index] += abs(yMin);
	}
}

//plots the map array using symbols to represent solid and empty space
void plot (int map[][MAX], int xMin,int yMin, int xMax, int yMax)
{
	for (int row = xMax; row >= xMin; row--)
	{
		for (int col = yMin; col <= yMax; col++)
		{
			if(map[row][col] == 1)
			cout << "*";
			if(map[row][col] == 0)
			cout << " ";
			if(map[row][col] == 2)
			cout << "X";
		}
		cout << endl;
	}
}

//finds the maximum and minimum x,y values for the array of coordinates
void findMaxima(int coordinates[][MAX], int & xMin, int & yMin,
			    int & xMax, int & yMax, int coordCount)
{
	for (int index = 0; index < coordCount; index++)
	{
		if (coordinates[0][index] > xMax)
			xMax = coordinates[0][index];
		if (coordinates[0][index] < xMin)
			xMin = coordinates[0][index];
			
		if (coordinates[1][index] > yMax)
			yMax = coordinates[1][index];
		if (coordinates[1][index] < yMin)
			yMin = coordinates[1][index];
	}
}

//Takes the maximum/minimum values and fills in the empty space in row/column
void fillBorders(int map[][MAX], int xMin, int yMin, int xMax, int yMax)
{
	for (int index = xMin; index <= xMax; index++)
	{
		map[index][yMin] = 1;
		map[index][yMax] = 1;
	}
	
	for (int index = yMin; index <= yMax; index++)
	{
		map[xMin][index] = 1;
		map[xMax][index] = 1;
	}
}

//uses a tolerance value to make remove the extra solid points near the maxima
void clearNoise(int map[][MAX], int xMin, int yMin, int xMax, int yMax)
{
	const int TOL = 4;
	for (int row = 0; row < MAX; row++)
	{
		for (int col = 0; col < MAX; col++)
		{
			if (map[row][col] == 2)
			{
				if (abs(row - xMin) <= TOL && abs(row - xMin) != 0)
					map[row][col] = 0;
				if (abs(col - yMin) <= TOL  && abs(col - yMin) != 0)
					map[row][col] = 0;
				if (abs(row - xMax) <= TOL  && abs(row - xMax) != 0)
					map[row][col] = 0;
				if (abs(col - yMax) <= TOL  && abs(col - yMax) != 0)
					map[row][col] = 0; 
			}
		}
	}
}

