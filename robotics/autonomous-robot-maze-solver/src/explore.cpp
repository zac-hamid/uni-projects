#include "explore.h"

Explore::Explore(drive::TrajectoryController& _tc) :
    tc{_tc}
{
    centreReached = false;
    is_moving = false;
    is_rotating = false;
    driveQueue = Queue<Drive_Cmd>();
    start_hand_distance = 45.0; // Hand will be held approx. 4.5cm away from lidar sensor
    xGoal = maxDim - 1;
    yGoal = maxDim - 1;
    for (int i = 0; i < MEDIAN_FILT_BUFFER_SIZE; i++) f_lidar_readings[i] = start_hand_distance + 10.0;//make initial readings above for median filter
    //set arduino to WAITING
    state = WAITING;
    start_hand_timer = 0;
    //set pose
    xPos = 0;
    yPos = 0;
    orientation = NORTH;
    initialHeading = NORTH;
    //initialise maze params
	dimDetermined = false;
	xGoal = maxDim - 1;
	yGoal = maxDim - 1;

    initialiseMaze();

    // pinMode(RED_LED_PIN, OUTPUT);
    // pinMode(GREEN_LED_PIN, OUTPUT);
    // digitalWrite(RED_LED_PIN, HIGH);
    // digitalWrite(GREEN_LED_PIN, LOW);
}

bool Explore::complete() {
    return state == COMPLETE;
}

