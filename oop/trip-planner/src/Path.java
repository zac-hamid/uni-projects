/** 
 * Representation of an edge in a MapGraph, connecting two City objects together.
 * @author Zac Hamid - z5059915
 *
 * @param <E> The type for identifier of the City, e.g <CODE>E</CODE> is of type <CODE>Integer</CODE>; City can be referred to as 1, 2, etc.
 */
public class Path<E> {
	private int travelTime;
	private City<E> from;
	private City<E> to;
	
	/**
	 * 
	 * @param from The starting City of the Path.
	 * @param to The ending City of the Path.
	 * @param travelTime Time spent traveling along Path.
	 */
	public Path(City<E> from, City<E> to, int travelTime){
		this.travelTime = travelTime;
		this.from = from;
		this.to = to;
	}
	
	public int getTravelTime(){
		return travelTime;
	}
	
	public City<E> getTo(){
		return to;
	}
	
	public City<E> getFrom(){
		return from;
	}
	
	@Override
	public String toString(){
		return from + " -> " + to + " [" + travelTime + "]";
	}
	
	@Override
	public boolean equals(Object o){
		if(((Path<?>)o).toString().equals(this.toString())){
			return true;
		}
		return false;
	}
}
