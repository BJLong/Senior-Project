#include <cstdio>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include "cube.h"
using namespace std;

const char cube::edgeMoves[6][4] = {
	{0,1,2,3},
	{0,4,11,5},
	{1,5,10,6},
	{2,6,9,7},
	{3,7,8,4},
	{8,9,10,11}
};
const char cube::cornerMoves[6][4] = {
	{0,1,2,3},		//white
	{1,0,7,6},		//blue
	{2,1,6,5},		//red
	{3,2,5,4},		//green
	{0,3,4,7},		//orange
	{4,5,6,7}		//yellow
};
const char cube::rotateUpArray[54] = {
	2, 5, 8,  1, 4, 7,  0, 3, 6,
	53,52,51, 50,49,48, 47,46,45,
	24,21,18, 25,22,19, 26,23,20,
	36,37,38, 39,40,41, 42,43,44,
	17,16,15, 14,13,12, 11,10,9,
	27,28,29, 30,31,32, 33,34,35,
};

const char cube::rotateLeftArray[54] = {
	9,10,11,  12,13,14, 15,16,17,
	18,19,20, 21,22,23, 24,25,26,
	27,28,29, 30,31,32, 33,34,35,
	0, 1, 2,  3, 4, 5,  6, 7, 8,
	42,39,36, 43,40,37, 44,41,38,
	47,50,53, 46,49,52, 45,48,51
};

const char cube::cornerMap[8] = {
	19, 7, 13, 25, 56, 44, 38, 50
};

const char cube::cornerFacelets[8][3] = {
	{36, 18, 11},
	{38, 9, 2},
	{44, 0, 29},
	{42, 27, 20},
	{45, 26, 33},
	{47, 35, 6},
	{53, 8, 15},
	{51, 17, 24}
};

const char cube::edgeMap[12] = {
	3, 5, 9, 17, 18, 6, 12, 24, 48, 40, 36, 34
};

const char cube::edgeFacelets[12][2] = {
	{37, 10},
	{41, 1},
	{43, 28},
	{39, 19},
	{14, 21},
	{12, 5},
	{32, 3},
	{30, 23},
	{48, 25},
	{46, 34},
	{50, 7},
	{52, 16}
};

void cube::rotateUp(){
	char temp[54];
	for(int i = 0; i < 54; i++){
	temp[i] = facelet[i];
	}
	for(int i = 0; i < 54; i++){
		facelet[i] = temp[rotateUpArray[i]];
	}
}

void cube::rotateLeft(){
	char temp[54];
	for(int i = 0; i < 54; i++){
	temp[i] = facelet[i];
	}
	for(int i = 0; i < 54; i++){
		facelet[i] = temp[rotateLeftArray[i]];
	}	
}

void cube::orientCube(char faceletIn[54]){
	for (int i = 0; i < 54; i++){
		facelet[i] = faceletIn[i];
	}
	if(facelet[49] == 0){
		rotateUp();
	}
	if(facelet[40] != 0){
		while(facelet[13] != 0){
			rotateLeft();
		}
		rotateUp();
	
}	while(facelet[13] != 1){
		rotateLeft();
	}
}

void cube::setCube(){
	//need a way to get positions and orientations from facelets
	int code;
	//populate corners[][]
//	char corner[2][8];
	for (int i = 0; i < 8; i++){
		code = 0;
		for (int j = 0; j < 3; j++){
			code += 1 << facelet[cornerFacelets[i][j]];
		}
//cout << "corner " << i << " value " << code << endl;
		for (int j = 0; j < 8; j++){
			if( code == cornerMap[j]){
				corner[0][i] = j;
			}
		}
		for (int j = 0; j < 3; j++){
			if(facelet[cornerFacelets[i][j]] == 0 || facelet[cornerFacelets[i][j]] == 5){
				corner[1][i] = j;
			}
		}
	}

	//populate edges[][]
	char edges[2][12];
	char edgeBadColors[12][4] = {
		{1,3,0,5},
		{1,3,0,5},
		{1,3,0,5},
		{1,3,0,5},
		{0,5,1,3},
		{0,5,1,3},
		{0,5,1,3},
		{0,5,1,3},
		{1,3,0,5},
		{1,3,0,5},
		{1,3,0,5},
		{1,3,0,5}
		
	};

	for (int i = 0; i < 12; i++){
		code = 0;
		for (int j = 0; j < 2; j++){
			code += 1 << facelet[edgeFacelets[i][j]];
		}
//cout << "edge " << i << " value " << code << endl;
		for (int j = 0; j < 12; j++){
			if( code == edgeMap[j]){
				edge[0][i] = j;

			}
		}
		if (facelet[edgeFacelets[i][0]] == edgeBadColors[i][0] || 
		facelet[edgeFacelets[i][0]] == edgeBadColors[i][1] || 
		facelet[edgeFacelets[i][1]] == edgeBadColors[i][2] || 
		facelet[edgeFacelets[i][1]] == edgeBadColors[i][3]){
			edge[1][i] = 1;
		}else{edge[1][i] = 0;}		
	}
}


