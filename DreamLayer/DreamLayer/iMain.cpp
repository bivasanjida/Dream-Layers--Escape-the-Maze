#define _CRT_SECURE_NO_WARNINGS
#include "iGraphics.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <mmsystem.h>


enum GameState {
	MENU,
	HOW_TO_PLAY,
	HIGH_SCORES,
	STORY_LINE,
	CREDITS,
	GAME_PLAY,
	GAME_OVER_WIN,
	GAME_OVER_LOSE
};

enum BoxContent {
	KEY,
	COIN,
	EMPTY,
	SHIELD,
	TIME_BONUS
};


struct Wall {
	int x, y, w, h;
};

struct TreasureBox {
	int x, y, w, h;
	bool isOpen;
	bool isItemCollected;
	BoxContent content;
};

struct Enemy {
	int x, y, w, h;
	int dx, dy;
	bool movesHorizontally;
	int currentTexture;
};

struct Star {
	int x, y, size;
};



GameState currentGameState = MENU;
bool gameWon = false;
bool keyCollected = false;

int currentLevel = 1;
const int MAX_LEVELS = 2;
int levelTimer = 0;
const int LEVEL1_TIME = 3000;
const int LEVEL2_TIME = 4000;
bool isShieldActive = false;
int shieldTimer = 0;
const int SHIELD_DURATION = 200;
bool timeBonusActive = false;
int timeBonusAmount = 0;

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 800;
int playerX = 80;
int playerY = 700;
int playerWidth = 30;
int playerHeight = 30;
int moveStep = 10;
const int originalMoveStep = 10;


int goalX = 295;
int goalY = 38;
int doorWidth = 60;
int doorHeight = 60;


#define TREASURE_COUNT_LEVEL1 3
#define TREASURE_COUNT_LEVEL2 5
#define MAX_TREASURE_COUNT 5
TreasureBox treasureBoxes[MAX_TREASURE_COUNT];
int currentTreasureCount = TREASURE_COUNT_LEVEL1;
bool isSpeedBoostActive = false;
int speedBoostTimer = 0;
const int SPEED_BOOST_DURATION = 150;

#define ENEMY_COUNT_LEVEL1 6
#define ENEMY_COUNT_LEVEL2 10
#define MAX_ENEMY_COUNT 10
Enemy guardians[MAX_ENEMY_COUNT];
int currentEnemyCount = ENEMY_COUNT_LEVEL1;

char * menuOptions[] = {
	"New Game",
	"Continue Game",
	"How to Play",
	"High Scores",
	"Story Line",
	"Credits",
	"Exit"
};
int numMenuOptions = sizeof(menuOptions) / sizeof(menuOptions[0]);
int selectedOption = 0;
bool showKeyMessage = false;
int keyMessageTimer = 0;


double pulse_effect = 0;
double menu_pulse_effect = 0;
#define STAR_COUNT 300
Star stars[STAR_COUNT];

#define SAVE_FILE_NAME "savegame.txt"
#define MUSIC_FILE_NAME "main menu.wav"
bool saveGameExists = false;
bool musicPlaying = false;

int playerTexture;
int menuBackgroundTexture;
int doorTexture;
int guardianTexture1;
int guardianTexture2;
int treasureBoxClosedTexture;
int treasureBoxOpenedTexture;
int keyTexture;
int coinTexture;
int shieldTexture;
int timeBonusTexture;

Wall level1Walls[] = {
	{ 50, 50, 250, 15 }, { 350, 50, 465, 15 }, { 50, 50, 15, 700 },
	{ 50, 750, 765, 15 }, { 815, 50, 15, 700 }, { 50, 225, 85, 15 },
	{ 135, 225, 15, 75 }, { 160, 50, 15, 100 }, { 195, 270, 90, 15 },
	{ 235, 150, 15, 120 }, { 285, 55, 15, 225 }, { 450, 50, 15, 100 },
	{ 400, 150, 65, 15 }, { 400, 150, 15, 100 }, { 400, 250, 90, 15 },
	{ 490, 250, 15, 120 }, { 490, 370, 60, 15 }, { 550, 300, 15, 70 },
	{ 550, 300, 65, 15 }, { 615, 240, 15, 60 }, { 550, 50, 15, 140 },
	{ 675, 170, 60, 15 }, { 735, 170, 15, 250 }, { 650, 420, 80, 15 },
	{ 650, 360, 15, 260 }, { 650, 620, 100, 15 }, { 750, 620, 15, 65 },
	{ 650, 685, 100, 15 }, { 750, 520, 15, 65 }, { 750, 520, 65, 15 },
	{ 50, 350, 140, 15 }, { 240, 350, 15, 70 }, { 240, 420, 65, 15 },
	{ 305, 420, 15, 130 }, { 205, 550, 110, 15 }, { 130, 420, 110, 15 },
	{ 130, 420, 15, 130 }, { 130, 485, 100, 15 }, { 130, 610, 120, 15 },
	{ 240, 550, 15, 65 }, { 130, 700, 80, 15 }, { 195, 660, 15, 45 },
	{ 195, 660, 380, 15 }, { 260, 660, 15, 45 }, { 575, 660, 15, 45 },
	{ 423, 660, 15, 45 }, { 423, 705, 65, 15 }, { 320, 330, 65, 15 },
	{ 385, 330, 15, 270 }, { 320, 600, 65, 15 }, { 385, 450, 180, 15 },
	{ 565, 520, 80, 15 }, { 475, 520, 15, 80 }, { 475, 600, 165, 15 },
	{ 310, 600, 15, 65 }
};

