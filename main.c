/*
In this program, we will use the C programming language to create a playable chess game in the DE1-SoC.

The game will be played on a chess board. The board will be represented by an 8 by 8 2D array of structs.
Those structs will contain the piece type, the color of the piece, and if a square is highlighted.
*/


#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


// Defines the ids for the pieces
typedef int PieceIdx;
#define EMPTY_SQUARE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6


// DE1-SOC FPGA devices base address
const int SDRAM_BASE            =0xC0000000;
const int FPGA_ONCHIP_BASE      =0xC8000000;
const int FPGA_CHAR_BASE        =0xC9000000;
const int LEDR_BASE             =0xFF200000;
const int HEX3_HEX0_BASE        =0xFF200020;
const int HEX5_HEX4_BASE        =0xFF200030;
const int SW_BASE               =0xFF200040;
const int KEY_BASE              =0xFF200050;
const int TIMER_BASE            =0xFF202000;
const int PIXEL_BUF_CTRL_BASE   =0xFF203020;
const int CHAR_BUF_CTRL_BASE    =0xFF203030;


// VGA colors
const int WHITE                 =0xFFFF;
const int BLACK                 =0x0000;
const int YELLOW                =0xFFE0;
const int RED                   =0xF800;
const int GREEN                 =0x07E0;
const int BLUE                  =0x001F;
const int CYAN                  =0x07FF;
const int MAGENTA               =0xF81F;
const int GREY                  =0xC618;
const int PINK                  =0xFC18;
const int ORANGE                =0xFC00;


// Resolution for the DE1-SoC VGA display
const int RESOLUTION_X          =320;
const int RESOLUTION_Y          =240;


// Global constants for the chess board
const int BOARD_SIZE            = 8;
const int SQUARE_SIZE           = 30;
const int WHITE_PIECE           = 1;
const int BLACK_PIECE           = 0;
const int WHITE_SQUARE          = 1;
const int BLACK_SQUARE          = 0;
const int EMPTY_PIECE           =-1;
	

// location of the pixel buffer in SDRAM
volatile int pixel_buffer_start;



//Piece struct holds information about a piece
// colour is the color of the piece (white or black)
// piece_ID is the id of the piece
typedef struct Piece
{
    short int colour;
    PieceIdx piece_ID;
} Piece;


//GridSquare struct holds information about a square on the chess board
typedef struct GridSquare
{
    //current piece on the square
    Piece piece;

    //colour of the square
    int colour;

    //Determines if square should be highlighted when drawn
    //Highlighted squares are used to show where a piece can move
    int highlighted;

    //Determines if square should be outlined when drawn
    //Outlined squares are used to show current selected square
    int outlined;
} GridSquare;


// Function prototypes for drawing to the VGA display
/////////////////////////////////////////////////////////////////////

//Plots a single pixel at the given coordinates with the given color
void plot_pixel(int x, int y, short int line_colour);

