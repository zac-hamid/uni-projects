#include "maze_layout.h"
#include "hardware.h"

namespace hardware {

cell_location operator+(const cell_location& cell, Move move) {
  cell_location new_cell{cell};
  switch (move) {
    case FORWARD:
      switch(new_cell.dir_) {
        case N:   ++new_cell.y_; break;
        case E:   ++new_cell.x_; break;
        case S:   --new_cell.y_; break;
        case W:   --new_cell.x_; break;
        default:  serial::print_line("operator+(FORWARD) broke");
      }
      break;

    case LEFT:
      switch(new_cell.dir_) {
        case N:   new_cell.dir_ = W; break;
        case E:   new_cell.dir_ = N; break;
        case S:   new_cell.dir_ = E; break;
        case W:   new_cell.dir_ = S; break;
        default:  serial::print_line("operator+(LEFT) broke");
      }
      break;

    case RIGHT:
      switch(new_cell.dir_) {
        case N:   new_cell.dir_ = E; break;
        case E:   new_cell.dir_ = S; break;
        case S:   new_cell.dir_ = W; break;
        case W:   new_cell.dir_ = N; break;
        default:  serial::print_line("operator+(RIGHT) broke");
      }
      break;

    default:
      serial::print_line("maze_layout::Canmove broke"); 
  }

  return new_cell;
}

bool maze_layout::canMove(Move move) {
  uint8_t x = robot_.getX(); 
  uint8_t y = robot_.getY();
  Direction dir = robot_.getDir(); 

  switch (move) {
    case FORWARD:
      switch(dir) {
        case N:   return (ver_[x][y+1]  == 0) ? true : false;
        case E:   return (hor_[x+1][y]  == 0) ? true : false;
        case S:   return (ver_[x][y]    == 0) ? true : false;
        case W:   return (hor_[x][y]    == 0) ? true : false;
        default:  
          serial::print_line("CanMove(FORWARD) broke");
          return false;
      }
      break;

    case LEFT:
      switch(dir) {
        case N:   return (hor_[x][y]    == 0) ? true : false;
        case E:   return (ver_[x][y+1]  == 0) ? true : false;
        case S:   return (hor_[x+1][y]  == 0) ? true : false;
        case W:   return (ver_[x][y]    == 0) ? true : false;
        default:  
          serial::print_line("CanMove(LEFT) broke");
          return false;
      }
      break;

    case RIGHT:
      switch(dir) {
        case N:   return (hor_[x+1][y]  == 0) ? true : false;
        case E:   return (ver_[x][y]    == 0) ? true : false;
        case S:   return (hor_[x][y]    == 0) ? true : false; 
        case W:   return (ver_[x][y+1]  == 0) ? true : false;
        default:  
          serial::print_line("CanMove(LEFT) broke");
          return false;
      }
      break;

    default:
      serial::print_line("maze_layout::Canmove broke");
      return false; 
  }

}

bool maze_layout::canMoveU(Move move) {
  uint8_t x = robot_.getX(); 
  uint8_t y = robot_.getY();
  Direction dir = robot_.getDir(); 

  switch (move) {
    case FORWARD:
      switch(dir) {
        case N:   return (ver_[x][y+1]  != WALL) ? true : false;
        case E:   return (hor_[x+1][y]  != WALL) ? true : false;
        case S:   return (ver_[x][y]    != WALL) ? true : false;
        case W:   return (hor_[x][y]    != WALL) ? true : false;
        default:  
          serial::print_line("CanMove(FORWARD) broke");
          return false;
      }
      break;

    case LEFT:
      switch(dir) {
        case N:   return (hor_[x][y]    != WALL) ? true : false;
        case E:   return (ver_[x][y+1]  != WALL) ? true : false;
        case S:   return (hor_[x+1][y]  != WALL) ? true : false;
        case W:   return (ver_[x][y]    != WALL) ? true : false;
        default:  
          serial::print_line("CanMove(LEFT) broke");
          return false;
      }
      break;

    case RIGHT:
      switch(dir) {
        case N:   return (hor_[x+1][y]  != WALL) ? true : false;
        case E:   return (ver_[x][y]    != WALL) ? true : false;
        case S:   return (hor_[x][y]    != WALL) ? true : false; 
        case W:   return (ver_[x][y+1]  != WALL) ? true : false;
        default:  
          serial::print_line("CanMove(LEFT) broke");
          return false;
      }
      break;

    default:
      serial::print_line("maze_layout::Canmove broke");
      return false; 
  }
}

maze_layout_message receive_maze_layout() {
	char msg[255];
	uint8_t i = 0; 

	bool finished = false;
	bool start = false; 
	char c; 
  int count = 0; 

	while (!finished && i != 255) {
		if (bluetooth::input_byte_in_buffer()) {
			c = bluetooth::read();
      count = 0; 

			if (start && !finished) {
				msg[i++] = c;
				if (c == '@') { 
          char* tempstr = calloc(strlen(msg)+1, sizeof(char));
          strcpy(tempstr, msg);
          char * delim = strtok(tempstr, ".");
          uint8_t w = atoi(delim);
          delim = strtok(NULL, ".");
          uint8_t h = atoi(delim);
          // #9.5.36.22.3.H011110000000001000010000000011111000.V0000000010000000100001001010010000000000@
          uint8_t min_length = h*(w-1) + w*(h-1) + 14; 
          // hardware::bluetooth::print(F("w\t"));
          // hardware::bluetooth::print(w);
          // hardware::bluetooth::print(F("\th\t"));
          // hardware::bluetooth::print_line(h);
          // hardware::bluetooth::print(F("i\t"));
          // hardware::bluetooth::print_line(i);
          // hardware::bluetooth::print(F("Expected l\t"));
          // hardware::bluetooth::print(min_length);
          if (i >= min_length && i <= (min_length +2)) {
           finished = true;
          } else {
            return maze_layout_message(0, false, 0);
          }
        }
			}
			
			if (!start) {
				if (c == '#') { start = true; } 
			}
		} else {
      // Timeout
      if (++count > 1000) return maze_layout_message(0, false, 0); 
    }
	}

	msg[i++] = 0;
	maze_layout_message maze{i, finished, msg};
	return maze;
}

maze_layout parse_maze_layout(maze_layout_message maze_msg) {
	char msg[255];
	String str = maze_msg.getMsg();
	uint8_t len = str.length(); 
	str.toCharArray(msg, len +1); 
	bluetooth::print(F("Received: "));
	bluetooth::print_line(msg);

	uint8_t width, height, robot_pos, dest, direction;  

	// Get header
	char * delim = strtok(msg, ".");
	width = atoi(delim);
	delim = strtok(NULL, ".");
	height = atoi(delim);
	delim = strtok(NULL, ".");
	robot_pos = atoi(delim);
	delim = strtok(NULL, ".");
	dest = atoi(delim);
	delim = strtok(NULL, ".");
	direction = atoi(delim);

	Direction dir; 
	switch(direction) {
		case 0: dir = N; break;
		case 1: dir = E; break;
		case 2: dir = S; break;
		case 3: dir = W; break;
		default: dir = NO_DIR; 
	}

	maze_layout maze{width, height, robot_pos, dest, dir};

	// Fill in horizontal walls
	uint8_t count = 1; 
	delim = strtok(NULL, ".");
	for (uint8_t x = 1; x < height; ++x) {
		for (uint8_t y = 0; y < width; ++y) {
			maze.setHor(x, y, delim[count++] - '0'); 
		}
	}

	count = 1;
	delim = strtok(NULL, ".");
	// Fill in vertical walls
	for (uint8_t x = 0; x < height; ++x) {
		for (uint8_t y = 1; y < width; ++y) {
			maze.setVer(x, y, delim[count++] - '0'); 
		}
	}
	
	return maze; 
}

void print_maze_serial(maze_layout& maze) {
	for (auto x = 0; x <= maze.getHeight(); ++x) {
		// Print horizontal walls
		for (auto y = 0; y < maze.getWidth(); ++y) {
			switch (maze.getHor(x, y)) {
				case 0x00: 	bluetooth::print("    "); break;
				case 0x01: 	bluetooth::print(" ---"); break;
				default: 	bluetooth::print(" ***"); // unknown
			}
		}
		bluetooth::print_line("");

		// Print vertical walls
		if (x != maze.getHeight()){
			for (auto y = 0; y <= maze.getWidth(); ++y) {
				switch (maze.getVer(x, y)) {
					case NO_WALL: 	bluetooth::print(" "); break;
					case WALL: 	bluetooth::print("|"); break;
					default: 	bluetooth::print("*"); // unknown
				}

				if (y != maze.getWidth()) {
					if (maze.isDest(cell_location(x,y))) {
						bluetooth::print(" X ");
					} else if (maze.isStart(cell_location(x,y))) {
						switch (maze.getStartDir()) {
							case N:		bluetooth::print(" N "); break;
							case E:		bluetooth::print(" E "); break;
							case S:		bluetooth::print(" S "); break;
							case W:		bluetooth::print(" W ");
						}
					} else {
						if (maze.getPos(x, y) < 10) 
							bluetooth::print(" ");
						if (maze.getPos(x, y) > 0)
							bluetooth::print(maze.getPos(x,y));
						else
							bluetooth::print(" ");
						bluetooth::print(" ");
					}
				}
			}
			bluetooth::print_line("");
		}
	}
}

} // end namespace Hardware