Wall level2Walls[] = {
	{ 50, 50, 800, 15 }, { 50, 50, 15, 700 }, { 50, 750, 800, 15 }, { 815, 50, 15, 700 },
	
	{ 200, 50, 15, 200 }, { 200, 200, 100, 15 }, { 350, 50, 15, 200 }, { 350, 200, 100, 15 },
	{ 500, 50, 15, 200 }, { 500, 200, 100, 15 }, { 650, 50, 15, 200 }, { 650, 200, 100, 15 },
	
	{ 100, 300, 200, 15 }, { 350, 300, 200, 15 }, { 600, 300, 200, 15 },
	{ 100, 300, 15, 150 }, { 200, 300, 15, 150 }, { 350, 300, 15, 150 },
	{ 500, 300, 15, 150 }, { 600, 300, 15, 150 }, { 750, 300, 15, 150 },
	
	{ 100, 500, 200, 15 }, { 350, 500, 200, 15 }, { 600, 500, 200, 15 },
	{ 100, 500, 15, 150 }, { 200, 500, 15, 150 }, { 350, 500, 15, 150 },
	{ 500, 500, 15, 150 }, { 600, 500, 15, 150 }, { 750, 500, 15, 150 },
	
	{ 150, 100, 50, 15 }, { 250, 100, 50, 15 }, { 400, 100, 50, 15 },
	{ 550, 100, 50, 15 }, { 700, 100, 50, 15 },
	
	{ 150, 150, 15, 50 }, { 250, 160, 15, 40 }, { 400, 160, 15, 40 },
	{ 550, 160, 15, 40 }, { 700, 160, 15, 40 },
	
	{ 150, 400, 50, 15 }, { 250, 400, 50, 15 }, { 400, 400, 50, 15 },
	{ 550, 400, 50, 15 }, { 700, 400, 50, 15 },
	
	{ 150, 450, 15, 50 }, { 250, 450, 15, 50 }, { 400, 450, 15, 50 },
	{ 550, 450, 15, 50 }, { 700, 450, 15, 50 },
	
	{ 150, 600, 50, 15 }, { 250, 600, 50, 15 }, { 400, 600, 50, 15 },
	{ 550, 600, 50, 15 }, { 700, 600, 50, 15 },
	
	{ 150, 650, 15, 50 }, { 250, 650, 15, 50 }, { 400, 650, 15, 50 },
	{ 550, 650, 15, 50 }, { 700, 650, 15, 50 },
	
	{ 300, 700, 200, 15 }, { 300, 700, 15, 50 }, { 500, 700, 15, 50 }
};

Wall* walls;
int wallCount;


bool isCollidingWithWalls(int nextX, int nextY, int objWidth, int objHeight);
void calculateItemPosition(TreasureBox box, int * itemX, int * itemY);
void initializeLevel();
void initializeEnemies();
void nextLevel();


int iTextWidth(char * str, void * font) {
	int width = 0;
	for (int i = 0; str[i] != '\0'; i++) {
		width += glutBitmapWidth(font, str[i]);
	}
	return width;
}

void drawBrickPattern(int wallX, int wallY, int wallW, int wallH) {
	int brickW = 15;
	int brickH = 7.5;
	int mortar = 2;
	iSetColor(60, 60, 60);
	iFilledRectangle(wallX, wallY, wallW, wallH);
	iSetColor(180, 80, 50);

	bool isVertical = (wallW < wallH);
	if (isVertical) {
		for (int currentX = wallX; currentX < wallX + wallW; currentX += (brickH + mortar)) {
			for (int currentY = wallY; currentY < wallY + wallH; currentY += (brickW + mortar)) {
				iFilledRectangle(currentX, currentY, brickH, brickW);
			}
		}
	}
	else {
		for (int currentY = wallY; currentY < wallY + wallH; currentY += (brickH + mortar)) {
			for (int currentX = wallX; currentX < wallX + wallW; currentX += (brickW + mortar)) {
				iFilledRectangle(currentX, currentY, brickW, brickH);
			}
		}
	}
}


