import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Stores important information about the world and agent.
 * Used for searching for paths through the world, knowing
 * what items the agent has picked up, where the agent wants
 * to go (goal positions/elements), and other information required
 * for making an informed decision on where to move next.
 * @author				Yaminn Aung, z5061216<br>
 * 						Zachary Hamid, z5059915
 */
public class State implements Comparable<State>{
	private Position pos;
	private int direction;
	private boolean have_raft;
	private boolean on_raft;
	private boolean off_map;
	private boolean have_axe;
	private boolean have_key;
	private boolean have_treasure;
	private int num_dynamites_held;
	private boolean on_water;
	private boolean is_exploring;
	
	private Map<Position, Character> elems;
	private List<Position> goalPositions;
	
	private State prevState;
	
	private int gCost;
	
	
	/**
	 * Constructs a State with the state's position, direction, map of elements to find with their position
	 * and character, goal cost, as well as this state's previous state.
	 * @param pos					The position of the state
	 * @param gCost					The total actual cost so far.								
	 * @param foundElems			The map of elements to find.
	 * @param previous				This state's previous state.
	 * 
	 */
	public State(Position pos, int dir, Map<Position,Character> foundElems, State previous, int gCost){
		this.pos = pos;
		this.direction = dir;
		this.have_axe     = false;
		this.have_key     = false;
		this.have_treasure= false;
		this.have_raft    = false;
		this.on_raft      = false;
		this.off_map      = false;
		this.on_water 	  = false;
		this.is_exploring = false;
		this.num_dynamites_held = 0;
		
		this.elems = new HashMap<Position, Character>();
		if (foundElems != null) elems.putAll(foundElems);
		
		goalPositions = null;
		
		this.prevState = previous;
		this.gCost = gCost;
		
	}
	
	/**
	 * Constructs a State with the state's position, direction, list of positions to go to,
	 * , goal cost, as well as this state's previous state.
	 * @param pos					The position of the state
	 * @param gCost					The total actual cost so far.								
	 * @param goalPosition			The list of positions to go to.
	 * @param previous				This state's previous state.
	 * 
	 */
	public State(Position pos, int dir, List<Position> goalPositions, State previous, int gCost){
		this.pos = pos;
		this.direction = dir;
		this.have_axe     = false;
		this.have_key     = false;
		this.have_treasure= false;
		this.have_raft    = false;
		this.on_raft      = false;
		this.off_map      = false;
		this.on_water 	  = false;
		this.is_exploring = false;
		this.num_dynamites_held = 0;
		
		this.goalPositions = new ArrayList<Position>();
		this.goalPositions.addAll(goalPositions);
		elems = null;
		
		this.prevState = previous;
		this.gCost = gCost;
		
	}
	
	/**
	 * Copy constructor for State.
	 * @param State		State to copy.
	 */
	public State(State s){
		this.pos = s.pos;
		this.direction = s.direction;
		this.have_axe = s.have_axe;
		this.have_treasure = s.have_treasure;
		this.have_raft = s.have_raft;
		this.on_raft = s.on_raft;
		this.off_map = s.off_map;
		this.on_water = s.on_water;
		this.num_dynamites_held = s.num_dynamites_held;
		this.is_exploring = s.is_exploring;
		
		this.goalPositions = s.goalPositions;
		this.elems = s.elems;
		
		this.prevState = s.prevState;
		this.gCost = s.gCost;
	}
	
	public Position getPos(){
		return pos;
	}
	
	public void setPos(Position pos){
		this.pos = pos;
	}
	
	public int getDir(){
		return direction;
	}
	
	public void setDir(int dir){
		this.direction = dir;
	}
	
	public void setAxe(boolean result){
		this.have_axe = result;
	}
	
	public boolean hasAxe(){
		return this.have_axe;
	}
	
	public void setKey(boolean result){
		this.have_key = result;
	}
	
