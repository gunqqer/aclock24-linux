/*
 * aclock - ascii clock for UNIX Console
 *
 * Copyright (c) 2002 Antoni Sawicki <as@tenoware.com>
 * Version 2.3 (unix-curses); Mountain View, July 2013
 *
 * Compilation: cc aclock-unix-curses.c -o aclock -lcurses -lm
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define FontWH_Ratio 2
#define Number_InsetDistance 1
#define HR24 1
#define Draw_Numbers 1

void draw_circle(int hand_max, int sYcen, int sXcen)
{
	int x, y, r;
	char c;

	for (r = 0; r < 60; r++)
	{
		x = cos(r * M_PI / 180 * 6) * hand_max * FontWH_Ratio + sXcen;
		y = sin(r * M_PI / 180 * 6) * hand_max + sYcen;
		switch (r)
		{
		case 0:
		case 5:
		case 10:
		case 15:
		case 20:
		case 25:
		case 30:
		case 35:
		case 40:
		case 45:
		case 50:
		case 55: c = 'o'; break;
		default: c = '.'; break;
		}
		mvaddch(y, x, c);
	}

	// Draw numbers
	if (!Draw_Numbers) { return; }
	int angle   = 360 / (HR24 ? 24 : 12);
	int xLength = (hand_max - Number_InsetDistance) * FontWH_Ratio;
	int yLength = (hand_max - Number_InsetDistance);

	for (r = 1; r < (HR24 ? 24 : 12) + 1; r++)
	{

		x = cos(((r * angle) - 90) * (M_PI / 180)) * xLength + sXcen;
		y = sin(((r * angle) - 90) * (M_PI / 180)) * yLength + sYcen;
		char str[3];
		sprintf(str, "%02d", r);
		mvaddstr(y, x, str);
	}
}

void draw_hand(int minute, int hlenght, char c, int sXcen, int sYcen)
{
	int x, y, n;
	float r = (minute - 15) * (M_PI / 180) * 6;

	for (n = 1; n < hlenght; n++)
	{
		x = cos(r) * n * FontWH_Ratio + sXcen;
		y = sin(r) * n + sYcen;
		mvaddch(y, x, c);
	}
}

int main(void)
{
	char INFO[] = "Copyright (c) 1994-2013 Antoni Sawicki <as@tenoware.com>\n"
	              "Version 2.3 (unix-curses); Mountain View, July 2013\n"
				  "Updated 2023 for 24hr by <gunqqer@friithian.dev>\n";
	char digital_time[15];
	int sXmax, sYmax, smax, hand_max, sXcen, sYcen;
	time_t t;
	struct tm * ltime;

	sXmax = sYmax = hand_max = sXcen = sYcen = 0;
	initscr();

	while (1)
	{
		time(&t);
		ltime = localtime(&t);
		sXmax = COLS;
		sYmax = LINES;

		if (sXmax / FontWH_Ratio <= sYmax)
			smax = sXmax / FontWH_Ratio;
		else
			smax = sYmax;

		hand_max = (smax / 2) - 1;

		sXcen = sXmax / 2;
		sYcen = sYmax / 2;

		erase();
		draw_circle(hand_max, sYcen, sXcen);

		draw_hand(
		    ((ltime->tm_hour * 5) + (ltime->tm_min / 10)) / (HR24 ? 2 : 1), //Half the minutes/angle if a full revolution is 24 hours
		    2 * hand_max / 3,
		    'h',
		    sXcen,
		    sYcen);
		draw_hand(ltime->tm_min, hand_max - 2, 'm', sXcen, sYcen);
		draw_hand(ltime->tm_sec, hand_max - 1, '.', sXcen, sYcen);

		const char* title = (HR24 ? ".:ACLOCK24:." : ".:ACLOCK:.");
		mvaddstr(sYcen - (3 * hand_max / 5), sXcen - 5, title); //TODO Fix the offset for the 24
		mvprintw(
		    sYcen + (3 * hand_max / 5),
		    sXcen - 5,
		    "[%02d:%02d:%02d]",
		    ltime->tm_hour,
		    ltime->tm_min,
		    ltime->tm_sec);

		refresh();
		sleep(1);
	}
	endwin();
	return 0;
}
