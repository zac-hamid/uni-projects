import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;


public class Main {

	public static void main(String[] args) {
		String currentPassword = "";
        String fileName = "extraction.txt";

        String line = null;

        try {
            FileReader fileReader = new FileReader(fileName);

            BufferedReader bufferedReader = new BufferedReader(fileReader);

            while((line = bufferedReader.readLine()) != null) {
                // Line has been obtained
            	char[] lineChars = line.toCharArray();
            	char pivot = lineChars[1];
            	char a = lineChars[0];
            	char b = lineChars[2];
            	// We know that each line has 3 characters
            	// Select middle point as pivot
            	
            	if (currentPassword.length() == 0) {
            		currentPassword = currentPassword.concat(line);
            	} else {
            		if (!charExistsInString(pivot,currentPassword)) {
            			// 3 cases, a exists, b exists or both exist
            			if (!charExistsInString(a, currentPassword)) {
            				if (!charExistsInString(b, currentPassword)) {
            					// place all in a row at the back of string?
            					currentPassword = currentPassword.concat(line); // ???
            				} else {
            					// Place pivot before b
            					// Place a before pivot
            					int x = charPositionInString(b, currentPassword);
            					String line1 = currentPassword.substring(0, x);
            					String line2 = currentPassword.substring(x);
            					line1 += a;
            					line1 += pivot;
            					currentPassword = line1.concat(line2);
            				}
            			} else {
            				// Place pivot after a
            				int x = charPositionInString(a, currentPassword);
        					String line1 = currentPassword.substring(0, x+1);
        					String line2 = currentPassword.substring(x+1);
        					line1 += pivot;
        					currentPassword = line1.concat(line2);
            			}
            		}
            		
            		// After pivot has been checked
            		// Checking a

        			int x = charPositionInString(pivot, currentPassword);
            		if (!charExistsInString(a,currentPassword)) {
    					String line1 = currentPassword.substring(0, x);
    					String line2 = currentPassword.substring(x);
    					line1 += a;
    					currentPassword = line1.concat(line2);
            		} else {
            			// Is a before pivot?
            			String beforePivot = currentPassword.substring(0,x);
            			if (!charExistsInString(a,beforePivot)){
            				int ax = charPositionInString(a, currentPassword);
            				StringBuilder sb = new StringBuilder(currentPassword);
            				sb.deleteCharAt(ax);
            				currentPassword = sb.toString();
            				// Move a to before pivot
            				beforePivot += a;
            				String afterPivot = currentPassword.substring(x+1);
            				currentPassword = beforePivot.concat(afterPivot);
            			}
            		}
            		
            		// Checking b

        			x = charPositionInString(pivot, currentPassword);
            		if (!charExistsInString(b, currentPassword)) {
    					String line1 = currentPassword.substring(0, x+1);
    					String line2 = currentPassword.substring(x+1);
    					line1 += b;
    					currentPassword = line1.concat(line2);
            		} else {
            			// Is b after pivot?
            			String afterPivot = currentPassword.substring(x+1);
            			if (!charExistsInString(b, afterPivot)) {
            				// Move b to after pivot
            				// Remove current occurrence of b
            				int bx = charPositionInString(b, currentPassword);
            				StringBuilder sb = new StringBuilder(currentPassword);
            				sb.deleteCharAt(bx);
            				currentPassword = sb.toString();
            				// Split on pivot
            				String splitPoint = currentPassword.substring(0,x);
            				splitPoint += b;
            				
            				currentPassword = splitPoint.concat(afterPivot);
            			}
            		}
            	}
            }
            System.out.println("Password is: " + currentPassword);

            bufferedReader.close();         
        }
        catch(FileNotFoundException e) {
            System.out.println("Unable to open file '" + fileName + "'");                
        }
        catch(IOException e) {
            e.printStackTrace();
        }
    }
	
	public static boolean charExistsInString(char c, String s) {
		for (int i = 0; i < s.length(); i++) {
			if (s.charAt(i) == c) {
				return true;
			}
		}
		return false;
	}
	
	public static int charPositionInString(char c, String s) {
		for (int i = 0; i < s.length(); i++) {
			if (s.charAt(i) == c) {
				return i;
			}
		}
		return -1;
	}
}
