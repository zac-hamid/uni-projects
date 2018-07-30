import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Handles all reservations in the system; including the creation, change and cancellation of reservations.
 * @author Zac Hamid - z5059915
 * @see VenueHireSystem
 */
public class ReservationHandler {
	List<Reservation> reservations;
	
	public ReservationHandler(){
		reservations = new ArrayList<Reservation>();
	}
	
	/**
	 * Gets a list of Reservation objects handled by this ReservationHandler.
	 * @return A list of <CODE>Reservation</CODE> objects stored in this <CODE>ReservationHandler</CODE> class.
	 */
	public List<Reservation> getReservations(){
		return reservations;
	}
	
	/**
	 * Creates a new reservation object, and makes a booking to satisfy the <CODE>roomRequest</CODE> map, if possible.
	 * <br>Otherwise, a rejection message will be output and the original reservation will be <i>preserved</i>.
	 * 
	 * @param venues - A list of venues to search through
	 * @param ID - The ID of the reservation, if one is created
	 * @param roomRequest - A map of requests, example: "1 large 1 small" becomes <"large", 1> and <"small", 1>
	 * @param startDate - The start date of the reservation
	 * @param endDate - The end date of the reservation
	 * @see {@link #canBeReserved(List, Venue, Map, Calendar, Calendar)}
	 */
	public void createReservation(List<Venue> venues, int ID, 
			Map<String, Integer> roomRequest, Calendar startDate, Calendar endDate){
		List<Room> roomCatch = new ArrayList<Room>();
		for (Reservation r : reservations){
			if (r.getID() == ID){
				System.out.println("Request rejected");
				return;
			}
		}
		for (Venue v : venues){
			if (canBeReserved(roomCatch, v, roomRequest, startDate, endDate)){
				System.out.print("Reservation " + ID + " " + v.getName());
				for (Room r : roomCatch){
					r.addBooking(startDate, endDate);
					System.out.print(" " + r.getName());
				}
				System.out.println("");
				reservations.add(new Reservation(ID, roomCatch, startDate, endDate));
				return;
			}
		}
		
		System.out.println("Request rejected");
	}
	
	/**
	 * Cancels reservation with <CODE>ID</CODE> and removes booking from all rooms 
	 * made under this reservation.
	 * 
	 * @param ID - ID of reservation to cancel
	 * @param suppressOutput - If <CODE>true</CODE>, this method will print no output.
	 */
	public void cancelReservation(int ID, boolean suppressOutput){
		for (Reservation r : reservations){
			if (r.getID() == ID){
				for (Room room : r.getRooms()){
					room.removeBooking(r.start);
				}
				reservations.remove(r);
				if (!suppressOutput) System.out.println("Cancel " + ID);
				return;
			}
		}
		if (!suppressOutput) System.out.println("Cancel rejected");
	}
	
	/**
	 * Changes reservation with <CODE>ID</CODE> to a new reservation that satisfies the <CODE>roomRequest</CODE> map, if possible.
	 *  <br>Otherwise, a rejection message will be output and the original reservation will be <i>preserved</i>.
	 *  
	 * @param venues - A list of venues to search through
	 * @param ID - The ID of the reservation to change
	 * @param roomRequest - A map of requests, example: "1 large 1 small" becomes <"large", 1> and <"small", 1>
	 * @param startDate - The start date of the reservation
	 * @param endDate - The end date of the reservation
	 * @see {@link #canBeReserved(List, Venue, Map, Calendar, Calendar)}
	 */
	public void changeReservation(List<Venue> venues, int ID, 
			Map<String, Integer> roomRequest, Calendar startDate, Calendar endDate){
		List<Room> roomCatch = new ArrayList<Room>();
		Reservation res = null;
		boolean found = false;
		for (Reservation r : reservations){
			if (r.getID() == ID){
				found = true;
				res = r;
			}
		}
		if (!found) {
			System.out.println("Change rejected");
			return;
		}
		
		for (Venue v : venues){
			cancelReservation(ID, true);
			if (canBeReserved(roomCatch, v, roomRequest, startDate, endDate)){
				System.out.print("Change " + ID + " " + v.getName());
				for (Room r : roomCatch){
					r.addBooking(startDate, endDate);
					System.out.print(" " + r.getName());
				}
				System.out.println("");
				reservations.add(new Reservation(ID, roomCatch, startDate, endDate));
				return;
			} else {
				for (Room r : res.reservedRooms){
					r.addBooking(res.start, res.finish);
				}
				reservations.add(res);
			}
		}
		System.out.println("Change rejected");
	}
	
	/**
	 * Determines if a Venue <CODE>v</CODE> contains rooms that 
	 * satisfy the <CODE>roomRequest</CODE> map.
	 * 
	 * @param roomCatch - A list of rooms that satisfy the <CODE>roomRequest</CODE> map, is cleared if Venue <CODE>v</CODE> does not satisfy the map.
	 * @param v - A Venue to be checked
	 * @param roomRequest - A map of requests, example: "1 large 1 small" becomes <"large", 1> and <"small", 1>
	 * @param startDate - The start date of the reservation
	 * @param endDate - The end date of the reservation
	 * @return <CODE>true</CODE> if Venue <CODE>v</CODE> satisfies the <CODE>roomRequest</CODE> map.
	 * <br><CODE>false</CODE> if venue does <i>not</i> satisfy the <CODE>roomRequest</CODE> map.
	 */
	private boolean canBeReserved(List<Room> roomCatch, Venue v, 
			Map<String, Integer> roomRequest, Calendar startDate, Calendar endDate){
		Map<String, Integer> temp = new HashMap<String, Integer>();
		
		for (String s : roomRequest.keySet()){
			temp.put(s, roomRequest.get(s));
		}

		for (Room r : v.getRooms()){
			for (String s : temp.keySet()){
				if (r.isAvailable(startDate, endDate) && s.equals(r.getSize()) && temp.get(s) > 0){
					roomCatch.add(r);
					temp.put(s, temp.get(s) - 1);
				}
			}
		}

		int roomsLeft = 0;
		for (String s : temp.keySet()){
			roomsLeft += temp.get(s);
		}
		if (roomsLeft == 0){
			return true;
		}
		
		roomCatch.clear();
		return false;
	}
}