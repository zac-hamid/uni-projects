#include "hardware.h"

#include "maze_layout.h"
#include "comms.h"

namespace hardware {


// DISPLAY
auto display::enable() -> void {
	lcd.init(); 
}

auto display::cursor(coordinate cursor_position) -> void {
	lcd.setCursor(cursor_position.column, cursor_position.row); 
}

auto display::print(char const * string) -> size_t {
	lcd.printstr(string); 
	return 0;
}

auto display::print(int n) -> size_t {
	char c[16];
	itoa(n, c, 10);
	lcd.printstr(c);
	return 16; 
}

auto display::print(double n) -> size_t {
	String s = String(n, 3); 
	int len = s.length() + 1;
	char c[len];
	s.toCharArray(c,len); 
	lcd.printstr(c);
	return len; 
}


auto display::clear() -> void {
	lcd.clear(); 
}

// char * receive_command(HardwareSerial &serial) {
// 	String str = "";
// 	char buf[100];
// 	while (str == "") {
// 		if (serial.available() > 0) {    
// 			while (serial.available() > 0) {
// 				str += static_cast<char>(serial.read());
// 			}
// 			str.toCharArray(buf, str.length());
// 		}
// 	}
// 	return buf;
// }

} // namespace hardware
// Comms
