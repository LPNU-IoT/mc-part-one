
#include <Keypad.h>

// <<constructor>> Allows custom keymap, pin configuration, and keypad sizes.
Keypad::Keypad()
{
}

Keypad::Keypad(S8 *userKeymap, U8 *row, U8 *col, U8 numRows, U8 numCols)
{
	init(userKeymap, row, col, numRows, numCols);
}

Keypad::init(S8 *userKeymap, U8 *row, U8 *col, U8 numRows, U8 numCols)
{
	rowPins = row;
	columnPins = col;
	sizeKpd.rows = numRows;
	sizeKpd.columns = numCols;

	begin(userKeymap);

	setDebounceTime(10);
	setHoldTime(500);
	keypadEventListener = 0;

	startTime = 0;
	single_key = false;
}

// Let the user define a keymap - assume the same row/column count as defined in constructor
void Keypad::begin(S8 *userKeymap)
{
	keymap = userKeymap;
}

// Returns a single key only. Retained for backwards compatibility.
S8 Keypad::getKey()
{
	single_key = true;

	if (getKeys() && key[0].stateChanged && (key[0].kstate == PRESSED))
		return key[0].kchar;

	single_key = false;

	return NO_KEY;
}

// Populate the key list.
bool Keypad::getKeys()
{
	bool keyActivity = false;

	// Limit how often the keypad is scanned. This makes the loop() run 10 times as fast.
	if ((millis() - startTime) > debounceTime)
	{
		scanKeys();
		keyActivity = updateList();
		startTime = millis();
	}

	return keyActivity;
}

// Private : Hardware scan
void Keypad::scanKeys()
{
	// Re-intialize the row pins. Allows sharing these pins with other hardware.
	for (U8 r = 0; r < sizeKpd.rows; r++)
	{
		pin_mode(rowPins[r], INPUT_PULLUP);
	}

	// bitMap stores ALL the keys that are being pressed.
	for (U8 c = 0; c < sizeKpd.columns; c++)
	{
		pin_mode(columnPins[c], OUTPUT);
		pin_write(columnPins[c], LOW); // Begin column pulse output.
		for (U8 r = 0; r < sizeKpd.rows; r++)
		{
			bitWrite(bitMap[r], c, !pin_read(rowPins[r])); // keypress is active low so invert to high.
		}
		// Set pin to high impedance input. Effectively ends column pulse.
		pin_write(columnPins[c], HIGH);
		pin_mode(columnPins[c], INPUT);
	}
}

// Manage the list without rearranging the keys. Returns true if any keys on the list changed state.
bool Keypad::updateList()
{

	bool anyActivity = false;

	// Delete any IDLE keys
	for (U8 i = 0; i < LIST_MAX; i++)
	{
		if (key[i].kstate == IDLE)
		{
			key[i].kchar = NO_KEY;
			key[i].kcode = -1;
			key[i].stateChanged = false;
		}
	}

	// Add new keys to empty slots in the key list.
	for (U8 r = 0; r < sizeKpd.rows; r++)
	{
		for (U8 c = 0; c < sizeKpd.columns; c++)
		{
			bool button = bitRead(bitMap[r], c);
			S8 keyChar = keymap[r * sizeKpd.columns + c];
			int keyCode = r * sizeKpd.columns + c;
			int idx = findInList(keyCode);
			// Key is already on the list so set its next state.
			if (idx > -1)
			{
				nextKeyState(idx, button);
			}
			// Key is NOT on the list so add it.
			if ((idx == -1) && button)
			{
				for (U8 i = 0; i < LIST_MAX; i++)
				{
					if (key[i].kchar == NO_KEY)
					{ // Find an empty slot or don't add key to list.
						key[i].kchar = keyChar;
						key[i].kcode = keyCode;
						key[i].kstate = IDLE; // Keys NOT on the list have an initial state of IDLE.
						nextKeyState(i, button);
						break; // Don't fill all the empty slots with the same key.
					}
				}
			}
		}
	}

	// Report if the user changed the state of any key.
	for (U8 i = 0; i < LIST_MAX; i++)
	{
		if (key[i].stateChanged)
			anyActivity = true;
	}

	return anyActivity;
}

// Private
// This function is a state machine but is also used for debouncing the keys.
void Keypad::nextKeyState(U8 idx, bool button)
{
	key[idx].stateChanged = false;

	switch (key[idx].kstate)
	{
	case IDLE:
		if (button == CLOSED)
		{
			transitionTo(idx, PRESSED);
			holdTimer = millis();
		} // Get ready for next HOLD state.
		break;
	case PRESSED:
		if ((millis() - holdTimer) > holdTime) // Waiting for a key HOLD...
			transitionTo(idx, HOLD);
		else if (button == OPEN) // or for a key to be RELEASED.
			transitionTo(idx, RELEASED);
		break;
	case HOLD:
		if (button == OPEN)
			transitionTo(idx, RELEASED);
		break;
	case RELEASED:
		transitionTo(idx, IDLE);
		break;
	}
}

bool Keypad::isPressed(S8 keyChar)
{
	for (U8 i = 0; i < LIST_MAX; i++)
	{
		if (key[i].kchar == keyChar)
		{
			if ((key[i].kstate == PRESSED) && key[i].stateChanged)
				return true;
		}
	}
	return false; // Not pressed.
}

// Search by character for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad::findInList(S8 keyChar)
{
	for (U8 i = 0; i < LIST_MAX; i++)
	{
		if (key[i].kchar == keyChar)
		{
			return i;
		}
	}
	return -1;
}

// Search by code for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int Keypad::findInList(int keyCode)
{
	for (U8 i = 0; i < LIST_MAX; i++)
	{
		if (key[i].kcode == keyCode)
		{
			return i;
		}
	}
	return -1;
}

S8 Keypad::waitForKey()
{
	S8 waitKey = NO_KEY;
	while ((waitKey = getKey()) == NO_KEY)
		; // Block everything while waiting for a keypress.
	return waitKey;
}

// Backwards compatibility function.
KEY_STATE Keypad::getState()
{
	return key[0].kstate;
}

// The end user can test for any changes in state before deciding
// if any variables, etc. needs to be updated in their code.
bool Keypad::keyStateChanged()
{
	return key[0].stateChanged;
}

// The number of keys on the key list, key[LIST_MAX], equals the number
// of bytes in the key list divided by the number of bytes in a Key object.
U8 Keypad::numKeys()
{
	return sizeof(key) / sizeof(Key);
}

// Minimum debounceTime is 1 mS. Any lower *will* slow down the loop().
void Keypad::setDebounceTime(uint debounce)
{
	debounce < 1 ? debounceTime = 1 : debounceTime = debounce;
}

void Keypad::setHoldTime(uint hold)
{
	holdTime = hold;
}

void Keypad::addEventListener(void (*listener)(S8))
{
	keypadEventListener = listener;
}

void Keypad::transitionTo(U8 idx, KEY_STATE nextState)
{
	key[idx].kstate = nextState;
	key[idx].stateChanged = true;

	// Sketch used the getKey() function.
	// Calls keypadEventListener only when the first key in slot 0 changes state.
	if (single_key)
	{
		if ((keypadEventListener != NULL) && (idx == 0))
		{
			keypadEventListener(key[0].kchar);
		}
	}
	// Sketch used the getKeys() function.
	// Calls keypadEventListener on any key that changes state.
	else
	{
		if (keypadEventListener != NULL)
		{
			keypadEventListener(key[idx].kchar);
		}
	}
}
