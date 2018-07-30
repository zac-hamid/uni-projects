import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**

 * Our program stores data in order to begin to decide what moves it should make. 
 * These include:
 * 	- A representation of the environment seen so far by the agent, relative to the starting location
 * 		stored in a HashMap to represent the Position and Character at that position
 * 	- A list of elements that the agent is able to view in its 5x5 grid
 * 		Such elements include axes, keys, trees, dynamite, gold, etc.
 *		This is stored as a Map with the location and the character representation of the element.
 *	- The current state of the agent
 *		see State.java
 *	- A goalState which determines where we would like to go, enabling us to plan a path to a position
 *
 * CurrMap, a Map representation of the environment seen so far by the agent, has keys that are Positions
 * relative to the starting position of the agent. This is set as 0,0 and the map can hold up to 160 x 160
 * elements, this is to ensure that no matter where the agent starts, there is room on the map to add all elements
 * relative to the agent. The agent updates this map through its rotatedView, another map which only shows 
 * the agent a 5x5 grid view of its world. 
 * 
 * The agent also uses this to decide what move it should make. It was decided that certain actions needed to be 
 * taken with more importance than others these actions are (in descending order of priority) are: returning to 
 * start with the treasure, getting tools/treasure, exploring the environment. It uses an A* search, 
 * with a Manhattan Distance Heuristic and a priority queue which compares the heuristic cost + the 
 * goal cost, to determine which path it should take to get either to an element of interest or the 
 * starting position. If a path is found then it will travel from the current position 
 * to the element of interest. Through this, if there are multiple elements of interest that 
 * can be seen in the view, it will find elements by its proximity to the agent. i.e.
 * getting the closest tools/treasure first. Not only this, through finding a path through A*, it will
 * find the shortest possible valid path to whatever its goal is. If the agent is unable to find a valid
 * path to an element, it will then continue searching the environment. To do this, it uses a Breadth-First
 * Search to find the closest valid unknown area relative to its current position and then goes there to map the
 * area. 
 * 
 * The A* Search and Breadth-First searches have been moved to different classes to avoid clutter.
 * 
 * 
 * @author				Yaminn Aung, z5061216<br>
 * 						Zachary Hamid, z5059915
 **/

public class DecisionMaker {
	
	private final Position startPos;
	
	private Map<Position, Character> currMap;
	private List<State> stateList;
	private State currentState;
	
	private List<Character> movesToMake;
	private State goalState;
	private Map<Position, Character> foundElems;
	
	private Searcher search;
	
	private boolean firstView = true;
	
	/**
	 * Constructs a DecisionMaker with a Map representation of the world, a map of elements to find,
	 * the starting position, a list of states executed, a default nullMap, the current state of
	 * the agent, a list of moves the agent must make to get to a goal.
	 * 
	 */
	public DecisionMaker(){
		currMap = new HashMap<Position,Character>();
		foundElems = new HashMap<Position, Character>();
		startPos = new Position(0,0);
		stateList = new ArrayList<State>();
		setupMap();
		Map<Position,Character> nullMap = null;
		currentState = new State(startPos, World.DIR_SOUTH, nullMap, null, 0);
		stateList.add(currentState);
		movesToMake = new ArrayList<Character>();
		goalState = null;
		search = new Searcher();
	}
	
