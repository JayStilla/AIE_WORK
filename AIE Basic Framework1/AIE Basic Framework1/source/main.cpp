//////////////////////////////////////////////////////////////////////////
#define _USE_MATH_DEFINES

#include "AIE.h"
#include "KeyCodes.h"
#include <string>
#include <cmath>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <iostream>


const int iScreenWidth = 1280;
const int iScreenHeight = 720;

using namespace std;
//initiating variables
struct point2D
{
	float x;
	float y;
};
struct moveableObject
{
	point2D position; 
	point2D speed; 
	int sprite; 
	int width;
	int height; 

};

int playerOneX = 100;
int playerOneW = 200;
int playerOneH = 288;

int playerTwoX = 1200;
int playerTwoW = 166;
int playerTwoH = 256;

int ballW = 276;
int ballH = 276;

int speed = 1; 
int hits = 0; 
int direction = rand() % 4 + 1;
int pOneScore = 0;
int pTwoScore = 0; 
int frameCounter = 0;
bool g_gameOver = false;

//Global variables
void setgame();

unsigned int bgImage = -1;
moveableObject Ball = {400, 250, 1, 1, -1, ballW, ballH};
moveableObject playerOne = {playerOneX, 100, 0, 0, -1, playerOneW, playerOneH};
moveableObject playerTwo = {playerTwoX, 100, 0, 0, -1, playerTwoW, playerTwoH};
int highScores[5] = {0}; 
bool gameOver = false; 

//subtracting points and returning a new point
point2D pointSubtract(point2D &v, float s)
{
	point2D result = {v.x - s, v.y - s};
	return result; 
}

//Adding two points and returning it as a new point
point2D pointAdd(point2D &v, float s)
{
	point2D result = {v.x + s, v.y + s}; 
	return result;
}

//multiplying points by scalar to return a new point
point2D multiplyScalar(point2D &v, float s)
{
	point2D result = {v.x*s, v.y*s};
	return result; 
}

//subtracting points by each other for a new point
point2D pointSubtract(point2D &v, point2D &v2)
{
	point2D result = {v.x - v2.x, v.y - v2.y};
	return result; 
}

//adding points by each other for a new point
point2D pointAdd(point2D &v, point2D &v2)
{
	point2D result = {v.x + v2.x, v.y+v2.y};
	return result; 
}

//Getting the normal of a point and returning it as a new point
point2D getNormal(point2D &v)
{
	float mag = sqrt (v.x*v.x + v.y*v.y);
	point2D result = {v.x/mag, v.y/mag};
	return result; 
}

//Getting magnitude for point
float getMagnitude(point2D &v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

//sorting an array of integers in descending order
void sort(int array[], int size){
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			if(array[i] > array [j]){
				int tmp = array[i];
				array [i] = array[j];
				array[j] = tmp;
			}
		}
	}
}

//Player controls
void move(moveableObject &player) {
	if(IsKeyDown ('W') && player.position.y > player.height/2) {
	player.position.y -= 1; 
}
else if (IsKeyDown ('S') && iScreenHeight - (player.height/2)){
	player.position.y += 1; 
}
}

void move2(moveableObject &player) {
	if(IsKeyDown (GLFW_KEY_UP) && player.position.y > player.height/2){
		player.position.y -= 1;
	}
	else if (IsKeyDown (GLFW_KEY_DOWN) && iScreenHeight - (player.height/2)){
		player.position.y += 1; 
	}
}

		


//Check for walls or out of bounds/score
bool testOnScreen(moveableObject& obj)
{
	if (obj.position.x > iScreenWidth)
	{
		obj.speed.x *= -1; 
		return true; 
	}
	if (obj.position.x < 0)
	{
		obj.speed.x *= -1;
		return true;
	}
	if (obj.position.y > iScreenHeight)
	{
		obj.speed.y *= -1;
	}
	if (obj.position.y < 0)
	{
		obj.speed.y *= -1;
	}
	return false;
}

//Update position of the ball
void updateBallPosition (moveableObject &obj)
{
	obj.position = pointAdd(obj.position, obj.speed);
}

//Check if ball has collided with paddle
bool checkPaddleCollision (moveableObject& ball, moveableObject& player){
	int speed = ball.speed.x;

	int ballHalfW = ball.width>>1;
	int playerHalfW = player.width>>1;
	int playerHalfH = player.height>>1;

	if (speed == 0)
		return false;

	if (speed < 0){
		if (ball.position.x - ballHalfW < player.position.x + playerHalfW &&
			ball.position.y > player.position.y - playerHalfH &&
			ball.position.y < player.position.y + player.height){
			return true;
	}
}
	else{
		if (ball.position.x + ballHalfW > player.position.x - playerHalfW &&
			ball.position.y > player.position.y - playerHalfH &&
			ball.position.y < player.position.y + player.height){
				return true;
		}
	}
	return false;
}
	

