/**
 * A class representing a 2D position in the world with x and y coordinates.
 * @author				Yaminn Aung, z5061216<br>
 * 						Zachary Hamid, z5059915
 */

public class Position {
	
	private int x;
	private int y;
	
	public Position(int x, int y){
		this.x = x;
		this.y = y;
	}
	
	public Position(Position pos){
		this.x = pos.getX();
		this.y = pos.getY();
	}
	
	public int getX(){
		return x;
	}
	
	public int getY(){
		return y;
	}

	public void setX(int x){
		this.x = x;
	}
	
	public void setY(int y){
		this.y = y;
	}
	
	@Override
	public String toString(){
		return "("+x+","+y+")";
	}
	
	//If the x and y value of positions are equal, the objects are equal
	@Override
	public boolean equals(Object o){
		if (((Position)o).getX() == this.getX() && ((Position)o).getY() == this.getY()){
			return true;
		}
		return false;
	}
	
	@Override
	public int hashCode(){
		return x + y;
	}
}
