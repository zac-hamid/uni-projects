import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Map;
import java.util.TreeMap;

/**
 * Can be booked for periods of time and is stored in a Venue.
 * @author Zac Hamid - z5059915
 * @see Venue
 */
public class Room {
	private String name;
	private String size;
	private Map<Calendar, Calendar> bookedDays;
	
	/**
	 * Gets the current room's name.
	 * @return <li>String <CODE>name</CODE>.</li>
	 */
	public String getName(){
		return name;
	}
	
	/**
	 * Gets the size of the current room.
	 * @return <li>String <CODE>size</CODE>.</li>
	 */
	public String getSize(){
		return size;
	}
	
	/**
	 * Gets a <CODE>TreeMap</CODE> of all current booked days for this room.
	 * @return <li>Map&lt;Calendar, Calendar&gt; <CODE>bookedDays</CODE>.</li>
	 */
	public Map<Calendar, Calendar> getBookedDays(){
		return bookedDays;
	}
	
	/**
	 * Adds a new booking to this Room's <CODE>bookedDays</CODE> map.
	 * @param start - The starting date of the booking.
	 * @param finish - The end date of the booking.
	 */
	public void addBooking(Calendar start, Calendar finish){
		Calendar s = new GregorianCalendar();
		Calendar f = new GregorianCalendar();
		s.setTime(start.getTime());
		f.setTime(finish.getTime());
		bookedDays.put(s, f);
	}
	
	/**
	 * Finds and removes a booking with starting date <CODE>start</CODE> from the Room's <CODE>bookedDays</CODE> map.
	 * @param start - The starting date of the booking to remove.
	 */
	public void removeBooking(Calendar start){
		for (Calendar c : bookedDays.keySet()){
			if (isSameDay(start,c)){
				bookedDays.remove(c);
				return;
			}
		}
	}
	
	public Room(String name, String size){
		this.name = name;
		this.size = size;
		bookedDays = new TreeMap<Calendar, Calendar>();
	}
	
	/**
	 * Checks if room is available for a given start and end date.
	 * @param start - the start date of reservation
	 * @param finish - the end date of reservation
	 * @return <CODE>true</CODE> if no reservation is made for specified dates.
	 * <br><CODE>false</CODE> if a reservation has already been made
	 */
	public boolean isAvailable(Calendar start, Calendar finish){
		//Iterate through all bookedDays
		for (Map.Entry<Calendar,Calendar> entry : bookedDays.entrySet()){
			Calendar s = entry.getKey();
			Calendar f = entry.getValue();
			/*
			 * There are 3 possible cases for a given booking:
			 * 1. The booking's start and end dates are before the currently checked booking.
			 * 2. The booking's start and end dates are after the currently checked booking.
			 * 3. The booking's start or end date is between the currently checked booking.
			 * Cases 1 and 2 mean check the next booking, case 3 is an automatic fail.
			 */
			if (!(finish.get(Calendar.DAY_OF_YEAR) < s.get(Calendar.DAY_OF_YEAR)) && !(start.get(Calendar.DAY_OF_YEAR) > f.get(Calendar.DAY_OF_YEAR))){
				return false;
			}
		}
		return true;
	}
	
	/**
	 * Gets the number of days from a start date to a finish date (inclusive).
	 * @param start - The start date
	 * @param finish - The finish date
	 * @return The number of days from <CODE>start</CODE> to <CODE>finish</CODE>.
	 *  Example: For <CODE>Mar 25</CODE> to <CODE>Mar 28</CODE>, will return <CODE>4</CODE>.
	 */
	
	public int numberOfDays(Calendar start, Calendar finish){
		long millies = finish.getTimeInMillis() - start.getTimeInMillis();
		int days = Math.round(millisecondsToDays(millies));
		return (days + 1);
	}
	
	/**
	 * Converts given milliseconds to days.
	 * @param milliseconds - The amount of milliseconds.
	 * @return The days that <CODE>milliseconds</CODE> correponds to, as a <CODE>float</CODE>.
	 */
	public float millisecondsToDays(long milliseconds){
		float days = (float) (milliseconds / (1000 * 60 * 60 * 24));
		return days;
	}
	
	/**
	 * Checks if two calendars have the same date.
	 * @param cal1 - First calendar to check.
	 * @param cal2 - Second calendar to check.
	 * @return <CODE>true</CODE> if the calendars have the same date.
	 * <br><CODE>false</CODE> if they have different dates.
	 */
	public boolean isSameDay(Calendar cal1, Calendar cal2){
		if (cal1.get(Calendar.DAY_OF_YEAR) == cal2.get(Calendar.DAY_OF_YEAR)){
			return true;
		}
		return false;
	}
}