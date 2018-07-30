/**
 * This class is responsible for storing all the constants that define the world, as well as the parameters of the
 * world map that has been stored by the agent.
 * <li>
 * DIR_X:		The direction with respect to the orientation of the world.
 * </li>
 * <li>
 * AGENT_X:		The character representing the agents direction in the overall world, used for determining how to rotate the stored map.
 * </li>
 * @author				Yaminn Aung, z5061216<br>
 * 						Zachary Hamid, z5059915
 */
public class World {
	//Constants reserved for determining which direction to rotate the agent
	public final static int ROTATE_LEFT = -1;
	public final static int ROTATE_RIGHT = 1;
	
	public final static int DIR_NORTH = 0;
	public final static int DIR_EAST = 1;
	public final static int DIR_SOUTH = 2;
	public final static int DIR_WEST = 3;
	
	//Characters specific to the world view that is passed into the agent
	public final static char AGENT_NORTH = '^';
	public final static char AGENT_SOUTH = 'v';
	public final static char AGENT_EAST = '>';
	public final static char AGENT_WEST = '<';
	
	public final static char WALL = '*';
	public final static char DOOR = '-';
	public final static char WATER = '~';
	public final static char TREE = 'T';
	
	public final static char AXE = 'a';
	public final static char KEY = 'k';
	public final static char DYNAMITE = 'd';
	public final static char TREASURE = '$';
	public final static char LAND = ' ';
	
	public final static char UNKNOWN = '?';
	
	public final static char WORLD_EDGE = '.';
	
	//Parameters of the map stored by the agent
	public final static int MAX_WIDTH = 80;
	public final static int MIN_WIDTH = -80;
	public final static int MAX_HEIGHT = 80;
	public final static int MIN_HEIGHT = -80;
}
