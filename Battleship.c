/*
 *  Author:                 Michael Iuzolino
 *  Date Created:           November 30th, 2015
 *
 *  Program Name:           Battleship
 *
 *                          This program is the game battleship!
 *
 *                          ***NOTE***
 *                          If Input file is used, make sure last line (y-coord) has a newline char after it (return key)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Global Variables
int PlayerShipsCounter, ComputerShipsCounter;
int tryCnt, range;
int origX, origY;
char firstHit;


// Structs

// Hit grid for ships
typedef struct shipGrid
{
    int X_loc;
    int Y_loc;
    struct shipGrid *shipGridPtr;
    
} shipGrid;

// Ships
typedef struct ship
{
    char shiptype[20];
    char shiptypeID;
    char shipOrientation;
    int shipsize;
    int *X_Location;
    int *Y_Location;
    struct shipGrid Grid;
    struct shipGrid *shipGridPtr;
    int *X_Hit;
    int *Y_Hit;
    int hitPts;
    char sunk;
    struct ship *shipPtr;
} ship;

// Scorecard
typedef struct score
{
    char playerName[100];
    int hits;
    int misses;
    int sunk;
} score;



// Function Prototypes
ship SetupGame(int numShips, char Board_Major[10][10], char CPU_Setup, ship *playerShips);
void PrintShipChoices(void);
void ShipPlacement(ship *currentShip, char PlayerBoard_Major[10][10], char CPU_Turn);
void PlaceShips(ship *Ships, char Board_Major[10][10]);
void PrintBoard(char Board[10][10], char majorBoard);
void InitializeBoard(char PlayerBoard_Major[10][10], char PlayerBoard_minor[10][10], char CompBoard_Major[10][10], char CompBoard_minor[10][10]);
void ShipTypeSymbolSelect(int i, ship *currentShip);
int PlayGame(ship *playerShips, ship *computerShips, char PlayerBoard_Major[10][10], char PlayerBoard_minor[10][10], char CompBoard_Major[10][10], char CompBoard_minor[10][10], char playerName[100], score *playerScore, score *CPUScore);
void RoundTurn(score *scoreCard, char Board_Major[10][10], char Opponent_Board_Major[10][10], ship *ships, char Board_minor[10][10], char CPU_Turn, int CPU_prev_hit_coords[2], int *CPU_prevhit);
int LetterToNum(char x_in);
char DetermineShipChoice(char shipTypeSymbol, ship *currentShip);
void PrintGameBoard(char MajorBoard[10][10], char minorBoard[10][10], score *scoreCard, score *CPUScore);
void InitializeScoreCard(score *playerScore, score *CPUScore);
void AddShip(ship *Ships, int num, int x, int y);
void RemoveShip(ship *Ships);
void AbandonShip(ship *currentShip, score *scoreCard, ship *opponents_Ships, int *CPU_prevhit, char Opponent_Board_Major[10][10]);
ship BoardInputSetup(int playerNum);
shipGrid CreateGrid(int shipSize);



/*
 * Function: main
 * --------------
 * Main function of the program that setups the ships and starts the game
 *
 */

int main(void)
{
    
    // Declarations
    int numShips, winner;
    char playerName[100];
    int boardSetup;
    srand((unsigned int)time(NULL));
    
    char PlayerBoard_Major[10][10];
    char PlayerBoard_minor[10][10];
    char CompBoard_Major[10][10];
    char CompBoard_minor[10][10];
    
    ship playerShips, temp;
    ship computerShips;
    score playerScore, CPUScore;
    
    
    // Introduction and info gathering from user
    printf("Welcome to Battleship!\n");
    printf("Please enter your name: ");
    scanf("%s", playerName);
    printf("Hello, %s! Would you like to manually setup your gameboard or load a file? ", playerName);
    printf("\n");
    printf("1. Manual\n");
    printf("2. Load file\n");
    printf("> ");
    scanf("%d", &boardSetup);
    
    // Initialize Board
    InitializeBoard(PlayerBoard_Major, PlayerBoard_minor, CompBoard_Major, CompBoard_minor);
    
    // Case 1: Manual board setup
    if (boardSetup == 1)
    {
        printf("How many ships will you use to play the game? ");
        scanf("%d", &numShips);
        PlayerShipsCounter = numShips;
        ComputerShipsCounter = numShips;
        
        
        
        // Game Setup
        printf("\nEntering Game Setup...\n\n");
        // Player Setup
        playerShips = SetupGame(numShips, PlayerBoard_Major, 'n', &temp);
        // Computer Setup
        computerShips = SetupGame(numShips, CompBoard_Major, 'y', &playerShips);
        printf("\n*** Game setup complete. ***\n\n");
    }
    
    // Case 2: Load board from file
    else if (boardSetup == 2)
    {
        printf("Please enter the filename for the player's ships: ");
        playerShips = BoardInputSetup(1);
        printf("Please enter the filename for the computer's ships: ");
        computerShips = BoardInputSetup(2);
        printf("Number of player ships: %d\n\n\n", PlayerShipsCounter);
        
        // Account for the head ship
        PlayerShipsCounter--;
        ComputerShipsCounter--;
        
        // Place the player's ships onto the board
        PlaceShips(&playerShips, PlayerBoard_Major);
        
        // Place the computer's ships onto the board
        PlaceShips(&computerShips, CompBoard_Major);
        
    }
    
    // Initialize Score Card
    InitializeScoreCard(&playerScore, &CPUScore);
    strcpy(playerScore.playerName, playerName);
    strcpy(CPUScore.playerName, "Computer");
    
    // Play the game
    printf("\nLet's play Battleship!\n");
    winner = PlayGame(&playerShips, &computerShips, PlayerBoard_Major, PlayerBoard_minor, CompBoard_Major, CompBoard_minor, playerName, &playerScore, &CPUScore);
    
    // ***Check winner***
    //Case 1: Player wins
    if (winner == 1)
    {
        printf("Congratulations player! You've won!\n");
    }
    
    // Case 2: Computer wins
    else if (winner == 2)
    {
        printf("You lost to the computer. Try again!\n");
    }
    
    // Case 3: Something went wrong
    else if (winner == -1)
    {
        printf("Something has gone wrong.\n");
    }
    
    // Print final game boards
    printf("Player's game board:\n");
    PrintBoard(PlayerBoard_Major, 'y');
    printf("\nComputer's game board:\n");
    PrintBoard(CompBoard_Major, 'y');
    
    return 0;
}



