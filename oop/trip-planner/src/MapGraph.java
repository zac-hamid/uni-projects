import java.util.List;

/**
 * A Graph to store a Map-like representation containing cities [{@link City}] 
 * as nodes and roads [{@link Path}] as edges.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier of the City objects, e.g <CODE>E</CODE> is of 
 * type <CODE>Integer</CODE>; City can be referred to as 1, 2, etc.
 * @see City
 * @see Path
 */
public interface MapGraph<E> {
	/**
	 * Adds a City object to MapGraph.
	 * @param name Identifier of City to add to MapGraph.
	 * @param transferTime Time spent at City, in between paths.
	 * @see City
	 */
	void addCity(E name, int transferTime);
	/**
	 * Adds a Path between two City objects and to MapGraph.
	 * @param from The starting City of the Path.
	 * @param to The ending City of the Path.
	 * @param travelTime Time spent traveling along Path.
	 * @see Path
	 */
	void addPath(E from, E to, int travelTime);
	
	/**
	 * Removes a City object from MapGraph.
	 * @param name Identifier of City to remove from MapGraph
	 * @see City
	 */
	void removeCity(E name);

	/**
	 * Removes a Path between two City objects and from MapGraph.
	 * @param from Identifier of start City.
	 * @param to Identifier of end City.
	 * @see Path
	 */
	void removePath(E from, E to);

	/**
	 * @param city The City to get neighbours from.
	 * @return A list of neighbouring City objects of <CODE>city</CODE>.
	 */
	List<City<E>> getNeighbourCities(City<E> city);

	/**
	 * Gets the travel time between two City objects along a Path.
	 * @param from Identifier of start City of Path.
	 * @param to Identifier of end City of Path.
	 * @return The time spent traveling along the Path.
	 */
	int getTravelTime(E from, E to);

	int getNumberOfCities();

	int getNumberOfPaths();

	/**
	 * Gets a List of all City objects in the MapGraph.
	 * @return A List all City objects in the MapGraph.
	 */
	List<City<E>> getCities();

	/**
	 * Finds a City object with a given identifier.
	 * @param name The identifier of the City to be found.
	 * @return A City object with the corresponding identifier <CODE>name</CODE>.
	 */
	City<E> findCity(E name);
	
	/**
	 * Finds a Path between two City objects <CODE>from</CODE> and <CODE>to</CODE>.
	 * @param from The identifier of the start City of the Path to be found.
	 * @param to The identifier of the end City of the Path to be found.
	 * @return A Path that connects City <CODE>from</CODE> and City <CODE>to</CODE>.
	 */
	Path<E> findPath(E from, E to);

}
