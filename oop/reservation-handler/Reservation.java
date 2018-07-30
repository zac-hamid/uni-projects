import java.util.Calendar;
import java.util.List;

/**
 * Stores information about a particular booking to a list of Rooms. 
 * Allows for bookings to be referred to by an <CODE>ID</CODE>.
 * @author Zac Hamid - z5059915
 * @see ReservationHandler
 */
public class Reservation {
	List<Room> reservedRooms;
	int reservationID;
	Calendar start;
	Calendar finish;
	
	/**
	 * Gets this reservation's ID
	 * @return <li>int <CODE>ID</CODE>.</li>
	 */
	public int getID(){
		return reservationID;
	}
	
	/**
	 * Gets the list of all Room objects booked under this reservation.
	 * @return <li>List of Room objects.</li>
	 */
	public List<Room> getRooms(){
		return reservedRooms;
	}
	
	/**
	 * Gets this reservation's starting date.
	 * @return <li>Calendar <CODE>start</CODE>.</li>
	 */
	public Calendar getStartDate(){
		return start;
	}

	/**
	 * Gets this reservation's end date.
	 * @return <li>Calendar <CODE>finish</CODE>.</li>
	 */
	public Calendar getEndDate(){
		return finish;
	}
	
	public Reservation(int ID, List<Room> rooms, Calendar startDate, Calendar finishDate){
		reservationID = ID;
		reservedRooms = rooms;
		start = startDate;
		finish = finishDate;
	}
}