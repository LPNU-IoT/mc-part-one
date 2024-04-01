#include <Key.h>

// default constructor
Key::Key() {
	kchar = NO_KEY;
	kstate = IDLE;
	stateChanged = false;
}

// constructor
Key::Key(S8 userKeyChar) {
	kchar = userKeyChar;
	kcode = -1;
	kstate = IDLE;
	stateChanged = false;
}


void Key::key_update (S8 userKeyChar, KEY_STATE userState, bool userStatus) {
	kchar = userKeyChar;
	kstate = userState;
	stateChanged = userStatus;
}