	/**
	 * Constructs a 160x160 grid map to store the agent's representation of the world that it has
	 * seen so far, initialises the grid to store all UNKNOWN (?) tiles.
	 * @see currMap
	 * 
	 */
	private void setupMap(){
		for (int x = World.MIN_WIDTH; x <= World.MAX_WIDTH; x++){
			for (int y = World.MIN_HEIGHT; y <= World.MAX_HEIGHT; y++){
				currMap.put(new Position(x,y), World.UNKNOWN);
			}
		}
	}
	
	
	/**
	 * Decides the next move the agent must make, whether it's turning, picking up tools or exploring.
	 * The priority of such actions are
	 * 1) Return to start with gold
	 * 3) Get tools and gold
	 * 4) Explore
	 * @param 	view					The 5x5 grid that the agent can immediately see
	 * <DL>
	 * <DT><b>Preconditions:</b>
	 * <DD>- the is view valid and the currentState is not null
	 * </DL>
	 * @return	char					returns the action the agent is to execute in the form of a char.
	 * <DL>
	 * <DT><b>PostConditions:</b>
	 * <DD>- Returned character must be a valid move
	 * </DL>
	 * 
	 * @see 	State
	 * @see		#firstView
	 * @see		#movesToMake
	 * @see		#goalState
	 * @see		#isExploring
	 * @see		#startPos
	 * @see		#currentState
	 */
	public char decide(char[][] view){
		
		//Since the agent can start with any rotation, sets the initial currentState to this direction upon first view
		//so that the world map can be rotated appropriately.
		if (firstView){
			switch(view[2][2]){
			case World.AGENT_NORTH:
				currentState.setDir(World.DIR_NORTH);
				break;
			case World.AGENT_SOUTH:
				currentState.setDir(World.DIR_SOUTH);
				break;
			case World.AGENT_EAST:
				currentState.setDir(World.DIR_EAST);
				break;
			case World.AGENT_WEST:
				currentState.setDir(World.DIR_WEST);
				break;
			}
			firstView = false;
		}
		loadView(view);
		
		//If moving forward isn't valid, or there are no moves to make, clear what we've found
		if (!search.valid(getFront(currentState.getPos(), currentState.getDir()),currentState,currMap) || movesToMake.isEmpty()){
			movesToMake.clear();
			goalState = null;
			currentState.setIsExploring(false);
		}
		
		//PRIORITY 1: WIN 
		if(currentState.hasTreasure()){
			List<Position> goalPos = new ArrayList<Position>();
			goalPos.add(startPos);
			movesToMake = search.aStarPos(currentState, goalPos, currMap);
		}

		if(getFront(currentState.getPos(), currentState.getDir()) == World.TREE &&
			currentState.hasAxe()){
			currentState.setRaft(true);
			return 'c';
		}
		
		if(getFront(currentState.getPos(), currentState.getDir()) == World.DOOR &&
				currentState.getKey()){
			return 'u';
		}	

		//Temporary, if there's no goal state, find one using BFS and get a path to it
		//Using A*
		
		//PRIORITY 2: Find and collect
		//Add any items to foundElems that are in our view
		if (movesToMake.isEmpty()) {
			if(!foundElems.isEmpty()) movesToMake = search.aStarElem(currentState, foundElems, currMap);
		}
		
		//PRIORITY 3: EXPLORE
		//If there are none to pick up or to get to, explore
		if (movesToMake.isEmpty()){
			while(goalState == null){
				currentState.setIsExploring(true);
				goalState = search.BFS(currentState, currMap);
				if (goalState != null){
					List<Position> goalPos = new ArrayList<Position>();
					goalPos.add(goalState.getPos());
					movesToMake = search.aStarPos(currentState, goalPos, currMap);
				}
			}
		}
		Character c = movesToMake.get(0);
		movesToMake.remove(0);

		//Update current state depending on the character returned
		switch(c){
		case 'r':
			currentState.setDir(rotateAgent(currentState.getDir(), World.ROTATE_RIGHT));
			break;
		case 'l':
			currentState.setDir(rotateAgent(currentState.getDir(), World.ROTATE_LEFT));
			break;
		case 'f':
			if(getFront(view) == World.TREASURE) currentState.setTreasure(true);
			if(getFront(view) == World.AXE) currentState.setAxe(true);
			if(getFront(view) == World.KEY) currentState.setKey(true);
			if(getFront(view) == World.WATER) currentState.setOnWater(true);
			if(getFront(view) == World.LAND && currentState.onWater()){
				currentState.setOnWater(false);
				currentState.setRaft(false);
			}
			if (search.valid(getFront(view), currentState, currMap)) {
				currentState.setPos(moveForward(currentState.getPos(), currentState.getDir()));
			}
			break;
		}
		return c;
	}
	
