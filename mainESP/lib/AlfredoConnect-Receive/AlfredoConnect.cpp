// TODO: for both keyboard keys and gamepad buttons, really want
// keyPressed, buttonPressed - true only on the frame of the press
// keyHeld, buttonHeld - true for the entire duration of the press
// this means we need to save previous state, doubling memory use
// TODO: could we implement gamepads as a dynamically allocated linked list?
// this would help alleviate memory use from storing both current and
// previous button states.

#include "AlfredoConnect.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ALFREDOCONNECT)
AlfredoConnectParser AlfredoConnect;
#endif

void AlfredoConnectParser::begin() {
    begin(Serial);
}

void AlfredoConnectParser::begin(Stream& inputStream) {
    this->inputStream = &inputStream;
}

/**
 * @param suppressErrors suppresses writing error output to the inputStream
 */
void AlfredoConnectParser::begin(Stream& inputStream, bool suppressErrors) {
    this->suppressErrors = suppressErrors;
    begin(inputStream);
}

/**
 * Parses packets from AlfredoConnect into keyboard and gamepad states.
 * 
 * @return 0 when successful, which includes when no packets were the stream. Return
 * values over 0 are failure codes.
 */
uint8_t AlfredoConnectParser::update() {
    if (inputStream == NULL) return 1;
    uint8_t keysPressed[NUM_KEYS];
    int keysPressedSize = 0;
    Gamepad gamepadsTemp[MAX_GAMEPADS];
    int gamepadsSizeTemp = 0;
    bool keyboardStateCleared = false; // We want to clear the keyboard state at most once per update
    while (inputStream->available()) {
        if (inputStream->peek() == '#') {
            readWithTimeout(inputStream); // read the hash
            
            // Keyboard
            if ((keysPressedSize = readWithTimeout(inputStream)) < 0) return 2;
            if (readAndDiscard(inputStream, keysPressed, NUM_KEYS, keysPressedSize) == 1) return 3;

            // Gamepads
            if ((gamepadsSizeTemp = readWithTimeout(inputStream)) < 0) return 4;
            for (uint8_t i = 0; i < min(gamepadsSizeTemp, MAX_GAMEPADS); i++) {

                // Axes
                if ((gamepadsTemp[i].axesSize = readWithTimeout(inputStream)) < 0) return 5;
                if (readAndDiscard(inputStream, gamepadsTemp[i].axes, MAX_AXES, gamepadsTemp[i].axesSize) < 0) return 6;

                // Buttons
                if ((gamepadsTemp[i].buttonsSize = readWithTimeout(inputStream)) < 0) return 7;
                uint8_t buttonsBytes = (gamepadsTemp[i].buttonsSize / 8) + (gamepadsTemp[i].buttonsSize % 8 == 0 ? 0 : 1);
                if (readAndDiscard(inputStream, gamepadsTemp[i].buttons, MAX_BUTTONS, buttonsBytes) < 0) return 9;
            }

            // If more than 8 gamepads were sent, discard the extras
            for (uint8_t i = MAX_GAMEPADS; i < gamepadsSizeTemp; i++) {
                int axesSize;
                if ((axesSize = readWithTimeout(inputStream)) < 0) return 10;
                for (uint8_t j = 0; j < axesSize; j++) if (readWithTimeout(inputStream) == -1) return 11;
                int buttonsSize;
                if ((buttonsSize = readWithTimeout(inputStream)) < 0) return 12;
                uint8_t buttonsBytes = (gamepadsTemp[i].buttonsSize / 8) + (gamepadsTemp[i].buttonsSize % 8 == 0 ? 0 : 1);
                for (uint8_t j = 0; j < buttonsBytes; j++) if (readWithTimeout(inputStream) == -1) return 13;
            }

            if (readWithTimeout(inputStream) != '$') return 14;
            if (!keyboardStateCleared) {
                keyboardStateCleared = true;
                memset(keyboardState, 0, NUM_KEYS);
            }
            for (int i = 0; i < keysPressedSize; i++) keyboardState[keysPressed[i] / 8] |= 1 << (keysPressed[i] % 8);
            memcpy(gamepads, gamepadsTemp, sizeof(Gamepad) * gamepadsSizeTemp);
            gamepadsSize = gamepadsSizeTemp;
        } else inputStream->read();
    }
    return 0;
}

