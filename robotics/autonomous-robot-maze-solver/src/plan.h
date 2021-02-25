#ifndef PLAN_H_
#define PLAN_H_

#include <Arduino.h>

#include "leo.h"
#include "hardware.h" 
#include "queue.h"

#define MAX_MOVES 50

// #define SHOW_MOVES 1
// #define PQ_DEBUG 1
// #define SHOW_FLOOD 1

using namespace hardware;

class PlanNode {
  public:
    // Default constructor
    PlanNode() : 
      cell_{}, move_{NO_MOVE}, num_{0}, num_forward_{0} {}
    
    // Constructor for single cell (usually start node)
    PlanNode(const cell_location& cell) : PlanNode{cell, NO_MOVE, 0, 0} {}
    
    // Proper constructor 
    PlanNode(cell_location cell, Move move, uint8_t num, uint8_t num_forward) :
      cell_{cell}, move_{move}, num_{num}, num_forward_{num_forward} {}
    
    // Copy constructor
    PlanNode(const PlanNode& other) :
      cell_{other.cell_},
      move_{other.move_}, 
      num_{other.num_},
      num_forward_{other.num_forward_} {}

    cell_location cell_;
    Move move_;
    uint8_t num_;
    uint8_t num_forward_;  
};

class Plan {
  public:
    explicit Plan(maze_layout maze) : maze_{maze} {
      error_known_ = false;
    } 

    ~Plan() = default; 

    maze_layout& getMaze() {return maze_; }
    Queue<Move> solve(cell_location start = cell_location(), cell_location dest = cell_location()); 

  private: 
    maze_layout maze_; 
    Move moves_[MAX_MOVES]; 
    bool error_known_;

    void solve_flood(cell_location& start, cell_location& dest); 
    void display_moves();
    void yield(); 
};

#endif // PLAN_H_
