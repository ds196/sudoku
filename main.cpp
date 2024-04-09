/**************************************************
 * David Sharpe
 * 09/04/2024
 * Sudoku Terminal Game
 * 
 * Usage:
 *     a### to add a number to the board
 *         (row, column, digit to add)
 *     r##  to remove a number previously added
 *         (row, column)
 *     x    to exit the game
 *         (no arguments)
 **************************************************/

#include <iostream>
#include <iomanip>
#include <string>
#include <climits>
#include <cstdlib>
#include <time.h>

using std::cout;
using std::cin;
using std::string;
using std::getline;
using std::time;
using std::rand;
using std::srand;

// Whether or not to use the included sample board.
// Warning: the random board generator performs no check
//     to determine if the board is actually solvable.
const bool SAMPLE_GRID = false;
// Number of random number gen iterations to perform on board
const int GENPASSES = 80;

// Number of rows on board
const unsigned ROWS = 9;
// Number of columns on board
const unsigned COLS = 9;

// Blank digit on board (i.e. no number placed)
const unsigned BLANK = 0;
// User wants to set/change a digit on board
const char ADD = 'a';
// User wants to clear a digit on board (i.e. set to BLANK)
const char REMOVE = 'r';
// User wants exit program
const char EXIT = 'x';
// Perform no action (i.e. invalid input)
const char NONE = 'n';

// Array to hold board digits
typedef unsigned Grid[ROWS][COLS];

// Board generated by program, for the user to solve
Grid startGrid;
// Manipulated by user to solve startGrid board
Grid userGrid;
// Number of each digit remaining to be placed on board by user
int numCounts[ROWS + 1];
// Coords on board of last action by user
unsigned lastPointActed[2] {0, 0}; // Location of the last action
// Last action performed by user (NONE, ADD, or REMOVE)
char lastActType = NONE; // n - no action yet, a - added, r - removed

//////////////////////////////////////////////////////////////

//https://stackoverflow.com/a/29997156
std::ostream& bold_on(std::ostream& os) {
    return os << "\e[1m";
}
std::ostream& bold_off(std::ostream& os) {
    return os << "\e[0m";
}
//https://stackoverflow.com/a/24281812
std::ostream& underline_on(std::ostream& os) {
    return os << "\e[4m";
}
std::ostream& underline_off(std::ostream& os) {
    return os << "\e[24m";
}
//https://stackoverflow.com/a/1508589
std::ostream& clear_line(std::ostream& os) {
    return os << "\e[2K\r";
}
//https://stackoverflow.com/a/2616912
std::ostream& clear_formatting(std::ostream& os) {
    return os << "\e[0m";
}
std::ostream& blue_fg(std::ostream& os) {
    return os << "\e[34m";
}
std::ostream& blue_bg(std::ostream& os) {
    return os << "\e[44m";
}

void up(unsigned);
void genGrid();
void clearUserGrid();
void combineGrids(Grid);
void printGrid();
void takeInput(char&, unsigned&, unsigned&, unsigned&);
void insert(unsigned, unsigned, unsigned);
void remove(unsigned, unsigned);
bool checkWin();
bool checkValid();
void clearFlags(bool[]);
void countNums();
unsigned charToUInt(char);


/////////////////////////////////////////////////////////////

int main(void) {
    cout << "Welcome to Sudoku!\n\n";

    // Whether or not the player has won yet, found with checkWin()
    bool won = false;

	clearUserGrid();
    genGrid();
    do {
        countNums();
        printGrid();
        
        // Take input //

        // Action the user wants to perform
        // (ADD, REMOVE, EXIT, or NONE)
        char cmd;
        // Row number for cmd
        unsigned row;
        // Column number for cmd
        unsigned col;
        // Digit to be placed by cmd
        unsigned digit;

        takeInput(cmd, row, col, digit);
        
        if(cmd == ADD) {
            insert(row, col, digit);
        } else if(cmd == REMOVE) {
            remove(row, col);
        }
        
        won = checkWin();
        if(!won) // If the player has not won yet, will print board again.
            up(17);
    } while(!won);

    // Exited while loop, player has won.
    cout << "Congratulations! You won!\n\n";
    return 0;
}

////////////////////////////////////////////////////////////

// Move cursor up amnt lines
// Warning: does not imply \r
void up(unsigned amnt) {
    for(unsigned i = 0; i < amnt; i++)
        cout << "\x1b[A";
}

