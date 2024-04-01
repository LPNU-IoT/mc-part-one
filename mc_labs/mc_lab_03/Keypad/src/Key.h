#ifndef Keypadlib_KEY_H_
#define Keypadlib_KEY_H_

#include <Arduino.h>
#include "TypeDefs.h"

#define OPEN LOW
#define CLOSED HIGH

typedef enum
{
	IDLE,
	PRESSED,
	HOLD,
	RELEASED
} KEY_STATE;

const S8 NO_KEY = '\0';

class Key
{
public:
	S8 kchar;
	int kcode;
	KEY_STATE kstate;
	bool stateChanged;

	Key();
	Key(S8 userKeyChar);
	void key_update(S8 userKeyChar, KEY_STATE userState, bool userStatus);

private:
};

#endif