/*
 * Function: CreateGrid
 * --------------------
 * Creates the ship's grid based off of ship size
 *
 */

shipGrid CreateGrid(int shipSize)
{
    // Declarations
    int i;
    shipGrid HeadGrid, *tempGrid, *currentGrid;
    
    
    // Set head grid
    currentGrid = &HeadGrid;
    
    // Create nodes
    for (i = 1; i < shipSize; i++)
    {
        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
        currentGrid->shipGridPtr = tempGrid;
        currentGrid = tempGrid;
    }
    
    // Set tail node ptr to NULL
    currentGrid->shipGridPtr = NULL;
    
    return HeadGrid;
    
}



/*
 * Function: BoardInputSetup
 * -------------------------
 * Sets up the board based off of input file
 *
 */

ship BoardInputSetup(int playerNum)
{
    // Declarations
    FILE *inputFile;
    char fileName[20], inputString[100];
    int i, j;
    int x, y, tempx, tempy;
    int shipSize = 0;
    char chrx, chry;
    ship HeadShip, *tempShip, *currentShip;
    shipGrid *tempGrid, *currentGrid;
    

    // Obtain filename from user
    scanf("%s", fileName);
    
    // Append .txt to user's filename
    strcat(fileName, ".txt");
    
    // Open file
    inputFile = fopen(fileName, "r");
    
    // Read File and create linked list
    currentShip = &HeadShip;
    tempShip = (ship *)malloc(sizeof(ship));
    currentShip->shipPtr = tempShip;
    currentShip = tempShip;
    
    
    // Sets initial ship counter
    i = 0;
    if (playerNum == 1)
    {
        PlayerShipsCounter = 2;
    }
    else if (playerNum == 2)
    {
        ComputerShipsCounter = 2;
    }
    
    // Scans through file line by line
    while (fgets(inputString, 100, inputFile) != NULL)
    {
        //printf("\nINPUT STRING: %s\n", inputString);
        if (strlen(inputString) > 1)
        {
            // Case 0: End of ship information
            if (i == 4)
            {
                // reset index
                i = 0;
                
                // Increment ship counters
                if (playerNum == 1)
                {
                    PlayerShipsCounter++;
                }
                else if (playerNum == 2)
                {
                    ComputerShipsCounter++;
                }
                
                // Create new node in ship list
                tempShip = (ship *)malloc(sizeof(ship));
                currentShip->shipPtr = tempShip;
                currentShip = tempShip;
            }
            

            // Case 1: Ship Type
            if (i == 0)
            {
                strcpy(currentShip->shiptype, inputString);
                currentShip->shiptypeID = currentShip->shiptype[0];
            }
            
            // Case 2: Ship size
            else if (i == 1)
            {
                // Read string and obtain ship size
                sscanf(inputString, "%d", &shipSize);
                
                // Set ship parameters
                currentShip->shipsize = shipSize;
                currentShip->hitPts = shipSize;
                currentShip->sunk = 'n';
                
                // Create grid
                currentShip->Grid = CreateGrid(shipSize);
                
                // Link ship to its grid
                currentShip->shipGridPtr = &currentShip->Grid;
            }
            
            // Case 3: X coordinates
            else if (i == 2)
            {
                // Initialize head grid
                currentGrid = &currentShip->Grid;
                
                // Iterate through string and find integer coordinates while skipping spaces
                for (j = 0; j < strlen(inputString); j++)
                {
                    // read char from string
                    chrx = inputString[j];
                    
                    // Turn char into integer
                    tempx = chrx - '0';
                    
                    // If negative value, means its a space. Skip it. Only log the integer values
                    if (tempx >= 0)
                    {
                        x = tempx;
                        
                        // Update ship grid
                        currentShip->X_Location = &x;
                        currentGrid->X_loc = x;
                        
                        // move to next grid position
                        tempGrid = currentGrid->shipGridPtr;
                        currentGrid = tempGrid;
                    }
                }
            }
            
            // Case 4: Y coordinates
            else if (i == 3)
            {
                // Initialize head grid
                currentGrid = &currentShip->Grid;
                
                // Iterate through string and find integer coordinates while skipping spaces
                for (j = 0; j < strlen(inputString); j++)
                {
                    // read char from string
                    chry = inputString[j];
                    
                    // Turn char into integer
                    tempy = chry - '0';
                    
                    // If negative value, means its a space. Skip it. Only log the integer values
                    if (tempy >= 0)
                    {
                        y = tempy;
                        
                        // Update ship grid
                        currentShip->X_Location = &y;
                        currentGrid->Y_loc = y;
                        
                        // move to next grid position
                        tempGrid = currentGrid->shipGridPtr;
                        currentGrid = tempGrid;
                    }
                }
            }
            // increment index
            i++;
        }
    }// End of while(1)
    
    // Append NULL to tail node's linked list pointer
    currentShip->shipPtr = NULL;
    
    // close file
    fclose(inputFile);
    
    return HeadShip;
}




/*
 * Function: PlaceShips
 * --------------------
 * Place ships on the board
 *
 */

void PlaceShips(ship *Ships, char Board_Major[10][10])
{
    // Declarations
    int shipNum, shipPosition;
    int shipSize;
    int x, y;
    char shipSymbol;
    
    shipGrid *tempGrid, *currentGrid;
    ship *tempShip, *currentShip;
    
    
    // Set current ship
    currentShip = Ships;
    tempShip = currentShip->shipPtr;
    currentShip = tempShip;
    
    // Iterate through ships in linked list
    for (shipNum = 0; shipNum < PlayerShipsCounter; shipNum++)
        
    {
        // Obtain ships parameters
        shipSymbol = currentShip->shiptypeID;
        shipSize = currentShip->shipsize;
        
        // Setup Ships Grid
        tempGrid = currentShip->shipGridPtr;
        currentGrid = tempGrid;
    
        
        // Iterate through ships' grid
        for (shipPosition = 0; shipPosition < shipSize; shipPosition++)
        {
            // Log coordinates of ship segment
            printf("Ship: %s\n", currentShip->shiptype);
            
            x = currentGrid->X_loc;
            y = currentGrid->Y_loc;
            
            printf("x: %d, y: %d\n", x, y);
            
            // Write to board
            Board_Major[y][x] = shipSymbol;
            
            // Move to next node in ship's grid
            tempGrid = currentGrid->shipGridPtr;
            currentGrid = tempGrid;
        }
        
        // Move to next ship
        tempShip = currentShip->shipPtr;
        currentShip = tempShip;
    }
    return;
}



