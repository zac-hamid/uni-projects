import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Queue;

public class Searcher {
	
	/**
	 * Implements a Breadth-First Search to find an UNKNOWN position.
	 * @param currentState	The starting state for the search.
	 * @param currMap		The current map of the world that has been stored.
	 * <DL>
	 * <DT><b>Preconditions:</b>
	 * <DD>- currentState is not null and the currentState is at some position.
	 * </DL>
	 * <DL>
	 * <DT><b>PostConditions:</b>
	 * <DD>- Returned character must have a valid path to get to it,
	 * <DD>- State is not altered in any way
	 * <DD>- The returned state is the closest Unknown.
	 * </DL>
	 * @return	State		Returns the State of the first found Unknown.
	 * 
	 * @see		#currentState
	 * @see		State
	 */
	public State BFS(State currentState, Map<Position,Character> currMap){
		
		List<Position> visited = new ArrayList<Position>();
		// Create a queue for BFS
	    Queue<State> queue = new LinkedList<State>();
	 
	    // Mark the current node as visited and enqueue it
	    visited.add(currentState.getPos());
	    queue.add(currentState);
	    
	    //To prevent ambiguity in overloaded State constructor, makes sure the null used is a Map
    	Map<Position, Character> nullMap = null;
	 
	    while (!(queue.isEmpty())){
	        // Dequeue a vertex from queue and print it
	    	State curr =  new State(queue.poll());
	        // Get all adjacent squares of the dequeued position
	        // If an adjacent has not been visited, then mark it
            // visited and enqueue it
	    	Position pos = curr.getPos();
	    	int x = pos.getX();
	    	int y = pos.getY();
	    	
	    	//Found the closest unknown, return it.
	    	if(currMap.get(pos) == World.UNKNOWN) {
	    		return curr;
	    	}
	    	
	    	Position upPos = new Position(x,y+1);
	    	Position downPos = new Position(x,y-1);
	    	Position rightPos = new Position(x+1,y);
	    	Position leftPos = new Position(x-1,y);
	    	
	    	//if it's not in the list of visited and we can go there, add it to the queue
	    	if(!(visited.contains(upPos)) && valid(upPos,currentState,currMap)){
	    		visited.add(upPos);
	    		State up = new State(upPos, World.DIR_NORTH, nullMap, curr, curr.getGCost()+1);
	    		queue.add(up);
	    	}
	    	
	    	if(!(visited.contains(downPos)) && valid(downPos,currentState,currMap)){
	    		visited.add(downPos);
	    		State down = new State(downPos, World.DIR_SOUTH, nullMap, curr, curr.getGCost()+1);
	    		queue.add(down);
	    	}
	    	
	    	if(!(visited.contains(rightPos)) && valid(rightPos,currentState,currMap)){
	    		visited.add(rightPos);
	    		State right = new State(rightPos, World.DIR_EAST, nullMap, curr, curr.getGCost()+1);
	    		queue.add(right);
	    	}
	    	
	    	if(!(visited.contains(leftPos)) && valid(leftPos,currentState,currMap)){
	    		visited.add(leftPos);
	    		State left = new State(leftPos, World.DIR_WEST, nullMap, curr, curr.getGCost()+1);
	    		queue.add(left);
	    	}

	     }
	     return null;
	}
	
