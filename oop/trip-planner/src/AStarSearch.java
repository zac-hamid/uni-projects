import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.PriorityQueue;

/**
 * Uses an A* search algorithm, and a heuristic process to find the shortest 
 * trip through City objects that will cover all Paths in a List <CODE>desiredTrips</CODE>.
 * 
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier for a City, 
 * e.g <CODE>E</CODE> is of type <CODE>Integer</CODE>; City can 
 * be referred to as 1, 2, etc.
 * @see MapGraph
 * @see Heuristic
 */
public class AStarSearch<E>{
	private List<Path<E>> desiredTrips;
	
	public AStarSearch(){
		desiredTrips = new ArrayList<Path<E>>();
	}
	
	/**
	 * Adds a desired trip to <CODE>desiredTrips</CODE>.
	 * @param g The MapGraph the search is working on.
	 * @param from The start city of the trip.
	 * @param to The end city of the trip.
	 */
	public void addTrip(MapGraph<E> g, E from, E to){
		desiredTrips.add(g.findPath(from, to));
	}
	/**
	 * Uses A* search to look for the shortest path in a MapGraph, 
	 * starting at a certain city, using a Heuristic strategy, 
	 * that includes all trips in <CODE>desiredTrips</CODE>.
	 * @param strategy The heuristic to apply to the search.
	 * @param g The MapGraph to perform the search on.
	 * @param start The starting City of the search.
	 * @see Heuristic
	 */
	public void search(Heuristic strategy, MapGraph<E> g, E start) {
		int nodesExpanded = 0;
		PriorityQueue<State<E>> pq = new PriorityQueue<State<E>>(1, new StateComparator<E>(strategy));
		State<E> initial = new State<E>(null, g.findCity(start), 0, desiredTrips);
		pq.add(initial);
		
		while(!pq.isEmpty()){
			State<E> currentState = pq.poll();
			if (currentState != null) nodesExpanded++;
			/*If a path has been found that covers all trips*/
			if (currentState.getDesiredTrips().size() == 0){
				/*Print relevant results from the search*/
				printResults(currentState, nodesExpanded);
				return;
			}
			int currentTransferTime = currentState.getCurrentCity().getTransferTime();
			int currentGCost = currentState.getGCost();
			if (currentState.getPrevious() == null) currentTransferTime = 0;
			
			/*Check if the state contains the start City of a trip in desiredTrips*/
			boolean isStartOfTrip = false;
			for (Path<E> trip : currentState.getDesiredTrips()){
				if (trip.getFrom().equals(currentState.getCurrentCity())){
					isStartOfTrip = true;
					break;
				}
			}
			
			/*If the state contains the start City of a trip, add all end City nodes from that corresponding
			 * trip to the PriorityQueue, and also add all start City nodes for all remaining trips in desiredTrips*/
			if (isStartOfTrip){
				for (City<E> city : g.getNeighbourCities(currentState.getCurrentCity())){
					for (Path<E> trip : currentState.getDesiredTrips()){
						if (trip.getFrom().equals(currentState.getCurrentCity()) && trip.getTo().equals(city)){
							State<E> nodeState = new State<E>(currentState, city, currentGCost + g.getTravelTime(city.getCityName(), currentState.getCurrentCity().getCityName()) + currentTransferTime, currentState.getDesiredTrips());
							nodeState.removeTrip(g.findPath(currentState.getCurrentCity().getCityName(), city.getCityName()));
							pq.add(nodeState);
						} else if (trip.getFrom().equals(city)){
							State<E> nodeState = new State<E>(currentState, city, currentGCost + g.getTravelTime(city.getCityName(), currentState.getCurrentCity().getCityName()) + currentTransferTime, currentState.getDesiredTrips());
							pq.add(nodeState);
						}
					}
				}
			}
			/*If the state does not contain the start City of a trip, just add all start City nodes for all remaining trips in desiredTrips*/
			else {
				for (City<E> city : g.getNeighbourCities(currentState.getCurrentCity())){
					for (Path<E> trip : currentState.getDesiredTrips()){
						if (trip.getFrom().equals(city)){
							State<E> nodeState = new State<E>(currentState, city, currentGCost + g.getTravelTime(city.getCityName(), currentState.getCurrentCity().getCityName()) + currentTransferTime, currentState.getDesiredTrips());
							pq.add(nodeState);
						}
					}
				}
			}
		}
	}
	
	/**
	 * Prints the required results of a successful {@link #search} as:
	 * <br><CODE>n</CODE> nodes expanded.
	 * <br>cost = Total G Cost of optimal path.
	 * <br>Trip <CODE>[city_1]</CODE> to <CODE>[city_2]</CODE>
	 * <br>Trip <CODE>[city_2]</CODE> to <CODE>[city_3]</CODE>
	 * <br>etc.
	 * @param finalState The State the {@link #search} completed on.
	 * @param nodesExpanded The number of nodes taken off a PriorityQueue during the {@link #search}.
	 * @see State
	 */
	public void printResults(State<E> finalState, int nodesExpanded){
		System.out.println(nodesExpanded + " nodes expanded");
		System.out.println("cost = " + finalState.getGCost());
		State<E> s = finalState;
		List<State<E>> statesTraversed = new ArrayList<State<E>>();
		while(s.getPrevious() != null){
			statesTraversed.add(s);
			s = s.getPrevious();
		}
		Collections.reverse(statesTraversed);
		for (State<E> state : statesTraversed){
			System.out.println("Trip " + state.getPrevious().getCurrentCity() + " to " + state.getCurrentCity());
		}
	}
}
