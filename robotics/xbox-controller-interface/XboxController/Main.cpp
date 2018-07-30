#include <iostream>
#include <stdlib.h>
#include "XboxController.h"

using namespace std;

void printBatteryLevel(XboxController);

int main() {
	XboxController first, second;
	bool running = true;
	while (running) {
		if (first.isKeyDown(XINPUT_GAMEPAD_BACK) || second.isKeyDown(XINPUT_GAMEPAD_BACK)) {
			running = false;
		}

		system("cls");

		if (!first.isControllerConnected()) {
			cout << "Error: Controller 1 has been disconnected!" << endl;
		}
		
		if (!second.isControllerConnected()) {
			cout << "Error: Controller 2 has been disconnected!" << endl;
		}

		printBatteryLevel(first);
		printBatteryLevel(second);

		if (first.isKeyDown(XINPUT_GAMEPAD_A) && first.isKeyDown(XINPUT_GAMEPAD_B) && first.isKeyDown(XINPUT_GAMEPAD_Y)) {
			cout << "A, B and Y have been pressed simultaneously!" << endl;
		}

		if (first.isKeyDown(XINPUT_GAMEPAD_X)) {
			if (first.getTriggerPosition(VK_PAD_LTRIGGER) > 0) {
				second.vibrate(5000, VIBRATE_LEFT_ONLY);
			}
			else {
				second.vibrate(0, VIBRATE_LEFT_ONLY);
			}
			if (first.getTriggerPosition(VK_PAD_RTRIGGER) > 0) {
				second.vibrate(5000, VIBRATE_RIGHT_ONLY);
			}
			else {
				second.vibrate(0, VIBRATE_RIGHT_ONLY);
			}
		}
		else {
			second.vibrate(0, VIBRATE_LEFT_AND_RIGHT);
		}
	}
	return 0;
}

void printBatteryLevel(XboxController cont) {
	cout << "Controller " << cont.getControllerIndex() + 1 << endl;
	cout << "--------------------------------------------------" << endl;
	cout << "Battery Level: ";
	switch (cont.getBatteryLevel()) {
	case XboxController::EMPTY:
		cout << "Empty" << endl;
		break;
	case XboxController::LOW:
		cout << "Low" << endl;
		break;
	case XboxController::MEDIUM:
		cout << "Medium" << endl;
		break;
	case XboxController::FULL:
		cout << "Full" << endl;
		break;
	case XboxController::UNKNOWN:
		cout << "Cannot detect!" << endl;
		break;
	}
	cout << "--------------------------------------------------" << endl;
}