void Explore::tick() {
    if (is_moving) {
        if (tc.is_complete()) {
            is_moving = false;
            switch (orientation) {
            case NORTH:
                yPos++;
                break;
            case EAST:
                xPos++;
                break;
            case SOUTH:
                yPos--;
                break;
            case WEST:
                xPos--;
                break;
            default:
                Serial2.println("error");
            }
            //printMaze();
        }
        return;
    } else if (is_rotating) {
        if (tc.is_complete()) {
            is_rotating = false;
            orientation = orientation_after_turning;
            //printMaze();
        }
        return;
    } else if (!driveQueue.is_empty()) {
        Drive_Cmd nextCmd = driveQueue.pop();
        if (nextCmd.cmd == MOVE) {
            tc.set_forwards_goal(units::millimeters{250});
            is_moving = true;
            
        } else if (nextCmd.cmd == TURN) {
            double turn_angle = (nextCmd.final_orientation - orientation)*90;
            orientation_after_turning = nextCmd.final_orientation;
            if (turn_angle > 180) {
                turn_angle = turn_angle - 360;
            } else if (turn_angle < -180) {
                turn_angle = turn_angle + 360;
            }
            drive::TrajectoryController::Direction turn_dir = (turn_angle > 0) ? drive::TrajectoryController::RIGHT : drive::TrajectoryController::LEFT;
            turn_angle = abs(turn_angle);
            Serial2.print("turn ");
            Serial2.print (turn_angle);
            if (turn_dir) {
                Serial2.println(" right");
            } else {
                Serial2.println(" left ");
            }
            tc.set_rotate_goal(turn_dir, DEG2RAD(turn_angle));
            is_rotating = true;
        }
        //printMaze();
        return;
    }
    
    if (state == WAITING) {//detect hand to start
        //digitalWrite(LED_BUILTIN, LOW);
        Serial2.println("detecting hand");
        double lidar_dist = get_f_lidar_dist();
        Serial2.println(lidar_dist);
        if (lidar_dist < start_hand_distance) {
            delay(1);
            start_hand_timer++;
            if (start_hand_timer > start_hand_wait_time) {
                state = STARTING;
                tc.reset();
            }
        } else {
            start_hand_timer = 0;
        }
    } else if (state == STARTING) {//hand remove to start exploring
        double lidar_dist = get_l_lidar_dist();
        Serial2.println(lidar_dist);
        Serial2.println("hand detected");
        if (lidar_dist > start_hand_distance) {
            state = FIND_OPENING;
            //digitalWrite(GREEN_LED_PIN, HIGH);
            //digitalWrite(RED_LED_PIN, LOW);
            Serial2.println("Press any key to continue");
            while(Serial2.available() > 0) {
                this->yield();
                Serial2.read();
            }
            while(Serial2.available() < 1) {
                this->yield();
            }
            while(Serial2.available() > 0) {
                this->yield();
                Serial2.read();
            }
        }
    } else if (state == FIND_OPENING) {
        Serial2.println("FINDING OPENING");
        if (front_wall_detect()) {
            if (left_wall_detect()) {
                if (right_wall_detect()) {
                    turn(SOUTH);
                    state = TRAPPED;
                    return;
                } else {
                    initialHeading = WEST;
                    turn(EAST);
                }
            } else {
                initialHeading = EAST;
			    turn(WEST);
            }
		}
        //Serial2.println(String(initialHeading));
        state = RECALIBRATE;
        //printMaze();
    } else if (state == TRAPPED) {
        if (front_wall_detect()) {
            Serial2.println("TRAPPED in FIND_OPENING");
            state = COMPLETE;
            return;
        }
        initialHeading = SOUTH;
        state = RECALIBRATE;
    } else if (state == RECALIBRATE) {
        Serial2.println("RECALIBRATING");
        orientation = NORTH;//set orientation to north
        state = FIND_ORIENTATION;
        //printMaze();
    } else if (state == FIND_ORIENTATION) {
        Serial2.println("FINDING ORIENTATION");
        //first turn determines orientation of maze
            addWalls();
            if (!left_wall_detect()) {
                transposeMaze();
                state = EXPLORING;
                generateBox(maxDim, maxDim);
                Serial2.println("OPENING FOUND");
                return;
            } else if (!right_wall_detect()) {
                state = EXPLORING;
                generateBox(maxDim, maxDim);
                Serial2.println("OPENING FOUND");
                return;
            } else if (front_wall_detect()) {
                Serial2.println("TRAPPED");
                state = DO_NOTHING;
                return;
            }
            moveForward();
    } else if (state == EXPLORING) {
        //parseMaze(); 
        Serial2.println("EXPLORING");
        if (!flood[yPos][xPos]) {
            // Isaac belives this is found end?? 
            state = COMPLETE; 
            //digitalWrite(RED_LED_PIN, LOW);
            //digitalWrite(GREEN_LED_PIN, HIGH);
            // UNCOMMENT IF YOU WISH TO NAVIGATE BACK TO START
            // Serial2.println("explore part done");
            // if (centreReached) {
            //     state = COMPLETE;
            // } else {
            //     xGoal = 0;
            //     yGoal = 0;
            //     digitalWrite(GREEN_LED_PIN, LOW);
            //     digitalWrite(RED_LED_PIN, HIGH);
            //     floodFill(xGoal, yGoal);
            //     centreReached = true;
            // }
        } else {
            addWalls();
            if (dimDetermined) {
                floodFill(xGoal, yGoal);
            } else {
                floodFill(maxDim - 1, maxDim - 1);
                if (xPos >= minDim) {
                    yGoal = minDim / 2;
                    xGoal = maxDim / 2;
                    generateBox(maxDim, minDim);
                    dimDetermined = true;
                    floodFill(xGoal, yGoal);
                }
                if (yPos >= minDim) {
                    yGoal = maxDim / 2;
                    xGoal = minDim / 2;
                    generateBox(minDim, maxDim);
                    dimDetermined = true;
                    floodFill(xGoal, yGoal);
                }
            }
        
            if (!horWalls[yPos + 1][xPos] && (flood[yPos][xPos] == flood[yPos + 1][xPos] + 1)) {
                turn(NORTH);
            } else if (!vertWalls[yPos][xPos + 1] && (flood[yPos][xPos] == flood[yPos][xPos + 1] + 1)) {
                turn(EAST);
            } else if (!horWalls[yPos][xPos] && (flood[yPos][xPos] == flood[yPos - 1][xPos] + 1)) {
                turn(SOUTH);
            } else if (!vertWalls[yPos][xPos] && (flood[yPos][xPos] == flood[yPos][xPos - 1] + 1)) {
                turn(WEST);
            } else {
                state = DO_NOTHING;
                Serial2.println("TRAPPED in EXPLORE");
            }
            moveForward();
        }
    } else if (state == COMPLETE) {
        addWalls();
	    floodFill(xGoal, yGoal);
        Serial2.println("COMPLETE");
        printFinalMaze();
        state = DO_NOTHING;
        //if (hardware::bluetooth::input_byte_in_buffer()) {
        //    
        //}
    } else if (state == DO_NOTHING) {
        Serial2.println(F("DO_NOTHING state"));
    } else if (state == TEST) {

        
        //Serial.print("left: ");
        // Serial.print(get_l_lidar_dist());
        // Serial.print("   Right: ");
        // Serial.print(get_r_lidar_dist());
        // Serial2.print("   Front: ");
        // Serial2.println(get_f_lidar_dist());
        // Serial2.print("   Left: ");
        // Serial2.println(get_l_lidar_dist());
        // Serial2.print("   Right: ");
        // Serial2.println(get_r_lidar_dist());
        //Serial2.println(front_wall_detect());
        Serial2.print(get_l_lidar_dist());
        Serial2.print("   ");
        Serial2.print(get_f_lidar_dist());
        Serial2.print("   ");
        Serial2.print(get_r_lidar_dist());
        Serial2.println("   ");
        //moveForward(); 
    } else {
        Serial2.println("unknown state");
    }
    
}