// Sets up startGrid with either included sample grid (if SAMPLE_GRID == true)
//   or randomly generates one
// Warning: random generator does not gaurantee board is solvable
void genGrid() {
    // First, clear //
    for(unsigned r = 0; r < ROWS; r++)
        for(unsigned c = 0; c < COLS; c++)
            startGrid[r][c] = BLANK;
    
    if(SAMPLE_GRID) {
        // Use sample grid from Wikipedia for testing
        //https://upload.wikimedia.org/wikipedia/commons/thumb/e/e0/Sudoku_Puzzle_by_L2G-20050714_standardized_layout.svg/1200px-Sudoku_Puzzle_by_L2G-20050714_standardized_layout.svg.png
        
        startGrid[0][0] = 5;
        startGrid[0][1] = 3;
            startGrid[0][4] = 7;

        startGrid[1][0] = 6;
            startGrid[1][3] = 1;
            startGrid[1][4] = 9;
            startGrid[1][5] = 5;

        startGrid[2][1] = 9;
        startGrid[2][2] = 8;
                startGrid[2][7] = 6;

        //

        startGrid[3][0] = 8;
            startGrid[3][4] = 6;
                startGrid[3][8] = 3;

        startGrid[4][0] = 4;
            startGrid[4][3] = 8;
            startGrid[4][5] = 3;
                startGrid[4][8] = 1;

        startGrid[5][0] = 7;
            startGrid[5][4] = 2;
                startGrid[5][8] = 6;

        //

        startGrid[6][1] = 6;
                startGrid[6][6] = 2;
                startGrid[6][7] = 8;

            startGrid[7][3] = 4;
            startGrid[7][4] = 1;
            startGrid[7][5] = 9;
                startGrid[7][8] = 5;

            startGrid[8][4] = 8;
                startGrid[8][7] = 7;
                startGrid[8][8] = 9;
    } else {

        srand(time(NULL)); // seed for rand

        for(unsigned r = 0; r < ROWS; r++)
            for(unsigned c = 0; c < COLS; c++)
                startGrid[r][c] = BLANK;

        for(unsigned i = 0; i < GENPASSES; i++) {
            unsigned r = rand() % ROWS; // random 0-8
            unsigned c = rand() % COLS; // random 0-8
            unsigned d = rand() % 10; // random 0-9, for digit in square

            unsigned previous = startGrid[r][c];
            startGrid[r][c] = d; // random 0-9

            if(!checkValid())
                startGrid[r][c] = previous;

        }
    }

}

// Sets all digits in userGrid to BLANK
void clearUserGrid() {
    for(unsigned r = 0; r < ROWS; r++)
        for(unsigned c = 0; c < COLS; c++)
            userGrid[r][c] = BLANK;
}

// Each element of result is sum of userGrid and startGrid
void combineGrids(Grid result) {
     for(unsigned r = 0; r < ROWS; r++) 
        for(unsigned c = 0; c < COLS; c++) 
           result[r][c] = userGrid[r][c] + startGrid[r][c];
}

// Prints board to screen for user
void printGrid() {
    //Grid cGrid; // combinedGrid
    //combineGrids(cGrid);

    //const string separator = "    +-+-+-+-+-+-+-+-+-+-+-+-+\n";
      const string separator = "    +-------+-------+-------+\n";

    cout << "      0 1 2   3 4 5   6 7 8  \n";
    cout << separator;
    for(unsigned r = 0; r < ROWS; r++) {
        cout << "  " << r << " | ";
        for(unsigned c = 0; c < COLS; c++) {
            //if same spot as last acted and number
            //if same spot as last acted and blank
            //startgrid number
            //usergrid number
            //blank

            bool samePoint = false;
            if(lastPointActed[0] == r && lastPointActed[1] == c)
                samePoint = true;
            
            if(samePoint && lastActType == ADD)          // same point, added
                cout << bold_on << blue_fg << userGrid[r][c];
            else if(samePoint && lastActType == REMOVE)  //same point, blank
                cout << bold_on << blue_fg << '_';
            else if(startGrid[r][c] != BLANK)            // startgrid
                cout << startGrid[r][c];
            else if(userGrid[r][c] != BLANK)             // usergrid
                cout << bold_on << userGrid[r][c];
            else if(userGrid[r][c] == BLANK && startGrid[r][c] == BLANK) // blank
                cout << ' ';
            
            // Num separator and clear formatting
            cout << clear_formatting << ' ';
            // Box separator
            if(c == 2 || c == 5)
                cout << "| ";
        }
        // End of line
        //cout << "|\n";
        cout << "|     " << r+1 << ": " << numCounts[r+1] << " left.\n";
        // Box separator
        if(r == 2 || r == 5 || r == 8)
            cout << separator;
    }

    /*cout << "    0 1 2   3 4 5   6 7 8  \n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "0 | # # # | # # # | # # # |\n";
    cout << "1 | # # # | # # # | # # # |\n";
    cout << "2 | # # # | # # # | # # # |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "3 | # # # | # # # | # # # |\n";
    cout << "4 | # # # | # # # | # # # |\n";
    cout << "5 | # # # | # # # | # # # |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "6 | # # # | # # # | # # # |\n";
    cout << "7 | # # # | # # # | # # # |\n";
    cout << "8 | # # # | # # # | # # # |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";*/
}