	/**
	 * Rotates and loads the current view into the map based on the current state. Also populates
	 * foundElems with any tools of interest it finds.
	 * @param 	view		The agents immediate view.
	 * 
	 * @see #rotateView(char[][])
	 * @see #foundElems
	 */
	private void loadView(char[][] view){
		int agentCenterX = 2;
		int agentCenterY = 2;
		
		char[][] rotatedView = rotateView(view);
		
		
		for (int j = 0; j < 5; j++){
			for (int i = 0; i < 5; i++){
				//Will make tiles relative to the center for position purposes
				//		0		1		2		3		4
				//0 [(-2,+2)|(-1,+2)|(+0,+2)|(+1,+2)|(+2,+2)]
				//1 [(-2,+1)|(-1,+1)|(+0,+1)|(+1,+1)|(+2,+1)]
				//2 [(-2,+0)|(-1,+0)|(+0,+0)|(+1,+0)|(+2,+0)]
				//3 [(-2,-1)|(-1,-1)|(+0,-1)|(+1,-1)|(+2,-1)]
				//4 [(-2,-2)|(-1,-2)|(+0,-2)|(+1,-2)|(+2,-2)]
				int xPos = j - agentCenterX;
				int yPos = agentCenterY - i;
				
				Position currPos = currentState.getPos();
				int mapXPos = currPos.getX() + xPos;
				int mapYPos = currPos.getY() + yPos;
				if(rotatedView[i][j] == World.AXE || 
					rotatedView[i][j] == World.KEY||
					rotatedView[i][j] == World.DYNAMITE||
					rotatedView[i][j] == World.TREASURE){
						foundElems.put(new Position(mapXPos,mapYPos), rotatedView[i][j]);
		        }
				
				if(rotatedView[i][j] == World.DOOR && currentState.getKey()){
					foundElems.put(new Position(mapXPos,mapYPos), rotatedView[i][j]);
				}
				
				if(rotatedView[i][j] == World.TREE && currentState.hasAxe() && !currentState.haveRaft()){
					foundElems.put(new Position(mapXPos,mapYPos), rotatedView[i][j]);
				}
				
				if (foundElems.get(new Position(mapXPos,mapYPos)) != null){
					if (foundElems.get(new Position(mapXPos,mapYPos)) != rotatedView[i][j]){
						foundElems.remove(new Position(mapXPos,mapYPos));
					}
				}
				currMap.put(new Position(mapXPos,mapYPos), rotatedView[i][j]);
			}
		}
	}
	
	
	/**
	 * Rotates the received 5x5 view of the world so that it always faces the same way.
	 * @param view	The 5x5 array of characters representing the view of the world that was passed to the agent.
	 * @return	char[][]	The corrected (read: rotated) view of the world.
	 */
	private char[][] rotateView(char[][] view){
		char[][] rotatedView = view;
		int agentChar = currentState.getDir();
		switch(agentChar){
		case World.DIR_NORTH:
			//No rotation is needed
			break;
		case World.DIR_EAST:
			//rotate view 90 deg clockwise
			rotatedView = MathHelper.rotateCW(rotatedView);
			break;
		case World.DIR_SOUTH:
			//rotate view 180 deg
			rotatedView = MathHelper.rotateCW(rotatedView);
			rotatedView = MathHelper.rotateCW(rotatedView);
			break;
		case World.DIR_WEST:
			//rotate view 90 anticlockwise
			rotatedView = MathHelper.rotateCW(rotatedView);
			rotatedView = MathHelper.rotateCW(rotatedView);
			rotatedView = MathHelper.rotateCW(rotatedView);
			break;
		default:
				break;
		}
		return rotatedView;
	}
	
	/**
	 * Gets the character that is directly in front of the agent.
	 * @param pos	The position of the agent.
	 * @param dir	The direction the agent is facing.
	 * @return Character	Returns the character that is in front of the agent.
	 */
	private Character getFront(Position pos, int dir){
		if (dir == World.DIR_NORTH)
			return currMap.get(new Position(pos.getX(),pos.getY()+1));
		else if (dir == World.DIR_SOUTH)
			return currMap.get(new Position(pos.getX(),pos.getY()-1));
		else if (dir == World.DIR_EAST)
			return currMap.get(new Position(pos.getX()+1,pos.getY()));
		else 
			return currMap.get(new Position(pos.getX()-1,pos.getY()));
	}
	
	/**
	 * Gets the character that is directly in front of the agent with respect to a 5x5 view of what is around the agent.
	 * @param view	The 5x5 view of the agent.
	 * @return char	Returns the character in front of the agent.
	 */
	private char getFront(char[][] view){
		int dir = currentState.getDir();
		Position currPos = currentState.getPos();
		int x = currPos.getX();
		int y = currPos.getY();
		if(dir == World.DIR_NORTH){
			return currMap.get(new Position(x, y+1));
		} else if(dir == World.DIR_EAST){
			return currMap.get(new Position(x+1,y));
		} else if(dir == World.DIR_SOUTH){
			return currMap.get(new Position(x, y-1));
		} else { //dir == DIR_WEST
			return currMap.get(new Position(x-1, y));
		}

	}
	