// Function to find the median of an array a[] of size size
double Explore::findMedian(double a[], uint8_t size) {
    if (size == 0) return a[0];
    
    double temp[size];
    double median;
    
    // Copy array to temp
    for (int i = 0; i < size; i++) temp[i] = a[i];
    
    // BubbleSort algorithm
    for (int i = 0; i < (size - 1); i++) {
        for (int j = 0; j < (size - (i + 1)); j++) {
            if (temp[j] > temp[j+1]) {
                double temp_val = temp[j];
                temp[j] = temp[j+1];
                temp[j+1] = temp_val;
            }
        }
    }
    
    // If size is odd
    if (size % 2 != 0) {
        uint8_t x = (size / 2) + 1;
        median = temp[x];
    } else {
        uint8_t x = (size / 2);
        median = (temp[x] + temp[x+1]) / 2;
    }
    return median;
}

double Explore::get_l_lidar_dist() {
    // for (int i = 0; i < MEDIAN_FILT_BUFFER_SIZE; i++) {
    //     l_lidar_readings[i] = (double)hardware::left_lidar::distance().count();
    // }
    // return findMedian(l_lidar_readings, MEDIAN_FILT_BUFFER_SIZE);
    return (double)hardware::left_lidar::distance().count();
}

double Explore::get_r_lidar_dist() {
    // for (int i = 0; i < MEDIAN_FILT_BUFFER_SIZE; i++) {
    //     r_lidar_readings[i] = (double)hardware::right_lidar::distance().count();
    // }
    // return findMedian(r_lidar_readings, MEDIAN_FILT_BUFFER_SIZE);
    return (double)hardware::right_lidar::distance().count();
}

double Explore::get_f_lidar_dist() {
    // for (int i = 0; i < MEDIAN_FILT_BUFFER_SIZE; i++) {
    //     f_lidar_readings[i] = (double)hardware::front_lidar::distance().count();
    // }
    // return findMedian(f_lidar_readings, MEDIAN_FILT_BUFFER_SIZE);
    return (double)hardware::front_lidar::distance().count();
}

bool Explore::left_wall_detect() {
    //Serial2.println("Sensing Left");
    return (get_l_lidar_dist() < lidar_wall_detect_dist);
}

bool Explore::right_wall_detect() {
    //Serial2.println("Sensing Right");
    return (get_r_lidar_dist() < lidar_wall_detect_dist);
}

bool Explore::front_wall_detect() {
    //Serial2.println("Sensing Front");
    return (get_f_lidar_dist() < lidar_wall_detect_dist);
}

//maze class functions
void Explore::transposeMaze() {
    uint8_t isHorWall;
	uint8_t isVertWall;
	uint8_t frontWallDetected = horWalls[yPos + 1][0];
	xPos = yPos;
	yPos = 0;
	orientation = EAST;
	for (int i = 0; i < maxDim; i++) {
		isHorWall = horWalls[i + 1][0];
		isVertWall = vertWalls[i][0];
		vertWalls[0][i + 1] = isHorWall;
		horWalls[1][i] = isVertWall;
	}
	for (int i = 0; i < maxDim; i++) {
		if (i > 1) {
			horWalls[i][0] = 2;
		}
		if (i > 0) {
			vertWalls[i][1] = 2;
		}        
	}
	if (frontWallDetected) {
		vertWalls[yPos][xPos + 1] = 1;
	} else {
        vertWalls[yPos][xPos + 1] = 0;
    }
	switch(initialHeading) {
    case NORTH: initialHeading = EAST; break;
    case EAST: initialHeading = SOUTH; break;
    case SOUTH: initialHeading = WEST; break;
    case WEST: initialHeading = NORTH;
    }
    //Serial2.println(String(initialHeading));
}

void Explore::generateBox(int cols, int rows) {
	//initialise vertical
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols + 1; j++) {
			if (j == 0 || j == cols) {
				vertWalls[i][j] = 1;
			}
		}
	}
	//initialise horizontal walls
	for (int i = 0; i < rows + 1; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == 0 || i == rows) {
				horWalls[i][j] = 1;
			}
		}
	}
}