// Prompt for and take input from user, processes it and confirms validity
// Sets cmd = NONE if invalid input
void takeInput(char& cmd, unsigned& row, unsigned& col, unsigned& digit) {
    /*
     * Commands:
     * r##  - remove user added number
     * a### - add number
     * x    - exit
     */
    string userInput;
    
    // Prompt //

    cout << "\n(Commands: r## (remove), a### (add), x (exit))\n";
    cout << clear_line; // Clears previous command
    cout << '\n';       // Makes sure error line is on screen from start
    up(1);              // "
    cout << "Enter command: ";
    
    getline(cin, userInput);

    // Safety check of cin status
    if(!cin.good()) {
        cout << "Oh crap\n";
        exit(1);
    }

    // Ignore empty input when only \n typed
    if(userInput.length() == 0) {
        cmd = NONE;  // Take no action on board
        return;      // No change to board, re-prints board and re-prompts user
    } // implied else for rest of function

    // Parse input //

    cmd = userInput[0];
    
    // Invalid input provided
    // Short-circuit logic is used to prevent seg fault
    if(!(
           (cmd == EXIT   && userInput.length() == 1 /*x*/)
        || (cmd == ADD    && userInput.length() == 4 /*a###*/ && isdigit(userInput[1]) && isdigit(userInput[2]) && isdigit(userInput[3])) 
        || (cmd == REMOVE && userInput.length() == 3 /*r##*/  && isdigit(userInput[1]) && isdigit(userInput[2])) 
        ))
    {
        cmd = NONE;  // Take no action on board
        cout << "Error: invalid input\r";
        return;      // No change to board, re-prints board and re-prompts user 
    }

    // Exit program on exit command
    if(cmd == EXIT) {
        cout << "Exit command received, exiting...\r";
        exit(0);
    }

    // Take provided row and column chars and cast to uInts
    row = charToUInt(userInput[1]);
    col = charToUInt(userInput[2]);
    if(cmd == ADD)
        digit = charToUInt(userInput[3]);
    
    // Returns via reference params cmd, row, col, and digit
}

// Set (row, col) to digit in userGrid
void insert(unsigned row, unsigned col, unsigned digit) {
    if(startGrid[row][col] != BLANK) {
        cout << "Error: cannot overwrite starting grid!\r";
        return;
    }
    if(userGrid[row][col] != BLANK)
        cout << "Warning: overwriting " << userGrid[row][col] << " at (" << row << ", " << col << ") with " << digit << ".\r";
    
    userGrid[row][col] = digit;
    
    lastPointActed[0] = row;
    lastPointActed[1] = col;
    lastActType = ADD;
}

// Set (row, col) in userGrid to BLANK
void remove(unsigned row, unsigned col) {
    if(startGrid[row][col] != BLANK) {
        cout << "Error: cannot remove starting numbers!\r";
        return;
    }
    if(userGrid[row][col] == 0)
        cout << "Warning: already blank at (" << row << ", " << col << ")\r";
    
    userGrid[row][col] = BLANK;

    lastPointActed[0] = row;
    lastPointActed[1] = col;
    lastActType = REMOVE;
}