	/**
	 * Rotates the agent in a given direction.
	 * <li> ROTATE_LEFT:	Rotates the agent counter-clockwise.</li>
	 * <li> ROTATE_RIGHT:	Rotates the agent clockwise. </li>
	 * @param agentDir		The current direction the agent is facing.
	 * @param rotateDirection	The desired direction of rotation (clockwise/counter-clockwise).
	 * @return	int		Returns the new direction of the agent.
	 */
	private int rotateAgent(int agentDir, int rotateDirection){
		int dir = agentDir;
		if (dir == World.DIR_NORTH && rotateDirection == World.ROTATE_LEFT){ dir = World.DIR_WEST; }
		else if (dir == World.DIR_WEST && rotateDirection == World.ROTATE_RIGHT){ dir = World.DIR_NORTH; }
		else { dir += rotateDirection; }
		return dir;
	}
	
	/**
	 * Gets the new position of the agent if it was to move forward in a given direction.
	 * @param agentPos	The current position of the agent.
	 * @param agentDir	The current direction of the agent.
	 * @return Position	Returns the new position of the agent if it moves forward in direction, <CODE>agentDir</CODE>.
	 */
	private Position moveForward(Position agentPos, int agentDir){
		Position pos = null;
		switch(agentDir){
		case World.DIR_NORTH:
			pos = new Position(agentPos.getX(), agentPos.getY()+1);
			break;
		case World.DIR_SOUTH:
			pos = new Position(agentPos.getX(), agentPos.getY()-1);
			break;
		case World.DIR_EAST:
			pos = new Position(agentPos.getX()+1, agentPos.getY());
			break;
		case World.DIR_WEST:
			pos = new Position(agentPos.getX()-1, agentPos.getY());
			break;
		}
		return pos;
	}

	/**
	 * Prints a 5x5 array of characters to the console (for debugging purposes).
	 * @param view	The 5x5 array to print.
	 */
	public void printView(char[][] view){
		int i,j;
	    System.out.println("\n+-----+");
	    for( i=0; i < 5; i++ ) {
	         System.out.print("|");
	         for( j=0; j < 5; j++ ) {
	            if(( i == 2 )&&( j == 2 )) {
	               System.out.print('^');
	            }
	            else {
	               System.out.print( view[i][j] );
	            }
	         }
	         System.out.println("|");
	      }
	      System.out.println("+-----+");
	}
	
	/**
	 * Prints the map stored by the agent to the console (for debugging purposes).
	 */
	public void printMap(){
		for (int i = World.MIN_WIDTH; i <= World.MAX_WIDTH; i++){
			if (i == World.MIN_WIDTH || i == World.MAX_WIDTH){
				System.out.print("+");
				if (i == World.MAX_WIDTH) System.out.println();
			} else {
				System.out.print("-");
			}
		}
		
		for (int j = World.MAX_HEIGHT; j >= World.MIN_HEIGHT; j--){
			for (int i = World.MIN_WIDTH; i <= World.MAX_WIDTH; i++){
				if (i == World.MIN_WIDTH){
					System.out.print("|");
				}
				char c = currMap.get(new Position(i, j));
				System.out.print(c);
				if (i == World.MAX_WIDTH){
					System.out.print("|");
				}
			}
			System.out.println();
		}

		for (int i = World.MIN_WIDTH; i <= World.MAX_WIDTH; i++){
			if (i == World.MIN_WIDTH || i == World.MAX_WIDTH){
				System.out.print("+");
				if (i == World.MAX_WIDTH) System.out.println();
			} else {
				System.out.print("-");
			}
		}
	}
	
	/**
	 * Prints the map stored by the agent to the console within the bounds (-boundWidth,boundWidth) and (-boundHeight,boundHeight) (for debugging purposes).
	 * @param boundWidth	The boundary width of the map to print.
	 * @param boundHeight	The boundary height of the map to print.
	 */
	public void printMap(int boundWidth, int boundHeight){
		for (int i = -boundWidth; i <= boundWidth; i++){
			if (i == -boundWidth || i == boundWidth){
				System.out.print("+");
				if (i == boundWidth) System.out.println();
			} else {
				System.out.print("-");
			}
		}
		
		for (int j = boundHeight; j >= -boundHeight; j--){
			for (int i = -boundWidth; i <= boundWidth; i++){
				if (i == -boundWidth){
					System.out.print("|");
				}
				char c = currMap.get(new Position(i, j));
				System.out.print(c);
				if (i == boundWidth){
					System.out.print("|");
				}
			}
			System.out.println();
		}

		for (int i = -boundWidth; i <= boundWidth; i++){
			if (i == -boundWidth || i == boundWidth){
				System.out.print("+");
				if (i == boundWidth) System.out.println();
			} else {
				System.out.print("-");
			}
		}
	}
}
