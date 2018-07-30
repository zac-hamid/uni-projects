import java.util.Comparator;

/**
 * Compares two States together by their <CODE>fCost</CODE>.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier for the City of a State, 
 * e.g <CODE>E</CODE> is of type <CODE>Integer</CODE>; City can 
 * be referred to as 1, 2, etc.
 * @see State
 */
public class StateComparator<E> implements Comparator<State<E>>{
	private Heuristic heuristic;
	
	public StateComparator(Heuristic heuristic){
		this.heuristic = heuristic;
	}
	/**
	 * Compares two States together by their <CODE>fCost</CODE>.
	 * @param state1 The first state to be compared.
	 * @param state2 The second state to be compared.
	 * @return 
	 * 		<li>0, if these states are equal.</li>
	 * 		<li>1, if <CODE>state1's</CODE> <CODE>fCost</CODE> is greater.</li>
	 * 		<li>-1, otherwise.</li>
	 */
	@Override
	public int compare(State<E> state1, State<E> state2) {
		int fCost1 = state1.getGCost() + heuristic.getHCost(state1);
		int fCost2 = state2.getGCost() + heuristic.getHCost(state2);
		
		if (fCost1 == fCost2) return 0;
		else if (fCost1 > fCost2) return 1;
		else return -1;
	}
}
