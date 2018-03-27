
import MineSweeper.MineSweeper;
import java.util.Random;
import java.util.Scanner;
/**
 *
 * @author Dylan Sikora
 * @version September 21, 2017
 */
public class MineSweeperDriver {
    public static final int NUM_OF_BOMBS=10;
    
    public static void main(String[] args){
        
        Random rand=new Random();
        Scanner scan=new Scanner(System.in);
        
        while(true){//This allows the game to run indefinitely. There's probably a better way to do this
        
        boolean guessingBomb;
        int bombCount=0;
        MineSweeper gameBoard=new MineSweeper();
        String userInput="";
        int xCord;
        int yCord;
        
        while(!userInput.equals("yes")){//prompts user to play the game, keeps prompting until yes or no is entered
            System.out.println("Would You like to play Mine Sweeper? Please answer yes or no.");
            userInput=scan.next();
            if(userInput.equals("no")){//ends the program when the user indicates they're done
                System.out.println("Program ending. Cya!");
                return;
            }
            
        }
           
        while(bombCount!=NUM_OF_BOMBS){//This part places the bombs
            xCord= (rand.nextInt(8)+1);
            yCord= (rand.nextInt(8)+1);
            if (gameBoard.getBomb(xCord,yCord)==true)bombCount--;//This line accounts for the mine being placed on a location with a mine in it
            bombCount++;
            gameBoard.setBomb(xCord,yCord);
        }
        
        while(bombCount!=0){//main gameplay loop. The bombCount Variable keeps track of how many bombs are left
            xCord=0;//resets variables so user input from last guess isnt floating around
            yCord=0;
            userInput="";
            guessingBomb=false;//this isnt necessary, but it gets rid of a netbeans warning
            
            System.out.println("\ncurrent game board");
            System.out.println(gameBoard);
            
            while((userInput.equalsIgnoreCase("yes")||(userInput.equalsIgnoreCase("no")))==false){//prompts user to declare what theyre guessing, keeps prompting until yes or no is entered
                System.out.println("Are you going to guess a mine's location? Please answer yes or no (or cheat).");
                userInput=scan.next();
                if(userInput.equalsIgnoreCase("no")){
                    guessingBomb=false;
                }
                if(userInput.equalsIgnoreCase("yes")){
                    guessingBomb=true;
                } 
                if(userInput.equalsIgnoreCase("cheat")){
                    System.out.println(gameBoard.cheat());
                }//super secret debug command. NO PEAKING.
            }
            
            
            while((yCord>8)||(yCord<1)){//prompts user for x coordinate, keeps prompting until it gets a number between 1 and 8 inclusive. I know these 2 vairables are backwards, sorry.
                System.out.println("X Coordinate? Must be between 1 and 8, inclusive");
                try{
                    yCord=scan.nextInt();
                }
                catch(Exception e){
                    System.out.println("Incorrectly formated input. Please enter a single integer");
                    scan.next();
                }
            }
            
            while((xCord>8)||(xCord<1)){//prompts user for y coordinate, keeps prompting until it gets a number between 1 and 8 inclusive
                System.out.println("Y Coordinate? Must be between 1 and 8, inclusive");
                try{
                    xCord=scan.nextInt();
                }
                catch(Exception e){
                    System.out.println("Incorrectly formated input. Please enter a single integer");
                    scan.next();
                }
            }
            
            if((guessingBomb)&&(gameBoard.getBomb(xCord, yCord))){//guessing yes, actually yes
                gameBoard.setRep(xCord , yCord, "m");
                if(gameBoard.getGuessed(xCord, yCord))bombCount ++;//this line stops user from guessing the same square over and over
                //gameBoard.setGuessed(xCord, yCord, true);
                bombCount --; 
            }
            
            if(guessingBomb&&(!(gameBoard.getBomb(xCord,yCord)))){//guessing yes, actually no
                System.out.println("\n You guessed there was a mine at ("+ yCord + "," + xCord + "). There wasn't one.\n \n GAME OVER \n");
                break;
            }
            
            if((!(guessingBomb))&&(gameBoard.getBomb(xCord,yCord))){//guessing no, actually yes
                System.out.println("\n You didn't guess there was a mine at ("+ yCord + "," + xCord + ") and there was.\n \n GAME OVER \n");
                break;
            }
            
            if((!(guessingBomb))&&(!(gameBoard.getBomb(xCord,yCord)))){//guessing no, actually no
                gameBoard.setRep(xCord,yCord,Integer.toString(gameBoard.countNeighbors(xCord,yCord)));
                gameBoard.setGuessed(xCord, yCord, true);
            }
            
            if(bombCount==0)System.out.println("\n You found all the Bombs! Winner winner, chicken dinner! \n \n");
        }
            
        }
    }
    
}