/**
 * Reads `numToRead` bytes from `inputStream`, placing up to `bufferLen` of them in `buffer`, discarding the rest.
 */
uint8_t AlfredoConnectParser::readAndDiscard(Stream* inputStream, uint8_t *buffer, uint8_t bufferLen, uint8_t numToRead) {
    if (inputStream->readBytes(buffer, min(bufferLen, numToRead)) != min(bufferLen, numToRead)) return 1;
    for (int i = bufferLen; i < numToRead; i++) if (readWithTimeout(inputStream) == -1) return 1;
    return 0;
}

/**
 * Similar to calling `read` on the provided stream, but with a timeout like `readBytes`.
 */
int AlfredoConnectParser::readWithTimeout(Stream* inputStream) {
    uint8_t c;
    if (inputStream->readBytes(&c, 1) != 1) return -1;
    return c;
}

bool AlfredoConnectParser::keyHeld(Key key) {
    return keyboardState[(uint8_t)key / 8] & (1 << ((uint8_t)key % 8));
}

uint8_t AlfredoConnectParser::getGamepadCount() {
    return gamepadsSize;
}

uint8_t AlfredoConnectParser::getAxisCount(uint8_t gamepad) {
    if (gamepad >= gamepadsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getAxisCount] Gamepad ");
            inputStream->print(gamepad);
            inputStream->println(" is not connected.");
        }
        return 0;
    }
    return gamepads[gamepad].axesSize;
}

uint8_t AlfredoConnectParser::getButtonCount(uint8_t gamepad) {
    if (gamepad >= gamepadsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getButtonCount] Gamepad ");
            inputStream->print(gamepad);
            inputStream->println(" is not connected.");
        }
        return 0;
    }
    return gamepads[gamepad].buttonsSize;
}

float AlfredoConnectParser::getAxis(uint8_t gamepad, uint8_t axis) {
    if (gamepad >= gamepadsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getAxis] Gamepad ");
            inputStream->print(gamepad);
            inputStream->println(" is not connected.");
        }
        return 0;
    }
    if (axis >= gamepads[gamepad].axesSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getAxis] Gamepad ");
            inputStream->print(gamepad);
            inputStream->print(" has no axis ");
            inputStream->print(axis);
            inputStream->println(".");
        }
        return 0;
    }
    return ((float)(gamepads[gamepad].axes[axis]) / 127.5) - 1;
}

uint8_t AlfredoConnectParser::getRawAxis(uint8_t gamepad, uint8_t axis) {
    if (gamepad >= gamepadsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getRawAxis] Gamepad ");
            inputStream->print(gamepad);
            inputStream->println(" is not connected.");
        }
        return 0;
    }
    if (axis >= gamepads[gamepad].axesSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::getRawAxis] Gamepad ");
            inputStream->print(gamepad);
            inputStream->print(" has no axis ");
            inputStream->print(axis);
            inputStream->println(".");
        }
        return 0;
    }
    return gamepads[gamepad].axes[axis];
}

bool AlfredoConnectParser::buttonHeld(uint8_t gamepad, uint8_t button) {
    if (gamepad >= gamepadsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::buttonHeld] Gamepad ");
            inputStream->print(gamepad);
            inputStream->println(" is not connected.");
        }
        return false;
    }
    if (button >= gamepads[gamepad].buttonsSize) {
        if (!suppressErrors) {
            inputStream->print("[ERROR - AlfredoConnectParser::buttonHeld] Gamepad ");
            inputStream->print(gamepad);
            inputStream->print(" has no button ");
            inputStream->print(button);
            inputStream->println(".");
        }
        return false;
    }
    return gamepads[gamepad].buttons[button / 8] & (1 << (button % 8));
}