void cube::twistClockwise(int face) {
	//twist corrosponding edges clockwise
	char temp = edge[0][edgeMoves[face][0]];
	edge[0][edgeMoves[face][0]] = edge[0][edgeMoves[face][3]];
	edge[0][edgeMoves[face][3]] = edge[0][edgeMoves[face][2]];
	edge[0][edgeMoves[face][2]] = edge[0][edgeMoves[face][1]];
	edge[0][edgeMoves[face][1]] = temp;

	//twist corrosponding corners clockwise
	temp = corner[0][cornerMoves[face][0]];
	corner[0][cornerMoves[face][0]] = corner[0][cornerMoves[face][3]];
	corner[0][cornerMoves[face][3]] = corner[0][cornerMoves[face][2]];
	corner[0][cornerMoves[face][2]] = corner[0][cornerMoves[face][1]];
	corner[0][cornerMoves[face][1]] = temp;

	//edge orientation
	if(face == 2 || face == 4){
		edge[1][edgeMoves[face][0]] = (edge[1][edgeMoves[face][0]] + 1) % 2;
		edge[1][edgeMoves[face][1]] = (edge[1][edgeMoves[face][1]] + 1) % 2;
		edge[1][edgeMoves[face][2]] = (edge[1][edgeMoves[face][2]] + 1) % 2;
		edge[1][edgeMoves[face][3]] = (edge[1][edgeMoves[face][3]] + 1) % 2;
	}

	//move orientations
	temp = edge[1][edgeMoves[face][0]];
	edge[1][edgeMoves[face][0]] = edge[1][edgeMoves[face][3]];
	edge[1][edgeMoves[face][3]] = edge[1][edgeMoves[face][2]];
	edge[1][edgeMoves[face][2]] = edge[1][edgeMoves[face][1]];
	edge[1][edgeMoves[face][1]] = temp;

	//twist corrosponding corners clockwise
	temp = corner[1][cornerMoves[face][0]];
	corner[1][cornerMoves[face][0]] = corner[1][cornerMoves[face][3]];
	corner[1][cornerMoves[face][3]] = corner[1][cornerMoves[face][2]];
	corner[1][cornerMoves[face][2]] = corner[1][cornerMoves[face][1]];
	corner[1][cornerMoves[face][1]] = temp;

	//corner orientation
	if(face != 0 && face != 5){
		corner[1][cornerMoves[face][0]] = (corner[1][cornerMoves[face][0]] + 2) % 3;
		corner[1][cornerMoves[face][1]] = (corner[1][cornerMoves[face][1]] + 1) % 3;
		corner[1][cornerMoves[face][2]] = (corner[1][cornerMoves[face][2]] + 2) % 3;
		corner[1][cornerMoves[face][3]] = (corner[1][cornerMoves[face][3]] + 1) % 3;
	}

}

