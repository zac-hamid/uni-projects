import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * An implementation of the MapGraph Graph representation.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier of the City objects, e.g <CODE>E</CODE> is of 
 * type <CODE>Integer</CODE>; City can be referred to as 1, 2, etc.
 * @see MapGraph
 */
public class EuroMap<E> implements MapGraph<E> {

	private int numberOfPaths = 0;
	private int numberOfCities = 0;
	
	private List<City<E>> cities;
	private List<Path<E>> paths;
	
	public EuroMap(){
		cities = new ArrayList<City<E>>();
		paths = new ArrayList<Path<E>>();
	}
	
	@Override
	public void addCity(E name, int transferTime) {
		for (City<E> c : cities){
			if (c.toString().equals(name)){
				return;
			}
		}
		cities.add(new City<E>(name, transferTime));
		numberOfCities++;
	}

	@Override
	public void addPath(E from, E to, int travelTime) {
		City<E> _from = findCity(from);
		City<E> _to = findCity(to);
		for (City<E> c : cities){
			if (c.getCityName().equals(from)){
				_from = c;
			}
			if (c.getCityName().equals(to)){
				_to = c;
			}
		}
		
		//Adds two paths, in opposite directions so the graph is not directed.
		Path<E> path1 = new Path<E>(_from, _to, travelTime);
		Path<E> path2 = new Path<E>(_to, _from, travelTime);
		
		paths.add(path1);
		paths.add(path2);
		numberOfPaths++;
		
		//Connect nodes to each other (undirected graph)
		City<E> pathFrom = path1.getFrom();
		City<E> pathTo = path1.getTo();
		for (City<E> _c : getNeighbourCities(pathFrom)){
			if (pathFrom.equals(_c)){
				return;
			}
		}
		for (City<E> _c : getNeighbourCities(pathTo)){
			if (pathTo.equals(_c)){
				return;
			}
		}
		pathTo.addConnectedCity(path1.getFrom());
		pathFrom.addConnectedCity(path1.getTo());
	}

	@Override
	public void removeCity(E name) {
		City<E> c = findCity(name);
		//Find city in the list of this map
		if(cities.contains(c)){
			//Find paths that contain this city
			for (Iterator<Path<E>> iterator = paths.iterator(); iterator.hasNext();){
				Path<E> e = iterator.next();
				if (e.getFrom().equals(c) || e.getTo().equals(c)){
					//Remove paths that contain this city
					removePath(e.getFrom().getCityName(), e.getTo().getCityName());
					iterator = paths.iterator();
				}
			}
			cities.remove(c);
			numberOfCities--;
		}
	}

	@Override
	public void removePath(E from, E to) {
		Path<E> edge = findPath(from, to);
		//If an edge has been found
		if (edge != null){
			//Remove cities from each other (undirected graph)
			findCity(from).removeConnectedCity(findCity(to));
			findCity(to).removeConnectedCity(findCity(from));
			paths.remove(edge);
			numberOfPaths--;
		}
	}

	@Override
	public List<City<E>> getNeighbourCities(City<E> city) {
		return city.getConnectedCities();
	}

	@Override
	public int getTravelTime(E from, E to) {
		Path<E> e = findPath(from, to);
		if (e != null) return e.getTravelTime();
		else return 0;
	}

	@Override
	public int getNumberOfCities() {
		return numberOfCities;
	}

	@Override
	public int getNumberOfPaths() {
		return numberOfPaths;
	}

	@Override
	public List<City<E>> getCities() {
		List<City<E>> cityList = new ArrayList<City<E>>();
		for(City<E> c : cities){
			cityList.add(c);
		}
		return cityList;
	}

	@Override
	public City<E> findCity(E name) {
		for (City<E> c : cities){
			if (c.getCityName().equals(name)){
				return c;
			}
		}
		return null;
	}

	@Override
	public Path<E> findPath(E from, E to) {
		for (Path<E> e : paths){
			if ((e.getFrom().getCityName().equals(from)) && (e.getTo().getCityName().equals(to))){
				return e;
			}
		}
		return null;
	}
}