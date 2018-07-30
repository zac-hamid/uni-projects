import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
/**
 * Stores the current {@link City} that {@link AStarSearch} is up to, as well as information about the search space
 * such as total cost of the paths taken to this State and the remaining trips to be found by AStarSearch.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier for the City of a State, 
 * e.g <CODE>E</CODE> is of type <CODE>Integer</CODE>; City can 
 * be referred to as 1, 2, etc.
 * 
 */
public class State<E> {
	private State<E> previous;
	private int gCost;
	private City<E> currentCity;
	private List<Path<E>> desiredTrips;
	
	public State(State<E> previous, City<E> city, int gCost, List<Path<E>> desiredTrips){
		this.previous = previous;
		this.currentCity = city;
		this.gCost = gCost;
		this.desiredTrips = new ArrayList<Path<E>>(desiredTrips);
	}
	
	public State<E> getPrevious(){
		return previous;
	}

	public int getGCost(){
		return gCost;
	}
	
	public City<E> getCurrentCity(){
		return currentCity;
	}
	
	public List<Path<E>> getDesiredTrips(){
		return desiredTrips;
	}
	
	/**
	 * Removes a Path from this State's <CODE>desiredTrips</CODE> List.
	 * @param trip The Path to remove.
	 */
	public void removeTrip(Path<E> trip){
		Iterator<Path<E>> i = desiredTrips.listIterator();
		while(i.hasNext()){
			Path<E> t = i.next();
			if (trip.equals(t)){
				i.remove();
			}
		}
	}
	
	@Override
	public String toString(){
		return currentCity + " [" + gCost + "]";
	}
}