void cube::twistCounterClockwise(int face) {
	//twist corrosponding edges clockwise
	char temp = edge[0][edgeMoves[face][0]];
	edge[0][edgeMoves[face][0]] = edge[0][edgeMoves[face][1]];
	edge[0][edgeMoves[face][1]] = edge[0][edgeMoves[face][2]];
	edge[0][edgeMoves[face][2]] = edge[0][edgeMoves[face][3]];
	edge[0][edgeMoves[face][3]] = temp;
	//twist corrosponding corners clockwise
	temp = corner[0][cornerMoves[face][0]];
	corner[0][cornerMoves[face][0]] = corner[0][cornerMoves[face][1]];
	corner[0][cornerMoves[face][1]] = corner[0][cornerMoves[face][2]];
	corner[0][cornerMoves[face][2]] = corner[0][cornerMoves[face][3]];
	corner[0][cornerMoves[face][3]] = temp;

	//edge orientation
	if(face == 2 || face == 4){
		edge[1][edgeMoves[face][0]] = (edge[1][edgeMoves[face][0]] + 1) % 2;
		edge[1][edgeMoves[face][1]] = (edge[1][edgeMoves[face][1]] + 1) % 2;
		edge[1][edgeMoves[face][2]] = (edge[1][edgeMoves[face][2]] + 1) % 2;
		edge[1][edgeMoves[face][3]] = (edge[1][edgeMoves[face][3]] + 1) % 2;
	}

	//corner orientation
	if(face != 0 && face != 5){
		corner[1][cornerMoves[face][0]] = (corner[1][cornerMoves[face][0]] + 1) % 3;
		corner[1][cornerMoves[face][1]] = (corner[1][cornerMoves[face][1]] + 2) % 3;
		corner[1][cornerMoves[face][2]] = (corner[1][cornerMoves[face][2]] + 1) % 3;
		corner[1][cornerMoves[face][3]] = (corner[1][cornerMoves[face][3]] + 2) % 3;
	}

	//move orientations
	temp = edge[1][edgeMoves[face][0]];
	edge[1][edgeMoves[face][0]] = edge[1][edgeMoves[face][1]];
	edge[1][edgeMoves[face][1]] = edge[1][edgeMoves[face][2]];
	edge[1][edgeMoves[face][2]] = edge[1][edgeMoves[face][3]];
	edge[1][edgeMoves[face][3]] = temp;
	//twist corrosponding corners clockwise
	temp = corner[1][cornerMoves[face][0]];
	corner[1][cornerMoves[face][0]] = corner[1][cornerMoves[face][1]];
	corner[1][cornerMoves[face][1]] = corner[1][cornerMoves[face][2]];
	corner[1][cornerMoves[face][2]] = corner[1][cornerMoves[face][3]];
	corner[1][cornerMoves[face][3]] = temp;

}

void cube::halfTwist(int face) {
	char temp = edge[0][edgeMoves[face][0]];
	edge[0][edgeMoves[face][0]] = edge[0][edgeMoves[face][2]];
	edge[0][edgeMoves[face][2]] = temp;
	temp = edge[0][edgeMoves[face][1]];
	edge[0][edgeMoves[face][1]] = edge[0][edgeMoves[face][3]];
	edge[0][edgeMoves[face][3]] = temp;

	temp = corner[0][cornerMoves[face][0]];
	corner[0][cornerMoves[face][0]] = corner[0][cornerMoves[face][2]];
	corner[0][cornerMoves[face][2]] = temp;
	temp = corner[0][cornerMoves[face][1]];
	corner[0][cornerMoves[face][1]] = corner[0][cornerMoves[face][3]];
	corner[0][cornerMoves[face][3]] = temp;
	//moving orientations
	temp = edge[1][edgeMoves[face][0]];
	edge[1][edgeMoves[face][0]] = edge[1][edgeMoves[face][2]];
	edge[1][edgeMoves[face][2]] = temp;
	temp = edge[1][edgeMoves[face][1]];
	edge[1][edgeMoves[face][1]] = edge[1][edgeMoves[face][3]];
	edge[1][edgeMoves[face][3]] = temp;

	temp = corner[1][cornerMoves[face][0]];
	corner[1][cornerMoves[face][0]] = corner[1][cornerMoves[face][2]];
	corner[1][cornerMoves[face][2]] = temp;
	temp = corner[1][cornerMoves[face][1]];
	corner[1][cornerMoves[face][1]] = corner[1][cornerMoves[face][3]];
	corner[1][cornerMoves[face][3]] = temp;
}