//Clears the screen with the colour black
void clear_screen();

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Draws background outline of the chess board
void draw_outline(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Synchronizes the double buffering of the VGA display
void wait_for_vsync();

//Draws all pieces on the chess board
void draw_pieces(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Draws a single piece on the chess board
void draw_piece(Piece piece, int xCoord, int yCoord);

//Draws a single square on the chess board
void draw_square(GridSquare square, int xCoord, int yCoord);

//Draws a pawn on the chess board
void draw_pawn(int piece_colour, int xCoord, int yCoord);

//Draws a knight on the chess board
void draw_knight(int piece_colour, int xCoord, int yCoord);

//Draws a bishop on the chess board
void draw_bishop(int piece_colour, int xCoord, int yCoord);

//Draws a rook on the chess board
void draw_rook(int piece_colour, int xCoord, int yCoord);

//Draws a queen on the chess board
void draw_queen(int piece_colour, int xCoord, int yCoord);

//Draws a king on the chess board
void draw_king(int piece_colour, int xCoord, int yCoord);

//Displays the winner of the game
void display_winner(int winner);

/////////////////////////////////////////////////////////////////////


// Function prototypes for the chess game
/////////////////////////////////////////////////////////////////////

//Enters chess game loop
void play_chess();

//Initializes the chess board to default state
void init_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes highlights
void init_highlights(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes colours of the chess grid
void init_colours(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes piece types in board
void init_pieces(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes empty squares in board
void init_empty_squares(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes backrank pieces given the board, colour and yCoord
void init_backrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord);

//Initializes frontrank pieces given the board, colour and yCoord
void init_frontrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord);

//get valid moves for a piece
int * get_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord);

//Checks if a move is valid
bool is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn);

//Checks if square is empty
bool is_empty_square(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord);

//Gets player selected piece from user input
//Returns x and y indexes of the chess board array of the piece selected
//If the input is invalid, loop until valid input is given
int * get_selected_piece(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Gets player move from user input
//Returns x and y indexes of the chess board array of the square selected
//If the input is invalid, loop until valid input is given
int * get_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int startingLocationX, int startingLocationY, int currentTurn);

//Plays a turn of the game
void play_turn(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Switches turns
void switch_turns(int * currentTurn);

//Determines if the game is over
bool is_game_over(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Determines the winner of the game
int get_winner(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

/////////////////////////////////////////////////////////////////////


// Function prototypes for helper functions
/////////////////////////////////////////////////////////////////////

//convert x index to x pixel coordinate
int x_to_pixel(int xCoord);

//convert y index to y pixel coordinate
int y_to_pixel(int yCoord);

//gets inputs from switches
int* get_input_from_switches();

/////////////////////////////////////////////////////////////////////





int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
    
    // declares the chessBoard
	GridSquare chessBoard[BOARD_SIZE][BOARD_SIZE];
    int currentTurn = WHITE_PIECE;
	
	//initializes the chessBoard to default values
    init_board(chessBoard);
	

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = FPGA_ONCHIP_BASE; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = SDRAM_BASE;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    //loop while game is not over and play game
    while (!is_game_over(chessBoard, currentTurn)){
        //Draws the chess board
        draw_board(chessBoard);

        //Plays a turn of the game
        play_turn(chessBoard, currentTurn);

        //Switches the turn
        switch_turns(&currentTurn);
    }

    //Draws the chess board one last time
    draw_board(chessBoard);

    //Determine the winner of the game
    int winner = get_winner(chessBoard, currentTurn);

    //Displays the winner of the game
    display_winner(winner);
}


// Function definitions for drawing to the VGA display
/////////////////////////////////////////////////////////////////////

//Plots a single pixel at the given coordinates with the given color
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

//Clears the screen with the colour black
void clear_screen()
{
        int y, x;

        // loop through every pixel and set it to zero (black)
        for (x = 0; x < RESOLUTION_X; x++)
                for (y = 0; y < RESOLUTION_Y; y++)
                        plot_pixel (x, y, 0);
}

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]){
	
    //Draws outline of the chess board
    draw_outline(board);

    //Draws the pieces on the chess board
    draw_pieces(board);

}

//Draws background outline of the chess board
void draw_outline(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Loops through each square on the chess board and draws the square
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++){
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++){
            draw_square(board[yCoord][xCoord], yCoord, xCoord);
        }
    }
}

//Synchronizes the double buffering of the VGA display
void wait_for_vsync(){
	
    volatile int *pixel_ctrl_ptr = (int *) PIXEL_BUF_CTRL_BASE;
	int status;
	
    //launches the swap process
	*pixel_ctrl_ptr = 1; //sets the S bit to 1 
	
    //poll for the status bit
	status = *(pixel_ctrl_ptr + 3); //OxFF20302C
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}
	
}

//Draws all pieces on the chess board
void draw_pieces(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Loops through chess board and draws all pieces
    for (int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for (int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            draw_piece(board[yCoord][xCoord].piece, yCoord, xCoord);
        }
    }
}

