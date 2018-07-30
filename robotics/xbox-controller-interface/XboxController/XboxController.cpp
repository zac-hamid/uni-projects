#include <iostream>
#include "XboxController.h"

using namespace std;

bool XboxController::connectedControllers[] = { 0,0,0,0 };

XboxController::XboxController()
{
	//Checks if there is a spare "slot" in the connected controllers array to add a new controller
	bool found = false;
	for (int i = 0; i < MAX_CONNECTED_CONTROLLERS; i++) {
		if (connectedControllers[i] == false) {
			controllerIndex = i;
			connectedControllers[i] = true;
			found = true;
			break;
		}
	}
	if (!found) {
		//If no slot is found, it is obvious that there have been more than 4 XboxController objects created.
		controllerIndex = -1;
		cerr << "An attempt was made to create more than 4 XboxController objects." << endl;
		cerr << "No controller index assigned to this object." << endl;
	}
}

int XboxController::getControllerIndex()
{
	return controllerIndex;
}

XboxController::BatteryLevel XboxController::getBatteryLevel() {
	//Returns a BatteryLevel enumeration with the level of battery remaining, if unknown
	//either the controller has no index or has an issue with the battery pack.
	if (controllerIndex != -1) {
		XINPUT_BATTERY_INFORMATION batteryInfo;
		XInputGetBatteryInformation(controllerIndex, BATTERY_DEVTYPE_GAMEPAD, &batteryInfo);
		switch (batteryInfo.BatteryLevel) {
		case BATTERY_LEVEL_EMPTY:
			battery = EMPTY;
			break;
		case BATTERY_LEVEL_LOW:
			battery = LOW;
			break;
		case BATTERY_LEVEL_MEDIUM:
			battery = MEDIUM;
			break;
		case BATTERY_LEVEL_FULL:
			battery = FULL;
			break;
		default:
			battery = UNKNOWN;
			break;
		}
		return battery;
	}
	else {
		return UNKNOWN;
	}
}

double XboxController::getTriggerPosition(int trigger)
{
	//Instead of a range of 0-UCHAR_MAX (255), the values have been normalised to be
	//between 0 and 1.
	if (controllerIndex != -1) {
		XINPUT_STATE state;
		XInputGetState(controllerIndex, &state);
		XINPUT_GAMEPAD gp = state.Gamepad;
		if (trigger == VK_PAD_LTRIGGER) {
			return ((double)gp.bLeftTrigger / BYTE_MAX);
		}
		else if (trigger == VK_PAD_RTRIGGER) {
			return ((double)gp.bRightTrigger / BYTE_MAX);
		}
		else {
			return 0;
		}
	}
	else {
		return 0;
	}
}

double XboxController::getThumbstickPosition(int gamepadFlag)
{
	//flag is the variable you want to return
	//There are 4 possibilities:
	//	- Left: X and Y
	//	- Right: X and Y
	if (controllerIndex != -1) {
		XINPUT_STATE state;
		XInputGetState(controllerIndex, &state);
		XINPUT_GAMEPAD gp = state.Gamepad;

		switch (gamepadFlag) {
			case GAMEPAD_THUMBSTICK_LEFT_X:
			case GAMEPAD_THUMBSTICK_LEFT_Y:
				return applyDeadZone(gp.sThumbLX, gp.sThumbLY, gamepadFlag);
			case GAMEPAD_THUMBSTICK_RIGHT_X:
			case GAMEPAD_THUMBSTICK_RIGHT_Y:
				return applyDeadZone(gp.sThumbRX, gp.sThumbRY, gamepadFlag);
		}
	}
	return 0.0;
}