void cube::slice(int face) {
	twistClockwise(face);
	switch(face){
		case 0:
			twistCounterClockwise(5);
			break;
		case 1:
			twistCounterClockwise(3);
			break;
		case 2:
			twistCounterClockwise(4);
			break;
		case 3:
			twistCounterClockwise(1);
			break;
		case 4:
			twistCounterClockwise(2);
			break;
		case 5:
			twistCounterClockwise(0);
			break;
	}
}

void cube::halfSlice(int face) {
	halfTwist(face);
	switch(face){
		case 0:
			halfTwist(5);
			break;
		case 1:
			halfTwist(3);
			break;
		case 2:
			halfTwist(4);
			break;
		case 3:
			halfTwist(1);
			break;
		case 4:
			halfTwist(2);
			break;
		case 5:
			halfTwist(0);
			break;
	}
}

void cube::antiSlice(int face) {
	twistClockwise(face);
	switch(face){
		case 0:
			twistClockwise(5);
			break;
		case 1:
			twistClockwise(3);
			break;
		case 2:
			twistClockwise(4);
			break;
		case 3:
			twistClockwise(1);
			break;
		case 4:
			twistClockwise(2);
			break;
		case 5:
			twistClockwise(0);
			break;
	}
}

void cube::antiSliceCounterClockwise(int face) {
	twistCounterClockwise(face);
	switch(face){
		case 0:
			twistCounterClockwise(5);
			break;
		case 1:
			twistCounterClockwise(3);
			break;
		case 2:
			twistCounterClockwise(4);
			break;
		case 3:
			twistCounterClockwise(1);
			break;
		case 4:
			twistCounterClockwise(2);
			break;
		case 5:
			twistCounterClockwise(0);
			break;
	}
}

int cube::phaseOneEncode(){
	int result = 0;
	for(int i = 0; i < 11; i++){
		result = result << 1;
		if(edge[1][i] == 1){
			result++;
		}
	}
	return result;
}

void cube::resetCube(){
	for(int j = 0; j < 8; j++){
		corner[0][j] = j;
		corner[1][j] = 0;
	}
	for(int i = 0; i < 12; i++){
		edge[0][i] = i;
		edge[1][i] = 0;
	}
}

void cube::moveCaller(int num){
	int white = 0;
	int blue = 1;
	int red = 2;
	int green = 3;
	int orange = 4;
	int yellow = 5;

	switch(num){
		case 0:
			twistClockwise(white);
			break;
		case 1:
			twistClockwise(blue);
			break;
		case 2:
			twistClockwise(red);
			break;
		case 3:
			twistClockwise(green);
			break;
		case 4:
			twistClockwise(orange);
			break;
		case 5:
			twistClockwise(yellow);
			break;
		case 6:
			twistCounterClockwise(white);
			break;
		case 7:
			twistCounterClockwise(blue);
			break;
		case 8:
			twistCounterClockwise(red);
			break;
		case 9:
			twistCounterClockwise(green);
			break;
		case 10:
			twistCounterClockwise(orange);
			break;
		case 11:
			twistCounterClockwise(yellow);
			break;
		case 12:
			halfTwist(white);
			break;
		case 13:
			halfTwist(blue);
			break;
		case 14:
			halfTwist(red);
			break;
		case 15:
			halfTwist(green);
			break;
		case 16:
			halfTwist(orange);
			break;
		case 17:
			halfTwist(yellow);
			break;
		case 18:
			slice(white);
			break;
		case 19:
			slice(blue);
			break;
		case 20:
			slice(red);
			break;
		case 21:
			slice(green);
			break;
		case 22:
			slice(orange);
			break;
		case 23:
			slice(yellow);
			break;
		case 24:
			halfSlice(white);
			break;
		case 25:
			halfSlice(blue);
			break;
		case 26:
			halfSlice(red);
			break;
		case 27:
			antiSlice(white);
			break;
		case 28:
			antiSlice(blue);
			break;
		case 29:
			antiSlice(red);
			break;
		case 30:
			antiSliceCounterClockwise(white);
			break;
		case 31:
			antiSliceCounterClockwise(blue);
			break;
		case 32:
			antiSliceCounterClockwise(red);
			break;
	}
}