//Draws a single piece on the chess board
void draw_piece(Piece piece, int xCoord, int yCoord) {
    
    //if there is no piece, do nothing
    if (piece.piece_ID == EMPTY_SQUARE) {
        return;
    }

    //Checks to see which piece to draw and draws it
    if (piece.piece_ID == PAWN) {
        draw_pawn(piece.colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == ROOK) {
        draw_rook(piece.colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == KNIGHT) {
        draw_knight(piece.colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == BISHOP) {
        draw_bishop(piece.colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == QUEEN) {
        draw_queen(piece.colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == KING) {
        draw_king(piece.colour, xCoord, yCoord);
    }
}

//Draws a single square on the chess board
void draw_square(GridSquare square, int xCoord, int yCoord) {
    
    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord);
    int startingPixelCoordY = y_to_pixel(yCoord);

    //Draws the square
    for (int pixelCoordX = startingPixelCoordX; pixelCoordX < startingPixelCoordX + SQUARE_SIZE; pixelCoordX++) {
        for (int pixelCoordY = startingPixelCoordY; pixelCoordY < startingPixelCoordY + SQUARE_SIZE; pixelCoordY++) {
            plot_pixel(pixelCoordX, pixelCoordY, square.colour);
        }
    }
}

//Draws a pawn on the chess board
void draw_pawn(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Draws a knight on the chess board
void draw_knight(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Draws a bishop on the chess board
void draw_bishop(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Draws a rook on the chess board
void draw_rook(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Draws a queen on the chess board
void draw_queen(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Draws a king on the chess board
void draw_king(int piece_colour, int xCoord, int yCoord) {
    return ;
}

//Displays the winner of the game
void display_winner(int winner) {
    return ;
}

/////////////////////////////////////////////////////////////////////


// Function definitions for the chess game
/////////////////////////////////////////////////////////////////////

//Enters chess game loop
void play_chess() {
    return ;
}

//Initializes the chess board to default state
void init_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Initialize highlights
    init_highlights(board);

    //Initialize pieces
    init_pieces(board);

    //Initialize empty squares
    init_empty_squares(board);

}

//Initializes highlights
void init_highlights(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Loops through the board and sets all highlights to false
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            board[yCoord][xCoord].highlighted = false;
        }
    }
}

//Initializes colours of the chess grid
void init_colours(GridSquare board[BOARD_SIZE][BOARD_SIZE]){

    //Loops through all squares and sets the colour of the square
    //Colour of the square is in a checkerboard pattern
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            if((xCoord + yCoord) % 2 == 0) {
                board[yCoord][xCoord].colour = WHITE_SQUARE;
            }
            else {
                board[yCoord][xCoord].colour = BLACK_SQUARE;
            }
        }
    }
}

//Initializes piece types in board
void init_pieces(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Initializes backrank pieces for white
    init_backrank(board, WHITE_PIECE, BOARD_SIZE-1);

    //Initializes frontrank pieces for white
    init_frontrank(board, WHITE_PIECE, BOARD_SIZE-2);

    //Initializes backrank pieces for black
    init_backrank(board, BLACK_PIECE, 0);

    //Initializes frontrank pieces for black
    init_frontrank(board, BLACK_PIECE, 1);

}

//Initializes empty squares in board
void init_empty_squares(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {

    //Sets constants for the empty squares
    const int EMPTY_SPACE_BEGIN = 2;
    const int EMPTY_SPACE_END = BOARD_SIZE - 3;

    //Loops through the empty squares in the board
    //Sets the piece type to EMPTY_SQUARE and piece color to EMPTY_PIECE
    for(int yCoord = EMPTY_SPACE_BEGIN; yCoord <= EMPTY_SPACE_END; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            board[yCoord][xCoord].piece.piece_ID = EMPTY_SQUARE;
            board[yCoord][xCoord].piece.colour = EMPTY_PIECE;
        }
    } 
}


//Initializes backrank pieces given the board, colour and yCoord
void init_backrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord) {
    
    //Manually set pieces to starting position with corresponding colour
    //Set up rook
    board[yCoord][0].piece.piece_ID = ROOK;
    board[yCoord][0].piece.colour = colour;

    //Set up knight
    board[yCoord][1].piece.piece_ID = KNIGHT;
    board[yCoord][1].piece.colour = colour;

    //Set up bishop
    board[yCoord][2].piece.piece_ID = BISHOP;
    board[yCoord][2].piece.colour = colour;

    //Set up queen
    board[yCoord][3].piece.piece_ID = QUEEN;
    board[yCoord][3].piece.colour = colour;

    //Set up king
    board[yCoord][4].piece.piece_ID = KING;
    board[yCoord][4].piece.colour = colour;

    //Set up bishop
    board[yCoord][5].piece.piece_ID = BISHOP;
    board[yCoord][5].piece.colour = colour;

    //Set up knight
    board[yCoord][6].piece.piece_ID = KNIGHT;
    board[yCoord][6].piece.colour = colour;

    //Set up rook
    board[yCoord][7].piece.piece_ID = ROOK;
    board[yCoord][7].piece.colour = colour;
}

//Initializes frontrank pieces given the board, colour and yCoord
void init_frontrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord) {
    
    //Loops through row at yCoord and set pieces to pawns of the correspending colour
    for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
        board[yCoord][xCoord].piece.piece_ID = PAWN;
        board[yCoord][xCoord].piece.colour = colour;
    }
}

//get valid moves for a piece
int * get_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord) {
    return ;
}

