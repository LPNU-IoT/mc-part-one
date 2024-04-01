/**
 * @file Keypad.h
 * @author VVA
 * 
 * @author Mark Stanley, VVA
 * @brief Based on the original Keypad library by Mark Stanley.
 * This library provides a simple interface for using matrix
 * keypads. It supports multiple keypresses while maintaining
 * backwards compatibility with the old single key library.
 *  It also supports user selectable pins and definable keymaps.
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include "Key.h"

#define LIST_MAX 10		// Max number of keys on the active list.
#define MAPSIZE 10		// MAPSIZE is the number of rows (times 16 columns)
#define makeKeymap(x) ((S8*)x)

// bperrybap - Thanks for a well reasoned argument and the following macro(s).
// See http://arduino.cc/forum/index.php/topic,142041.msg1069480.html#msg1069480
#ifndef INPUT_PULLUP
#warning "Using  pinMode() INPUT_PULLUP AVR emulation"
#define INPUT_PULLUP 0x2
#define pinMode(_pin, _mode) _mypinMode(_pin, _mode)
#define _mypinMode(_pin, _mode)  \
do {							 \
	if(_mode == INPUT_PULLUP)	 \
		pinMode(_pin, INPUT);	 \
		digitalWrite(_pin, 1);	 \
	if(_mode != INPUT_PULLUP)	 \
		pinMode(_pin, _mode);	 \
}while(0)
#endif

typedef S8 KeypadEvent;

typedef struct {
    U8 rows;
    U8 columns;
} KeypadSize_t;


//class Keypad : public Key, public HAL_obj {
class Keypad : public Key {
public:

	Keypad();// = default;

	Keypad(S8 *userKeymap, U8 *row, U8 *col, U8 numRows, U8 numCols);

	init(S8 *userKeymap, U8 *row, U8 *col, U8 numRows, U8 numCols);

	virtual void pin_mode(U8 pinNum, U8 mode) { pinMode(pinNum, mode); }
	virtual void pin_write(U8 pinNum, bool level) { digitalWrite(pinNum, level); }
	virtual int  pin_read(U8 pinNum) { return digitalRead(pinNum); }

	uint bitMap[MAPSIZE];	// 10 row x 16 column array of bits. Except Due which has 32 columns.
	Key key[LIST_MAX];
	ulong holdTimer;

	S8 getKey();
	bool getKeys();
	KEY_STATE getState();
	void begin(S8 *userKeymap);
	bool isPressed(S8 keyChar);
	void setDebounceTime(uint);
	void setHoldTime(uint);
	void addEventListener(void (*listener)(S8));
	int findInList(S8 keyChar);
	int findInList(int keyCode);
	S8 waitForKey();
	bool keyStateChanged();
	U8 numKeys();

private:
	ulong startTime;
	S8 *keymap;
    U8 *rowPins;
    U8 *columnPins;
	KeypadSize_t sizeKpd;
	uint debounceTime;
	uint holdTime;
	bool single_key;

	void scanKeys();
	bool updateList();
	void nextKeyState(U8 n, bool button);
	void transitionTo(U8 n, KEY_STATE nextState);
	void (*keypadEventListener)(S8);
};

#endif //! KEYPAD_H

