
package MineSweeper;

/**
 *
 * @author Dylan Sikora
 * @version September 21, 2017
 */
public class MineSweeper {
    public static final int BOARD_SIZE =10;
    private Cell[][] board;
    
   
    public MineSweeper(){
        board=new Cell[BOARD_SIZE][BOARD_SIZE];
        for(int i=0;i<board.length;i++){
            for(int j=0;j<board[1].length;j++){
                board[i][j]=new Cell();
            }
        }
    }
    
   /**
    * 
    * @param x represents the column of the cell array to be looked at
    * @param y represents the row of the cell array to be looked at
    * @return Returns the number of cells adjacent to the specified cell that contain a bomb
    */
    public int countNeighbors(int x, int y){
        int count=0;
        if (board[x+1][y+1].getBomb()==true)count++;
        if (board[x+1][y].getBomb()==true)count++;
        if (board[x+1][y-1].getBomb()==true)count++;
        if (board[x-1][y+1].getBomb()==true)count++;
        if (board[x-1][y].getBomb()==true)count++;
        if (board[x-1][y-1].getBomb()==true)count++;
        if (board[x][y+1].getBomb()==true)count++;
        if (board[x][y-1].getBomb()==true)count++;
        
        return count;
    }
    
    /**
     * 
     * @param x represents the column of the cell array to be looked at
     * @param y represents the row of the cell array to be looked at
     * @return true if the cell at the specified location of a MineSweeper contains a bomb
     */
    public boolean getBomb(int x, int y){
        return board[x][y].getBomb();
    }
    
    /**
     * 
     * @param x represents the column of the cell array to have a bomb placed at
     * @param y represents the row of the cell array to have a bomb placed at
     */
    public void setBomb(int x, int y){
        board[x][y].setBomb(true);
    }
    
    
    /**
     * 
     * @param x represents the column of the cell array that is to have its graphical representation changed
     * @param y represents the row of the cell array that is to have its graphical representation changed
     * @param s represents the string or character that the specified cell will have its graphical representation changed to
     */
    public void setRep(int x, int y, String s){
        board[x][y].setRep(s);
    }
    
    /**
     * 
     * @param x represents the column of the cell array that is to have its "isGuessed" field changed
     * @param y represents the row of the cell array that is to have its "isGuessed" field changed
     * @param b represents what the specified cell's "isGuessed" field will be changed to. Should ideally always be true, as a cell can't be "unguessed"
     */
    public void setGuessed(int x, int y, boolean b){
        board[x][y].setGuessed(b);
    }
    
    /**
     * 
     * @param x represents the column of the cell array that is to have its "isGuessed" field accessed
     * @param y represents the row of the cell array that is to have its "isGuessed" field accessed
     * @return returns true when the specified cell has previously been guessed
     */
    public boolean getGuessed(int x, int y){
        return board[x][y].getGuessed();
    }
    
    /**
     * 
     * @return returns a cheater string, showing "true" in all locations that have mines in them. For cheaters only.
     */
    public String cheat(){
        StringBuilder sb= new StringBuilder();
        sb.append("  1\t 2\t 3\t 4\t 5\t 6\t 7\t 8\t \n");
        for(int i=1;i<board.length-1;i++){
            sb.append(i);
            sb.append(" ");
            for(int j=1;j<board[1].length-1;j++){
                sb.append(board[i][j].getBomb());
                sb.append("\t");
            }
        sb.append("\n");
        }
        return sb.toString();
     
    }
    /**
     * 
     * @return returns a string representing the current state of the game board. Uses each cell's "rep" string.
     */
    @Override
    public String toString(){
        StringBuilder sb= new StringBuilder();
        sb.append("  1 2 3 4 5 6 7 8 \n");
        for(int i=1;i<board.length-1;i++){
            sb.append(i);
            sb.append(" ");
            for(int j=1;j<board[1].length-1;j++){
                sb.append(board[i][j]);
                sb.append(" ");
            }
        sb.append("\n");
        }
        return sb.toString();
    }
    
    
    private class Cell{
        private boolean isGuessed;
        private boolean isBomb;
        private String rep;
                
        public Cell(){
            isGuessed=false;
            isBomb=false;
            rep="-";
        }
        
        /**
         * 
         * @return returns a cell's "isBomb" field
         */
        public boolean getBomb(){
            return isBomb;
        }
        /**
         * 
         * @return returns a cell's "isGuessed" field
         */
        public boolean getGuessed(){
            return isGuessed;
        }
        /**
         * 
         * @return returns a cell's "rep" field, which is a graphical representation of a cell
         */
        public String getRep(){
            return rep;
        }
        
        /**
         * 
         * @param isBomb represents value to be placed in a cell's "isBomb" field, which represents whether or not a particular cell is a mine
         */
        public void setBomb(boolean isBomb){
            this.isBomb=isBomb;
        }
        /**
         * 
         * @param isGuessed represents value to be placed in a cell's "isGuessed" field, which represents whether or not a particular cell has been guessed by the user
         */
        public void setGuessed(boolean isGuessed){
            this.isGuessed=isGuessed;
        }
        /**
         * 
         * @param rep represents a string to be placed in a cell's "rep" field, which is a graphical representation of a cell
         */
        public void setRep(String rep){
            this.rep=rep;
        }
        
        /**
         * 
         * @return returns a string containing a cell's "rep" field, which is a graphical representation of a cell
         */
        @Override
        public String toString(){
            return rep;
        }
    }
    
}