//Checks if a move is valid
bool is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn) {
    return ;
}

//Checks if square is empty
bool is_empty_square(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord) {

    //Checks if the square is empty
    if(board[yCoord][xCoord].piece.piece_ID == EMPTY_SQUARE) {
        return true;
    }
    
    return false;

}

//Gets player selected piece from user inpu
//Returns x and y indexes of the chess board array of the piece selected
//If the input is invalid, loop until valid input is given
int * get_selected_piece(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {

    //Initialize variables
    int * selectedPiece = malloc(sizeof(int) * 2);
    int xCoord = 0;
    int yCoord = 0;

    //Loop until valid input is given
    while(true) {
        
        int* userInput = get_input_from_switches();

        //Get x and y coordinates of the piece selected
        xCoord = userInput[0];
        yCoord = userInput[1];

        //Check if the piece selected is valid
        if(!is_empty_square(board, xCoord, yCoord)) {
            selectedPiece[0] = xCoord;
            selectedPiece[1] = yCoord;
            break;
        }
    }

    return selectedPiece;

}

//Gets player move from user input
//Returns x and y indexes of the chess board array of the square selected
//If the input is invalid, loop until valid input is given
int * get_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int startingLocationX, int startingLocationY, int currentTurn) {

    //Initialize variables
    int * move = malloc(sizeof(int) * 2);
    int xCoord = 0;
    int yCoord = 0;

    //Loop until valid input is given
    while(true) {
        
        int* userInput = get_input_from_switches();

        //Get x and y coordinates of the square selected
        xCoord = userInput[0];
        yCoord = userInput[1];

        //Check if the move is valid
        if(is_valid_move(board, startingLocationX, startingLocationY, xCoord, yCoord, currentTurn)) {
            move[0] = xCoord;
            move[1] = yCoord;
            break;
        }
    }

    return move;
}

//Plays a turn of the game
void play_turn(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {
    return ;
}

//Switches turns
void switch_turns(int * currentTurn) {

    //Switch turns
    if(*currentTurn == WHITE_PIECE) {
        *currentTurn = BLACK_PIECE;
    } else {
        *currentTurn = WHITE_PIECE;
    }

}

//Determines if the game is over
bool is_game_over(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {
    return ;
}

//Determines the winner of the game
int get_winner(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {
    return ;
}

/////////////////////////////////////////////////////////////////////


// Function prototypes for helper functions
/////////////////////////////////////////////////////////////////////

//convert x index to x pixel coordinate
//pixel index will map to the top left most pixel of the square
int x_to_pixel(int xCoord){
    return xCoord * SQUARE_SIZE;
}

//convert y index to y pixel coordinate
//pixel index will map to the top left most pixel of the square
int y_to_pixel(int yCoord){
    return yCoord * SQUARE_SIZE;
}

//gets inputs from switches
int* get_input_from_switches() {
    return ;
}

/////////////////////////////////////////////////////////////////////