void saveGame() {
	FILE * fp;
	errno_t err = fopen_s(&fp, SAVE_FILE_NAME, "w");
	if (err != 0) {
		saveGameExists = false;
		return;
	}
	fprintf(fp, "%d\n", currentLevel);
	fprintf(fp, "%d\n", playerX);
	fprintf(fp, "%d\n", playerY);
	fprintf(fp, "%d\n", keyCollected);
	fprintf(fp, "%d\n", levelTimer);
	fclose(fp);
	saveGameExists = true;
}

void loadGame() {
	FILE * fp;
	errno_t err = fopen_s(&fp, SAVE_FILE_NAME, "r");
	if (err != 0) {
		saveGameExists = false;
		return;
	}
	fscanf(fp, "%d\n", &currentLevel);
	fscanf(fp, "%d\n", &playerX);
	fscanf(fp, "%d\n", &playerY);
	fscanf(fp, "%d\n", (int *)& keyCollected);
	fscanf(fp, "%d\n", &levelTimer);
	fclose(fp);
	saveGameExists = true;

	shieldTexture = iLoadImage("shield.png");
	timeBonusTexture = iLoadImage("treasure1.png");
	
	initializeLevel();
	initializeEnemies();

	currentGameState = GAME_PLAY;
}

void checkIfSaveGameExists() {
	FILE * fp;
	errno_t err = fopen_s(&fp, SAVE_FILE_NAME, "r");
	if (err == 0) {
		fclose(fp);
		saveGameExists = true;
	}
	else {
		saveGameExists = false;
	}
}

void initializeLevel() {
	if (currentLevel == 1) {
		walls = level1Walls;
		wallCount = sizeof(level1Walls) / sizeof(level1Walls[0]); 
		currentTreasureCount = TREASURE_COUNT_LEVEL1;
		currentEnemyCount = ENEMY_COUNT_LEVEL1;
		levelTimer = LEVEL1_TIME;
		treasureBoxes[0] = { 200, 500, 40, 40, false, false, EMPTY };
		treasureBoxes[1] = { 600, 450, 40, 40, false, false, EMPTY };
		treasureBoxes[2] = { 700, 200, 40, 40, false, false, EMPTY };
	}
	else if (currentLevel == 2) {
		walls = level2Walls;
		wallCount = sizeof(level2Walls) / sizeof(level2Walls[0]);
		currentTreasureCount = TREASURE_COUNT_LEVEL2;
		currentEnemyCount = ENEMY_COUNT_LEVEL2;
		levelTimer = LEVEL2_TIME;
		treasureBoxes[0] = { 170, 170, 40, 40, false, false, EMPTY };
		treasureBoxes[1] = { 320, 170, 40, 40, false, false, EMPTY };
		treasureBoxes[2] = { 620, 170, 40, 40, false, false, EMPTY };
		treasureBoxes[3] = { 170, 320, 40, 40, false, false, EMPTY };
		treasureBoxes[4] = { 620, 320, 40, 40, false, false, EMPTY };
	}

	BoxContent contents[MAX_TREASURE_COUNT];
	if (currentLevel == 1) {
		contents[0] = KEY;
		contents[1] = COIN;
		contents[2] = EMPTY;
	}
	else {
		contents[0] = KEY;
		contents[1] = COIN;
		contents[2] = SHIELD;
		contents[3] = TIME_BONUS;
		contents[4] = EMPTY;
	}

	for (int i = 0; i < currentTreasureCount; i++) {
		int j = rand() % currentTreasureCount;
		BoxContent temp = contents[i];
		contents[i] = contents[j];
		contents[j] = temp;
	}

	for (int i = 0; i < currentTreasureCount; i++) {
		treasureBoxes[i].content = contents[i];
		treasureBoxes[i].isOpen = false;
		treasureBoxes[i].isItemCollected = false;
	}

	moveStep = originalMoveStep;
	isSpeedBoostActive = false;
	speedBoostTimer = 0;
	isShieldActive = false;
	shieldTimer = 0;
	timeBonusActive = false;
	timeBonusAmount = 0;
}

void resetGame() {
	currentLevel = 1;
	playerX = 80;
	playerY = 700;
	gameWon = false;
	keyCollected = false;
	showKeyMessage = false;
	keyMessageTimer = 0;
	initializeLevel();
	initializeEnemies();
	currentGameState = GAME_PLAY;
}

void nextLevel() {
	if (currentLevel < MAX_LEVELS) {
		currentLevel++;
		playerX = 80;
		playerY = 700;
		keyCollected = false;
		showKeyMessage = false;
		keyMessageTimer = 0;
		initializeLevel();
		initializeEnemies();
		currentGameState = GAME_PLAY;
	}
	else {
		gameWon = true;
	}
}


