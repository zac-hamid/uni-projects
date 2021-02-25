#include <Arduino.h>
#include <HardwareSerial.h>
#include "common_maze_rep.h"

void setup() {
    Serial.begin(9600, SERIAL_8N1);
    pinMode(13, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        char buf[common::BUF_SIZE];
        if (Serial.read() == common::TRAPA) {
            Serial.println("<<Recieved trap a");
            buf[0] = common::TRAPA;
            digitalWrite(13, HIGH);

            while (Serial.available() <= 0) {;}
            if (Serial.read() == common::TRAPB) {
                Serial.println("Recieved trap b");
                buf[1] = common::TRAPB;

                /*
                if (Serial.readBytes(&buf[2], sizeof(common::MazeRep)) != sizeof(common::MazeRep)) {
                    Serial.println("Didnt get enough bytes!");
                    return;
                }*/

                Serial.println("Read Bytes:");
                for (int i = 0; i < sizeof(common::MazeRep); i++) {
                    while (Serial.available() <= 0) {;}
                    buf[i+2] = Serial.read();
                    //Serial.print("Read ");
                    Serial.print(buf[i+2]);
                }

                digitalWrite(13, LOW);
                char msg[300];
                bool succ;
                common::MazeRep maze(buf, succ);
                Serial.print("Success: ");
                Serial.println(succ);
                Serial.print("chksum was: ");
                Serial.println(maze.chk_);

                Serial.print("Calculated: ");
                Serial.println(maze.calc_chk());
                maze.to_string(msg, 300);
                Serial.print("msg is ");
                Serial.print(strlen(msg));
                Serial.println(" Bytes");
                Serial.write(msg, strlen(msg));

                Serial.println(">>");
            
            }
        }
    }

    //Serial.println("He llo\nLine2");

}
