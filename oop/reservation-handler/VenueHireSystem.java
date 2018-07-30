import java.io.FileNotFoundException;
import java.io.FileReader;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Scanner;
import java.util.TimeZone;

/**
 * Main system that handles all major, top-level processes, such as parsing input and sending it to relevant methods, 
 * managing all Venues, and opening new requests for creating, changing and cancelling Reservations.
 * @author Zac Hamid - z5059915
 */
public class VenueHireSystem {
	
	public static final int CURRENT_YEAR = 2016;

	private List<Venue> venues = new ArrayList<Venue>();
	private ReservationHandler resHandler;
	
	public static void main(String[] args) throws FileNotFoundException{
		VenueHireSystem vhs = new VenueHireSystem();
		vhs.Start(args);
	}
	
	/**
	 * Scans input and sends it to {@link #parseLine(String)}.
	 * @param args - The path to the file to be scanned for input.
	 */
	private void Start (String[] args){
		resHandler = new ReservationHandler();
		Scanner sc = null;
		try
		{
			sc = new Scanner(new FileReader(args[0]));
			/* Keep scanning to end of file*/
			while(sc.hasNext()){
				String next = sc.nextLine();
				parseLine(next);
			}
			
		} catch (FileNotFoundException | ParseException e){ e.printStackTrace(); }
		finally
		{
			if (sc != null) sc.close();
		}
	}
	
	/**
	 * Parses a line of input, determines what command the input corresponds to, and then proceeds to execute said command.
	 * <br>Possible inputs are:
	 * <ul>
	 * 	<li>Venue [venue_name] [room_name] [room_size] - Creates a new Venue with name <CODE>[venue_name]</CODE>, if not already created, 
	 * then adds a Room object with name <CODE>[room_name]</CODE> and size <CODE>[room_size]</CODE>.</li>
	 * 	<li>[request_type] ... - See {@link #parseRequest(String[])}</li>
	 * 	<li> Cancel [request_ID] - Cancels a request with ID <CODE>[request_ID]</CODE>.</li>
	 * 	<li> Print [venue_name] - See {@link #printOccupancy(Venue)}</li>
	 * </ul>
	 * @param line - The line of input to be parsed.
	 * @throws ParseException
	 */
	private void parseLine(String line) throws ParseException{
		String[] keywords = line.split(" ");
		switch(keywords[0]){
			case "Venue":
				Venue venue = new Venue(keywords[1]);
				addVenue(venue);
				for (Venue v : venues){
					if (v.equals(venue)){
						v.addRoom(new Room(keywords[2], keywords[3]));
					}
				}
				break;
			case "Request":
				parseRequest(keywords);
				break;
			case "Change":
				parseRequest(keywords);
				break;
			case "Cancel":
				resHandler.cancelReservation(Integer.parseInt(keywords[1]), false);
				break;
			case "Print":
				for (Venue v : venues){
					if (v.getName().equals(keywords[1])){
						printOccupancy(v);
					}
				}
				break;
			default:
				break;
		}
	}
	
	/**
	 * Adds a new Venue to <CODE>venues</CODE> list, if it does not exist.
	 * @param toAdd - The venue to be added.
	 */
	private void addVenue(Venue toAdd){
		for (Venue v : venues){
			if (toAdd.equals(v)){
				return;
			}
		}
		venues.add(toAdd);
	}
	
	/**
	 * Parses a new "Request" or "Change" given an array of keywords.
	 * <br>If <CODE>[request_type]</CODE> is "Request", a new Reservation will be made, if possible.
	 * <br>If <CODE>[request_type]</CODE> is "Change", an existing reservation with ID <CODE>[request_ID]</CODE> will be changed.
	 * @param keywords - An array containing keywords of type: <CODE>[request_type] [request_ID] [start_date] [end_date] [num_of_(size_1)] [size_1] [num_of_(size_2)] [size_2] ...</CODE>
	 * @throws ParseException
	 * */
	private void parseRequest(String[] keywords) throws ParseException{
		int requestID = Integer.parseInt(keywords[1]);
		Map<String, Integer> requestMap = new HashMap<String, Integer>();
		
		Calendar startDate = new GregorianCalendar();
		Calendar endDate = new GregorianCalendar();
		
		SimpleDateFormat sdf = new SimpleDateFormat("MMM dd", Locale.getDefault());
		startDate.setTime(sdf.parse(keywords[2] + " " + keywords[3]));
		endDate.setTime(sdf.parse(keywords[4] + " " + keywords[5]));
		
		//Avoids issues with Daylight-Savings Time and leap years
		startDate.setTimeZone(TimeZone.getTimeZone("UTC"));
		endDate.setTimeZone(TimeZone.getTimeZone("UTC"));
		startDate.set(Calendar.YEAR, CURRENT_YEAR);
		endDate.set(Calendar.YEAR, CURRENT_YEAR);
		
		/* 
		 * Each even word after the 6th keyword is a size, and every odd is a number of 
		 * that size to be requested [these are offset by 1 since the array starts at 0]
		 */
		
		for (int i = 6; i < keywords.length; i++){
			if (i % 2 == 0){
				requestMap.put(keywords[i+1], Integer.parseInt(keywords[i]));
			}
		}
		
		switch (keywords[0]){
			case "Request":
				resHandler.createReservation(venues, requestID, requestMap, startDate, endDate);
				break;
			case "Change":
				resHandler.changeReservation(venues, requestID, requestMap, startDate, endDate);
				break;
			default:
				break;
		}
	}
	/**
	 * Prints occupancy of all rooms in Venue <CODE>v</CODE>, including the start day of each booking of that room, and the number of days the booking lasts for.
	 * 
	 * @param v - The Venue to print.
	 */
	private void printOccupancy(Venue v){
		for (Room r : v.getRooms()){
			System.out.print(v.getName() + " " + r.getName());
			for (Calendar cal : r.getBookedDays().keySet()){
				System.out.print(" " + cal.getDisplayName(Calendar.MONTH, Calendar.SHORT, Locale.getDefault()) + " " + cal.get(Calendar.DATE) + " " + r.numberOfDays(cal, r.getBookedDays().get(cal)));
			}
			System.out.println("");
		}
	}
}