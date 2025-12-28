#pragma once

#include <Arduino.h>

class Button {
private:
    uint32_t _lastUpdateTime = 0;
    uint32_t _lastPressTime = 0;
    uint32_t _holdTimeThreshold = 700; // Default hold time threshold in milliseconds
    uint32_t _debounceTimeThreshold = 10; // Default debounce time threshold in milliseconds

    bool _isPressed = false;
    bool _wasReleased = false;
    bool _wasHold = false;
    bool _wasClicked = false;

public:
    // Update the button state
    void update(uint32_t time, bool pressed) {
        if (time - _lastUpdateTime < _debounceTimeThreshold) {
            return; // Ignore updates within the debounce threshold
        }

        _lastUpdateTime = time;

        if (pressed) {
            if (!_isPressed) {
                _isPressed = true;
                _lastPressTime = time;
            }
        } else {
            if (_isPressed) {
                _isPressed = false;
                _wasReleased = true;
                uint32_t pressDuration = time - _lastPressTime;

                if (pressDuration >= _holdTimeThreshold) {
                    _wasHold = true;
                } else {
                    _wasClicked = true;
                }
            }
            else {
                _wasReleased = false;
                _wasClicked = false;
                _wasHold = false;
            }
        }
    }

    // Check if the button is currently pressed
    bool isPressed() const {
        return _isPressed;
    }

    // Check if the button was pressed and then released
    bool wasReleased() {
        return _wasReleased;
    }

    // Check if the button is being held
    bool isHolding() const {
        return _isPressed && (_lastUpdateTime - _lastPressTime >= _holdTimeThreshold);
    }

    // Check if the button was clicked (pressed and released, but not held)
    bool wasClicked() {
        return _wasClicked;
    }

    // Check if the button was held (pressed and released after holding)
    bool wasHold() {
        return _wasHold;
    }

    // Set the debounce time threshold
    void setDebounceTimeThreshold(uint32_t debounceTime) {
        _debounceTimeThreshold = debounceTime;
    }

    // Set the hold time threshold
    void setHoldTimeThreshold(uint32_t holdTime) {
        _holdTimeThreshold = holdTime;
    }
};
    