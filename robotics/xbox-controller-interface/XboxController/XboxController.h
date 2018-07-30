#ifndef XBOXCONTROLLER_H
#define XBOXCONTROLLER_H

#define MAX_CONNECTED_CONTROLLERS		4

#define VIBRATE_LEFT_ONLY				0x100
#define VIBRATE_RIGHT_ONLY				0x101
#define VIBRATE_LEFT_AND_RIGHT			0x102

#define GAMEPAD_THUMBSTICK_LEFT_X		0x200
#define GAMEPAD_THUMBSTICK_LEFT_Y		0x201
#define GAMEPAD_THUMBSTICK_RIGHT_X		0x202
#define GAMEPAD_THUMBSTICK_RIGHT_Y		0x203

#define BYTE_MAX UCHAR_MAX

#include <iostream>
#include <Windows.h>
#include <string>
#include <Xinput.h>

class XboxController {
public:
	enum BatteryLevel {
		EMPTY,
		LOW,
		MEDIUM,
		FULL,
		UNKNOWN
	};
private:
	BatteryLevel battery;
	int controllerIndex = -1;
	static bool connectedControllers[MAX_CONNECTED_CONTROLLERS];
	double deadZoneX, deadZoneY = 0.0;

	double applyDeadZone(double valueX, double valueY, int flag);

public:
	XboxController();
	int getControllerIndex();
	BatteryLevel getBatteryLevel();
	double getTriggerPosition(int trigger);
	double getThumbstickPosition(int gamepadFlag);
	void setDeadZone(double _deadZoneX, double _deadZoneY);
	bool isControllerConnected();
	bool isKeyDown(int xinputBitmask);
	void vibrate(int vibrationLevel, int vibrateFlag);
	~XboxController();
};
#endif