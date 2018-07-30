import java.util.Map;

/**
 * This class lies within the Heuristic interface as a method of calculating a heuristic.
 * This heuristic uses the Manhattan Distance Heuristic
 * 
 * @author 			Yaminn Aung, z5061216
 * 					Zachary Hamid, z5059915
 *
 */

public class ManDistHeuristic implements Heuristic {
	
	public int getHCost(State s){
		int lowest = Integer.MAX_VALUE;
		//If the state is looking for elements on the map
		if (s.getGoalElems() != null){
			for (Map.Entry<Position,Character> entry : s.getGoalElems().entrySet()){
				int stateX = s.getPos().getX();
				int stateY = s.getPos().getY();
				
				int goalX = entry.getKey().getX();
				int goalY = entry.getKey().getY();
				
				int manDistX = Math.abs(stateX - goalX);
				int manDistY = Math.abs(stateY - goalY);
				
				int manDist = manDistX + manDistY;
				
				if (manDist < lowest){
					lowest = manDist;
				}
			}
		} else if (s.getGoalPositions() != null){ //if the state is looking for specific positions on the maps
			for (int i = 0; i < s.getGoalPositions().size(); i++){
				int stateX = s.getPos().getX();
				int stateY = s.getPos().getY();
				
				int goalX = s.getGoalPositions().get(i).getX();
				int goalY = s.getGoalPositions().get(i).getY();
				
				int manDistX = Math.abs(stateX - goalX);
				int manDistY = Math.abs(stateY - goalY);
				
				int manDist = manDistX + manDistY;
				
				if (manDist < lowest){
					lowest = manDist;
				}
			}
		}
		return lowest;
	}
}
