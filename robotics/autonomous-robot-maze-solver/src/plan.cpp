#include "plan.h"

#include "comms.cpp.h"
#include "comms.h"
#include "trajectory_controller.h"



void Plan::yield() {
    drive::traj_controller.tick(); 
}

void Plan::solve_flood(cell_location& start, cell_location& dest) {
	maze_layout maze{maze_};
	if (start != cell_location()) maze.setStart(start);
	if (dest != cell_location()) maze.setDest(dest);

	uint8_t h = maze.getHeight(); 
	uint8_t w = maze.getWidth();

	yield(); // run drive::traj_controller.tick(); 

	int8_t flood[h][w];
	for (uint8_t x = 0; x != h; ++x) 
		for (uint8_t y = 0; y != w; ++y) 
			flood[x][y] = -1; 

	flood[maze.getDest().getX()][maze.getDest().getY()] = 0;

	yield(); // run traj_controller.tick(); 

	Queue<cell_location> q;
	q.push(cell_location(maze.getDest().getX(), maze.getDest().getY()));
	
	yield(); // run traj_controller.tick(); 

	cell_location curr;
	int curr_flood, tmp_flood;
	uint8_t x, y; 

	/* Fill the flood fill maze */
	while (!q.is_empty()) {
		yield(); // run traj_controller.tick(); 
		curr = q.pop();
		x = curr.getX();
		y = curr.getY();

		curr_flood = flood[x][y]; 

		// NORTH
		if (y+1 <= w && flood[x][y+1] == -1 && !maze.getVer(x, y+1)) {
			flood[x][y+1] = curr_flood + 1; 
			q.push(cell_location(x, y+1));
		}

		// EAST
		if (x+1 <= h && flood[x+1][y] == -1 && !maze.getHor(x+1, y)) {
			flood[x+1][y] = curr_flood + 1; 
			q.push(cell_location(x+1, y));
		}

		// SOUTH
		if (y - 1 >= 0 && flood[x][y-1] == -1 && !maze.getVer(x, y)) {
			flood[x][y-1] = curr_flood + 1; 
			q.push(cell_location(x, y-1));
		}

		// WEST
		if (x - 1 >= 0 && flood[x-1][y] == -1 && !maze.getHor(x, y)) {
			flood[x-1][y] = curr_flood + 1;
			q.push(cell_location(x-1, y));
		}
	}

	yield(); // run traj_controller.tick(); 
	
	curr = maze.getRob();

	if (flood[curr.getX()][curr.getY()] == -1) {
		serial::print_line("No paths found");
		error_known_ = true; 
		return maze_; 
	}

	cell_location cells[MAX_MOVES];
	for (uint8_t i = 0; i != MAX_MOVES; ++i) {
		moves_[i] = NO_MOVE;
		cells[i] = cell_location(); 
		yield(); // run traj_controller.tick(); 
	}

	Queue<PlanNode> dfsq;
	dfsq.push(PlanNode(maze.getStart()));
	PlanNode curr_node; 
	cell_location next_node, tmp; 
	boolean found_move; 

  	while (!dfsq.is_empty()) {
		  yield(); // run traj_controller.tick(); 
		curr_node = dfsq.pop();
		curr = curr_node.cell_;
		maze.setRob(curr);
		x = curr_node.cell_.getX();
		y = curr_node.cell_.getY();
		curr_flood = flood[x][y];

		moves_[curr_node.num_] = curr_node.move_;

		if (curr_node.move_ == FORWARD) {
			cells[curr_node.num_forward_] = curr_node.cell_;
		}

		if (maze.isDest(curr_node.cell_)) {
			for (uint8_t i = 0; i != curr_node.num_forward_; ++i)
				maze.setPos(cells[i].getX(), cells[i].getY(), i);
			break; 
		}

		yield(); // run traj_controller.tick(); 
		found_move = false;

		if (maze.canMove(LEFT)) {
      	next_node = curr + LEFT;
      	tmp = next_node + FORWARD;
			tmp_flood = flood[tmp.getX()][tmp.getY()];

      	if (tmp_flood == curr_flood - 1 && tmp_flood != - 1) {
        		dfsq.push_front(PlanNode(next_node, LEFT, curr_node.num_+1, curr_node.num_forward_));
				found_move = true;
			}
    	} 

		if (maze.canMove(RIGHT)) {
			next_node = curr + RIGHT;
			tmp = next_node + FORWARD;
			tmp_flood = flood[tmp.getX()][tmp.getY()];

			if (tmp_flood == curr_flood - 1 && tmp_flood != - 1) {
				found_move = true;
				dfsq.push_front(PlanNode(next_node, RIGHT, curr_node.num_+1, curr_node.num_forward_)); 
			}
		} 

		if (maze.canMove(FORWARD)) {
			tmp = curr_node.cell_ + FORWARD;
			tmp_flood = flood[tmp.getX()][tmp.getY()];

			if (tmp_flood == curr_flood - 1 && tmp_flood != - 1) {
				found_move = true;
        		dfsq.push_front(PlanNode(tmp, FORWARD, curr_node.num_+1, curr_node.num_forward_+1)); 
			}
		}

		yield(); // run traj_controller.tick(); 
		if (!found_move) {
			tmp = curr + RIGHT; 
			dfsq.push_front(PlanNode(tmp, RIGHT, curr_node.num_+1, curr_node.num_forward_));
		}
	}
	yield(); // run traj_controller.tick(); 
	print_maze_serial(maze);
}

void Plan::display_moves() {
	if (error_known_) return;
	uint8_t i = 1;
	while (i < MAX_MOVES && moves_[i] != NO_MOVE) {
		yield(); // run traj_controller.tick(); 
		bluetooth::print(i);
		bluetooth::print(F(" - "));
		yield(); // run traj_controller.tick(); 
		switch (moves_[i]) {
			case FORWARD:	bluetooth::print_line(F("forward")); break;
			case LEFT:		bluetooth::print_line(F("left")); break; 
			case RIGHT:		bluetooth::print_line(F("right")); break; 
			default:
				bluetooth::print_line(F("Error Plan::display_moves"));
		}
		++i; 
	}
	return;
}

Queue<Move> Plan::solve(cell_location start, cell_location dest) {
	yield(); // run traj_controller.tick(); 
	solve_flood(start, dest);
	
	yield(); // run traj_controller.tick(); 
	display_moves();

	yield(); // run traj_controller.tick(); 
	Queue<Move> return_moves; 
	uint8_t i = 0; 
	while (++i < MAX_MOVES && moves_[i] != NO_MOVE) {
		return_moves.push(moves_[i]);
		yield(); // run traj_controller.tick(); 
	}
		
	hardware::bluetooth::print_line("DONE SOLVE!!");
	
	return return_moves;
}
