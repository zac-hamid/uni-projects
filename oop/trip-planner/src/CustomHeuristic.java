/**
 * A customised Heuristic to find an approximation as to how far away a State is from a goal State.
 * @author Zac Hamid - z5059915
 * @see Heuristic
 * @see ZeroHeuristic
 */
public class CustomHeuristic implements Heuristic{
	/**
	 * This Heuristic adds all path travel times in <CODE>desiredTrips</CODE> as
	 * well as all transfer times except the two most expensive. This guarantees
	 * that the hCost will always be &le; the true cost from <CODE>state</CODE> to
	 * the goal State.
	 */
	@Override
	public <E> int getHCost(State<E> state) {
		int hCost = 0;
		int totalTransferTime = 0;
		int highestTransferTime = 0;
		int secondHighestTransferTime = 0;
		for (Path<E> trip : state.getDesiredTrips()){
			hCost += trip.getTravelTime();
			totalTransferTime += trip.getTo().getTransferTime();
			if (trip.getTo().getTransferTime() > highestTransferTime){
				highestTransferTime = trip.getTo().getTransferTime();
			} else if (trip.getTo().getTransferTime() > secondHighestTransferTime){
				secondHighestTransferTime = trip.getTo().getTransferTime();
			}
		}
		totalTransferTime -= highestTransferTime;
		totalTransferTime -= secondHighestTransferTime;
		hCost += totalTransferTime;
		return hCost;
	}
}
