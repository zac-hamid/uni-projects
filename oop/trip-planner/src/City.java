import java.util.ArrayList;
import java.util.List;

/**
 * Representation of a node in a MapGraph.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier of the City, e.g <CODE>E</CODE> is of 
 * type <CODE>Integer</CODE>; City can be referred to as 1, 2, etc.
 */
public class City<E> {
	private E name;
	private int transferTime;
	private List<City<E>> connectedCities;
	
	/**
	 * 
	 * @param name The identifier for the City.
	 * @param transferTime Time spent at City, in between paths.
	 */
	public City(E name, int transferTime){
		this.name = name;
		this.transferTime = transferTime;
		connectedCities = new ArrayList<City<E>>();
	}
	
	public E getCityName(){
		return name;
	}
	
	public int getTransferTime(){
		return transferTime;
	}
	
	public List<City<E>> getConnectedCities(){
		return connectedCities;
	}
	/**
	 * Removes a City from this City's <CODE>connectedCities</CODE>.
	 * @param c The City to be removed.
	 */
	public void removeConnectedCity(City<E> c){
		for (City<E> _c : connectedCities){
			if (_c.equals(c)){
				connectedCities.remove(_c);
				return;
			}
		}
	}
	/**
	 * Adds a City to this City's <CODE>connectedCities</CODE>.
	 * @param c The City to be added.
	 */
	public void addConnectedCity(City<E> c){
		for (City<E> _c : connectedCities){
			if (_c.equals(c)){
				return;
			}
		}
		connectedCities.add(c);
	}
	
	@Override
	public String toString(){
		return name.toString();
	}
	
	@Override
	public boolean equals(Object o){
		if(((City<?>)o).toString().equals(this.toString())){
			return true;
		}
		return false;
	}
}