void initializeStars() {
	for (int i = 0; i < STAR_COUNT; i++) {
		stars[i].x = rand() % WINDOW_WIDTH;
		stars[i].y = rand() % WINDOW_HEIGHT;
		stars[i].size = rand() % 3 + 1;
	}
}

void initializeEnemies() {
	if (currentLevel == 1) {
		guardians[0] = { 100, 180, 40, 40, 3, 0, true, guardianTexture1 };
		guardians[1] = { 770, 200, 40, 40, 0, 2, false, guardianTexture1 };
		guardians[2] = { 80, 450, 40, 40, 2, 0, true, guardianTexture1 };
		guardians[3] = { 420, 550, 40, 40, 0, -3, false, guardianTexture2 };
		guardians[4] = { 580, 550, 40, 40, -4, 0, true, guardianTexture2 };
		guardians[5] = { 300, 100, 40, 40, 4, 0, true, guardianTexture1 };

	}
	else if (currentLevel == 2) {
		guardians[0] = { 80, 100, 40, 40, 2, 0, true, guardianTexture1 };
		guardians[1] = { 700, 115, 40, 40, 2, 0, true, guardianTexture1 };
		guardians[2] = { 120, 350, 40, 40, 2, 0, true, guardianTexture1 };
		guardians[3] = { 700, 350, 40, 40, 0, -2, false, guardianTexture2 };
		guardians[4] = { 120, 550, 40, 40, -2, 0, true, guardianTexture2 };
		guardians[5] = { 700, 550, 40, 40, 0, 2, false, guardianTexture1 };
		guardians[6] = { 230, 115, 40, 40, 2, 0, true, guardianTexture2 };
		guardians[7] = { 530, 115, 40, 40, -2, 0, true, guardianTexture1 };
		guardians[8] = { 230, 550, 40, 40, 0, -2, false, guardianTexture2 };
		guardians[9] = { 530, 550, 40, 40, 0, 2, false, guardianTexture1 };
	}
}


void checkPlayerEnemyCollision() {
	if (isShieldActive) {
		return;
	}

	for (int i = 0; i < currentEnemyCount; i++) {
		if (playerX < guardians[i].x + guardians[i].w && playerX + playerWidth > guardians[i].x &&
			playerY < guardians[i].y + guardians[i].h && playerY + playerHeight > guardians[i].y) {
			currentGameState = GAME_OVER_LOSE;
		}
	}
}

bool isCollidingWithWalls(int nextX, int nextY, int objWidth, int objHeight) {
	for (int i = 0; i < wallCount; i++) {
		if (nextX < walls[i].x + walls[i].w && nextX + objWidth > walls[i].x &&
			nextY < walls[i].y + walls[i].h && nextY + objHeight > walls[i].y) {
			return true;
		}
	}
	return false;
}

void calculateItemPosition(TreasureBox box, int * itemX, int * itemY) {
	int itemSize = 30;
	int itemPadding = 5;

	if (!isCollidingWithWalls(box.x + box.w + itemPadding, box.y, itemSize, itemSize)) {
		*itemX = box.x + box.w + itemPadding;
		*itemY = box.y + (box.h - itemSize) / 2;
	}
	else if (!isCollidingWithWalls(box.x - itemSize - itemPadding, box.y, itemSize, itemSize)) {
		*itemX = box.x - itemSize - itemPadding;
		*itemY = box.y + (box.h - itemSize) / 2;
	}
	else if (!isCollidingWithWalls(box.x, box.y - itemSize - itemPadding, itemSize, itemSize)) {
		*itemX = box.x + (box.w - itemSize) / 2;
		*itemY = box.y - itemSize - itemPadding;
	}
	else {
		*itemX = box.x + (box.w - itemSize) / 2;
		*itemY = box.y + box.h + itemPadding;
	}
}