int cube::oppositeOf(int num){
	int white = 0;
	int blue = 1;
	int red = 2;
	int green = 3;
	int orange = 4;
	int yellow = 5;

	switch(num){
		case 0:
			return 6;
		case 1:
			return 7;
		case 2:
			return 8;
		case 3:
			return 9;
		case 4:
			return 10;
		case 5:
			return 11;
		case 6:
			return 0;
		case 7:
			return 1;
		case 8:
			return 2;
		case 9:
			return 3;
		case 10:
			return 4;
		case 11:
			return 5;
		case 12:
			return 12;
		case 13:
			return 13;
		case 14:
			return 14;
		case 15:
			return 15;
		case 16:
			return 16;
		case 17:
			return 17;
		case 18:
			return 23;
		case 19:
			return 21;
		case 20:
			return 22;
		case 21:
			return 19;
		case 22:
			return 20;
		case 23:
			return 18;
		case 24:
			return 24;
		case 25:
			return 25;
		case 26:
			return 26;
		case 27:
			return 30;
		case 28:
			return 31;
		case 29:
			return 32;
		case 30:
			return 27;
		case 31:
			return 28;
		case 32:
			return 29;
	}
}

void generateListOne(){
	cout << "generating list one" << endl;
	cube c;
	cube current;
	c.resetCube();
	int list[2048][2];
	for(int i = 0; i < 2048;i++){
		list[i][0] = 33;
		list[i][1] = 33;
	}
	list[0][0] = -1;
    list[0][1] = -1;
	int prevEncoding = 0;
	int encoding = 0;
    int count=0;
	queue <cube> cubes;
	cubes.push(c); //the root cube
	//while(count < 2048){
	while(!cubes.empty()){
		current = cubes.front();
		cubes.pop();
		for(int i = 0; i < 33; i++){
			prevEncoding = current.phaseOneEncode();
			current.moveCaller(i);
			encoding = current.phaseOneEncode();
			if(list[encoding][0] == 33){
				cubes.push(current);
				list[encoding][1] = prevEncoding;
				list[encoding][0] = current.oppositeOf(i);
count++;
if (count % 1000 == 0) {
  cout << '\r' << count;
  cout.flush();
}
			}
			current.moveCaller(current.oppositeOf(i));
		}
	}
	int pin = open("Phase1.txt",O_WRONLY|O_CREAT,0666);
	write(pin, list,2048*2*sizeof(int));
	close(pin);
// 	FILE * pFILE;
// 	pFILE = fopen("Phase1.txt", "w");
// 	fwrite(list, sizeof(int), sizeof(list),pFILE);
// 	fclose(pFILE);
}

int choose(int n,int k){
/*	if(k == 0){ return 1;}
	if(n == 0){ return 0;}
	return ((n * choose(n - 1, k - 1)) / k);*/
if (k>n) return 0;
int ans=1;
if (k > n-k) k = n-k;
for (int j=0;j<k;j++) {
ans *= n - j;
ans /= 1 + j;
}
return ans;
}

int cube::phaseTwoEncode(){
	int result = 0;
	int cornerEncode = 0;
	int e = 0;
	int eighthCorner=0;
	//corner encoding
	for(int i = 0; i < 7; i++){
		cornerEncode *= 3;
		cornerEncode += corner[1][i];
		eighthCorner += corner[1][i];
	}
	eighthCorner = eighthCorner % 3;
	// cout << "cornerEncode: " << cornerEncode << endl;

	//edge encoding;
	int edgeEncode = 0;
	vector<char> edgeIndex;
	for(int i = 0; i < 12; i++){
		switch(edge[0][i]){
			case 4:
				edgeIndex.push_back(i);
				break;
			case 5:
				edgeIndex.push_back(i);
				break;
			case 6:
				edgeIndex.push_back(i);
				break;
			case 7:
				edgeIndex.push_back(i);
				break;
			default:
				break;
		}
	}
	if(!edgeIndex.empty()){
		edgeEncode += choose(edgeIndex.back(),4);
		edgeIndex.pop_back();
		edgeEncode += choose(edgeIndex.back(),3);
		edgeIndex.pop_back();
		edgeEncode += choose(edgeIndex.back(),2);
		edgeIndex.pop_back();
		edgeEncode += choose(edgeIndex.back(),1);
		edgeIndex.pop_back();
	}else{cout << "wrong" << endl;}
	// cout << "edgeEncode: " << edgeEncode << endl;
	return cornerEncode * 495 + edgeEncode;
}

