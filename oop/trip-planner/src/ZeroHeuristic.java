/**
 * A Heuristic that returns 0 for it's approximation, 
 * hence essentially simulating Dijksta's algorithm.
 * @author Zac Hamid - z5059915
 * @see Heuristic
 * @see CustomHeuristic
 *
 */
public class ZeroHeuristic implements Heuristic{
	
	@Override
	public <E> int getHCost(State<E> state) {
		return 0;
	}
	
}
