#pragma once

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/io.h>

class ProGamer {
private:
	#define DEFAULT_REFRESH_RATE 50
	#define FLASH_LOOP_TIME 20

	#define DARK_TICKS_ON 1
	#define DARK_TICKS_OFF 3
	#define LIGHT_TICKS_ON 3
	#define LIGHT_TICKS_OFF 2

	enum RenderMode {
		RM_NONE,
		RM_SCORE,
		RM_STRING,
	};

public:
	#define UP 0
	#define LEFT 1
	#define RIGHT 2
	#define DOWN 3
	#define START 4
	//Note: Gamer v1.9 is Capacitive touch instead of LDR.
	#define LDR 5
	#define CAPTOUCH 6

	#define TRACK_SIZE(_TRACK_) (sizeof(_TRACK_) / sizeof(_TRACK_[0]))

	enum PixelColour {
      ZERO,
      DARK,
      LIGHT,
      ONE
    };

	struct Note {
		enum Pitch {
			E3 = 50,
			DSHARP3 = 57,
			D3 = 64,
			CSHARP3 = 68,
			C3 = 73,
			B3 = 77,
			ASHARP3 = 88,
			A3 = 94,
			GSHARP2 = 99,
			G2 = 105,
			FSHARP2 = 122,
			F2 = 129,
			E2 = 137,
			DSHARP2 = 145,
			D2 = 153,
			CSHARP2 = 179,
			C2 = 190,
			B2 = 201,
			ASHARP2 = 213,
			A2 = 226,
			GSHARP1 = 240,
			G1 = 254
		};

		byte value;
		byte duration;
	};

	byte pulseCount;
	byte currentRow;
	byte counter;

	ProGamer();

	void irEnd();
	void irBegin();

	void begin();
	void update();

	void setFramelength(int value);
	int getFramelength();

	bool isRenderingSpecial();

	int ldrValue();
	void setldrThreshold(uint16_t threshold);
	void setRefreshRate(uint16_t refreshRate);
	
	// Inputs
	bool isPressed(uint8_t input);
	bool isHeld(uint8_t input);

	// Outputs
	void allOn();
	void clear();
	void setPixel(int x, int y, byte colour);
	byte getPixel(int x, int y);
	void printImage(byte* img);
	void printImage(byte* img, int x, int y);
	void printString(String string);
	void showScore(int n);
	void appendColumn(uint16_t col);
	void appendColumn(byte col);
	void setLED(bool value);
	void toggleLED();

	void playTrack(int trackSize, Note track[], int beatLength, int pitchModifier = 0);
	void playSFX(int trackSize, byte track[], int beatLength);
	void setSoundOn(bool value);
	bool isSoundOn();

	void isrRoutine();
	
private:
	// Keywords
	#define CLK1 6
	#define DAT 8
	#define LAT 9
	#define CLK2 7
	#define DAT 8
	#define LAT 9
	#define OE 10
	#define LED 13
	#define BUZZER 2
	#define RX 5
	#define TX 4
	#define DEBOUNCETIME 50

	uint16_t _refreshRate;
	uint16_t ldrThreshold;

	byte byteImage[8];
	uint16_t image[8];
	int frameLength = FLASH_LOOP_TIME;
	long tick;
	byte pressedInputs;
	byte currentInputState;
	byte lastInputState;
	int lastLDRValue;
	bool capTouchFlag;

	byte renderMode = RM_NONE;
	int renderVar;
	int renderVar2;
	String currentString;

	Note *currentTrack = nullptr;
	int trackIdx;
	int trackEndIdx;
	int beatLength;
	int pitchModifier;
	int noteTime;
	bool soundOn = true;
	byte *currentSFX = nullptr;
	int sfxTick;
	int sfxEndIdx;
	int sfxBeatLength;

	bool colourToBinaryDigit(byte colour);
	byte colourRowToByte(uint16_t row);

	void renderScore();
	void renderString();
	void printDigit(int digit, int x);

	bool capTouch();
	void updateInputs();
	void updateDisplay();
	void updateAudio();

	void playTone(int note);
	void stopTone();

	void writeToDriver(byte dataOut);
	void writeToRegister(byte dataOut);
	void checkInputs();
	void updateRow();

	// Numbers and letters for printString
	#define LETEND B10101010
	const static uint8_t allLetters[85][9];
	const static uint8_t allNumbers[10][8];
};