int listTwo[1082565][2]; //needed to be a global because of size

void generateListTwo(){
	//cannot use moves twistCW twistCCW slice antisliceCW antisliceCCW on red/orange
	cube c;
	cube current;
	c.resetCube();
	for(int i = 0; i < 1082565; i++){
		listTwo[i][0] = 33;
		listTwo[i][1] = 33;
	}
	listTwo[69][0] = -1;
    listTwo[69][1] = -1;
	int prevEncoding = 0;
	int encoding = 0;
int count=0,count2=0;
	queue <cube> cubes;
	cubes.push(c); //the root cube
cout << c.phaseTwoEncode() << endl;
	while(!cubes.empty()){
count2++;
		current = cubes.front();
		cubes.pop();
if (count2 % 1000 == 0) {
  cout << '\r' << count << "     " << count2;
  cout.flush();
}
		for(int i = 0; i < 33; i++){
//cout << current.phaseTwoEncode() << ' ';
			if(i != 2 && i != 4 &&i != 8 && i != 10 && i != 20 && i != 22 && i != 29 && i != 32){
				prevEncoding = current.phaseTwoEncode();
				current.moveCaller(i);
				encoding = current.phaseTwoEncode();
if (encoding == 146091)
  cout << "\n***" << prevEncoding << "   " << encoding << endl;
				if(listTwo[encoding][0] == 33){
count++;
if (encoding == 146091)
  cout << "Yes" << listTwo[145101][1] << endl;
if (encoding == 145101)
  cout << "encoding 145101 has prev " << prevEncoding << endl;
					cubes.push(current);
					listTwo[encoding][1] = prevEncoding;
					listTwo[encoding][0] = current.oppositeOf(i);
				}
				current.moveCaller(current.oppositeOf(i));
			}
		}
//cout << endl;
	}
for (int i=1;i<1082565;i++)
  if (listTwo[i][0] == 33)
    cout << "Error, state " << i << " not reached." << endl;

	int pin = open("Phase2.txt",O_WRONLY|O_CREAT,0666);
	write(pin, listTwo,1082565*2*sizeof(int));
	close(pin);
}

int factorial(int n){
	if(n == 0){
		return 0;
	}
	if (n == 1){
		return 1;
	}else{
		return n * factorial(n-1);
	}
}

int cube::phaseThreeCornerEncoding(int i, int array[]){
	if(i > 0){
		int num = 0;
		for(int j = i-1; j >= 0; j--){
			if(array[i] < array[j]){
				num++;
			}
		}
		int x = num * factorial(i);
		// cout << "num: " << x << endl;
		return (x + phaseThreeCornerEncoding(i - 1, array));
	}else {return 0;}
}

