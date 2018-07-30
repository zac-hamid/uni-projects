import java.util.ArrayList;
import java.util.List;

/**
 * Stores a list of Rooms and acts as their "parent", allowing for Rooms to have a common object to which they may be searched from and be sorted by.
 * @author Zac Hamid - z5059915
 * @see Room
 */
public class Venue {
	private String name;
	private List<Room> rooms;
	
	/**
	 * Gets the current venue's name.
	 * @return <li>String <CODE>name</CODE>.</li>
	 */
	public String getName(){
		return name;
	}
	
	/**
	 * Gets a list of rooms "parented" by this venue.
	 * @return <li>List of Room objects.</li>
	 */
	public List<Room> getRooms(){
		return rooms;
	}
	
	/**
	 * Adds a Room object to this venue's list of Rooms.
	 * @param room - The Room object to add.
	 */
	public void addRoom(Room room){
		rooms.add(room);
	}
	
	public Venue(String name){
		this.name = name;
		rooms = new ArrayList<Room>();
	}
	
	/**
	 * Check if an Object <CODE>o</CODE> is equal to this Venue.
	 * @return <CODE>true</CODE> if Object <CODE>o</CODE> equals this Venue.
	 * <br><CODE>false</CODE> if they are not equal.
	 */
	@Override
	public boolean equals(Object o){
		if (this == o) return true;
		else if (getClass().equals(o.getClass())){
			Venue v = (Venue) o;
			return (name.equals(v.getName()));
		}
		else return false;
	}
}