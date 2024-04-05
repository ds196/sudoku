#include <iostream>
#include <iomanip>
#include <string>
#include <climits>
#include <cstdlib>

using namespace std;

const bool SAMPLE_GRID = true;

const unsigned ROWS = 9;
const unsigned COLS = 9;

const unsigned BLANK = 0;
const char ADD = 'a';
const char REMOVE = 'r';
const char EXIT = 'x';
const char NONE = 'n';

typedef unsigned Grid[ROWS][COLS];

Grid startGrid;
Grid userGrid;
int numCounts[ROWS + 1];
unsigned lastPointActed[2] {0, 0}; // Location of the last action
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
void clearFlags(bool[]);
void countNums();
unsigned charToUInt(char);


/////////////////////////////////////////////////////////////

int main(void) {
    cout << "Welcome to Sudoku!\n\n";

    bool won = false;

	clearUserGrid();
    genGrid();
    do {
        countNums();
        printGrid();
        // Take input
        char cmd;
        unsigned row;
        unsigned col;
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

void up(unsigned amnt) {
    for(unsigned i = 0; i < amnt; i++)
        cout << "\x1b[A";
}

void genGrid() {
    // First, clear
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
        for(unsigned r = 0; r < ROWS; r++)
            for(unsigned c = 0; c < COLS; c++)
                startGrid[r][c] = rand() % 9; // Random number 0-9
    }

}

void clearUserGrid() {
    for(unsigned r = 0; r < ROWS; r++)
        for(unsigned c = 0; c < COLS; c++)
            userGrid[r][c] = BLANK;
}

void combineGrids(Grid result) {
     for(unsigned r = 0; r < ROWS; r++) 
        for(unsigned c = 0; c < COLS; c++) 
           result[r][c] = userGrid[r][c] + startGrid[r][c];
}

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
                cout << blue_fg << userGrid[r][c];
            else if(samePoint && lastActType == REMOVE)  //same point, blank
                cout << blue_fg << '_';
            else if(startGrid[r][c] != BLANK)            // startgrid
                cout << bold_on << startGrid[r][c];
            else if(userGrid[r][c] != BLANK)             // usergrid
                cout << userGrid[r][c];
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
    
    /*cout << "0 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "1 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "2 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "3 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "4 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "5 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";
    cout << "6 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "7 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "8 | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " | " << "#" << " " << "#" << " " << "#" << " |\n";
    cout << "  +-+-+-+-+-+-+-+-+-+-+-+-+\n";*/

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

void takeInput(char& cmd, unsigned& row, unsigned& col, unsigned& digit) {
    /*
     * Commands:
     * r##  - remove user added number
     * a### - add number
     * x    - exit
     */
    char rowC;
    char colC;
    char digitC;
    cout << "\n(Commands: r## (remove), a### (add), x (exit))\n";
    cout << clear_line; // clears previous command
    cout << '\n'; // Makes sure error line is on screen from start
    up(1);        // "
    cout << "Enter command: ";
    cin >> cmd;
    cout << clear_line;  // Clear error line
    if(!cin) {
        cout << "Oh crap\n";
        exit(1);
    }
    if(cmd == EXIT) {
        cout << "Exit command received, exiting...\n";
        exit(0);
    } else 
    if(cmd == ADD || cmd == REMOVE) {
        cin >> rowC;
        cin >> colC;
        if(!cin) {
            cout << "Oh crap\n";
            exit(1);
        }
        row = charToUInt(rowC);
        col = charToUInt(colC);
    }
    if(cmd == ADD) {
        cin >> digitC;
        if(!cin) {
            cout << "Oh crap\n";
            exit(1);
        }
        digit = charToUInt(digitC);
    }
    cin.ignore(INT_MAX, '\n');
}

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

void clearFlags(bool flags[]) {
    for(unsigned i = 0; i < ROWS + 1; i++)
        flags[i] = false;
}

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

unsigned charToUInt(char c) {
    return (unsigned)(c - '0');
}