	/**
	 * This method searches for the optimal path to travel along (the path with the least cost)
	 * given a list of elements we wish to collect. It then returns the path the agent is to take
	 * to collect all these elements. 
	 * @param	currentState		The starting state of the search.
	 * @param 	foundElems			A list of elements of interest that lie within a 5x5 grid of the agent
	 * <DL>
	 * <DT><b>Preconditions:</b>
	 * <DD>- foundElems is not empty
	 * </DL>
	 * @param	currMap				The current map of the world that has been stored.
	 * @return	List<Character>		returns a list of moves the agent is to make to get to the list of elements
	 * <DL>
	 * <DT><b>PostConditions:</b>
	 * <DD>- Returned path is valid,
	 * <DD>- currentState is not altered,
	 * <DD>- Returned path is the optimal path to collect all foundElems
	 * <DD>- If no path is found, an empty list is returned.
	 * </DL>
	 * 
	 * @see 	State
	 * @see		#foundElems
	 * @see		#currentState
	 */
	public List<Character> aStarElem(State currentState, Map<Position,Character> foundElems, Map<Position,Character> currMap){
		PriorityQueue<State> pq = new PriorityQueue<State>();
		List<Position> visited = new ArrayList<Position>();
		State startState = new State(currentState);
		startState.setGoalElems(foundElems);
		pq.add(startState);
		State curr = null;
		
		while(!(pq.isEmpty())){
			curr = pq.poll();
			
			visited.add(curr.getPos());
			//If the current position is == to the remaining goal state  then we're done with A*!
			if(curr.getGoalElems().isEmpty()) {
				return moveList(currentState.getDir(), backtrackStates(currentState, curr), currMap);
			}
			
			/*
			 * Otherwise for each of the found elements in elemInView create a state if it is valid
			 * if the state travels through water, increase the fCost of the state to discourage water
			 * travel, if the new state's position has an item of interest, remove it from our goal list
			 * of elements in the new state, if we're trying to reach a tree and it would be in front of
			 * us subsequent our new state, remove the tree from the goal for the new state.
			 */
			int posX = curr.getPos().getX();
			int posY = curr.getPos().getY();
			int fCostIncrease = 0;
			
			Position rightPos = new Position(posX+1,posY);
			Position leftPos = new Position(posX-1,posY);
			Position upPos = new Position(posX,posY+1);
			Position downPos = new Position(posX,posY-1);
			
			if(valid(rightPos, currentState, currMap) && !(visited.contains(rightPos))){
				if (currMap.get(rightPos) == World.WATER) { fCostIncrease = 10; }
				State right = new State(rightPos, World.DIR_EAST, curr.getGoalElems(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalElems().containsKey(rightPos)){
					right.getGoalElems().remove(rightPos);
				}
				pq.add(right);
			}
			
			if(valid(leftPos, currentState, currMap) && !(visited.contains(leftPos))){
				if (currMap.get(leftPos) == World.WATER) { fCostIncrease = 10; }
				State left = new State(leftPos, World.DIR_WEST, curr.getGoalElems(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalElems().containsKey(leftPos)){
					left.getGoalElems().remove(leftPos);
				}
				pq.add(left);
			}
			
			if(valid(upPos, currentState, currMap) && !(visited.contains(upPos))){
				if (currMap.get(upPos) == World.WATER) { fCostIncrease = 10; }
				State up = new State(upPos, World.DIR_NORTH, curr.getGoalElems(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalElems().containsKey(upPos)){
					up.getGoalElems().remove(upPos);
				}
				pq.add(up);
			}
			
			if(valid(downPos, currentState, currMap) && !(visited.contains(downPos))){
				if (currMap.get(downPos) == World.WATER) { fCostIncrease = 10; }
				State down = new State(downPos, World.DIR_SOUTH, curr.getGoalElems(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalElems().containsKey(downPos)){
					down.getGoalElems().remove(downPos);
				}
				pq.add(down);
			}
		}
		/*
		 * Couldn't find a path, return an empty list
		 */
		foundElems.clear();
		List<Character> emptySolution = new ArrayList<>();
		return emptySolution;
	}

	/**
	 * This method searches for the optimal path to travel along (the path with the least cost)
	 * given a list of positions we wish to go to. It then returns the path the agent is to travel to
	 * these positions.
	 * @param	currentState		The starting state of the search.
	 * @param 	goalPositions		A list of positions the agent would like to travel to.
	 * <DL>
	 * <DT><b>Preconditions:</b>
	 * <DD>- currState is not null and goalPositions is not empty.
	 * </DL>
	 * @param	currMap				The current map of the world that has been stored.
	 * @return	List<Character>		returns a list of moves the agent is to make to visit all goal positions.
	 * <DL>
	 * <DT><b>PostConditions:</b>
	 * <DD>- Returned path is valid,
	 * <DD>- currentState is not altered,
	 * <DD>- Returned path is the optimal path to collect all foundElems. 
	 * <DD>- If no path is found, an empty list is returned.
	 * </DL>
	 * 
	 * @see 	State
	 * @see		#goalPositions
	 * @see		#currentState
	 */
	public List<Character> aStarPos(State currentState, List<Position> goalPositions, Map<Position,Character> currMap){
		PriorityQueue<State> pq = new PriorityQueue<State>();
		List<Position> visited = new ArrayList<Position>();
		State startState = new State(currentState.getPos(), currentState.getDir(), goalPositions, null, 0);
		pq.add(startState);
		State curr = null;
		while(!(pq.isEmpty())){
			curr = pq.poll();

			//If the current position is == to the goal state then we're done with A*!
			if(curr.getGoalPositions().isEmpty()) {
				return moveList(currentState.getDir(), backtrackStates(currentState, curr), currMap);
			}
			
			/*
			 * Otherwise for each of the found elements in elemInView create a state if it is valid
			 * if the state travels through water, increase the fCost of the state to discourage water
			 * travel, if the new state's position has an item of interest, remove it from our goal list
			 * of elements in the new state.
			 */
			int posX = curr.getPos().getX();
			int posY = curr.getPos().getY();
			int fCostIncrease = 0;
			
			Position rightPos = new Position(posX+1,posY);
			Position leftPos = new Position(posX-1,posY);
			Position upPos = new Position(posX,posY+1);
			Position downPos = new Position(posX,posY-1);
			
			if(valid(rightPos, currentState, currMap) && !visited.contains(rightPos)){
				if (currMap.get(rightPos) == World.WATER) { fCostIncrease = 10; }
				State right = new State(rightPos, World.DIR_EAST, curr.getGoalPositions(), curr, curr.getGCost() + 1 + fCostIncrease);
				if(curr.getGoalPositions().contains(rightPos)){
					right.getGoalPositions().remove(rightPos);
				}
				pq.add(right);
				visited.add(rightPos);
			}
			
			if(valid(leftPos, currentState, currMap) && !visited.contains(leftPos)){
				if (currMap.get(leftPos) == World.WATER) { fCostIncrease = 10; }
				State left = new State(leftPos, World.DIR_WEST, curr.getGoalPositions(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalPositions().contains(leftPos)){
					left.getGoalPositions().remove(leftPos);
				}
				pq.add(left);
				visited.add(leftPos);
			}
			
			if(valid(upPos, currentState, currMap) && !visited.contains(upPos)){
				if (currMap.get(upPos) == World.WATER) { fCostIncrease = 10; }
				State up = new State(upPos, World.DIR_NORTH, curr.getGoalPositions(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalPositions().contains(upPos)){
					up.getGoalPositions().remove(upPos);
				}
				pq.add(up);
				visited.add(upPos);
			}
			
			if(valid(downPos,currentState,currMap) && !visited.contains(downPos)){
				if (currMap.get(downPos) == World.WATER) { fCostIncrease = 10; }
				State down = new State(downPos, World.DIR_SOUTH, curr.getGoalPositions(), curr, curr.getGCost()+1+fCostIncrease);
				if(curr.getGoalPositions().contains(downPos)){
					down.getGoalPositions().remove(downPos);
				}
				pq.add(down);
				visited.add(downPos);
			}
		}
		/*
		 * No solution found, return an empty list
		 */
		List<Character> emptySolution = new ArrayList<>();
		return emptySolution;
	}
	
	/**
	 * Backtracks to start state and builds a list of positions from start to goal.
	 * 
	 * @param startState			the starting state of the agent.
	 * @param endState				the end state of the agent.
	 * @return List<Position>		returns a list of positions the agent took to get from the start to the end.
	 */
	private List<Position> backtrackStates(State startState, State endState){
		List<Position> pathList = new ArrayList<Position>();
		State curr = endState;
		while(curr.getPos() != startState.getPos()){
			pathList.add(curr.getPos());
			curr = curr.getPreviousState();
		}
		pathList.add(curr.getPos());
		Collections.reverse(pathList);
		return pathList;
	}
	
	/**
	 * Builds up a list of characters to perform the path required and returns a list of moves the
	 * agent is to make.
	 * @param start_dir			Integer starting direction of the agent
	 * @param pathList			A list of positions that the agent moved around.s
	 * @param currMap			The current map of the world that has been stored.
	 * @return					A list of characters that detail the direction and movements.
	 */
	private List<Character> moveList(int start_dir, List<Position> pathList, Map<Position,Character> currMap){
		List<Character> charList = new ArrayList<Character>();
		int currDir = start_dir;
		
		if (pathList.size() > 1){
			for (int i = 0; i < pathList.size()-1; i++){
				Position currPos = pathList.get(i);
				Position nextPos = pathList.get(i+1);
				
				//Next pos is to the right
				if (nextPos.getX() > currPos.getX()){
					switch(currDir){
					case World.DIR_NORTH:
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_SOUTH:
						charList.add('l');
						charList.add('f');
						break;
					case World.DIR_EAST:
						charList.add('f');
						break;
					case World.DIR_WEST:
						charList.add('r');
						charList.add('r');
						charList.add('f');
						break;
					}
					if (currMap.get(nextPos) == World.WALL){
						charList.add('b');
					} else if (currMap.get(nextPos) == World.TREE){
						charList.add('c');
					}
					currDir = World.DIR_EAST;
				} else if (nextPos.getX() < currPos.getX()){ //Next pos is to the left
					switch(currDir){
					case World.DIR_NORTH:
						charList.add('l');
						charList.add('f');
						break;
					case World.DIR_SOUTH:
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_EAST:
						charList.add('r');
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_WEST:
						charList.add('f');
						break;
					}
					if (currMap.get(nextPos) == World.WALL){
						charList.add('b');
					} else if (currMap.get(nextPos) == World.TREE){
						charList.add('c');
					}
					currDir = World.DIR_WEST;
				} else if (nextPos.getY() > currPos.getY()){ // Next pos is up
					switch(currDir){
					case World.DIR_NORTH:
						charList.add('f');
						break;
					case World.DIR_SOUTH:
						charList.add('r');
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_EAST:
						charList.add('l');
						charList.add('f');
						break;
					case World.DIR_WEST:
						charList.add('r');
						charList.add('f');
						break;
					}
					if (currMap.get(nextPos) == World.WALL){
						charList.add('b');
					} else if (currMap.get(nextPos) == World.TREE){
						charList.add('c');
					}
					currDir = World.DIR_NORTH;
				} else if (nextPos.getY() < currPos.getY()){ // Next pos is down
					switch(currDir){
					case World.DIR_NORTH:
						charList.add('r');
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_SOUTH:
						charList.add('f');
						break;
					case World.DIR_EAST:
						charList.add('r');
						charList.add('f');
						break;
					case World.DIR_WEST:
						charList.add('l');
						charList.add('f');
						break;
					}
					if (currMap.get(nextPos) == World.WALL){
						charList.add('b');
					} else if (currMap.get(nextPos) == World.TREE){
						charList.add('c');
					}
					currDir = World.DIR_SOUTH;
				}
			}
		}

		return charList;
	}
	
	
	
	/**
	 * Checks if the position is a valid for the agent to move to via comparing it to
	 * the map representation of the world the agent has seen so far.
	 * Returns a boolean identifying whether it is valid or not.
	 * @param 	pos			Requested position the agent would like to go to.
	 * @param 	s			State of the agent when requesting validity of movement.
	 * @param 	currMap		The current map of the world that has been stored.
	 * @return				Boolean: true means it is a valid move, false is otherwise.
	 * 
	 * @see #currMap
	 */
	public boolean valid(Position pos, State s, Map<Position,Character> currMap){
		char req = currMap.get(pos);
		
		if((req == World.WATER && !s.haveRaft()) || (req == World.WALL && s.getNumDynamites() == 0)|| req == World.WORLD_EDGE){
			return false;
		} else if(req == World.DOOR && !(s.getKey())) {
			return false;
		} else if(req == World.UNKNOWN && !(s.getIsExploring())){
			return false;
		} else if(req == World.TREE && !(s.hasAxe())){
			return false;
		}
		return true;
	}
	
	/**
	 * Checks if the character is a valid for the agent to move to.
	 * Returns a boolean identifying whether it is valid or not.
	 * @param 	c		Requested character the agent would like to go to.
	 * @param 	s		State of the agent when requesting validity of movement.
	 * @param 	currMap	The current map of the world.
	 * @return			Boolean: true means it is a valid move, false is otherwise.
	 * 
	 */
	public boolean valid(Character c, State s, Map<Position,Character> currMap){
		if((c == World.WATER && !s.haveRaft()) ||
			(c == World.WALL && s.getNumDynamites() == 0) ||
			c == World.TREE ||
			c == World.WORLD_EDGE){
			return false;
		} else if(c == World.DOOR && !(s.getKey())) {
			return false;
		} else if(c == World.UNKNOWN && !(s.getIsExploring())){
			return false;
		} else if(c == World.TREE && !(s.hasAxe())){
			return false;
		}
		
		return true;
	}
}