double XboxController::applyDeadZone(double valueX, double valueY, int flag) {
	//This clamps the normalised value between -1 and 1 since SHRT_MIN is -32768 while SHRT_MAX is 32767,
	//this means you'd never be able to reach 1 without the clamping.
	double normX = max(-1, valueX / SHRT_MAX);
	double normY = max(-1, valueY / SHRT_MAX);

	double X, Y = 0.0;

	if (abs(normX) < deadZoneX) {
		X = 0;
	}
	else {
		//normX / abs(normX) simply retains the original sign (+/-)
		X = (abs(normX) - deadZoneX) * (normX / abs(normX));
	}

	//This conditional expression prevents arithmetic error of dividing by 0, if the deadZone is 0,
	//we leave the position of X unchanged.
	//X/(1-deadZoneX) makes X a percentage between 0 (the edge of the deadZone), and 1 (the edge of the joystick range)
	(deadZoneX > 0 ? X = X / (1 - deadZoneX) : X);

	if (abs(normY) < deadZoneY) {
		Y = 0;
	}
	else {
		Y = (abs(normY) - deadZoneY) * (normY / abs(normY));
	}

	(deadZoneY > 0 ? Y = Y / (1 - deadZoneY) : Y);

	switch (flag) {
		case GAMEPAD_THUMBSTICK_LEFT_X:
		case GAMEPAD_THUMBSTICK_RIGHT_X:
			return X;
		case GAMEPAD_THUMBSTICK_LEFT_Y:
		case GAMEPAD_THUMBSTICK_RIGHT_Y:
			return Y;
	}
	return 0.0;
}

void XboxController::setDeadZone(double _deadZoneX, double _deadZoneY)
{
	deadZoneX = _deadZoneX;
	deadZoneY = _deadZoneY;


	//Keep dead-zone values clamped between 0 and 1.
	if (deadZoneX < 0) deadZoneX = 0;
	else if (deadZoneX > 1) deadZoneX = 1;

	if (deadZoneY < 0) deadZoneY = 0;
	else if (deadZoneY > 1) deadZoneY = 1;
}

bool XboxController::isControllerConnected() {
	//If no controller index is assigned (-1), automatically returns false for controller connection.
	//Otherwise, returns connection status of the controller
	if (controllerIndex != -1) {
		XINPUT_STATE controllerState;
		DWORD returnCode = XInputGetState(controllerIndex, &controllerState);
		switch (returnCode) {
			case ERROR_SUCCESS:
				return true;
			case ERROR_DEVICE_NOT_CONNECTED:
				return false;
			default:
				return false;
				break;
		}
	}
	else {
		return false;
	}
}

bool XboxController::isKeyDown(int xinputBitmask)
{
	//bitmask represents the XInput defined bitmasks for the position in a string of bits that represents a certain button
	//Applying the & bit operator filters through only the state of the button requested by bitmask
	if (controllerIndex != -1) {
		XINPUT_STATE state;
		XInputGetState(controllerIndex, &state);
		XINPUT_GAMEPAD gp = state.Gamepad;
		if ((gp.wButtons & xinputBitmask) != 0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void XboxController::vibrate(int vibrationLevel, int vibrateFlag)
{
	//Sets the vibration of the controller to vibrationLevel
	//vibrateType represents the sides of the controller you want to vibrate
	//These values are defined in XboxController.h
	if (controllerIndex != -1) {
		XINPUT_VIBRATION vibration;
		switch (vibrateFlag) {
			case VIBRATE_LEFT_ONLY:
				vibration.wLeftMotorSpeed = vibrationLevel;
				break;
			case VIBRATE_RIGHT_ONLY:
				vibration.wRightMotorSpeed = vibrationLevel;
				break;
			case VIBRATE_LEFT_AND_RIGHT:
				vibration.wLeftMotorSpeed = vibrationLevel;
				vibration.wRightMotorSpeed = vibrationLevel;
				break;
			default:
				break;
		}
		XInputSetState(controllerIndex, &vibration);
	}
}

XboxController::~XboxController()
{
	//If the XboxController object is destroyed, it should be removed from the connectedControllers array.
	if (controllerIndex < MAX_CONNECTED_CONTROLLERS && controllerIndex >= 0) {
		connectedControllers[controllerIndex] = false;
	}
}