void Explore::initialiseMaze() {
    //initialise flood with -1
	for (int i = 0; i < maxDim; i++) {
		for (int j = 0; j < maxDim; j++) {
			flood[i][j] = -1;
		}
	}

	//initialise vertical with unknown walls
	for (int i = 0; i < maxDim; i++) {
		for (int j = 0; j < maxDim + 1; j++) {
			vertWalls[i][j] = 2;
		}
	}

	//initialise horizontal with unknown walls
	for (int i = 0; i < maxDim + 1; i++) {
		for (int j = 0; j < maxDim; j++) {
			horWalls[i][j] = 2;
		}
	}
}

void Explore::addWalls() {
    Serial2.print("Detecting Walls");
    bool front = front_wall_detect();
    bool left = left_wall_detect();
    bool right = right_wall_detect();
	switch (orientation) {
	case NORTH:
		horWalls[yPos + 1][xPos] = front ? 1 : 0;
		vertWalls[yPos][xPos] = left ? 1 : 0;
		vertWalls[yPos][xPos + 1] = right ? 1 : 0;
		break;
	case EAST:
		vertWalls[yPos][xPos + 1] = front ? 1 : 0;
		horWalls[yPos + 1][xPos] = left ? 1 : 0;
		horWalls[yPos][xPos] = right ? 1 : 0;
		break;
	case SOUTH:
		horWalls[yPos][xPos] = front ? 1 : 0;
		vertWalls[yPos][xPos + 1] = left ? 1 : 0;
		vertWalls[yPos][xPos] = right ? 1 : 0;
		break;
	case WEST:
		vertWalls[yPos][xPos] = front ? 1 : 0;
		horWalls[yPos][xPos] = left ? 1 : 0;
		horWalls[yPos + 1][xPos] = right ? 1 : 0;
		break;
	default:
		Serial2.println("error");
	}
    Serial2.print("Finished Detecting");
}

void Explore::turn(int heading) {
    if (orientation == heading) return;
    Drive_Cmd turnCmd;
    turnCmd.cmd = TURN;
    turnCmd.final_orientation = heading;
    driveQueue.push(turnCmd);
}

void Explore::moveForward() {
    Drive_Cmd moveCmd;
    moveCmd.cmd = MOVE;
    moveCmd.final_orientation = orientation;
    driveQueue.push(moveCmd);
}

void Explore::floodFill(int xGoal, int yGoal) {
	int floodVal = 0;
	int xCur = xGoal;
	int yCur = yGoal;
	//set flood to -1
	for (int i = 0; i < maxDim; i++) {
		for (int j = 0; j < maxDim; j++) {
			flood[i][j] = -1;
		}
	}
	Queue<int> queue = Queue<int>();
	queue.push(xCur);
	queue.push(yCur);
	flood[yCur][xCur] = 0;
	while (!queue.is_empty()) {
		xCur = queue.pop();
		yCur = queue.pop();
		floodVal = flood[yCur][xCur];
		//north wall
		if (horWalls[yCur + 1][xCur] != 1 && flood[yCur + 1][xCur] == -1) {
			flood[yCur + 1][xCur] = floodVal + 1;
			queue.push(xCur);
			queue.push(yCur + 1);
		}
		//East Wall
		if (vertWalls[yCur][xCur + 1] != 1 && flood[yCur][xCur + 1] == -1) {
			flood[yCur][xCur + 1] = floodVal + 1;
			queue.push(xCur + 1);
			queue.push(yCur);
		}
		//South Wall
		if (horWalls[yCur][xCur] != 1 && flood[yCur - 1][xCur] == -1) {
			flood[yCur - 1][xCur] = floodVal + 1;
			queue.push(xCur);
			queue.push(yCur - 1);
		}
		//West Wall
		if (vertWalls[yCur][xCur] != 1 && flood[yCur][xCur - 1] == -1) {
			flood[yCur][xCur - 1] = floodVal + 1;
			queue.push(xCur - 1);
			queue.push(yCur);
		}

        this->yield();
	}
}

