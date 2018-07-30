# COMP3411
http://www.cse.unsw.edu.au/~cs3411/17s1/hw3raft/

## Assignment 3 Plan

### Storing information
* Store the map in a 160x160 Map which represents x,y coordiates relative to the startng position
* Store locations of items such as axes, keys, etc. 
* Store what the agent currently possesses as booleans 
* Store the direction the agent is currently facing.
* Final sequence of moves is stored as a list.

### Targets in prioroty order (planning):
1) Return to start with gold

2) Get gold

3) Get tools

4) Explore

### Finding out where the agent should go
* LOCALLY OBJECTIVE BASED SEARCHING/PLAN 
* Use A* search/Greedy, with Manhattan disance as heuristic to implement priority queue with lower costs as higher priority
* If an item appears in the 5*5 grid, execute greedy/A* to pick it up.
* f(n) = (2-w)g(n) + wh(n)
	* set w=1 for A* , w = 2 for greedy
* Priority of tools is sorted by proximity to the agent. 
* If it can't do any of the first three targets, then explore using BFS
    * look for unmapped points to map, including borders
    * go to the first unmapped point it finds.
* If no unmapped points - DFS to know where to go (idk what the goal would be)

* When we have a raft, treat land->water and water->water transitions as passable but after water->land then impassable again

#### General rules:
* do not use raft until we have explored everything that there is a path to without using a raft
* '.' is impassable 
* do not use dynamite  unless in A* if theres no more paths that you can take and no unrevealed area that you can get to then you have to blow something up


#### Stored in the states:
* number of moves so far
* elements picked up so far
* previous state
* position

* cost for water is 1000.