int cube::phaseThreeEdgeEncoding(){
	int edgeEncode = 0;
	int whiteRedEncode = 0;
	int yellowGreenEncode = 0;
	vector<char> edgeIndex;
	vector<int> whiteRed;
	vector<int> yellowGreen;
	int badEdges = 0;
	int x[4] = {-1,-1,-1,-1};
	//grab edges in positions 1,3,8,10
	x[0] = edge[0][1];
	x[1] = edge[0][3];
	x[2] = edge[0][8];
	x[3] = edge[0][10];
	for(int i = 0; i < 4; i++){
		if(x[i] == 0 || x[i] == 2 || x[i] == 9 || x[i] == 11){
			badEdges++;
			whiteRed.push_back(i);
		}
	}
	//grab edges in positions 0,2,9,11
	x[0] = edge[0][0];
	x[1] = edge[0][2];
	x[2] = edge[0][9];
	x[3] = edge[0][11];
	for(int i = 0; i < 4; i++){
		if(x[i] == 1 || x[i] == 3 || x[i] == 8 || x[i] == 10){
			yellowGreen.push_back(i);
		}
	}

	if(badEdges == 4){
		return 69;
	}else if(badEdges == 0){
		return 0;
	}else if(badEdges == 1){
		whiteRedEncode += choose(whiteRed.back(),1);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),1);
		yellowGreen.pop_back();
		return whiteRedEncode * 4 + yellowGreenEncode + 1;
	}else if(badEdges == 2){
		whiteRedEncode += choose(whiteRed.back(),2);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),2);
		yellowGreen.pop_back();
		whiteRedEncode += choose(whiteRed.back(),1);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),1);
		yellowGreen.pop_back();
		return whiteRedEncode * 6 + yellowGreenEncode + 17;
	}else if(badEdges == 3){
		whiteRedEncode += choose(whiteRed.back(),3);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),3);
		yellowGreen.pop_back();
		whiteRedEncode += choose(whiteRed.back(),2);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),2);
		yellowGreen.pop_back();
		whiteRedEncode += choose(whiteRed.back(),1);
		whiteRed.pop_back();
		yellowGreenEncode += choose(yellowGreen.back(),1);
		yellowGreen.pop_back();
		return whiteRedEncode * 4 + yellowGreenEncode + 53;
	}
}

int cube::phaseThreeEncoding(){
	int cornerArray[8];
	for(int i = 0; i < 8; i++){
		cornerArray[i] = corner[0][i];
	}
	return this->phaseThreeEdgeEncoding() + (this->phaseThreeCornerEncoding(7, cornerArray) * 70);
}

int listThree[2822400][2];

void generateListThree(){
	//cannot use moves twistCW twistCCW slice antisliceCW antisliceCCW on red/orange/blue/green
	cube c;
	cube current;
	c.resetCube();
	for(int i = 0; i < 2822400; i++){
		listThree[i][0] = 33;
		listThree[i][1] = 33;
	}
	listThree[0][0] = -1;
	int prevEncoding = 0;
	int encoding = 0;
int count=0;
	queue <cube> cubes;
	cubes.push(c); //the root cube
	while(!cubes.empty()){
		current = cubes.front();
		cubes.pop();
		for(int i = 0; i < 33; i++){
			if(i != 2 && i != 4 && i != 8 && i != 10 && i != 20 && i != 22 && i != 29 && i != 32 && i != 1 && i != 3 &&i != 7 && i != 9 && i != 19 && i != 21 && i != 28 && i != 31){
				prevEncoding = current.phaseThreeEncoding();
				current.moveCaller(i);
				encoding = current.phaseThreeEncoding();
				if(listThree[encoding][0] == 33){
count++;
if (count % 1000 == 0) {
  cout << '\r' << count;
  cout.flush();
}
					cubes.push(current);
					listThree[encoding][1] = prevEncoding;
					listThree[encoding][0] = current.oppositeOf(i);
				}
				current.moveCaller(current.oppositeOf(i));
			}
		}
	}
	int pin = open("Phase3.txt",O_WRONLY|O_CREAT,0666);
	write(pin, listThree,2822400*2*sizeof(int));
	close(pin);
}

int listFour[663552][2];

int cube::phaseFourCornerEncoding(){
	int even[4];
	int odd = -1;
	int evenEncoding = 0;
	//pull even numbers
	even[0] = corner[0][0];
	even[1] = corner[0][2];
	even[2] = corner[0][4];
	even[3] = corner[0][6];

	evenEncoding = phaseThreeCornerEncoding(3, even);
	//odd encoding
	odd = corner[0][1];
	odd = (odd - 1) / 2;
	return evenEncoding * 4 + odd;
}

