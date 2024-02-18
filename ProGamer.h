#pragma once

#include "Arduino.h"
#include <Gamer.h>
#include <avr/interrupt.h>
#include <avr/io.h>

class ProGamer : Gamer {
private:
	static const int REFRESH_TIME = 50;
	static constexpr int REFRESH_RATE = 1000 / REFRESH_TIME;

public:
	#define CAPTOUCH 6

	#define TRACK_SIZE(_TRACK_) (sizeof(_TRACK_) / sizeof(_TRACK_[0]))

	enum PixelColour {
      ZERO,
      QUARTER,
      HALF,
      ONE
    };

	struct Note {
		byte value;
		byte duration;
	};

	// Constructor
	using Gamer::Gamer;
	
	using Gamer::begin;
	using Gamer::ldrValue;
	using Gamer::setldrThreshold;
	using Gamer::setRefreshRate;
	using Gamer::setLED;
	using Gamer::toggleLED;
	using Gamer::irBegin;
	using Gamer::irEnd;

	using Gamer::playTone;
	using Gamer::stopTone;

	void update();

	void setFramelength(int value);
	int getFramelength();
	
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

	void playTrack(int trackSize, Note track[], int beatLength, int pitchModifier = 0);
	void setSoundOn(bool value);
	bool isSoundOn();
	
private:
	byte image[16];
	int frameLength = REFRESH_TIME;
	long tick;
	byte pressedInputs;
	byte heldInputs;

	Note *currentTrack = nullptr;
	int trackIdx;
	int trackEndIdx;
	int beatLength;
	int pitchModifier;
	int noteTime;
	bool soundOn;

	byte colourToBinaryDigit(byte colour);

	void updateInputs();
	void updateDisplay();
	void updateAudio();

	void copyBaseDisplay();
	/*
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
	
	// Variables
	uint16_t _refreshRate;
	bool buttonFlags[6];
	unsigned long buttonLastPressed[6];
	int lastInputState[6];
	uint16_t ldrThreshold;
	
	// Functions
	void setupLetters();
	void writeToDriver(byte dataOut);
	void writeToRegister(byte dataOut);
	void checkSerial();
	void checkInputs();
	void updateRow();
	int currentInputState[6];
	boolean tog;

	// Numbers and letters for printString
	#define LETEND B10101010
	const static uint8_t allLetters[85][9];
	const static uint8_t allNumbers[10][8];*/
};