//Check if object has collided with another object
bool checkCollision (moveableObject& obj1, moveableObject& obj2)
{
	point2D diff = pointSubtract(obj1.position, obj2.position);
	float mag = getMagnitude(diff);

	if (mag > 0 && mag < obj1.height)
	{
		obj1.speed = multiplyScalar( getNormal(diff), 5);
		obj2.speed = multiplyScalar( getNormal(diff), -5);
		return true; 
	}
	return false;

}
//Paddle updates according to ball position
void seek(moveableObject &player, moveableObject& ball)
{
	float speed = sqrt(ball.speed.x*ball.speed.x + ball.speed.y);

	if(player.position.y < ball.position.y - speed)
	{
		int diff = ball.position.y - player.position.y;
		if (diff > speed)
			player.position.y += speed; 
		else
			player.position.y += diff; 
	}
}
//initialize everything in the game
void initGame()
{
	srand(time(0)); 

	bgImage = CreateSprite("./images/AttackOnTitan.png", iScreenWidth, iScreenHeight, true);
	MoveSprite(bgImage, iScreenWidth>>1, iScreenHeight>>1);

	playerOne.sprite = CreateSprite("./images/Eren.png", playerOneW, playerOneH, true );
	playerTwo.sprite = CreateSprite("./images/Mikasa.png", playerTwoW, playerTwoH, true );
	Ball.sprite = CreateSprite("./images/TitanHead.png", ballW, ballH, true );
}

//destroy all and clean up memory
void destroyGame(){
	DestroySprite(bgImage);
	DestroySprite(playerOne.sprite);
	DestroySprite(playerTwo.sprite);
	DestroySprite(Ball.sprite); 
}
	
//update game logic
void updateGame(){
	updateBallPosition(Ball); 

	move(playerOne);
	move2(playerTwo);

	if(Ball.speed.x < 0){ 
		/*seek(playerOne, Ball);*/
	}
	else{
		/*seek(playerTwo, Ball); */
	}
	if(testOnScreen(Ball)){
		if(Ball.position.x < 100) {
			pTwoScore++;
		}
		else{
			pOneScore++;
		}
	}

//game over
	if(g_gameOver == false){
		if(abs(pOneScore - pTwoScore) >= 3){
			g_gameOver = true; 

		}
	}

	if(Ball.speed.x < 0) {
		if(checkPaddleCollision(Ball, playerOne) == true) {
			Ball.speed.x *= -1; 
		}
	}
	else {
		if(checkPaddleCollision(Ball, playerTwo) == true) {
			Ball.speed.x *= -1;
		}
	}
	
	RotateSprite(playerOne.sprite, 0); 
	MoveSprite(playerOne.sprite, playerOne.position.x, playerOne.position.y);

	RotateSprite(playerTwo.sprite, 0); 
	MoveSprite(playerTwo.sprite, playerTwo.position.x, playerTwo.position.y);

	MoveSprite(Ball.sprite, (int)Ball.position.x, (int)Ball.position.y);
}
	
//draw each frame
void drawGame() {
	static char outScore1[15] = {'\n'};
	static char outScore2[15] = {'\n'};

	DrawSprite(bgImage);
	DrawSprite(Ball.sprite);
	DrawSprite(playerOne.sprite); 
	DrawSprite(playerTwo.sprite);
	
	/*sprintf(outScore1, "Player 1: %d", pOneScore);
	sprintf(outScore2, "Player 2: %d", pTwoScore);*/

	//draw scores
	/*DrawString(outScore1, 50, 50, SColour(255, 0, 0,255));
	DrawString(outScore2, 1000, 50, SColour(0, 255, 0, 255));*/


if(g_gameOver == true){
	DrawString("Game Over" , iScreenWidth/2, 300);

	/*char score[15];
	int y = 350;
	for(int i = 0; i<5; i++) {
		if(g_highScores[i] !=0){
			sprintf(score, "player %d: %d", i, g_highScores[i]);
			DrawString(score, iScreenWidth/2, y);
			y += 30;
		}*/
	}
}

		
//Game framework
int main()
{
	Initialise(iScreenWidth, iScreenHeight, false);

	initGame();

	do 
	{
		frameCounter++;
		
		if (frameCounter > 5000)
			frameCounter = 0;

		ClearScreen();

		updateGame();

		drawGame();
	}
	while (FrameworkUpdate() == false);

	destroyGame();

	Shutdown(); 
	
	
	return 0;
}