	public boolean getKey(){
		return this.have_key;
	}
	
	public boolean onWater(){
		return this.on_water;
	}
	
	public void setOnWater(boolean onWater){
		this.on_water = onWater;
	}

	public boolean hasTreasure(){
		return this.have_treasure;
	}
	
	public void setTreasure(boolean result){
		this.have_treasure = result;
	}
	
	public boolean haveRaft(){
		return this.have_raft;
	}
	
	public void setRaft(boolean result){
		this.have_raft = result;
	}
	
	public boolean getIsExploring(){
		return is_exploring;
	}
	
	public void setIsExploring(boolean is_exploring){
		this.is_exploring = is_exploring;
	}
	
	public int getNumDynamites(){
		return this.num_dynamites_held;
	}
	
	public void setNumDynamites(int dynamites){
		this.num_dynamites_held = dynamites;
	}

	/**
	 * Sets the goal elements of the state
	 * @param elems Map containing goal elements to reach
	 */
	public void setGoalElems(Map<Position,Character> elems){
		this.elems = new HashMap<Position, Character>();
		if (elems != null) this.elems.putAll(elems);
	}
	
	public void setGoalPositions(List<Position> goals){
		this.goalPositions = new ArrayList<Position>();
		this.goalPositions.addAll(goals);
	}
	/**
	 * Adds to the number of dynamite that the agent has at this state.
	 * @see		#num_dynamites_held
	 */
	public void addDynamite(){
		this.num_dynamites_held+=1;
	}
	
	/**
	 * Removes a dynamite after use, returns boolean to determine whether it was done/possible
	 * @return	Boolean determining whether the dynamite was successfully used or not.
	 * @see		#num_dynamites_held
	 */
	public boolean removeDynamite(){
		if(this.num_dynamites_held <= 0){
			return false;
		} else {
			this.num_dynamites_held-=1;
			return true;
		}
	}
	
	public State getPreviousState(){
		return prevState;
	}
	
	public Map<Position,Character> getGoalElems(){
		return elems;
	}
	
	public List<Position> getGoalPositions(){
		return goalPositions;
	}
	
	/**
	 * Calculates the final cost of this state.
	 * <DL>
	 * <DT><b>Preconditions:</b>
	 * <DD>- Heuristic calculated is >= 0
	 * <DD>- gCost >=0.
	 * </DL>
	 * @return	Integer representation of an estimated final cost for this path.
	 * @see		Heuristic#getHCost(State)
	 * @see		#gCost
	 */
	public int getFCost(){
		//Heuristic path algorithm to enable tweaking of search type
		int w = 1;
		Heuristic h = new ManDistHeuristic();
		return (2-w)*gCost + w*h.getHCost(this);
	}

	public int getGCost() {
		return this.gCost;
	}

	@Override
	public boolean equals(Object o){
		if (this.have_axe == ((State)o).have_axe && 
				this.have_key == ((State)o).have_key &&
				this.have_raft == ((State)o).have_raft &&
				this.have_treasure == ((State)o).have_treasure &&
				this.off_map == ((State)o).off_map &&
				this.on_raft == ((State)o).on_raft &&
				this.pos == ((State)o).pos &&
				this.on_water == ((State)o).on_water &&
				this.num_dynamites_held == ((State)o).num_dynamites_held &&
				this.direction == ((State)o).direction && 
				this.gCost == ((State)o).gCost &&
				this.prevState == ((State)o).prevState &&
				this.elems == ((State)o).elems){
			return true;
		}
		return false;
	}
	
	@Override
	public int compareTo(State s) {
		if (this.getFCost() < s.getFCost()){
			return -1;
		} else if (this.getFCost() > s.getFCost()){
			return 1;
		} else {
			return 0;
		}
	}
	
	@Override
	public String toString(){
		return pos.toString() + " G Cost: " + gCost + " -> Prev: " + prevState;
	}
	
}