/*
 * Function: InitializeBoard
 * -------------------------
 * Initialize the board as spaces
 *
 */

void InitializeBoard(char PlayerBoard_Major[10][10], char PlayerBoard_minor[10][10], char CompBoard_Major[10][10], char CompBoard_minor[10][10])
{
    // Declarations
    int i, j;
    char space = ' ';
    
    // Iterate through rows
    for (i = 0; i < 10; i++)
    {
        // Iterate through columns
        for (j = 0; j < 10; j++)
        {
            // Place spaces
            PlayerBoard_Major[i][j] = space;
            PlayerBoard_minor[i][j] = space;
            CompBoard_Major[i][j] = space;
            CompBoard_minor[i][j] = space;
        }
    }
    return;
}



/*
 * Function: SetupGame
 * -------------------
 *
 *
 */

ship SetupGame(int numShips, char Board_Major[10][10], char CPU_Setup, ship *playerShips)
{
    // Declarations
    int i, shipSize = 0;
    char shipTypeSymbol = '0';
    
    ship headShip;
    ship *tempShip, *currentShip;
    ship *tempPlayerShip, *currentPlayerShip;
    

    // Case 1: Player Setup
    if (CPU_Setup == 'n')
    {
        currentShip = &headShip;
        tempShip = (ship *)malloc(sizeof(ship));
        currentShip->shipPtr = tempShip;
        currentShip = tempShip;
        
        // Iterate through ships
        for (i = 1; i <= numShips; i++)
        {
            // Obtains ship type from user
            ShipTypeSymbolSelect(i, currentShip);
            
            // Assigns type symbol and size to structure
            shipTypeSymbol = currentShip->shiptypeID;
            shipSize = currentShip->shipsize;
            
            // Print major board pre-ship-placement
            PrintBoard(Board_Major, 'y');
            
            // Obtains ship placement from user
            ShipPlacement(currentShip, Board_Major, CPU_Setup);
            
            // Print major board after placement
            PrintBoard(Board_Major, 'y');
            
            // Create next list node
            tempShip = (ship *)malloc(sizeof(ship));
            currentShip->shipPtr = tempShip;
            currentShip = tempShip;
            
            // Last node - set ptr to NULL
            if (i == numShips)
            {
                currentShip->shipPtr = NULL;
            }
        }
    }
    
    
    // Case 2: CPU Setup
    else if (CPU_Setup == 'y')
    {
        // Initialize player ships to guide ship choice of computer ships
        tempPlayerShip = playerShips->shipPtr;
        currentPlayerShip = tempPlayerShip;
    
        // Initialize computer ships
        currentShip = &headShip;
        tempShip = (ship *)malloc(sizeof(ship));
        currentShip->shipPtr = tempShip;
        currentShip = tempShip;
        
        
        for (i = 0; i < numShips; i++)
        {
            // Copy pertinent data from user to CPU ships
            currentShip->shiptypeID = currentPlayerShip->shiptypeID;
            currentShip->shipsize = currentPlayerShip->shipsize;
            strcpy(currentShip->shiptype, currentPlayerShip->shiptype);
            currentShip->hitPts = currentPlayerShip->hitPts;
            
            // Randomly places computer ship on board
            ShipPlacement(currentShip, Board_Major, CPU_Setup);
            
            // Moves to next node in CPU linked list
            tempShip = (ship *)malloc(sizeof(ship));
            currentShip->shipPtr = tempShip;
            currentShip = tempShip;
            
            // Moves to next node in players linked list
            tempPlayerShip = currentPlayerShip->shipPtr;
            currentPlayerShip = tempPlayerShip;
        }
        // Set TAIL ptr to NULL
        currentShip->shipPtr = NULL;
    }
    
    
    return headShip;
}



/*
 * Function: ShipTypeSymbolSelect
 * ------------------------------
 * Select type of ship by symbol from chart
 *
 */

void ShipTypeSymbolSelect(int i, ship *currentShip)
{
    // Declarations
    char correctShipChoice;
    char shipTypeSymbol;
    
    // Initialize choice
    correctShipChoice = 'n';
    
    // Obtain choice from user - loop until they enter valid input
    while (correctShipChoice != 'y')
    {
        // Print the ship choice menu
        PrintShipChoices();
        
        // Obtain choice
        printf("Enter board symbol to choose ship type for ship #%d: ", i);
        scanf(" %c", &shipTypeSymbol);
        
        // Set ship values
        currentShip->shiptypeID = shipTypeSymbol;
        currentShip->sunk = 'n';
        
        // Determine if the ship choice is appropriate
        correctShipChoice = DetermineShipChoice(shipTypeSymbol, currentShip);
    }
    
    return;
}



/*
 * Function: DetermineShipChoice
 * ------------------------------
 * Determine if the symbol is a valid choice. Update ship data if it is.
 *
 */

char DetermineShipChoice(char shipTypeSymbol, ship *currentShip)
{
    // Declarations
    char correctShipChoice;
    int shipSize;
    
    // Check cases of user-input symbol
    switch (shipTypeSymbol)
    {
        // Aircraft Carrier
        case 'a':
        case 'A':
            correctShipChoice = 'y';
            shipTypeSymbol = 'A'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Aircraft Carrier");
            shipSize = 5;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Battleship
        case 'b':
        case 'B':
            correctShipChoice = 'y';
            shipTypeSymbol = 'B'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Battleship");
            shipSize = 4;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Cruiser
        case 'c':
        case 'C':
            correctShipChoice = 'y';
            shipTypeSymbol = 'C'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Cruiser");
            shipSize = 3;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Submarine
        case 's':
        case 'S':
            correctShipChoice = 'y';
            shipTypeSymbol = 'S'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Submarine");
            shipSize = 3;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Patrol Boat
        case 'p':
        case 'P':
            correctShipChoice = 'y';
            shipTypeSymbol = 'P'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Patrol Boat");
            shipSize = 2;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Kayak
        case 'k':
        case 'K':
            correctShipChoice = 'y';
            shipTypeSymbol = 'K'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Kayak");
            shipSize = 1;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        
        // Row boat
        case 'r':
        case 'R':
            correctShipChoice = 'y';
            shipTypeSymbol = 'R'; // Ensures uppercase for board
            currentShip->shiptypeID = shipTypeSymbol;
            strcpy(currentShip->shiptype, "Row boat");
            shipSize = 1;
            currentShip->shipsize = shipSize;
            currentShip->hitPts = shipSize;
            break;
        default:
            correctShipChoice = 'n';
            printf("\n***Invalid choice***\n");
            printf("Please enter your choice again.\n");
            break;
    }
    
    return correctShipChoice;
}


