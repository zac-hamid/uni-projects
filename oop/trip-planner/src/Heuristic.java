/**
 * A strategy to be utilised by {@link AStarSearch} to optimize the search.
 * @author Zac Hamid - z5059915
 * @see CustomHeuristic
 * @see ZeroHeuristic
 */
public interface Heuristic {
	/**
	 * Finds an approximation as to how far away the current {@link State} is from a goal State.
	 * @param state The current State.
	 * @return A guess of how far away the current State is from the goal State, the {@link StateComparator} then uses this guess
	 * to sort the State objects in a PriorityQueue by <CODE>f(x) = g(x) + h(x)</CODE>, where:
	 * <ul>
	 * 	<li>f(x) an approximation of how much the path containing this State will cost</li>
	 * 	<li>g(x) is the actual cost of the path taken till the current State.</li>
	 * 	<li>h(x) the guess returned by {@link #getHCost(State)}.</li>
	 * </ul>
	 */
	<E> int getHCost(State<E> state);
}
