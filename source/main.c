#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NULL
#define NULL   ((void *) 0) //to get rid of Eclipse's NULL errors
#endif
#define DEFfloorHeight 15
#define DEFflappyX 75
#define DEFpipeCount 6
#define DEFspaceHorizontal 65
#define DEFpipeSpace 70
#define DEFpipeWidth 35
#define DEFflappyPixelSize 13
#define DEFpower 17
#define DEFvelDivider 3
#define DEFvelChange 1
#define DEFspeed 2
#define ver "1.1"

struct pipe {
	s16 posX;
	u8 height;
} pipes[DEFpipeCount];

enum gameState {
	RUNNING, GAMEOVER, MAIN, OPTIONS
} gState;

s32 i;
s32 j;
touchPosition tPos;
s16 vel;
u8 altitude;
u16 score;
u16 highscore;
s16 oldPos;
s16 scoreSpot;
u8 isHighscored;
u8 floorHeight;
u16 flappyX;
u8 pipeCount;
u16 spaceHorizontal;
u8 pipeSpace;
u8 pipeWidth;
u8 flappyPixelSize;
s8 power;
s8 velDivider;
s8 velChange;
s8 speed;
u8 isDefaultSettings;
u8 printed;
u8 counter;

void clrScreen(u8 *fb) {
	memset(fb, 0, 240 * 400 * 3);
}

void setPixel(u8 *fb, u16 x, u8 y, u8 red, u8 green, u8 blue) {
	fb[3 * (240 - y + (x - 1) * 240)] = blue;
	fb[3 * (240 - y + (x - 1) * 240) + 1] = green;
	fb[3 * (240 - y + (x - 1) * 240) + 2] = red;
}

void drawRectangle(u8 *fb, u16 x1, u8 y1, u16 x2, u8 y2, u8 red, u8 green,
		u8 blue) {
	u16 y;
	while (x1 <= x2) {
		y = y1;
		while (y <= y2) {
			setPixel(fb, x1, y, red, green, blue);
			y++;
		}
		x1++;
	}
}

void resetOptions() {
	floorHeight = DEFfloorHeight;
	flappyX = DEFflappyX;
	pipeCount = DEFpipeCount;
	spaceHorizontal = DEFspaceHorizontal;
	pipeSpace = DEFpipeSpace;
	pipeWidth = DEFpipeWidth;
	flappyPixelSize = DEFflappyPixelSize;
	power = DEFpower;
	velDivider = DEFvelDivider;
	velChange = DEFvelChange;
	speed = DEFspeed;

	isDefaultSettings = 1;
}

void resetGame() {
	vel = 16;
	altitude = 120;
	score = 0;
	gState = RUNNING;
	isHighscored = 0;
	i = 0;
	counter = 0;
	while (i < pipeCount) {
		pipes[i].posX = 401 + i * (spaceHorizontal + pipeWidth);
		i++;
	}
	printed = 0;
	scoreSpot = flappyX + flappyPixelSize / 2 - pipeWidth / 2;
}

void loadHighScore() {
	FILE *file = fopen("flappypixel.bin", "rb");
	if (file == NULL) {
		highscore = 0;
		return;
	}
	fread(&highscore, 1, 2, file);
	fclose(file);
}

void saveHighScore() {
	FILE *file = fopen("flappypixel.bin", "w+b");
	if (file == NULL) {
		return;
	}
	fwrite(&highscore, 2, 1, file);
	fclose(file);
}

u8 touchControl(u16 x, u8 y, u16 x2, u8 y2) {
	if (tPos.px >= x && tPos.px <= x2 && tPos.py >= y && tPos.py <= y2)
		return 1;
	else
		return 0;
}