void drawMenu() {
	iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, menuBackgroundTexture);

	
	char titlePart1[] = "DREAM LAYERS:";
	char titlePart2[] = "ESCAPE THE MAZE";
	int titlePart1Width = iTextWidth(titlePart1, GLUT_BITMAP_TIMES_ROMAN_24);
	int titlePart2Width = iTextWidth(titlePart2, GLUT_BITMAP_TIMES_ROMAN_24);
	int totalTitleWidth = titlePart1Width + titlePart2Width + 25;
	int xPosPart1 = (WINDOW_WIDTH - totalTitleWidth) / 2;
	int xPosPart2 = xPosPart1 + titlePart1Width + 25;
	int yPosTitle = WINDOW_HEIGHT - 150;

	iSetColor(0, 0, 0);
	iText(xPosPart1 + 3, yPosTitle - 3, titlePart1, GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 255, 255);
	iText(xPosPart1, yPosTitle, titlePart1, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(0, 0, 0);
	iText(xPosPart2 + 3, yPosTitle - 3, titlePart2, GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 215, 0);
	iText(xPosPart2, yPosTitle, titlePart2, GLUT_BITMAP_TIMES_ROMAN_24);

	int startY = WINDOW_HEIGHT / 2 + 100;
	for (int i = 0; i < numMenuOptions; i++) {
		char * optionText = menuOptions[i];
		void * font;
		int textWidth;
		int currentPulseOffset = 0;

		if (i == selectedOption) {
			font = GLUT_BITMAP_TIMES_ROMAN_24;
			textWidth = iTextWidth(optionText, font);
			currentPulseOffset = (int)(12 * sin(menu_pulse_effect));
		}
		else {
			font = GLUT_BITMAP_HELVETICA_18;
			textWidth = iTextWidth(optionText, font);
		}

		int xPos = (WINDOW_WIDTH - textWidth) / 2;
		int yPos = startY - i * 50;

		iSetColor(0, 0, 0);
		iText(xPos + 4, yPos - 4, optionText, font);

		if (i == selectedOption) {
			iSetColor(255, 255, 0);
			iText(xPos + currentPulseOffset, yPos, optionText, font);
		}
		else if (strcmp(optionText, "Continue Game") == 0 && !saveGameExists) {
			iSetColor(120, 120, 120);
			iText(xPos, yPos, optionText, font);
		}
		else {
			iSetColor(255, 255, 255);
			iText(xPos, yPos, optionText, font);
		}
	}
}