// Processes startGrid and userGrid together to find if user has solved board
// Checks for conflicting numbers in rows, columns, and boxes, returns true if no conflicts and no BLANK squares
bool checkWin() {
    Grid cGrid; // combinedGrid
    combineGrids(cGrid);

    bool flags[ROWS + 1];
    clearFlags(flags);

    // Check if columns are discrete
    for(unsigned r = 0; r < ROWS; r++) {
        for(unsigned c = 0; c < COLS; c++)
            if(cGrid[r][c] == BLANK) // Can't win with blank squares
                return false;
            else // Not blank
                if(!flags[cGrid[r][c]]) // Digit hasn't been found
                    flags[cGrid[r][c]] = true; // Now it has been found
                else // if it has already been found
                    return false;
        clearFlags(flags);
    }
    
    // Check if rows are discrete
    for(unsigned c = 0; c < COLS; c++) {
        for(unsigned r = 0; r < ROWS; r++)
            // Already checked for blanks and found none
            if(!flags[cGrid[r][c]]) // Digit hasn't been found
                flags[cGrid[r][c]] = true; // Now it has been found
            else // if it has already been found
                return false;
        // Clear after every column
        clearFlags(flags);
    }

    // Check if boxes are discrete
    for(unsigned rBox = 0; rBox < 3; rBox++) { // 3 boxes per row
        for(unsigned cBox = 0; cBox < 3; cBox++) { // 3 boxes per col
            // Inside box
            for(unsigned r = rBox*3; r < (rBox+1)*3; r++) { // 3 rows per box, start at beginning of box and end before start of next box (even if the next box doesn't exist)
                for(unsigned c = rBox*3; c < (rBox+1)*3; c++) // 3 cols per box
                    // Already checked for blanks and found none
                    if(!flags[cGrid[r][c]]) // Digit hasn't been found
                        flags[cGrid[r][c]] = true; // Now it has been found
                    else // if it has already been found
                        return false;
            }
            // Clear after every box
            clearFlags(flags);
        }
    }

    // Didn't find any errors, player won
    return true;
}

// For generating new board, checks for any conflicting numbers
// Returns true when no conflicts in rows, columns, or boxes
bool checkValid() {
    bool flags[ROWS + 1];
    clearFlags(flags);

    // Check if columns are discrete
    for(unsigned r = 0; r < ROWS; r++) {
        for(unsigned c = 0; c < COLS; c++)
            if(startGrid[r][c] != BLANK) {  // We don't care about how many blank squares there are for generating
                if(!flags[startGrid[r][c]]) // Digit hasn't been found
                    flags[startGrid[r][c]] = true; // Now it has been found
                else // if it has already been found
                    return false;
            }
        clearFlags(flags);
    }
    
    // Check if rows are discrete
    for(unsigned c = 0; c < COLS; c++) {
        for(unsigned r = 0; r < ROWS; r++)
            if(startGrid[r][c] != BLANK) { // We don't care about how many blank squares there are for generating
                if(!flags[startGrid[r][c]]) // Digit hasn't been found
                    flags[startGrid[r][c]] = true; // Now it has been found
                else // if it has already been found
                    return false;
            }
        // Clear after every column
        clearFlags(flags);
    }

    // Check if boxes are discrete
    for(unsigned rBox = 0; rBox < 3; rBox++) { // 3 boxes per row
        for(unsigned cBox = 0; cBox < 3; cBox++) { // 3 boxes per col
            // Inside box
            for(unsigned r = rBox*3; r < (rBox+1)*3; r++) { // 3 rows per box, start at beginning of box and end before start of next box (even if the next box doesn't exist)
                for(unsigned c = cBox*3; c < (cBox+1)*3; c++) // 3 cols per box
                    if(startGrid[r][c] != BLANK) { // We don't care about how many blank squares there are for generating
                        if(!flags[startGrid[r][c]]) // Digit hasn't been found
                            flags[startGrid[r][c]] = true; // Now it has been found
                        else // if it has already been found
                            return false;
                    }
            }
            // Clear after every box
            clearFlags(flags);
        }
    }

    // No errors found, current board is valid
    return true;
}

// Sets every element of flags to false
void clearFlags(bool flags[]) {
    for(unsigned i = 0; i < ROWS + 1; i++)
        flags[i] = false;
}

// Sets numCounts by counting occurances of each digit in board
// and subtracting each from 9 to find number of each digit left to be placed
void countNums() {
    Grid cGrid; // combinedGrid
    combineGrids(cGrid);

    // First, reset
    for(unsigned i = 0; i < ROWS + 1; i++)
            numCounts[i] = 9;
    
    // Now, count down
    for(unsigned r = 0; r < ROWS; r++)
        for(unsigned c = 0; c < COLS; c++)
            numCounts[cGrid[r][c]]--;
}

// Typecasts char to unsigned int
// If char is not a digit, returns 0
unsigned charToUInt(char c) {
    if(isdigit(c))
        return (unsigned)(c - '0');
    else
        return 0;
}