int cube::phaseFourEdgeEncoding(){
	int orbitOne[4];
	int orbitTwo[4];
	int orbitThree[4];
	//pull orbitOne
	orbitOne[0] = edge[0][1];
	orbitOne[1] = edge[0][10];
	orbitOne[2] = edge[0][8];
	orbitOne[3] = edge[0][3];
	int orbitOneEncoding = phaseThreeCornerEncoding(3, orbitOne);
	orbitOneEncoding = orbitOneEncoding / 2;
	//pull orbitTwo
	orbitTwo[0] = edge[0][0];
	orbitTwo[1] = edge[0][2];
	orbitTwo[2] = edge[0][9];
	orbitTwo[3] = edge[0][11];
	int orbitTwoEncoding = phaseThreeCornerEncoding(3, orbitTwo);
	//pull orbitThree
	orbitThree[0] = edge[0][4];
	orbitThree[1] = edge[0][5];
	orbitThree[2] = edge[0][6];
	orbitThree[3] = edge[0][7];
	int orbitThreeEncoding = phaseThreeCornerEncoding(3, orbitThree);

	return orbitOneEncoding * 576 + orbitTwoEncoding * 24 + orbitThreeEncoding;
}

int cube::phaseFourEncoding(){
	return this->phaseFourEdgeEncoding() + this->phaseFourCornerEncoding() * 6912;
}

void generateListFour(){
	//can only use half turns
	cube c;
	cube current;
	c.resetCube();
	for(int i = 0; i < 663552; i++){
		listFour[i][0] = 33;
		listFour[i][1] = 33;
	}
	listFour[4032][0] = -1;
    listFour[4032][1] = -1;
	int prevEncoding = 0;
	int encoding = 0;
int count=0;
	queue <cube> cubes;
	cubes.push(c); //the root cube
cout << c.phaseFourEncoding() << endl;
	while(!cubes.empty()){
		current = cubes.front();
		cubes.pop();
		for(int i = 0; i < 33; i++){
			if(i == 12 || i == 13 || i == 14 || i == 15 || i == 16 || i == 17 || i == 24 || i == 25 || i == 26){
//cout << current.phaseFourEncoding() << ' ';
				prevEncoding = current.phaseFourEncoding();
				current.moveCaller(i);
				encoding = current.phaseFourEncoding();
				if(listFour[encoding][0] == 33){
count++;
if (count % 1000 == 0) {
  cout << '\r' << count;
  cout.flush();
}
					cubes.push(current);
					listFour[encoding][1] = prevEncoding;
					listFour[encoding][0] = current.oppositeOf(i);
				}
				current.moveCaller(current.oppositeOf(i));
			}
		}
//cout << endl;
	}
for (int i=1;i<663552;i++)
  if (listFour[i][0] == 33)
    cout << "Error: state " << i << " not reached." << endl;

	int pin = open("Phase4.txt",O_WRONLY|O_CREAT,0666);
	write(pin, listFour,663552*2*sizeof(int));
	close(pin);
}

void cube::dumpCubeArrays(void) {
  cout << "corner positions:";
  for (int i=0;i<8;i++)
    cout << ' ' << (int)corner[0][i];
  cout << endl;
  cout << "corner orientations:";
  for (int i=0;i<8;i++)
    cout << ' ' << (int)corner[1][i];
  cout << endl;
  cout << "edge positions:";
  for (int i=0;i<12;i++)
    cout << ' ' << (int)edge[0][i];
  cout << endl;
  cout << "edge orientations:";
  for (int i=0;i<12;i++)
    cout << ' ' << (int)edge[1][i];
  cout << endl;
}

void cube::dumpCubeFacelets(void) {
  int map[] = {
97,97,97,44,43,42,98,
97,97,97,41,40,39,98,
97,97,97,38,37,36,98,
0,1,2,9,10,11,18,19,20,27,28,29,98,
3,4,5,12,13,14,21,22,23,30,31,32,98,
6,7,8,15,16,17,24,25,26,33,34,35,98,
97,97,97,53,52,51,98,
97,97,97,50,49,48,98,
97,97,97,47,46,45,98,99};
for (int i=0;map[i]!=99;i++)
  if (map[i] == 97)
    cout << ' ';
  else if (map[i] == 98)
    cout << endl;
  else
    cout << "wbrgoy"[facelet[map[i]]];
}
// int main (){
// 	cube b;
// 	b.resetCube();
// 	generateListOne();
// 	//generateListTwo();
// 	//generateListThree();
// 	// generateListFour();
// 	return 0;
// }