void drawHowToPlay() {
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	iSetColor(255, 255, 255);
	iText(WINDOW_WIDTH / 2 - iTextWidth("HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT - 100, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(100, WINDOW_HEIGHT - 200, "1. Use ARROW keys to move.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 230, "2. Open treasure boxes to find items.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 260, "3. If you find the key, walk over it to collect it.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 290, "4. The coin gives you a temporary speed boost!", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 320, "5. Once the key is collected, touch the door to win.", GLUT_BITMAP_HELVETICA_18);
	iSetColor(255, 100, 100);
	iText(100, WINDOW_HEIGHT - 350, "6. WATCH OUT FOR THE DREAM GUARDIANS!", GLUT_BITMAP_HELVETICA_18);
	iSetColor(255, 255, 255);
	iText(100, 100, "Press ESC to return to Main Menu", GLUT_BITMAP_HELVETICA_18);
}

void drawHighScores() {
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	iSetColor(255, 255, 255);
	iText(WINDOW_WIDTH / 2 - iTextWidth("HIGH SCORES", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT - 100, "HIGH SCORES", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(WINDOW_WIDTH / 2 - iTextWidth("No high scores yet!", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2, "No high scores yet!", GLUT_BITMAP_HELVETICA_18);
	iText(100, 100, "Press ESC to return to Main Menu", GLUT_BITMAP_HELVETICA_18);
}

void drawStoryLine() {
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	iSetColor(255, 255, 255);
	iText(WINDOW_WIDTH / 2 - iTextWidth("STORY LINE", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT - 100, "STORY LINE", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(100, WINDOW_HEIGHT - 200, "You wake up, lost inside a dream. But this is no ordinary dream.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 230, "The only escape is through layers � each deeper, darker, and harder.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 260, "Find the hidden totem to open the door, or remain trapped forever.", GLUT_BITMAP_HELVETICA_18);
	iText(100, WINDOW_HEIGHT - 290, "Avoid the guardians. Survive the traps. Beat the dream.", GLUT_BITMAP_HELVETICA_18);
	iText(100, 100, "Press ESC to return to Main Menu", GLUT_BITMAP_HELVETICA_18);
}

void drawCredits() {
	iSetColor(0, 0, 0);
	iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	iSetColor(255, 255, 255);
	iText(WINDOW_WIDTH / 2 - iTextWidth("CREDITS", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT - 100, "CREDITS", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(WINDOW_WIDTH / 2 - iTextWidth("Game Design & Programming:", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2 + 60, "Game Design & Programming:", GLUT_BITMAP_HELVETICA_18);
	iText(WINDOW_WIDTH / 2 - iTextWidth("Sanjida Jahan, Annesha Datta Arin, Ishrat Jannat", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2 + 30, "Sanjida Jahan, Annesha Datta Arin, Ishrat Jannat", GLUT_BITMAP_HELVETICA_18);
	iText(WINDOW_WIDTH / 2 - iTextWidth("Supervised by:", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2 - 10, "Supervised by:", GLUT_BITMAP_HELVETICA_18);
	iText(WINDOW_WIDTH / 2 - iTextWidth("Saha Reno, Mustofa Ahmed", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2 - 40, "Saha Reno, Mustofa Ahmed", GLUT_BITMAP_HELVETICA_18);
	iText(100, 100, "Press ESC to return to Main Menu", GLUT_BITMAP_HELVETICA_18);
}

void drawGameplay() {
	if (musicPlaying) {
		PlaySound(NULL, NULL, 0);
		musicPlaying = false;
	}
	if (gameWon) {
		currentGameState = GAME_OVER_WIN;
	}

	
	iSetColor(25, 25, 40);
	iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	iSetColor(100, 100, 120);
	for (int i = 0; i < STAR_COUNT; i++) {
		iFilledCircle(stars[i].x, stars[i].y, stars[i].size);
	}

	
	for (int i = 0; i < wallCount; i++) {
		drawBrickPattern(walls[i].x, walls[i].y, walls[i].w, walls[i].h);
	}


	for (int i = 0; i < currentTreasureCount; i++) {
		if (!treasureBoxes[i].isOpen) {
			iShowImage(treasureBoxes[i].x, treasureBoxes[i].y, treasureBoxes[i].w, treasureBoxes[i].h, treasureBoxClosedTexture);
		}
		else {
			iShowImage(treasureBoxes[i].x, treasureBoxes[i].y, treasureBoxes[i].w, treasureBoxes[i].h, treasureBoxOpenedTexture);
			if (!treasureBoxes[i].isItemCollected && treasureBoxes[i].content != EMPTY) {
				int itemX, itemY;
				calculateItemPosition(treasureBoxes[i], &itemX, &itemY);
				if (treasureBoxes[i].content == KEY) {
					iShowImage(itemX, itemY, 30, 30, keyTexture);
				}
				else if (treasureBoxes[i].content == COIN) {
					iShowImage(itemX, itemY, 30, 30, coinTexture);
				}
				else if (treasureBoxes[i].content == SHIELD) {
					iShowImage(itemX, itemY, 30, 30, shieldTexture);
				}
				else if (treasureBoxes[i].content == TIME_BONUS) {
					iShowImage(itemX, itemY, 30, 30, timeBonusTexture);
				}
			}
		}
	}

	
	if (keyCollected) {
		int goalPulseSize = (int)(4 * sin(pulse_effect + 1) + 4);
		iSetColor(255, 215, 0);
		iFilledCircle(goalX + doorWidth / 2, goalY + doorHeight / 2, doorWidth / 2 + goalPulseSize);
	}
	iShowImage(goalX, goalY, doorWidth, doorHeight, doorTexture);

	
	if (showKeyMessage) {
		char msg[] = "Key required";
		int textWidth = iTextWidth(msg, GLUT_BITMAP_HELVETICA_18);
		int textX = goalX + (doorWidth - textWidth) / 2;
		int textY = goalY + doorHeight + 10;
		iSetColor(0, 0, 0);
		iText(textX + 2, textY - 2, msg, GLUT_BITMAP_HELVETICA_18);
		iSetColor(255, 255, 255);
		iText(textX, textY, msg, GLUT_BITMAP_HELVETICA_18);
	}
	char levelMsg[50];
	sprintf(levelMsg, "Level %d", currentLevel);
	iSetColor(255, 255, 255);
	iText(10, WINDOW_HEIGHT - 30, levelMsg, GLUT_BITMAP_HELVETICA_18);

	char timerMsg[50];
	sprintf(timerMsg, "Time: %.1fs", (float)levelTimer * 20 / 1000.0);
	if (levelTimer < 500) {
		iSetColor(255, 0, 0);
	}
	else {
		iSetColor(255, 255, 255);
	}
	iText(10, WINDOW_HEIGHT - 50, timerMsg, GLUT_BITMAP_HELVETICA_18);

	if (keyCollected) {
		iSetColor(255, 215, 0);
		iText(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 30, "Key Found!", GLUT_BITMAP_HELVETICA_18);
	}

	if (isSpeedBoostActive) {
		char speedMsg[50];
		sprintf(speedMsg, "Speed Boost! %.1fs", (float)speedBoostTimer * 20 / 1000.0);
		iSetColor(50, 200, 255);
		iText(10, 30, speedMsg, GLUT_BITMAP_HELVETICA_18);
	}

	if (isShieldActive) {
		char shieldMsg[50];
		sprintf(shieldMsg, "Shield Active! %.1fs", (float)shieldTimer * 20 / 1000.0);
		iSetColor(0, 255, 0);
		iText(10, 50, shieldMsg, GLUT_BITMAP_HELVETICA_18);
	}

	
	iShowImage(playerX, playerY, playerWidth, playerHeight, playerTexture);
	for (int i = 0; i < currentEnemyCount; i++) {
		iShowImage(guardians[i].x, guardians[i].y, guardians[i].w, guardians[i].h, guardians[i].currentTexture);
	}
}


void iDraw() {
	iClear();
	switch (currentGameState) {
	case MENU:
		drawMenu();
		if (!musicPlaying) {
			PlaySound(TEXT(MUSIC_FILE_NAME), NULL, SND_ASYNC | SND_LOOP);
			musicPlaying = true;
		}
		break;
	case HOW_TO_PLAY:
		drawHowToPlay();
		break;
	case HIGH_SCORES:
		drawHighScores();
		break;
	case STORY_LINE:
		drawStoryLine();
		break;
	case CREDITS:
		drawCredits();
		break;
	case GAME_PLAY:
		drawGameplay();
		break;
	case GAME_OVER_WIN:
		iSetColor(0, 0, 0);
		iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		iSetColor(0, 255, 128);
		iText(WINDOW_WIDTH / 2 - iTextWidth("YOU ESCAPED THE DREAM!", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT / 2 + 50, "YOU ESCAPED THE DREAM!", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(255, 255, 255);
		iText(WINDOW_WIDTH / 2 - iTextWidth("Press ESC to return to Menu", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2, "Press ESC to return to Menu", GLUT_BITMAP_HELVETICA_18);
		if (musicPlaying) {
			PlaySound(NULL, NULL, 0);
			musicPlaying = false;
		}
		break;
	case GAME_OVER_LOSE:
		iSetColor(0, 0, 0);
		iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		iSetColor(255, 0, 0);
		iText(WINDOW_WIDTH / 2 - iTextWidth("GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24) / 2, WINDOW_HEIGHT / 2 + 50, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
		iSetColor(255, 255, 255);
		if (levelTimer <= 0) {
			iText(WINDOW_WIDTH / 2 - iTextWidth("Time's up! The dream trapped you.", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2, "Time's up! The dream trapped you.", GLUT_BITMAP_HELVETICA_18);
		}
		else {
			iText(WINDOW_WIDTH / 2 - iTextWidth("The dream has consumed you.", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2, "The dream has consumed you.", GLUT_BITMAP_HELVETICA_18);
		}
		iText(WINDOW_WIDTH / 2 - iTextWidth("Press ESC to return to Menu", GLUT_BITMAP_HELVETICA_18) / 2, WINDOW_HEIGHT / 2 - 40, "Press ESC to return to Menu", GLUT_BITMAP_HELVETICA_18);
		if (musicPlaying) {
			PlaySound(NULL, NULL, 0);
			musicPlaying = false;
		}
		break;
	}
}

void update() {
	pulse_effect += 0.1;
	menu_pulse_effect += 0.05;

	if (keyMessageTimer > 0) {
		keyMessageTimer--;
		if (keyMessageTimer == 0) {
			showKeyMessage = false;
		}
	}

	if (currentGameState == GAME_PLAY) {
		if (levelTimer > 0) {
			levelTimer--;
			if (levelTimer <= 0) {
				currentGameState = GAME_OVER_LOSE;
			}
		}

		if (isSpeedBoostActive) {
			speedBoostTimer--;
			if (speedBoostTimer <= 0) {
				isSpeedBoostActive = false;
				moveStep = originalMoveStep;
			}
		}

		if (isShieldActive) {
			shieldTimer--;
			if (shieldTimer <= 0) {
				isShieldActive = false;
			}
		}

		if (timeBonusActive) {
			levelTimer += timeBonusAmount;
			timeBonusActive = false;
			timeBonusAmount = 0;
		}

		for (int i = 0; i < currentEnemyCount; i++) {
			int nextX = guardians[i].x + guardians[i].dx;
			int nextY = guardians[i].y + guardians[i].dy;
			if (isCollidingWithWalls(nextX, nextY, guardians[i].w, guardians[i].h)) {
				if (guardians[i].movesHorizontally) {
					guardians[i].dx = -guardians[i].dx;
					guardians[i].currentTexture = (guardians[i].dx > 0) ? guardianTexture1 : guardianTexture2;
				}
				else {
					guardians[i].dy = -guardians[i].dy;
					guardians[i].currentTexture = (guardians[i].dy > 0) ? guardianTexture1 : guardianTexture2;
				}
			}
			else {
				guardians[i].x = nextX;
				guardians[i].y = nextY;
			}
		}
		checkPlayerEnemyCollision();
	}
}


void iKeyboard(unsigned char key) {
	if (key == 'q' || key == 'Q') {
		exit(0);
	}
	if (key == 27) {
		if (currentGameState != MENU) {
			if (currentGameState == GAME_PLAY) {
				saveGame();
			}
			currentGameState = MENU;
			selectedOption = 0;
			checkIfSaveGameExists();
		}
		return;
	}
	if (currentGameState == MENU && key == '\r') {
		if (musicPlaying) {
			PlaySound(NULL, NULL, 0);
			musicPlaying = false;
		}
		switch (selectedOption) {
		case 0: resetGame(); break;
		case 1: if (saveGameExists) loadGame(); break;
		case 2: currentGameState = HOW_TO_PLAY; break;
		case 3: currentGameState = HIGH_SCORES; break;
		case 4: currentGameState = STORY_LINE; break;
		case 5: currentGameState = CREDITS; break;
		case 6: exit(0); break;
		}
		return;
	}
}

void iSpecialKeyboard(unsigned char key) {
	if (currentGameState == GAME_PLAY) {
		int nextX = playerX;
		int nextY = playerY;

		if (key == GLUT_KEY_RIGHT) nextX += moveStep;
		if (key == GLUT_KEY_LEFT) nextX -= moveStep;
		if (key == GLUT_KEY_UP) nextY += moveStep;
		if (key == GLUT_KEY_DOWN) nextY -= moveStep;

		for (int i = 0; i < currentTreasureCount; i++) {
			if (!treasureBoxes[i].isOpen &&
				nextX < treasureBoxes[i].x + treasureBoxes[i].w && nextX + playerWidth > treasureBoxes[i].x &&
				nextY < treasureBoxes[i].y + treasureBoxes[i].h && nextY + playerHeight > treasureBoxes[i].y) {
				treasureBoxes[i].isOpen = true;
			}
		}

		bool isTouchingDoor = (nextX < goalX + doorWidth && nextX + playerWidth > goalX &&
			nextY < goalY + doorHeight && nextY + playerHeight > goalY);
		if (isTouchingDoor) {
			if (keyCollected) {
				if (currentLevel < MAX_LEVELS) {
					nextLevel();
				}
				else {
					gameWon = true;
				}
			}
			else {
				showKeyMessage = true;
				keyMessageTimer = 150;
			}
			return;
		}

		if (!isCollidingWithWalls(nextX, nextY, playerWidth, playerHeight)) {
			playerX = nextX;
			playerY = nextY;
		}

		for (int i = 0; i < currentTreasureCount; i++) {
			if (treasureBoxes[i].isOpen && !treasureBoxes[i].isItemCollected && treasureBoxes[i].content != EMPTY) {
				int itemX, itemY;
				calculateItemPosition(treasureBoxes[i], &itemX, &itemY);
				int itemSize = 30;

				if (playerX < itemX + itemSize && playerX + playerWidth > itemX &&
					playerY < itemY + itemSize && playerY + playerHeight > itemY) {
					treasureBoxes[i].isItemCollected = true;
					if (treasureBoxes[i].content == KEY) {
						keyCollected = true;
					}
					else if (treasureBoxes[i].content == COIN) {
						isSpeedBoostActive = true;
						speedBoostTimer = SPEED_BOOST_DURATION;
						moveStep = originalMoveStep + 5;
					}
					else if (treasureBoxes[i].content == SHIELD) {
						isShieldActive = true;
						shieldTimer = SHIELD_DURATION;
					}
					else if (treasureBoxes[i].content == TIME_BONUS) {
						timeBonusActive = true;
						timeBonusAmount = 500;
					}
				}
			}
		}
	}
	else if (currentGameState == MENU) {
		if (key == GLUT_KEY_UP) {
			selectedOption = (selectedOption - 1 + numMenuOptions) % numMenuOptions;
		}
		else if (key == GLUT_KEY_DOWN) {
			selectedOption = (selectedOption + 1) % numMenuOptions;
		}
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my) {}


int main() {
	srand(time(NULL));
	iSetTimer(20, update); 

	iInitialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Dream Layers: Escape The Maze");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	playerTexture = iLoadImage("one.png");
	menuBackgroundTexture = iLoadImage("bg.bmp");
	doorTexture = iLoadImage("door.png");
	guardianTexture1 = iLoadImage("dreamGuardian_1-removebg-preview.png");
	guardianTexture2 = iLoadImage("dreamGuardian_2-removebg-preview.png");
	treasureBoxClosedTexture = iLoadImage("treasureBoxClosed.png");
	treasureBoxOpenedTexture = iLoadImage("treasureBoxOpened.png");
	keyTexture = iLoadImage("key.png");
	coinTexture = iLoadImage("coin.png");
	shieldTexture = iLoadImage("shield.png");
	timeBonusTexture = iLoadImage("treasure1.png");

	initializeStars();
	initializeEnemies();
	checkIfSaveGameExists();

	iStart(); 
	return 0;
}