/*
 * Function: ShipPlacement
 * -----------------------
 * Places the ship for user-input ship information
 *
 */

void ShipPlacement(ship *currentShip, char PlayerBoard_Major[10][10], char CPU_Turn)
{
    // Declarations
    char x_in = '0';
    int y_in = 0;
    char shipDirection = '0';
    int i, j, randNum;
    int x = 0, y = 0;
    int shipSize = currentShip->shipsize;
    char shipTypeSymbol = currentShip->shiptypeID;
    char successfulPlacement = 'n';
    char offBoard = 'n';
    char placementConflict = 'n';
    char successfulOrientation = 'n';
    char directionArray[8] = {'u', 'd', 'l', 'r', 'q', 'e', 'a', 's'};

    shipGrid *tempGrid, *currentGrid;
    
    // Create ship grid
    tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
    currentShip->shipGridPtr = tempGrid;
    currentGrid = tempGrid;
    
    
    // Placement of ship onto board
    while (successfulPlacement == 'n')
    {
        
        // Case 1: Player's setup
        if (CPU_Turn == 'n')
        {
            // Obtain coordinates from user
            printf("\n\nEnter the coordinates (e.g., A5) where you'd like to place the ship: ");
            scanf(" %c%d", &x_in, &y_in);
            x = LetterToNum(x_in);
            y = y_in - 1;
        }
        
        // Case 2: CPU setup
        else if (CPU_Turn == 'y')
        {
            // Obtain random numbers
            x = (rand() % 10);
            y = (rand() % 10) + 1;
        }
        
        
        // Check if coordinate is occupied
        
        // Case 1: Position Occupied
        if (PlayerBoard_Major[y][x] != ' ')
        {
            if (CPU_Turn == 'n')
            {
                printf("Position already occupied!\n");
                printf("Try another orientation");
            }
        }
        
        // Case 2: Position Not Occupied
        else
        {
            currentShip->X_Location = &x;
            currentShip->Y_Location = &y;
            
            successfulPlacement = 'y';
        }
    }
    
    
    // Case: Small ship. No orientation possible
    if (shipSize == 1)
    {
        PlayerBoard_Major[y][x] = shipTypeSymbol;
        currentShip->shipOrientation = 'z';
        currentGrid->X_loc = x;
        currentGrid->Y_loc = y;
        return;
    }
    
    
    // Obtain desired orientation from user
    while (successfulOrientation == 'n')
    {
        offBoard = 'n';
        placementConflict = 'n';
        
        
        // Case 1: User setup
        if (CPU_Turn == 'n')
        {
            printf("\nEnter orientation of ship with origin at %c%d\n", x_in, y_in);
            printf("[u]p\n");
            printf("[d]own\n");
            printf("[l]eft\n");
            printf("[r]ight\n");
            printf("Diagonal: up-left [q], up-right [e], down-left [a], down-right [s]\n");
            printf("> ");
            scanf(" %c", &shipDirection);
            currentShip->shipOrientation = shipDirection;
        }
        
        // Case 2: CPU Setup
        else if (CPU_Turn == 'y')
        {
            randNum = rand() % 8;
            shipDirection = directionArray[randNum];
            currentShip->shipOrientation = shipDirection;
        }
        
        
        // Check cases of ship direction
        switch (shipDirection)
        {
                
            // Case 'u': Upward placement
            case 'u':
            case 'U':
                
                // Case 1: Out of boundaries
                if (y_in - shipSize < 0)
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    offBoard = 'y';
                    break;
                }
                
                // Case 2: Collision with other ships
                else if (y_in - shipSize >= 0)
                {
                    offBoard = 'n';
                    
                    // Checks if collision occurs
                    for (i = y; i > y - shipSize; i--)
                    {
                        
                        if (PlayerBoard_Major[i][x] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    for (i = y; i > y - shipSize; i--)
                    {
                        PlayerBoard_Major[i][x] = shipTypeSymbol;
                        
                        currentGrid->X_loc = x;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
   
            // Case 'd': Downward placement
            case 'd':
            case 'D':
                
                // Case 1: Out of boundaries
                if (y + shipSize > 10)
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else if (y_in + shipSize <= 10)
                {
                    offBoard = 'n';
                    
                    for (i = y; i < shipSize + y; i++)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[i][x] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                    }
                }
                
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    for (i = y; i < shipSize + y; i++)
                    {
                        PlayerBoard_Major[i][x] = shipTypeSymbol;
                        currentGrid->X_loc = x;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
                
            // Case 'l': Leftward placement
            case 'l':
            case 'L':
                
                // Case 1: Out of boundaries
                if (x - shipSize < -1)
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else if (x - shipSize >= -1)
                {
                    offBoard = 'n';
                    
                    for (i = 0; i < shipSize; i++)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[y][x-i] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                    }
                }

                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    for (i = x; i >= (x+1) - shipSize; i--)
                    {
                        PlayerBoard_Major[y][i] = shipTypeSymbol;
                        currentGrid->X_loc = i;
                        currentGrid->Y_loc = y;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
                
            // Case 'r': Rightward placement
            case 'r':
            case 'R':
                
                // Case 1: Out of boundaries
                if (x + shipSize > 10)
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else if (x + shipSize <= 10)
                {
                    offBoard = 'n';
                    
                    for (i = 0; i < shipSize; i++)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[y][x+i] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    for (i = x; i < x + shipSize; i++)
                    {
                        PlayerBoard_Major[y][i] = shipTypeSymbol;
                        currentGrid->X_loc = i;
                        currentGrid->Y_loc = y;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;

            // Case 'q': diagonal up-left
            case 'q':
            case 'Q':
                
                // Case 1: Out of boundaries
                if ((y_in - shipSize < 0) || (x - shipSize < -1))
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else
                {
                    offBoard = 'n';
                    
                    j = x;
                    for (i = y; i > y - shipSize; i--)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[i][j] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                        j--;
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    j = x;
                    for (i = y; i > y - shipSize; i--)
                    {
                        PlayerBoard_Major[i][j] = shipTypeSymbol;
                        currentGrid->X_loc = j;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                        
                        j--;
                        
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
                
            // case 'e': diagonal up-right
            case 'e':
            case 'E':
                // Case 1: Out of boundaries
                if ((y_in - shipSize < 0) || (x + shipSize > 10))
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else
                {
                    offBoard = 'n';
                    
                    j = x;
                    for (i = y; i > y - shipSize; i--)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[i][j] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                        j++;
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    j = x;
                    for (i = y; i > y - shipSize; i--)
                    {
                        PlayerBoard_Major[i][j] = shipTypeSymbol;
                        currentGrid->X_loc = j;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                        
                        j++;
                        
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;

            // case 'a': diagonal down-left
            case 'a':
            case 'A':
                // Case 1: Out of boundaries
                if ((y + shipSize > 10) || (x - shipSize < -1))
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else
                {
                    offBoard = 'n';
                    
                    j = x;
                    for (i = y; i < y + shipSize; i++)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[i][j] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                        j--;
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    j = x;
                    for (i = y; i < y + shipSize; i++)
                    {
                        PlayerBoard_Major[i][j] = shipTypeSymbol;
                        currentGrid->X_loc = j;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                        j--;
                        
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
 
            // case's': diagonal down-right
            case 's':
            case 'S':
                // Case 1: Out of boundaries
                if ((y + shipSize > 10) || (x + shipSize > 10))
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("\n***Ship placed off of board.\n");
                    }
                    successfulOrientation = 'n';
                    break;
                }
                
                // Case 2: Collision with other ships
                else
                {
                    offBoard = 'n';
                    
                    j = x;
                    for (i = y; i < y + shipSize; i++)
                    {
                        // Checks if collision occurs
                        if (PlayerBoard_Major[i][j] != ' ')
                        {
                            // Only prints for user's board
                            if (CPU_Turn == 'n')
                            {
                                printf("\n***Conflicts with other ship's placement!\n");
                            }
                            placementConflict = 'y';
                            break;
                        }
                        j++;
                    }
                }
                
                // Case 3: In bounds and no collisions
                if ((placementConflict == 'n') && (offBoard == 'n'))
                {
                    
                    successfulOrientation = 'y';
                    
                    // Writes ship orientation onto board
                    j = x;
                    for (i = y; i < y + shipSize; i++)
                    {
                        PlayerBoard_Major[i][j] = shipTypeSymbol;
                        currentGrid->X_loc = j;
                        currentGrid->Y_loc = i;
                        
                        tempGrid = (shipGrid *)malloc(sizeof(shipGrid));
                        currentGrid->shipGridPtr = tempGrid;
                        currentGrid = tempGrid;
                        
                        j++;
                        
                    }
                    // Set TAIL ptr to NULL
                    currentGrid->shipGridPtr = NULL;
                    break;
                }
                
                // Case 4: Out of bounds and/or Collisions
                else
                {
                    // Only prints for user's board
                    if (CPU_Turn == 'n')
                    {
                        printf("Try again with a different orientation.\n");
                    }
                    successfulOrientation = 'n';
                }
                break;
        }
    }
    return;
}




/*
 * Function: PrintShipChoices
 * --------------------------
 * Prints the ship choices
 *
 */

void PrintShipChoices(void)
{
    printf("Ship Type \t \t Board Symbol \t  \tShip Size\n");
    printf("---------------------------------------------\n");
    printf("Aircraft Carrier \t A \t \t \t \t \t 5\n");
    printf("Battleship \t \t \t B \t \t \t \t \t 4\n");
    printf("Cruiser \t \t \t C \t \t \t \t \t 3\n");
    printf("Submarine \t \t \t S \t \t \t \t \t 3\n");
    printf("Patrol Boat \t \t P \t \t \t \t \t 2\n");
    printf("Kayak \t \t \t \t K \t \t \t \t \t 1\n");
    printf("Row boat \t \t \t R \t \t \t \t \t 1\n");
    printf("---------------------------------------------\n");
    printf("\n");
    
    return;
}


/*
 * Function: PrintBoard
 * --------------------
 *  Displaces the board. Used for displaying final game boards
 *
 */

void PrintBoard(char Board[10][10], char majorBoard)
{
    // Declarations
    int i, j;
    char vertBar = '|';
    
    printf("\n");
    printf("\t%4c %5c %5c %5c %5c %5c %5c %5c %5c %5c\n", 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J');
    printf("\t-------------------------------------------------------------\n");
    
    for (i = 0; i < 10; i++)
    {
        if (majorBoard == 'y')
        {
            printf("%5c %5c %5c %5c %5c %5c %5c %5c %5c %5c %5c\n", vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar);
        }
        printf("%d", i+1);
        for (j = 0; j <= 10; j++)
        {
            if (j == 0)
            {
                printf("\t%1c %2c %2c ", vertBar, Board[i][j], vertBar);
            }
            else if ((j > 0) && (j <= 9))
            {
                printf("%2c %2c ", Board[i][j], vertBar);
            }
            
        }
        printf("\n");
        if (majorBoard =='y')
        {
            printf("%5c %5c %5c %5c %5c %5c %5c %5c %5c %5c %5c\n", vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar);
        }
        printf("\t-------------------------------------------------------------\n");
    }
    printf("\n");
    
    return;
}




/*
 * Function: PrintGameBoard
 * ------------------------
 *  Prints the game board to user: their major and minor boards
 *
 */

void PrintGameBoard(char MajorBoard[10][10], char minorBoard[10][10], score *scoreCard, score *CPUScore)
{
    // Declarations
    int i, j, k, p;
    char vertBar = '|';
    
    printf("\n");
    printf("\t%4c %5c %5c %5c %5c %5c %5c %5c %5c %5c", 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J');
    printf("\t%16c %5c %5c %5c %5c %5c %5c %5c %5c %5c\n", 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J');
    printf("\t-------------------------------------------------------------");
    printf("\t\t\t-------------------------------------------------------------\n");
    
    
    // Initialize values
    i = 0;
    k = 0;
    p = 0;
    while(1)
    {
        printf("%5c %5c %5c %5c %5c %5c %5c %5c %5c %5c %5c", vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar);
        p++;
        // Sunk
        if (i == 11)
        {
            printf("\t\t Sunk: %2d  \t\t Sunk: %2d", scoreCard->sunk, CPUScore->sunk);
        }
        if (i < 10)
        {
            // Minor board row 1
            printf("%8d", i+1);
            for (j = 0; j <= 10; j++)
            {
                if (j == 0)
                {
                    printf("\t%1c %2c %2c ", vertBar, minorBoard[i][j], vertBar);
                }
                else if ((j > 0) && (j <= 9))
                {
                    printf("%2c %2c ", minorBoard[i][j], vertBar);
                }
            }
        }
        
        printf("\n");
        
        
        // Major board row 1
        printf("%d", k+1);
        for (j = 0; j <= 10; j++)
        {
            if (j == 0)
            {
                printf("\t%1c %2c %2c ", vertBar, MajorBoard[k][j], vertBar);
            }
            else if ((j > 0) && (j <= 9))
            {
                printf("%2c %2c ", MajorBoard[k][j], vertBar);
            }
            
        }
        p++;
        // Names
        if (k == 5)
        {
            printf("\t\t%s  \t\t\t%s", scoreCard->playerName, CPUScore->playerName);
            
        }
        
        
        
        
        if (i < 10)
        {
            printf("\t\t\t-------------------------------------------------------------\n");
        }
        else
        {
            printf("\n");
        }
        
        printf("%5c %5c %5c %5c %5c %5c %5c %5c %5c %5c %5c", vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar, vertBar);
        p++;
        // Hit
        if (i == 10)
        {
            printf("\t\t Hits: %2d  \t\t Hits: %2d", scoreCard->hits, CPUScore->hits);
        }
        
        
        // Minor board row 2
        if (i < 10)
        {
            i++;
            printf("%8d", i+1);
            for (j = 0; j <= 10; j++)
            {
                if (j == 0)
                {
                    printf("\t%1c %2c %2c ", vertBar, minorBoard[i][j], vertBar);
                }
                else if ((j > 0) && (j <= 9))
                {
                    printf("%2c %2c ", minorBoard[i][j], vertBar);
                }
            }
        }
        
        printf("\n");
        printf("\t-------------------------------------------------------------");
        // Miss
        if (i == 10)
        {
            printf("\t\t Miss: %2d  \t\t Miss: %2d", scoreCard->misses, CPUScore->misses);
        }
        if (i < 10)
        {
            printf("\t\t\t-------------------------------------------------------------\n");
        }
        
        else
        {
            printf("\n");
        }
        
        // Increment counters
        p++;
        k++;
        i++;
        if (k == 10)
        {
            break;
        }
    }
    
    printf("\n");
    return;
}




/*
 * Function: LetterToNum
 * ---------------------
 *  Converts user entered letter to corresponding board number
 *
 */

int LetterToNum(char x_in)
{
    switch(x_in)
    {
        case 'a':
        case 'A':
            return 0;
            break;
        case 'b':
        case 'B':
            return 1;
            break;
        case 'c':
        case 'C':
            return 2;
            break;
        case 'd':
        case 'D':
            return 3;
            break;
        case 'e':
        case 'E':
            return 4;
            break;
        case 'f':
        case 'F':
            return 5;
            break;
        case 'g':
        case 'G':
            return 6;
            break;
        case 'h':
        case 'H':
            return 7;
            break;
        case 'i':
        case 'I':
            return 8;
            break;
        case 'j':
        case 'J':
            return 9;
            break;
        default:
            
            break;
    }
    
    return -1;
}




/*
 * Function: PlayGame
 * ------------------
 * Main play function
 *
 */

int PlayGame(ship *playerShips, ship *computerShips, char PlayerBoard_Major[10][10], char PlayerBoard_minor[10][10], char CompBoard_Major[10][10], char CompBoard_minor[10][10], char playerName[100], score *playerScore, score *CPUScore)
{
    // Declarations
    char gameOver = 'n';
    int CPU_prev_hit_coords[2];
    int CPU_prevhit = 0;
    
    // Display initial board to user
    PrintGameBoard(PlayerBoard_Major, PlayerBoard_minor, playerScore, CPUScore);
    
    // Main game loop
    while (gameOver == 'n')
    {
        // Player's turn
        printf("\n%s's Turn.", playerName);
        
        RoundTurn(playerScore, PlayerBoard_Major, CompBoard_Major, computerShips, PlayerBoard_minor, 'n', CPU_prev_hit_coords, &CPU_prevhit);
        if (ComputerShipsCounter == 0)
        {
            return 1;
        }
        // Display updated board to user
        PrintGameBoard(PlayerBoard_Major, PlayerBoard_minor, playerScore, CPUScore);
        
        printf("\nEnd of %s's turn.", playerName);
        
        // CPU's turn
        printf("\nCPU's Turn.");
        RoundTurn(CPUScore, CompBoard_Major, PlayerBoard_Major, playerShips, CompBoard_minor, 'y', CPU_prev_hit_coords, &CPU_prevhit);
        if (PlayerShipsCounter == 0)
        {
            return 2;
        }
        // Display updated board after CPU attack to user
        PrintGameBoard(PlayerBoard_Major, PlayerBoard_minor, playerScore, CPUScore);
    }
    
    return -1;
}



/*
 * Function: InitializeScoreCard
 * -----------------------------
 *  Initialize the score card
 *
 */

void InitializeScoreCard(score *playerScore, score *CPUScore)
{
    // Initialize player's scores
    playerScore->hits = 0;
    playerScore->misses = 0;
    playerScore->sunk = 0;
    
    // Initialize computer's scores
    CPUScore->hits = 0;
    CPUScore->misses = 0;
    CPUScore->sunk = 0;
}



/*
 * Function: RoundTurn
 * -------------------
 *
 *
 */

void RoundTurn(score *scoreCard, char Board_Major[10][10], char Opponent_Board_Major[10][10], ship *opponents_Ships, char Board_minor[10][10], char CPU_Turn, int CPU_prev_hit_coords[2], int *CPU_prevhit)
{
    // Declarations
    int i, j, k, shipSize;
    int randSign;
    int x, y, y_in;
    int shipCnt = 0;
    int tempx, tempy;
    char x_in = '0';
    char abandonShip = 'n';
    char coord_found;

    ship *tempShip, *prevShip, *currentShip;
    shipGrid *tempGrid, *currentGrid;

    // Sets Head ship
    currentShip = opponents_Ships;
    
    // Advance past first NULL head ship
    tempShip = currentShip->shipPtr;
    currentShip = tempShip;
    
    
    // Case 1: Players turn
    if (CPU_Turn == 'n')
    {
        // Obtains attack coordinates from user
        printf("Choose attack coordinate (e.g., A5): ");
        scanf(" %c%d", &x_in, &y_in);
        x = LetterToNum(x_in);
        y = y_in - 1;
        
        // Checks for shot going off of the board
        if ((x > 10) || (x < 0) || (y > 10) || (y < 0))
        {
            printf("\n**Shot went off the board! Watch your aim.\n");
            return;
        }
    }
    
    // Case 2: CPU's turn
    else
    {
        // Case 1: CPU Not implementing tracking algorithm
        if (*CPU_prevhit == 0)
        {
            // Stays in loop until random shot hits empty spot
            while (1)
            {
                x = (rand() % 10);
                y = (rand() % 10);
                if (Board_minor[y][x] == ' ')
                {
                    break;
                }
            }
        }
        
        // Case 2: Previous shots hit, implementing search algorithm to find ship
        else if (*CPU_prevhit == 1)
        {
            if (firstHit == 'y')
            {
                origX = CPU_prev_hit_coords[0];
                origY = CPU_prev_hit_coords[1];
            }
            
            tempx = CPU_prev_hit_coords[0];
            tempy = CPU_prev_hit_coords[1];
            
            
            while (1)
            {
                // Expands range after certain number of failed tries around a given point
                if (tryCnt > 7)
                {
                    tryCnt = 0;
                    range++;
                }
                
                // Determines which case will randomly be chosen.
                randSign = rand() % 8;
                
                // Cases of possible movement directions
                if (randSign == 0)
                {
                    x = tempx;
                    y = tempy + range;
                }
                else if (randSign == 1)
                {
                    x = tempx + range;
                    y = tempy;
                }
                else if (randSign == 2)
                {
                    x = tempx + range;
                    y = tempy + range;
                }
                else if (randSign == 3)
                {
                    x = tempx;
                    y = tempy - range;
                }
                else if (randSign == 4)
                {
                    x = tempx - range;
                    y = tempy;
                }
                else if (randSign == 5)
                {
                    x = tempx - range;
                    y = tempy - range;
                }
                else if (randSign == 6)
                {
                    x = tempx + range;
                    y = tempy - range;
                }
                else if (randSign == 7)
                {
                    x = tempx - range;
                    y = tempy + range;
                }
                
                // Ensures randomly generated number is on the board. Disallows jumps to other side of board via pacman effect (torus geometry?)
                if ( ((x >= 0) && (x < 10)) && ((y >= 0) && (y < 10)) )
                {
                    if (Board_minor[y][x] == ' ')
                    {
                        break;
                    }
                    else
                    {
                        tryCnt++;
                    }
                }
            }
        }
    }
    
    
    // Scan Opponent_Board_Major to see if hit
    // Case 1: Miss
    if (Opponent_Board_Major[y][x] == ' ')
    {
        printf("\nMiss!");
        // Update boards for misses
        Opponent_Board_Major[y][x] = 'X';
        Board_minor[y][x] = 'X';
        scoreCard->misses++;
    }
    
    
    // Case 2: Hit previously hit target
    else if (Opponent_Board_Major[y][x] == 'H')
    {
        scoreCard->misses++;
        printf("You've already hit that target!");
    }
    
    // Case 3: Hit previously missed target
    else if (Opponent_Board_Major[y][x] == 'X')
    {
        scoreCard->misses++;
        printf("Miss, again.");
    }
    
    // Case 4: Hit
    else if (Opponent_Board_Major[y][x] != ' ')
    {
        
        // Computer tracks hit to update next hit
        if (CPU_Turn == 'y')
        {
            if (*CPU_prevhit == 0)
            {
                firstHit = 'y';
            }
            else
            {
                firstHit = 'n';
            }
            *CPU_prevhit = 1;
            range = 1;
            tryCnt = 0;
            CPU_prev_hit_coords[0] = x;
            CPU_prev_hit_coords[1] = y;
        }
        // Update boards for hits
        Opponent_Board_Major[y][x] = 'H';
        Board_minor[y][x] = 'H';
        scoreCard->hits++;
        
        i = 0;
        
        coord_found = 'n';
        if (CPU_Turn == 'n')
        {
            if (ComputerShipsCounter == 1)
            {
                shipCnt = ComputerShipsCounter + 1;
            }
            else
            {
                shipCnt = ComputerShipsCounter;
            }
        }
        else if (CPU_Turn == 'y')
        {
            if (PlayerShipsCounter == 1)
            {
                shipCnt = PlayerShipsCounter + 1;
            }
            else
            {
                shipCnt = PlayerShipsCounter;
            }
        }
        
        while (coord_found == 'n')
        {
            // Looks through each ship in linked list
            for (k = 0; k < shipCnt-1; k++)
            {
                if (i > 0)
                {
                    prevShip = currentShip;
                    tempShip = currentShip->shipPtr;
                    currentShip = tempShip;
                }
                
                shipSize = currentShip->shipsize;
                
                currentGrid = currentShip->shipGridPtr;
                
                // Scans through each ship's parts in linked list
                for (j = 0; j < shipSize; j++)
                {
                    // Case 1: Finds part of ship that is hit
                    if ((currentGrid->X_loc == x) && (currentGrid->Y_loc == y))
                    {
                        coord_found = 'y';
                        //printf("\n****HP: %d\n", currentShip->hitPts);
                        currentShip->hitPts--;
                        //printf("\n****HP: %d\n", currentShip->hitPts);
                        // Case 1: Ship is Sunk
                        if (currentShip->hitPts == 0)
                        {
                            printf("\n\n***%s sunk!***\n", currentShip->shiptype);
                            scoreCard->sunk++;
                            currentShip->sunk = 'y';
                            
                            RemoveShip(opponents_Ships);
                            
                            if (CPU_Turn == 'n')
                            {
                                ComputerShipsCounter--;
                            }
                            else
                            {
                                *CPU_prevhit = 0;
                                PlayerShipsCounter--;
                            }
                            
                            return;
                        }
                        
                        // Case: Ship equal to or larger than size 3, only 1 hit point remaining, and asks if user wants to abandon ship
                        else if ((currentShip->shipsize >= 3) && (currentShip->hitPts == 1) && CPU_Turn == 'y')
                        {
                            while (1)
                            {
                                printf("Your ship has 1 hit left until it is sunk!\n");
                                printf("Would you like to abandon ship? (y/n) ");
                                scanf(" %c", &abandonShip);
                                if (abandonShip == 'y')
                                {
                                    AbandonShip(currentShip, scoreCard, opponents_Ships, CPU_prevhit, Opponent_Board_Major);
                                    return;
                                }
                                
                                else if (abandonShip == 'n')
                                {
                                    printf("Good luck!\n");
                                    return;
                                }
                                else
                                {
                                    printf("Does not computer. Try again!\n");
                                }
                            }
                        }
                    }
                    
                    // Advance to next position of ship in linked list to check for match
                    tempGrid = currentGrid->shipGridPtr;
                    currentGrid = tempGrid;
                }
                printf("\n");
                i++;
            }
        }
        
    }
    
    return;
}


/*
 * Function: AbandonShip
 * ---------------------
 * Enables the user to abandon ship
 *
 */

void AbandonShip(ship *currentShip, score *scoreCard, ship *opponents_Ships, int *CPU_prevhit, char Opponent_Board_Major[10][10])
{
    // Declarations
    shipGrid *tempGrid, *currentGrid;
    int k, new_x, new_y;
    
    // Update hit and sunk values
    currentShip->hitPts--;
    scoreCard->sunk++;
    currentShip->sunk = 'y';
    
    // Abandon ship, causing it to sink
    RemoveShip(opponents_Ships);
    
    // Removes computer algorithm from searching for ship, because the ship is now sunk.
    *CPU_prevhit = 0;
    
    // Decrements the player ships counter
    PlayerShipsCounter--;
    
    // Update board board
    currentGrid = currentShip->shipGridPtr;
    while (currentGrid->shipGridPtr != NULL)
    {
        // Update game board to Hits
        Opponent_Board_Major[currentGrid->Y_loc][currentGrid->X_loc] = 'H';
        
        // Iterate to next item in list
        tempGrid = currentGrid->shipGridPtr;
        currentGrid = tempGrid;
    }
    
    
    for (k = 0; k < 2; k++)
    {
        // Calculate new positions for kayak deployment
        
        new_x = (rand() % 10);
        new_y = (rand() % 10);
        
        
        // Case 1: Kayak successfully deployed
        if (Opponent_Board_Major[new_y][new_x] == ' ')
        {
            AddShip(opponents_Ships, 2, new_x, new_y);
            Opponent_Board_Major[new_y][new_x] = 'K';
        }
        // Case 2: Kayak deployed to bad position and destroyed.
        else if (Opponent_Board_Major[new_y][new_x] != ' ')
        {
            if (Opponent_Board_Major[new_y][new_x] != 'H')
            {
                printf("Kayak %d smashed into a flaming ship and was destroyed!\n", k+1);
            }
            else if (Opponent_Board_Major[new_y][new_x] != 'X')
            {
                printf("Kayak %d crashed into the debris of your enemy's missed attack and was destroyed!\n", k+1);
            }
            else
            {
                printf("Kayak %d crashed into one of your other ships and was destroyed!\n", k+1);
            }
        }
    }
    return;
}







/*
 * Function: RemoveShip
 * --------------------
 * Remove ship from linked list
 *
 */

void RemoveShip(ship *Ships)
{
    // Declarations
    int i;
    ship *prevShip, *tempShip, *currentShip;
    
    // Set head ship and move to start of linked list
    currentShip = Ships;
    prevShip = currentShip;
    tempShip = currentShip->shipPtr;
    currentShip = tempShip;
    
    i = 0;
    while (1)
    {
        // Enables tracking of previous ship in list
        if (i > 0)
        {
            prevShip = currentShip;
            tempShip = currentShip->shipPtr;
            currentShip = tempShip;
            
        }
        
        // Found sunk ship
        if (currentShip->sunk == 'y')
        {
            tempShip = currentShip->shipPtr;
            prevShip->shipPtr = tempShip;
            
            return;
        }
        i++;
    }
    
}



/*
 * Function: AddShip
 * -----------------
 * Adds ship to linked list
 *
 */

void AddShip(ship *Ships, int num, int x, int y)
{
    // Declarations
    ship *newShip, *tempShip, *currentShip;
    shipGrid *newNode;
    
    // Update playership counter
    PlayerShipsCounter++;
    
    // Creates new ship
    newShip = (ship *)malloc(sizeof(ship));
    
    // Create information for kayak
    newShip->shipsize = 1;
    newShip->hitPts = 1;
    newShip->shiptypeID = 'K';
    strcpy(newShip->shiptype, "Kayak");
    
    // Create ship hit grid
    newNode = (shipGrid *)malloc(sizeof(shipGrid));
    newShip->shipGridPtr = newNode;
    newNode->X_loc = x;
    newNode->Y_loc = y;
    newNode->shipGridPtr = NULL;
    
    // Avances to next ship in linked list
    currentShip = Ships;
    tempShip = currentShip->shipPtr;
    currentShip = tempShip;
    
    // Searches for last node in linked list
    while (1)
    {
        tempShip = currentShip->shipPtr;
        
        if (tempShip == NULL)
        {
            currentShip->shipPtr = newShip;
            break;
        }
        currentShip = tempShip;
    }
    return;
}