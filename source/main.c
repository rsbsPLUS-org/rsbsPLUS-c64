//SPDX-License-Identifier: BSD-3-Clause
//SPDX-FileCopyrightText: 2020 Lorenzo Cauli (lorecast162)

#include<c64.h>
#include <joystick.h>
#include<stdio.h>
#include<conio.h>

//macro to replicate BASIC V+n to get VIC addresses
#define VIC_ADDR(n) *((volatile unsigned char*)0xD000 + (n))
#define BG_COLOR VIC_ADDR(0x20)
#define FG_COLOR VIC_ADDR(0x21)
#define SPRITE_SETTINGS VIC_ADDR(0x15)
#define TEXT_COLOR *((volatile unsigned char*)0x0286)
#define SPRITE1_LOCATION *((volatile unsigned char*)0x07F8)
#define SPRITE1_DATA *((volatile unsigned char*)0x3000)
#define SPRITE1_SETPIXEL(p) *((volatile unsigned char*)0x3000 + (p))
#define SPRITE1_COLOR VIC_ADDR(39)
#define SPRITE1_X VIC_ADDR(0)
#define SPRITE1_Y VIC_ADDR(1)

const unsigned char sprite[] = {
	0x01,0xfc,0x00,0x07,0xff,0x00,0x0f,0xff,
	0x80,0x1f,0xff,0xc0,0x3f,0xff,0xe0,0x7f,
	0xff,0xf0,0x7f,0xff,0xf0,0xff,0xff,0xf8,
	0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,
	0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,
	0xff,0xf8,0x7f,0xff,0xf0,0x7f,0xff,0xf0,
	0x3f,0xff,0xe0,0x1f,0xff,0xc0,0x0f,0xff,
	0x80,0x07,0xff,0x00,0x01,0xfc,0x00,0x01,
};

int main() {
	char key;
	//for joystick error
	char err;

	unsigned char curPalette = 0;
	unsigned char prevPalette = 0;

	int i = 0;
	unsigned char joyStatus = 0;

	err = joy_install(c64_stdjoy_joy);
	if (err != JOY_ERR_OK) {
		printf("JOYSTICK DRIVER ERROR!. %x\n", err);
	}
	else {
		BG_COLOR = 0;
		FG_COLOR = 0;
		//clear screen
		asm("jsr $e544");
		//enable sprite 1
		SPRITE_SETTINGS = 1;
		SPRITE1_COLOR = COLOR_RED;
		//set sprite 1 memory location
		SPRITE1_LOCATION = 192;
		SPRITE1_X = 148;
		SPRITE1_Y = 100;
		//copy sprite data
		for (i = 0; i < 64; i++) {
			SPRITE1_SETPIXEL(i) = sprite[i];
		}
		//game loop
		for(;;) {
			//wait to get to raster line 1. simulates VSync
			while (VIC_ADDR(0x12) != 0x1) {}

			//get joystick 2 status
			joyStatus = joy_read(JOY_2);

			key = 0x20;
			if (kbhit()) key = cbm_k_getin();

			//horiz movement
			if (JOY_LEFT(joyStatus) || key == 0x9d) SPRITE1_X -= 1;
			else if (JOY_RIGHT(joyStatus) || key == 0x1d) SPRITE1_X += 1;

			//color switch
			if (JOY_UP(joyStatus) || key == 0x91) curPalette = 2;
			else if (JOY_DOWN(joyStatus) || key == 0x11) curPalette = 1;
			else curPalette = 0;

			if (curPalette != prevPalette) switch(curPalette) {
				case 0:
					SPRITE1_COLOR = COLOR_RED;
					break;
				case 1:
					SPRITE1_COLOR = COLOR_BLUE;
					break;
				case 2:
					SPRITE1_COLOR = COLOR_GREEN;
					break;

			}

			prevPalette = curPalette;
		}
	}
	return 0;
}
