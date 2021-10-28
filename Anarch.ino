/**
 @file main_nibble.ino

 This is Nibble (CircuitMess) implementation of the game front end.

 by Miloslav Ciz (drummyfish), 2021

 Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
 plus a waiver of all other intellectual property. The goal of this work is to
 be and remain completely in the public domain forever, available for any use
 whatsoever.
*/

#include <Arduino.h>
#include <CircuitOS.h>
#include <ByteBoi.h>
#include <lgfx/v1/misc/colortype.hpp>

#define PLUS_BRIGHTNESS 0 // this can be changed (max: 8)

#define SFG_AVR 1

#define SFG_SCREEN_RESOLUTION_X 160
#define SFG_SCREEN_RESOLUTION_Y 120
#define SFG_FPS 35

#define SFG_RAYCASTING_MAX_STEPS 40
#define SFG_RAYCASTING_SUBSAMPLE 2
#define SFG_DIMINISH_SPRITES 1
#define SFG_DITHERED_SHADOW 1

#define SFG_CAN_EXIT 1/* If the game is compiled into loeader, this can be set
                          to 1 which will show the "exit" option in the menu. */
#include "src/game.h"
#include "src/Anarch.h"
Display* display;
Sprite* sprite;
uint8_t buttons[7];
uint16_t paletteRAM[256];
Anarch* game;

void SFG_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
	Color c = paletteRAM[colorIndex];
	lgfx::rgb565_t c2(c);
	c2.set(c2.R8(),c2.G8(),c2.B8());
	game->drawPixel(x, y,c2.operator unsigned short());
}

uint32_t SFG_getTimeMs()
{
	return millis();
}

void SFG_sleepMs(uint16_t timeMs)
{
	delay(timeMs);
}

int8_t SFG_keyPressed(uint8_t key)
{
	return key < 7 ? buttons[key] : 0;
}

void SFG_getMouseOffset(int16_t *x, int16_t *y)
{
}

void SFG_setMusic(uint8_t value)
{
}

void SFG_save(uint8_t data[SFG_SAVE_SIZE])
{
}

void SFG_processEvent(uint8_t event, uint8_t data)
{
}

uint8_t SFG_load(uint8_t data[SFG_SAVE_SIZE])
{
	return 0;
}

void SFG_playSound(uint8_t soundIndex, uint8_t volume)
{
	switch (soundIndex)
	{
		case 0: Playback.tone(200, 45,Wave::SAW); break; // shot
		case 1: Playback.tone(200, 30,Wave::SAW); break; // door
		case 2: Playback.tone(80, 60,Wave::SAW);  break; // explosion
		case 3: Playback.tone(220, 50,Wave::SAW); break; // click
		case 4: Playback.tone(180, 60,Wave::SAW); break; // plasma
		case 5: Playback.tone(300, 10,Wave::SQUARE); break; // monster
		default: break;
	}
}

// create button callbacks:

#define cbf(b,n) void b ## _down() { buttons[n] = 255; } void b ## _up() { buttons[n] = 0; }
cbf(BTN_UP,0)
cbf(BTN_RIGHT,1)
cbf(BTN_DOWN,2)
cbf(BTN_LEFT,3)
cbf(BTN_A,4)
cbf(BTN_B,5)
cbf(BTN_C,6)
#undef cbf

void setup()
{
	Serial.begin(115200);
	ByteBoi.begin();
	display = ByteBoi.getDisplay();
	//ByteBoi.bindMenu();
	BatteryPopup.enablePopups(true);
	ByteBoi.setGameID("Anarc");
	sprite = display->getBaseSprite();

	SFG_init();

	for (uint8_t i = 0; i < 7; ++i)
		buttons[i] = 0;

	// move palette to RAM plus increase brightness of the colors:

	for (int i = 0; i < 256; ++i)
	{
		int helper = i % 8;
		helper = (helper < 8 - PLUS_BRIGHTNESS) ? PLUS_BRIGHTNESS : (7 - helper);
		paletteRAM[i] = pgm_read_word(paletteRGB565 + i + helper);
	}

	// register button callbacks:

#define cb(b) \
    Input::getInstance()->setBtnPressCallback(b,b ## _down); \
    Input::getInstance()->setBtnReleaseCallback(b,b ## _up);

	cb(BTN_UP)
	cb(BTN_DOWN)
	cb(BTN_LEFT)
	cb(BTN_RIGHT)
	cb(BTN_A)
	cb(BTN_B)
	cb(BTN_C)

#undef cb

	game = new Anarch(display);
	game->unpack();
	ByteBoi.splash();
	game->start();
}

void loop()
{
	LoopManager::loop();
}