void Explore::printMaze() {
	for (int i = maxDim - 1; i >= 0; i--) {
		for (int j = 0; j < maxDim; j++) {
			if (horWalls[i + 1][j] == 1) {
				Serial2.print(" --- ");
            } else if (horWalls[i + 1][j] == 2) {
                Serial2.print(" *** ");
			} else {
				Serial2.print("     ");
			}
		}
        this->yield();
		Serial2.println();
		for (int j = 0; j < maxDim; j++) {
            this->yield();
			if (vertWalls[i][j] == 1) {
				Serial2.print("|");
            } else if (vertWalls[i][j] == 2) {
                Serial2.print("*");
			} else {
				Serial2.print(" ");
			}
			if (xPos == j && yPos == i) {
				switch (orientation) {
				case NORTH: Serial2.print(" ^ ");
					break;
				case EAST: Serial2.print(" > ");
					break;
				case SOUTH: Serial2.print(" V ");
					break;
				case WEST: Serial2.print(" < ");
					break;
				default: Serial2.print("error");
				}
			} else {
                Serial2.print(" ");
				Serial2.print(flood[i][j]);
                Serial2.print(" ");
			}
			Serial2.print(" ");
		}
		//last column
		if (vertWalls[i][maxDim] == 1) {
			Serial2.print("|");
		} else if (vertWalls[i][maxDim] == 2) {
            Serial2.print("*");
        } else {
			Serial2.print(" ");
		}
		Serial2.println();
	}
	//last row
	for (int i = 0; i < maxDim; i++) {
        this->yield();
		if (horWalls[0][i] == 1) {
			Serial2.print(" --- ");
		} else if (horWalls[0][i] == 2) {
            Serial2.print(" *** ");
        } else {
			Serial2.print("     ");
		}
	}
    Serial2.println();
    Serial2.println();
}

void Explore::printFinalMaze() {
    	for (int i = (yGoal * 2); i >= 0; i--) {
            this->yield();
            for (int j = 0; j < (xGoal * 2) + 1; j++) {
                this->yield();
                if (horWalls[i + 1][j] == 1) {
                    Serial2.print(" --- ");
                } else if (horWalls[i + 1][j] == 2) {
                    Serial2.print(" *** ");
                } else {
                    Serial2.print("     ");
                }
            }
            Serial2.println();
            for (int j = 0; j < (xGoal * 2) + 1; j++) {
                this->yield();
                if (vertWalls[i][j] == 1) {
                    Serial2.print("|");
                } else if (vertWalls[i][j] == 2) {
                    Serial2.print("*");
                } else {
                    Serial2.print(" ");
                }
                if(i == 0 && j == 0) {
                    Serial2.print(" ");
                    switch(initialHeading) {
                        case NORTH: Serial2.print('N'); break;
                        case EAST: Serial2.print('E'); break;
                        case SOUTH: Serial2.print('S'); break;
                        case WEST: Serial2.print('W');
                    }
                    Serial2.print(" ");
                } else if (i == yGoal && j == xGoal) {
                    Serial2.print(" ");
                    Serial2.print("X");
                    Serial2.print(" ");
                } else {
                    Serial2.print("   ");
                }
                Serial2.print(" ");
            }
            //last column
            if (vertWalls[i][maxDim] == 1) {
                Serial2.print("|");
            } else if (vertWalls[i][maxDim] == 2) {
                Serial2.print("*");
            } else {
                Serial2.print(" ");
            }
            Serial2.println();
	}
	//last row
	for (int i = 0; i < (xGoal * 2) + 1; i++) {
		if (horWalls[0][i] == 1) {
			Serial2.print(" --- ");
		} else if (horWalls[0][i] == 2) {
            Serial2.print(" *** ");
        } else {
			Serial2.print("     ");
		}
	}
    Serial2.println();
    Serial2.println();
}

hardware::maze_layout Explore::parseMaze() {
    uint8_t width = xGoal*2+1;
    uint8_t height = yGoal*2+1;
    uint8_t robot = width*(height-1);
    uint8_t dest = width*(height/2)+(width/2);
    Direction heading;
    switch (initialHeading) {
        case NORTH: heading = W; break;
        case EAST:  heading = N; break;
        case SOUTH: heading = E; break;
        case WEST:  heading = S; break;
    }

    Serial2.print("w: "); 
    Serial2.print(width);
    Serial2.print("\th: ");
    Serial2.println(height);
    Serial2.print("Rob: "); 
    Serial2.print(robot);
    Serial2.print("\tdest: ");
    Serial2.println(dest);
    
    hardware::maze_layout maze{width, height, robot, dest, heading};
    // add in vertical walls
    for (uint8_t x = 0; x != height; ++x)
        for (uint8_t y = 1; y != width; ++y)
            maze.setVer(x, y, vertWalls[height - 1 - x][y]);

    // Add in horizontal walls
    for (uint8_t x = 1; x != height; ++x)
        for (uint8_t y = 0; y != width; ++y)
            maze.setHor(x, y, horWalls[height - x][y]); 

    Serial2.println("Calvin's maze");
    printFinalMaze();
    Serial2.println("Conversion to Isaac");
    print_maze_serial(maze);    
    return maze;
}