int main() {
	gfxInitDefault();
	//gfxSet3D(true);
	consoleInit(GFX_BOTTOM, NULL);

	resetOptions();
	resetGame();
	loadHighScore();
	gState = MAIN;

	// Main loop
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		hidTouchRead(&tPos);

		/***** Game Mechanichs *****/

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		if (gState == RUNNING) {
			if (kDown & KEY_A || kDown & KEY_B || kDown & KEY_Y || kDown & KEY_X
					|| kDown & KEY_L || kDown & KEY_R || kDown & KEY_ZL
					|| kDown & KEY_ZR || kDown & KEY_TOUCH || kDown & KEY_UP
					|| kDown & KEY_CPAD_UP || kDown & KEY_CSTICK_UP)
				vel = power;

			altitude += vel / velDivider;
			vel -= velChange;
			//		if (vel <= -18)
			//			vel = -18;

			if (altitude < floorHeight + flappyPixelSize) {
				vel = 12;
				altitude = floorHeight + flappyPixelSize;
			}
			if (altitude > 239) {
				vel = 0;
				altitude = 239;
			}
			i = 0;
			while (i < pipeCount) {
				oldPos = pipes[i].posX;
				pipes[i].posX -= speed;
				if (pipes[i].posX + pipeWidth < 1) { //a pipe is far left on the screen, so move it next to most far right pipe
					s8 before = i - 1;
					if (before <= -1)
						before = pipeCount - 1;
					pipes[i].posX = pipes[before].posX + spaceHorizontal
							+ pipeWidth;
					if(isDefaultSettings==0){
						srand(osGetTime());
						pipes[i].height = rand()
								% (240 - floorHeight - pipeSpace - 40) + 20;
					}
				}
				if (pipes[i].posX <= 400 && oldPos > 400) { //randomize pipe height when it arrives to far right of the screen
					srand(osGetTime());
					pipes[i].height = rand()
							% (240 - floorHeight - pipeSpace - 40) + 20;
				}
				if (pipes[i].posX <= scoreSpot && oldPos > scoreSpot) {
					score++;
					printed = 0;
				}

				//detect collision
				if (pipes[i].posX
						>= scoreSpot - pipeWidth / 2 - flappyPixelSize / 2
						&& pipes[i].posX
								<= scoreSpot + pipeWidth / 2
										+ flappyPixelSize / 2) { //this checks if this pipe is a place where player potentially hit it
					if (240 - altitude < pipes[i].height
							|| 240 - altitude + flappyPixelSize
									> pipes[i].height + pipeSpace) {
						gState = GAMEOVER;
						printed = 0;
						if (score > highscore && isDefaultSettings == 1) {
							isHighscored = 1;
							highscore = score;
							saveHighScore();
						}
					}
				}
				i++;
			}
		} else if (gState == GAMEOVER) {
			if (kDown & KEY_Y)
				resetGame();
			else if (kDown & KEY_TOUCH && touchControl(30, 145, 275, 163))
				resetGame();
		} else if (gState == MAIN) {
			if (kDown & KEY_A)
				resetGame();
		} else if (gState == OPTIONS) {
			if (kDown & KEY_A) {
				printed = 0;
				gState = MAIN;
			} else if (kDown & KEY_B) {
				printed = 0;
				isDefaultSettings = 1;
				resetOptions();
				gState = MAIN;
			} else if (kDown & KEY_X) {
				isDefaultSettings = 1;
				resetOptions();
				printed = 0;
			}
			if (kDown & KEY_TOUCH)
				counter = 0;
			if (tPos.px >= 120 && tPos.px <= 145) {
				if (tPos.py > 36 && tPos.py <= 52) {
					if (counter == 0) {
						flappyPixelSize--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 68) {
					if (counter == 0) {
						pipeWidth--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 84) {
					if (counter == 0) {
						spaceHorizontal--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 100) {
					if (counter == 0) {
						pipeSpace--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 116) {
					if (counter == 0 && floorHeight>5) {
						floorHeight--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 132) {
					if (counter == 0) {
						power--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 148) {
					if (counter == 0) {
						velDivider--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 164) {
					if (counter == 0) {
						velChange--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 180) {
					if (counter == 0 && speed>1) {
						speed--;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 196) {
					if (flappyX > 1) {
						flappyX--;
						isDefaultSettings = 0;
					}
					counter++;
				}
				printed = 0;
			}
			if (tPos.px >= 168 && tPos.px <= 193) {
				if (tPos.py > 36 && tPos.py <= 52) {
					if (counter == 0) {
						flappyPixelSize++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 68) {
					if (counter == 0) {
						pipeWidth++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 84) {
					if (counter == 0) {
						spaceHorizontal++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 100) {
					if (counter == 0) {
						pipeSpace++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 116) {
					if (counter == 0 && floorHeight<237) {
						floorHeight++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 132) {
					if (counter == 0) {
						power++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 148) {
					if (counter == 0) {
						velDivider++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 164) {
					if (counter == 0) {
						velChange++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 180) {
					if (counter == 0) {
						speed++;
						isDefaultSettings = 0;
					}
					counter++;
				} else if (tPos.py <= 196) {
					if (flappyX < 400) {
						flappyX++;
						isDefaultSettings = 0;
					}
					counter++;
				}
				printed = 0;
			}
			if (counter >= 6)
				counter = 0;
			altitude = 120 + flappyPixelSize / 2;
			srand(1337);
			i = 0;
			while (i < pipeCount) {
				pipes[i].posX = 20 + i * (spaceHorizontal + pipeWidth);
				pipes[i].height = (rand() + i * 1337)
						% (240 - floorHeight - pipeSpace - 40) + 20;
				i++;
			}
		}
		if (gState == GAMEOVER || gState == MAIN) {
			if (kDown & KEY_TOUCH && touchControl(45, 215, 260, 239) == 1) {
				gState = OPTIONS;
				printed = 0;
			}
		}

		/***** Draw Stuff *****/

		u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		//u8* fb2 = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
		//clrScreen(fb);

		//draw blue background
		drawRectangle(fb, 1, 1, 400, 240 - floorHeight, 150, 150, 255);

		//draw ground
		drawRectangle(fb, 1, 240 - floorHeight, 400, 240, 20, 170, 20);

		//draw pattern on ground
		i = 1;
		while (i <= 400) {
			setPixel(fb, i, 240 - floorHeight, 10, 100, 10);
			if ((i - pipes[0].posX + 1) % 8 == 0)
				setPixel(fb, i, 241 - floorHeight, 10, 100, 10);
			if ((i - pipes[0].posX + 2) % 8 == 0)
				setPixel(fb, i, 242 - floorHeight, 10, 100, 10);
			if ((i - pipes[0].posX + 3) % 8 == 0)
				setPixel(fb, i, 243 - floorHeight, 10, 100, 10);
			if ((i - pipes[0].posX + 4) % 8 == 0)
				setPixel(fb, i, 244 - floorHeight, 10, 100, 10);
			i++;
		}

		//draw pipes
		if (gState != MAIN) {
			i = 0;
			while (i < pipeCount) {
				u16 x1, x2;

				if (pipes[i].posX > 400) {
					i++;
					continue;
				} else if (pipes[i].posX < 1)
					x1 = 1;
				else
					x1 = pipes[i].posX;

				if (pipes[i].posX + pipeWidth > 400)
					x2 = 400;
				else if (pipes[i].posX + pipeWidth < 1)
					x2 = 1;
				else
					x2 = pipes[i].posX + pipeWidth;

				drawRectangle(fb, x1, 1, x2, pipes[i].height, 100, 100, 100); //top pipe
				drawRectangle(fb, x1, pipes[i].height + pipeSpace, x2,
						239 - floorHeight, 100, 100, 100); //bottom pipe
				i++;
			}
		}

		//draw flappy rectangle
		if (gState == RUNNING || gState == OPTIONS)
			drawRectangle(fb, flappyX, 240 - altitude,
					flappyX + flappyPixelSize, 240 + flappyPixelSize - altitude,
					200 + vel * -1.5, 200 + vel * 1.5, 200 + vel * 1.5);

		//draw it red when game over
		else if (gState == GAMEOVER)
			drawRectangle(fb, flappyX, 240 - altitude,
					flappyX + flappyPixelSize, 240 + flappyPixelSize - altitude,
					220, 0, 0);

		//print stuff
		if (printed == 0) {
			printf("\033[2J");
			if (gState == MAIN) {
				printf("\033[12;11fPress A to start");
				printf("\033[14;10fPress START to exit");
				printf("\033[17;15fHighscore");
				printf("\033[19;18f%d", highscore);
				printf("\033[0;0fVer. %s", ver);
				printf("\033[28;6fTap here to break the game");
				if (isDefaultSettings == 0) {
					printf("\033[6;10fYou broke the game!");
					printf("\033[7;7fHighscore won't be saved.");
				} else
					printf("\033[8;13fFlappy Pixel");
			} else if (gState == RUNNING) {
				printf("\033[8;16fScore");
				printf("\033[10;18f%d", score);
			} else if (gState == GAMEOVER) {
				printf("\033[6;14fGAME OVER");
				printf("\033[9;16fScore");
				printf("\033[11;18f%d", score);
				printf("\033[28;6fTap here to break the game");
				if (isHighscored == 0) {
					printf("\033[14;14fHighscore");
					printf("\033[16;18f%d", highscore);
				} else
					printf("\033[15;12fNEW HIGHSCORE!");
				printf("\033[19;4fPress Y or tap here to restart");
				printf("\033[21;10fPress START to exit");
			} else if (gState == OPTIONS) {
				printf("\033[1;1fA: Confirm   X: Default");
				printf("\033[2;1fB: Exit with default settings");
				printf("\033[26;0fMessing with these can\ncause crash/freeze.");
				i = 0;
				while (i < 10) {
					printf("\033[%d;16f-", i * 2 + 5);
					printf("\033[%d;22f+", i * 2 + 5);
					i++;
				}
				printf("\033[5;0fFlappy Size");
				printf("\033[7;0fPipe Width");
				printf("\033[9;0fHor. Pipe Space");
				printf("\033[11;0fVer. Pipe Space");
				printf("\033[13;0fFloor Height");
				printf("\033[15;0fFlap Power");
				printf("\033[17;0fVel. Divider");
				printf("\033[19;0fGravity");
				printf("\033[21;0fSpeed");
				printf("\033[23;0fX Pos.");

				printf("\033[5;18f%d", flappyPixelSize);
				printf("\033[7;18f%d", pipeWidth);
				printf("\033[9;18f%d", spaceHorizontal);
				printf("\033[11;18f%d", pipeSpace);
				printf("\033[13;18f%d", floorHeight);
				printf("\033[15;18f%d", power);
				printf("\033[17;18f%d", velDivider);
				printf("\033[19;18f%d", velChange);
				printf("\033[21;18f%d", speed);
				printf("\033[23;18f%d", flappyX);
			}
			printed = 1;
		}

		//printf("\033[29;0f%d %d          ", tPos.px, tPos.py);

		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}
