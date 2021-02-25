#ifndef COMMS_CPP_H_
#define COMMS_CPP_H_


#include "comms.h"

#include <Arduino.h>

#include "hardware.h"
#include "units.h"
#include "maze_layout.h"

static DFRobot_LCD lcd{4, 4};

/***********************************************/
/*************** serial_api ********************/
/***********************************************/
namespace hardware {

template <uint8_t id>
class serial_tag {
public:
   static constexpr uint8_t serial_id = id;
}; 

static const HardwareSerial* serial_ports[4] = {&Serial, &Serial1, &Serial2, &Serial3};

static inline auto find_base(int base) {
   switch(base) {
      case 2: return BIN;
      case 6: return HEX;
      case 8: return OCT;
      default: return DEC; // case 10:
   }
}

template <typename tag>
template <typename T>
auto serial_api<tag>::print_vector(const T& first) -> char_count {
    return print(first);
}

template <typename tag>
template <typename T, typename... Args>
auto serial_api<tag>::print_vector(T first, Args... rest) -> char_count {
    char_count sum = print_vector(first);
    print(", ");
    sum += print_vector(rest...);
    return sum;
}

template<typename tag>
auto serial_api<tag>::print(const __FlashStringHelper* string) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(string);
}

template<typename tag>
auto serial_api<tag>::print(char const * string) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(string);
}

template<typename tag>
auto serial_api<tag>::print(char c) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(c);
}

template<typename tag>
auto serial_api<tag>::print(int i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print(long i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print(unsigned long i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print(unsigned char c, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(c, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print(double i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->print(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(const __FlashStringHelper* string) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(string);
}

template<typename tag>
auto serial_api<tag>::print_line(char const * string) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(string);
}

template<typename tag>
auto serial_api<tag>::print_line(char c) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(c);
}

template<typename tag>
auto serial_api<tag>::print_line(int i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(unsigned int i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(long i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(unsigned long i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(unsigned char c, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(c, find_base(base));
}

template<typename tag>
auto serial_api<tag>::print_line(double i, int base) -> char_count {
   return hardware::serial_ports[tag::serial_id]->println(i, find_base(base));
}

template<typename tag>
auto serial_api<tag>::write(unsigned long n) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(n);
}

template<typename tag>
auto serial_api<tag>::write(long n) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(n);
}

template<typename tag>
auto serial_api<tag>::write(int n) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(n);
}

template<typename tag>
auto serial_api<tag>::write(uint8_t n) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(n);
}

template<typename tag>
auto serial_api<tag>::write(const char * str) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(str);
}

template<typename tag>
auto serial_api<tag>::write(char const * string, size_t size) -> char_count {
   return hardware::serial_ports[tag::serial_id]->write(string, size);
}

template<typename tag>
auto serial_api<tag>::enable(unsigned long baud) -> void {
   return hardware::serial_ports[tag::serial_id]->begin(baud);
   while (!hardware::serial_ports[tag::serial_id]){
      // Wait for serial port to connect
   }
}

template<typename tag>
auto serial_api<tag>::end() -> void {
   hardware::serial_ports[tag::serial_id]->end();
}

template<typename tag>
auto serial_api<tag>::input_byte_in_buffer() -> int {
   return hardware::serial_ports[tag::serial_id]->available();
}

template<typename tag>
auto serial_api<tag>::output_buffer_space() -> int {
   return hardware::serial_ports[tag::serial_id]->availableForWrite();
}

template<typename tag>
auto serial_api<tag>::peek() -> int {
   return hardware::serial_ports[tag::serial_id]->peek();
}

template<typename tag>
auto serial_api<tag>::read() -> int {
   return hardware::serial_ports[tag::serial_id]->read();
}

template<typename tag>
auto serial_api<tag>::read_bytes(char * buffer, char_count length) -> char_count {
   return hardware::serial_ports[tag::serial_id]->readBytes(buffer, length);
}

template<typename tag>
auto serial_api<tag>::read_bytes_until(
   char terminator, char * buffer, char_count length) -> char_count {
   String str = hardware::serial_ports[tag::serial_id]->readStringUntil(terminator);
   str.toCharArray(buffer, length);
   return (char_count) str.length(); 
}

template<typename tag>
auto serial_api<tag>::timeout_duration(units::milliseconds timeout) -> void {
   return hardware::serial_ports[tag::serial_id]->setTimeout(static_cast<int>(timeout));
}

template<typename tag>
auto serial_api<tag>::flush() -> void {
   return hardware::serial_ports[tag::serial_id]->flush();
}

template<typename tag>
auto serial_api<tag>::clear() -> void {
   hardware::serial_ports[tag::serial_id]->write(27);
}
} // namespace hardware




#endif
