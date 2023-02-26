//
//  main.c
//  Extension
//
//  Created by Dave Hayden on 7/30/14.
//  Copyright (c) 2014 Panic, Inc. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "pd.h"
#include "board.h"


void updateMenu(void);

LCDFont* font = NULL;

/*
#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = (400-TEXT_WIDTH)/2;
int y = (240-TEXT_HEIGHT)/2;
int dx = 1;
int dy = 2;
*/

void start(void){
	pd->system->logToConsole("Start");
	pd->display->setRefreshRate(30);
	font = loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft");
	initBoard();
	showBoard();
	newGame();
	updateMenu();
}

void updateMenu(void){
	menuBoard();
}

void update(void)
{
	updateBoard();
	pd->graphics->clear(kColorWhite);
	pd->sprite->updateAndDrawSprites();
	
	/*pd->graphics->setFont(font);
	pd->graphics->drawText("Hello zzzz", strlen("Hello zzzz"), kASCIIEncoding, x, y);

	x += dx;
	y += dy;
	
	if ( x < 0 || x > LCD_COLUMNS - TEXT_WIDTH )
		dx = -dx;
	
	if ( y < 0 || y > LCD_ROWS - TEXT_HEIGHT )
		dy = -dy;
     */   
	pd->system->drawFPS(0,0);

}

