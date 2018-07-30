public class MathHelper{
	/**
	 * Rotates a given matrix (read: array) of characters clockwise by 90 degrees.
	 * @param mat	The character array to be rotated.
	 * @return char[][] The rotated character array.
	 */
	static char[][] rotateCW(char[][] mat) {
	    final int M = mat.length;
	    final int N = mat[0].length;
	    char[][] ret = new char[N][M];
	    for (int r = 0; r < M; r++) {
	        for (int c = 0; c < N; c++) {
	            ret[c][M-1-r] = mat[r][c];
	        }
	    }
	    return ret;
	}
}
