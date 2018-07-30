import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.Scanner;
/**
 * {@link #parseInput(String) Parses input} from a file, creates a MapGraph based off of this input,
 * and starts an A* search process to find the cheapest path of planned trips from this input.
 * @author Zac Hamid - z5059915
 * @see AStarSearch
 * @see MapGraph
 */
public class TripPlanner {
	private	MapGraph<String> euroMap;
	private AStarSearch<String> aStar;
	
	public static void main(String args[]){
		TripPlanner tp = new TripPlanner();
		tp.Start(args);
	}
	
	/**
	 * Scans input and sends it to {@link #parseInput(String)}.
	 * @param args The path to the file to be scanned for input.
	 */
	private void Start(String args[]){
		euroMap = new EuroMap<String>();
		aStar = new AStarSearch<String>();
		Scanner sc = null;
		try
		{
			sc = new Scanner(new FileReader(args[0]));
			while(sc.hasNextLine()){
				String line = sc.nextLine();
				parseInput(line);
			}
		}
		catch (FileNotFoundException e) {}
		finally
		{
			if (sc != null) sc.close();
		}
		
		aStar.search(new CustomHeuristic(), euroMap, "London");
	}
	
	/**
	 * Parses a line of input, determines what command the input corresponds to, 
	 * and then proceeds to execute said command.
	 * <br>Possible inputs are:
	 * <ul>
	 * 	<li>Transfer [transfer_time] [city_identifier] - Creates a new City object in a 
	 * 		MapGraph with identifier <CODE>[city_identifier]</CODE>, and a transfer time 
	 * 		of <CODE>[transfer_time]</CODE>. See {@link City} for use.</li>
	 * 	<li>Time [travel_time] [city_1] [city_2] - Creates a new Path between existing 
	 * 		City objects in a MapGraph <CODE>[city_1]</CODE> and <CODE>[city_2]</CODE> 
	 * 		with travel time <CODE>[travel_time]</CODE>. See {@link Path} for use.</li>
	 * 	<li>Trip [city_1] [city_2] - Requests a trip from <CODE>[city_1]</CODE> to 
	 * 		<CODE>[city_2]</CODE> to be fulfilled by {@link AStarSearch A* Search}.
	 * 
	 * @param line The line of input to be parsed.
	 */
	private void parseInput(String line){
		String[] keywords = line.split(" ");
		switch(keywords[0]){
			case "Transfer":
				int transferTime = Integer.parseInt(keywords[1]);
				String name = keywords[2];
				euroMap.addCity(name, transferTime);
				break;
			case "Time":
				int travelTime = Integer.parseInt(keywords[1]);
				String startCity = keywords[2];
				String endCity = keywords[3];
				euroMap.addPath(startCity, endCity, travelTime);
				break;
			case "Trip":
				String startTrip = keywords[1];
				String endTrip = keywords[2];
				aStar.addTrip(euroMap, startTrip, endTrip);
				break;
			default:
				break;
		}
	}

}
