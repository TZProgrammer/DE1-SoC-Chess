/*
In this program, we will use the C programming language to create a playable chess game in the DE1-SoC.

The game will be played on a chess board. The board will be represented by an 8 by 8 2D array of structs.
Those structs will contain the piece type, the color of the piece, and if a square is highlighted.
*/


#include <stdlib.h>
#include <time.h>
#include <math.h>
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
int  SDRAM_BASE            = 0xC0000000;
int  FPGA_ONCHIP_BASE      = 0xC8000000;
int  FPGA_CHAR_BASE        = 0xC9000000;
int* LEDR_BASE             = (int*)0xFF200000;
int* HEX3_HEX0_BASE        = (int*)0xFF200020;
int* HEX5_HEX4_BASE        = (int*)0xFF200030;
int* SW_BASE               = (int*)0xFF200040;
int* KEY_BASE              = (int*)0xFF200050;
int* TIMER_BASE            = (int*)0xFF202000;
int* PIXEL_BUF_CTRL_BASE   = (int*)0xFF203020;
int* CHAR_BUF_CTRL_BASE    = (int*)0xFF203030;


// VGA colors
const int WHITE_SOFT            =0xeeeed2;
const int WHITE                 =0xffff;
const int BLACK                 =0x0000;
const int YELLOW                =0xFFE0;
const int RED                   =0xF800;
const int GREEN                 =0x769656;
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
const int SQUARE_BORDER_SIZE    = 2;
const int WHITE_PIECE           = 1;
const int BLACK_PIECE           = 0;
const int WHITE_SQUARE_COLOUR   =0xeeeed2;
const int BLACK_SQUARE_COLOUR   =0x769656;
const int EMPTY_PIECE           =-1;
const int STALEMATE             =-1;
	

// location of the pixel buffer in SDRAM
volatile int  pixel_buffer_start;
volatile int *pixel_ctrl_ptr = (int*)0xFF203020;



//Piece struct holds information about a piece
typedef struct Piece
{

    //Colour of the piece
    short int colour;

    //ID of the piece (Determines which piece it is or if it is empty)
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
    bool highlighted;

    //Determines if square should be outlined when drawn
    //Outlined squares are used to show current selected square
    bool outlined;
} GridSquare;


// Function prototypes for drawing to the VGA display
/////////////////////////////////////////////////////////////////////

//Plots a single pixel at the given coordinates with the given color
void plot_pixel(int x, int y, short int line_colour);

//Clears the screen with the colour black
void clear_screen();

//Set pixel buffer addresses
void set_pixel_buffer_addresses();

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Draws background outline of the chess board
void draw_squares(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Synchronizes the double buffering of the VGA display
void wait_for_vsync();

//Draws all pieces on the chess board
void draw_pieces(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Draws a single piece on the chess board
void draw_piece(Piece piece, int xCoord, int yCoord);

//Draws a single square on the chess board
void draw_square(GridSquare square, int xCoord, int yCoord);

//Draws a pawn on the chess board
void draw_pawn(int pieceColour, int xCoord, int yCoord);

//Draws a knight on the chess board
void draw_knight(int pieceColour, int xCoord, int yCoord);

//Draws a bishop on the chess board
void draw_bishop(int pieceColour, int xCoord, int yCoord);

//Draws a rook on the chess board
void draw_rook(int pieceColour, int xCoord, int yCoord);

//Draws a queen on the chess board
void draw_queen(int pieceColour, int xCoord, int yCoord);

//Draws a king on the chess board
void draw_king(int pieceColour, int xCoord, int yCoord);

//Displays the winner of the game
void display_winner(int winner);

//Displays 1 in the HEX display
void display_white();

//Displays -1 in the HEX display
void display_black();

//Displays 0 in the HEX display
void display_draw();

/////////////////////////////////////////////////////////////////////


// Function prototypes for drawing primitives to the VGA display
/////////////////////////////////////////////////////////////////////

//Draws a square with the left top corner at (x,y) with size (size) and colour (colour)
void draw_square_primitive(int xPixelCoord, int yPixelCoord, int size, short int colour);

//Draws a rectangle with the left top corner at (x,y) with size (width,height) and colour (colour)
void draw_rectangle_primitive(int xPixelCoord, int yPixelCoord, int width, int height, short int colour);

//Draws a circle with the center at (x,y) with radius (radius) and colour (colour)
void draw_circle_primitive(int xPixelCoord, int yPixelCoord, int radius, short int colour);

/////////////////////////////////////////////////////////////////////

// Function prototypes for the chess game
/////////////////////////////////////////////////////////////////////

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

//Initializes outlines of the chess grid
void init_outlines(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes backrank pieces given the board, colour and yCoord
void init_backrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord);

//Initializes frontrank pieces given the board, colour and yCoord
void init_frontrank(GridSquare board[BOARD_SIZE][BOARD_SIZE], int colour, int yCoord);

//highlights valid moves for a piece at square xCoord, yCoord
void highlight_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord, int currentTurn);

//Checks if a move is valid
bool is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn);

//Checks if a move is valid without check
bool is_valid_move_without_check(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn);

//Checks if it is a valid pawn move
bool is_valid_pawn_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn);

//Checks if it is a valid knight move
bool is_valid_knight_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Checks if it is a valid bishop move
bool is_valid_bishop_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Checks if it is a valid rook move
bool is_valid_rook_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Checks if it is a valid queen move
bool is_valid_queen_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Checks if it is a valid king move
bool is_valid_king_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Checks if a piece has valid moves
bool has_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord, int currentTurn);

//Checks if king is in check
bool is_in_check(GridSquare board[BOARD_SIZE][BOARD_SIZE], int pieceColour);

//Checks if square is empty
bool is_empty_square(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoord, int yCoord);

//Gets player selected piece from user input
//Returns x and y indexes of the chess board array of the piece selected
//If the input is invalid, loop until valid input is given
int * get_selected_piece_location(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Gets player move from user input
//Returns x and y indexes of the chess board array of the square selected
//If the input is invalid, loop until valid input is given
int * get_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int startingLocationX, int startingLocationY, int currentTurn);

//Plays a turn of the game
void play_turn(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Move a piece from one square to another
void move_piece(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

//Switches turns
void switch_turns(int * currentTurn);

//Determines if the game is over
bool is_game_over(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Determines the winner of the game
int get_winner(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Checks if game ended in stalemate
bool is_stalemate(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

//Checks if the game is in checkmate
bool is_checkmate(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn);

/////////////////////////////////////////////////////////////////////


// Function prototypes for helper functions
/////////////////////////////////////////////////////////////////////

//convert x index to x pixel coordinate
int x_to_pixel(int xCoord);

//convert y index to y pixel coordinate
int y_to_pixel(int yCoord);

//gets inputs from switches
int* get_input_from_switches();

//Copy board to another board
void copy_board(GridSquare board[BOARD_SIZE][BOARD_SIZE], GridSquare copyBoard[BOARD_SIZE][BOARD_SIZE]);

//Gets king position
int* get_king_position(GridSquare board[BOARD_SIZE][BOARD_SIZE], int pieceColour);

/////////////////////////////////////////////////////////////////////





int main(void)
{

    //Declares the chessBoard
	GridSquare chessBoard[BOARD_SIZE][BOARD_SIZE];

    //Initializes initial turn to white
    int currentTurn = WHITE_PIECE;
	
	//Initializes the chessBoard to default values
    init_board(chessBoard);
	
    //Initializes pixel buffer addresses
    set_pixel_buffer_addresses();
    

    //Loop while game is not over and play game
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

    return 0;
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

//Set pixel buffer addresses
void set_pixel_buffer_addresses(){

    //Set front pixel buffer to start of FPGA On-chip memory
    *(pixel_ctrl_ptr + 1) = FPGA_ONCHIP_BASE;

    //Swap the front/back buffers, to set the front buffer location
    wait_for_vsync();

    //Initialize a pointer to the pixel buffer, used by drawing functions
    pixel_buffer_start = *pixel_ctrl_ptr;

    //Reset screen to black on this buffer
    clear_screen();

    //Set back pixel buffer to start of SDRAM memory
    *(pixel_ctrl_ptr + 1) = SDRAM_BASE;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);

    //Clear screen on back buffer
    clear_screen();

}

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]){
	
    //Draws outline of the chess board
    draw_squares(board);

    //Draws the pieces on the chess board
    draw_pieces(board);

    //Swaps the front and back buffers
    wait_for_vsync();
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);
}

//Draws background outline of the chess board
void draw_squares(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {
    
    //Loops through each square on the chess board and draws the square
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++){
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++){
            draw_square(board[yCoord][xCoord], xCoord, yCoord);
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
            draw_piece(board[yCoord][xCoord].piece, xCoord, yCoord);
        }
    }
}

//Draws a single piece on the chess board
void draw_piece(Piece piece, int xCoord, int yCoord) {
    
    //if there is no piece, do nothing
    if (piece.piece_ID == EMPTY_SQUARE) {
        return;
    }

    //Determines colour of piece
    short int colour;
    if (piece.colour == WHITE_PIECE) {
        colour = WHITE;
    } else {
        colour = BLACK;
    }



    //Checks to see which piece to draw and draws it
    if (piece.piece_ID == PAWN) {
        draw_pawn(colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == ROOK) {
        draw_rook(colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == KNIGHT) {
        draw_knight(colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == BISHOP) {
        draw_bishop(colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == QUEEN) {
        draw_queen(colour, xCoord, yCoord);
    }
    else if (piece.piece_ID == KING) {
        draw_king(colour, xCoord, yCoord);
    }
}

//Draws a single square on the chess board
void draw_square(GridSquare square, int xCoord, int yCoord) {
    
    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord);
    int startingPixelCoordY = y_to_pixel(yCoord);

    //Draws the background of the square in black if it's not outlined and in magenta if it is
    if (square.outlined == 0) 
        draw_square_primitive(startingPixelCoordX, startingPixelCoordY, SQUARE_SIZE, BLACK);
    else 
        draw_square_primitive(startingPixelCoordX, startingPixelCoordY, SQUARE_SIZE, MAGENTA);



    //Draws the square foreground with it's colour depending on the highlighted status
    if (square.highlighted == 0) 
        draw_square_primitive(startingPixelCoordX + SQUARE_BORDER_SIZE, startingPixelCoordY + SQUARE_BORDER_SIZE, SQUARE_SIZE - SQUARE_BORDER_SIZE*2, square.colour);
    else 
        draw_square_primitive(startingPixelCoordX + SQUARE_BORDER_SIZE, startingPixelCoordY + SQUARE_BORDER_SIZE, SQUARE_SIZE - SQUARE_BORDER_SIZE*2, YELLOW);
}

//Draws a pawn on the chess board
void draw_pawn(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/2-4;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*3;

	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 4,2, pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+2, 8,4, pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+6, 4,4, pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+10, 8,4, pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-6, startingPixelCoordY+14, 16,4, pieceColour);

}

//Draws a knight on the chess board
void draw_knight(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/2-8;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*3;

	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 2,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+5, startingPixelCoordY, 2,1,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+1, 10,1,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+2, 11,1,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+3, 7,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+8, startingPixelCoordY+3, 4,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+4, 7,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+8, startingPixelCoordY+4, 5,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+5, 15,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+6, 15,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+7, 15,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+14, startingPixelCoordY+8, 3,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+14, startingPixelCoordY+9, 3,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+8, 15,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+1, startingPixelCoordY+10, 15,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+11, 10,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+1, startingPixelCoordY+14, 11,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+15, 13,1,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+16, 14,3,pieceColour);
}

//Draws a bishop on the chess board
void draw_bishop(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/2-2;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*3;

	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 4,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+2, 8,3, pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-6, startingPixelCoordY+6, 16,4,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+10, 8,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+14, 4,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-6, startingPixelCoordY+16, 16,3,pieceColour);
}

//Draws a rook on the chess board
void draw_rook(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/4;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*3;

	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 3,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+6, startingPixelCoordY, 3,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+12, startingPixelCoordY, 3,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+3, 15,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX+3, startingPixelCoordY+6,9,9,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+15, 15,3,pieceColour);
	
}

//Draws a queen on the chess board
void draw_queen(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/2-2;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*3;

    draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 4,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-4, startingPixelCoordY, 2,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX+6, startingPixelCoordY, 2,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-3, startingPixelCoordY+2, 10,1,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+3, 8,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+5, 4,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+7, 4,6,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+13, 8,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-3, startingPixelCoordY+15, 10,2,pieceColour);
    draw_rectangle_primitive(startingPixelCoordX-4, startingPixelCoordY+17, 12,2,pieceColour);

}

//Draws a king on the chess board
void draw_king(int pieceColour, int xCoord, int yCoord) {

    //Convert xCoord and yCoord to pixel coordinates
    int startingPixelCoordX = x_to_pixel(xCoord)+SQUARE_SIZE/2-2;
    int startingPixelCoordY = y_to_pixel(yCoord)+SQUARE_BORDER_SIZE*1;

	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY, 4,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-2, startingPixelCoordY+2, 8,3,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+5, 4,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-3, startingPixelCoordY+7, 10,4,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-1, startingPixelCoordY+11, 6,2,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX, startingPixelCoordY+13, 4,6,pieceColour);
	draw_rectangle_primitive(startingPixelCoordX-3, startingPixelCoordY+19, 10,4,pieceColour);
}

//Displays the winner of the game
void display_winner(int winner) {
    
    if     (winner == WHITE_PIECE) display_white();
    else if(winner == BLACK_PIECE) display_black();
    else if(winner == STALEMATE)   display_draw();

}

//Displays 2 in the HEX display
//Lights up left hand side of LEDS
void display_white() {

    *HEX3_HEX0_BASE = 0x6;
    *LEDR_BASE      = 0x1F;

}

//Displays -1 in the HEX display
//Lights up right hand side of LEDS
void display_black() {

    *HEX3_HEX0_BASE = 0x4006;
    *LEDR_BASE      = 0x3E0;

}

//Displays 0 in the HEX display
//Lights up both sides of LEDS
void display_draw() {

    *HEX3_HEX0_BASE = 0x3F;
    *LEDR_BASE      = 0x3FF;

}


/////////////////////////////////////////////////////////////////////


// Function prototypes for drawing primitives to the VGA display
/////////////////////////////////////////////////////////////////////

//Draws a square with the left top corner at (x,y) with size (size) and colour (colour)
void draw_square_primitive(int xPixelCoord, int yPixelCoord, int size, short int colour) {

    //draws rectangle with width equal to height of length size
    draw_rectangle_primitive(xPixelCoord, yPixelCoord, size, size, colour);

}

//Draws a rectangle with the left top corner at (x,y) with size (width,height) and colour (colour)
void draw_rectangle_primitive(int xPixelCoord, int yPixelCoord, int width, int height, short int colour) {

    //Loops through each pixel in the rectangle and draws it
    for (int xCoord = xPixelCoord; xCoord < xPixelCoord + width; xCoord++) {
        for (int yCoord = yPixelCoord; yCoord < yPixelCoord + height; yCoord++) {
            plot_pixel(xCoord, yCoord, colour);
        }
    }
}

//Draws a circle with the center at (x,y) with radius (radius) and colour (colour)
void draw_circle_primitive(int xPixelCoord, int yPixelCoord, int radius, short int colour) {

    //Loops through each pixel in the circle and draws it
    for (int xCoord = xPixelCoord - radius; xCoord < xPixelCoord + radius; xCoord++) {
        for (int yCoord = yPixelCoord - radius; yCoord < yPixelCoord + radius; yCoord++) {
            if (pow(xCoord - xPixelCoord, 2) + pow(yCoord - yPixelCoord, 2) <= pow(radius, 2)) {
                plot_pixel(xCoord, yCoord, colour);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////


// Function definitions for the chess game
/////////////////////////////////////////////////////////////////////

//Initializes the chess board to default state
void init_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {

    //Initialize colour of each square
    init_colours(board);

    //Initialize highlights
    init_highlights(board);

    //Initialize pieces
    init_pieces(board);

    //Initialize empty squares
    init_empty_squares(board);

    //Initialize outlines
    init_outlines(board);

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
                board[yCoord][xCoord].colour = WHITE_SQUARE_COLOUR;
            }
            else {
                board[yCoord][xCoord].colour = BLACK_SQUARE_COLOUR;
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

//Initializes outlines of the chess grid
void init_outlines(GridSquare board[BOARD_SIZE][BOARD_SIZE]) {

    //Loops through every square and sets the outline to false
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            board[yCoord][xCoord].outlined = false;
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

//highlights valid moves for a piece at square xCoord, yCoord
void highlight_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xStartingCoord, int yStartingCoord, int currentTurn) {

    //Loops through board and set grid highlight to true based on if the move is valid
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            if(is_valid_move(board, xStartingCoord, yStartingCoord, xCoord, yCoord, currentTurn)) {
                board[yCoord][xCoord].highlighted = true;
            }
        }
    }
}

//Checks if a move is valid
bool is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn) {

    //Checks if the king would be in check after the move

    //Create temporary board to test the move
    GridSquare tempBoard[BOARD_SIZE][BOARD_SIZE];
    copy_board(board, tempBoard);

    //Move the piece in the starting square to the ending square
    tempBoard[yCoordEnd][xCoordEnd].piece = tempBoard[yCoordStart][xCoordStart].piece;

    //Set the piece in the starting square to empty
    tempBoard[yCoordStart][xCoordStart].piece.piece_ID = EMPTY_SQUARE;

    //Check if the king would be in check after the move
    if(is_in_check(tempBoard, currentTurn)) {
        return false;
    }

    //Checks if the move is a valid move without check
    return is_valid_move_without_check(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd, currentTurn);
}

//Checks if a move is valid without check
bool is_valid_move_without_check(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn) {

    //Checks if the move is in the board
    if(xCoordEnd < 0 || xCoordEnd > BOARD_SIZE-1 || yCoordEnd < 0 || yCoordEnd > BOARD_SIZE-1) {
        return false;
    }

    //Checks if starting square is empty
    if(board[yCoordStart][xCoordStart].piece.piece_ID == EMPTY_SQUARE) {
        return false;
    }

    //Checks if ending square has piece of the same colour as the piece in the starting square
    if(board[yCoordStart][xCoordStart].piece.colour == board[yCoordEnd][xCoordEnd].piece.colour) {
        return false;
    }

    //Checks if the piece in the starting square is of the same colour as currentTurn
    if(board[yCoordStart][xCoordStart].piece.colour != currentTurn) {
        return false;
    }

    //Checks if the move is valid according to the piece type at starting square
    switch(board[yCoordStart][xCoordStart].piece.piece_ID) {
        case PAWN:
            return is_valid_pawn_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd, currentTurn);
        case ROOK:
            return is_valid_rook_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd);
        case KNIGHT:
            return is_valid_knight_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd);
        case BISHOP:
            return is_valid_bishop_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd);
        case QUEEN:
            return is_valid_queen_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd);
        case KING:
            return is_valid_king_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd);
        default:
            return false;
    }
}

//Checks if it is a valid pawn move
bool is_valid_pawn_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd, int currentTurn){

    //If pawn is white
    if(board[yCoordStart][xCoordStart].piece.colour == WHITE_PIECE) {


        //Checks pawn move is moving diagonally and capturing a piece
        if(board[yCoordEnd][xCoordEnd].piece.piece_ID != EMPTY_SQUARE && abs(xCoordStart - xCoordEnd) == 1 && yCoordEnd - yCoordStart == -1) {
            return true;
        }

        //Checks pawn move is moving forward and it's path is free
        if((board[yCoordEnd][xCoordEnd].piece.piece_ID == EMPTY_SQUARE && yCoordEnd - yCoordStart == -1) && (xCoordEnd == xCoordStart)) {
            return true;
        }

        //Checks if pawn is in starting location and moving forward two squares and it's path is free
        if ((yCoordStart == BOARD_SIZE-2 && yCoordEnd == BOARD_SIZE-4)
            && (board[yCoordEnd][xCoordEnd].piece.piece_ID == EMPTY_SQUARE)
            && (board[yCoordEnd + 1][xCoordEnd].piece.piece_ID == EMPTY_SQUARE)
            && (xCoordEnd == xCoordStart)) {
            return true;
        }
        
    }
    
    //If pawn is black
    if(board[yCoordStart][xCoordStart].piece.colour == BLACK_PIECE) {

        //Checks pawn move is moving diagonally and capturing a piece
        if(board[yCoordEnd][xCoordEnd].piece.piece_ID != EMPTY_SQUARE && abs(xCoordStart - xCoordEnd) == 1 && yCoordEnd - yCoordStart == 1) {
            return true;
        }

        //Checks pawn move is moving forward and it's path is free
        if(board[yCoordEnd][xCoordEnd].piece.piece_ID == EMPTY_SQUARE && (yCoordEnd - yCoordStart == 1)  && (xCoordEnd == xCoordStart)) {
            return true;
        }

        //Checks if pawn is in starting location and moving forward two squares and it's path is free
        if(yCoordStart == 1 && yCoordEnd == 3 
            && board[yCoordEnd][xCoordEnd].piece.piece_ID == EMPTY_SQUARE 
            && board[yCoordEnd - 1][xCoordEnd].piece.piece_ID == EMPTY_SQUARE
            && xCoordEnd == xCoordStart) {
            return true;
        }
    }

    return false;
}

//Checks if it is a valid knight move
bool is_valid_knight_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd){

    //Checks if the move is a valid knight move
    if(abs(xCoordStart - xCoordEnd) == 2 && abs(yCoordStart - yCoordEnd) == 1) {
        return true;
    }
    else if(abs(xCoordStart - xCoordEnd) == 1 && abs(yCoordStart - yCoordEnd) == 2) {
        return true;
    }
    else {
        return false;
    }
}

//Checks if it is a valid bishop move
bool is_valid_bishop_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd){

    //Checks if the move is a valid bishop move
    if(abs(xCoordStart - xCoordEnd) != abs(yCoordStart - yCoordEnd)) {
        return false;
    }

    //Checks if the path of the bishop is clear
    for(int distance = 1; distance < abs(xCoordStart - xCoordEnd); distance++) {
        if(board[yCoordStart + distance * (yCoordEnd - yCoordStart) / abs(xCoordEnd - xCoordStart)][xCoordStart + distance * (xCoordEnd - xCoordStart) / abs(xCoordEnd - xCoordStart)].piece.piece_ID != EMPTY_SQUARE) {
            return false;
        }
    }

    return true;
}

//Checks if it is a valid rook move
bool is_valid_rook_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd){

    //Checks if the move is a valid rook move
    if(xCoordStart != xCoordEnd && yCoordStart != yCoordEnd) {
        return false;
    }
    
    //Checks if the path of the rook is clear
    if(xCoordStart == xCoordEnd) {
        for(int distance = 1; distance < abs(yCoordStart - yCoordEnd); distance++) {
            if(board[yCoordStart + distance * (yCoordEnd - yCoordStart) / abs(yCoordEnd - yCoordStart)][xCoordStart].piece.piece_ID != EMPTY_SQUARE) {
                return false;
            }
        }
    }
    else {
        for(int distance = 1; distance < abs(xCoordStart - xCoordEnd); distance++) {
            if(board[yCoordStart][xCoordStart + distance * (xCoordEnd - xCoordStart) / abs(xCoordEnd - xCoordStart)].piece.piece_ID != EMPTY_SQUARE) {
                return false;
            }
        }
    }

    return true;
}

//Checks if it is a valid queen move
bool is_valid_queen_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd){

    //Checks if the move is both a valid bishop and a valid rook move
    if(is_valid_bishop_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd) || is_valid_rook_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd)) {
        return true;
    }

    return false;
}

//Checks if it is a valid king move
bool is_valid_king_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd){

    //Checks if the move is a valid king move
    if(abs(xCoordStart - xCoordEnd) <= 1 && abs(yCoordStart - yCoordEnd) <= 1) {
        return true;
    }

    return false;
}

//Checks if a piece has valid moves
bool has_valid_moves(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int currentTurn) {

    //For the piece in xCoord, yCoord, check if it has any valid moves using function is_valid_move
    for(int xCoordEnd = 0; xCoordEnd < BOARD_SIZE; xCoordEnd++) {
        for(int yCoordEnd = 0; yCoordEnd < BOARD_SIZE; yCoordEnd++) {
            if(is_valid_move(board, xCoordStart, yCoordStart, xCoordEnd, yCoordEnd, currentTurn)) {
                return true;
            }
        }
    }

    return false;
}

//Checks if king is in check
bool is_in_check(GridSquare board[BOARD_SIZE][BOARD_SIZE], int pieceColour) {
    
    //Find king position
    int* kingPosition = get_king_position(board, pieceColour);

    //Check if king is in check
    for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
        for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
            if(board[yCoord][xCoord].piece.piece_ID != EMPTY_SQUARE && board[yCoord][xCoord].piece.piece_ID != KING && board[yCoord][xCoord].piece.piece_ID != pieceColour) {
                if(is_valid_move_without_check(board, xCoord, yCoord, kingPosition[0], kingPosition[1], !pieceColour)) {
                    return true;
                }
            }
        }
    }

    return false;
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
int * get_selected_piece_location(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {

    //Initialize variables
    int * selectedPiece = malloc(sizeof(int) * 2);
    int xCoord = 0;
    int yCoord = 0;

    //Set LED 0 to 1 to indicate that the user is selecting a piece
    *LEDR_BASE = 1;

    //Loop until valid input is given
    while(true) {
        
        int* userInput = get_input_from_switches();

        //If the tenth bit is not set, continue polling for input
        if(userInput[2] == 0) {

            //Reset outline of grid squares
            init_outlines(board);

            //Reset highlighted squares
            init_highlights(board);

            //Set the outline of the selected square to true
            board[userInput[1]][userInput[0]].outlined = true;

            //Draw the board
            draw_board(board);

            continue;
        }

        //Get x and y coordinates of the piece selected
        xCoord = userInput[0];
        yCoord = userInput[1];

        //Check if the piece selected is valid
        if(!is_empty_square(board, xCoord, yCoord) && board[yCoord][xCoord].piece.colour == currentTurn && has_valid_moves(board, xCoord, yCoord, currentTurn)) {
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
    int * move;
    int xCoord = 0;
    int yCoord = 0;

    //Loop until valid input is given
    while(true) {

        //Set LED 0 to 2 to indicate that the user is selecting a move
        *LEDR_BASE = 2;
        
        move = get_input_from_switches();

        //If the tenth bit is not set, continue polling for input
        if(move[2] == 0) {

            //Reset outline of grid squares
            init_outlines(board);

            //Reset highlighted squares
            init_highlights(board);

            //Set the outline of the selected square to true
            board[move[1]][move[0]].outlined = true;

            //Loops through all the squares in the board and set highlighted to true if move is legal
            for(int xCoordEnd = 0; xCoordEnd < BOARD_SIZE; xCoordEnd++) {
                for(int yCoordEnd = 0; yCoordEnd < BOARD_SIZE; yCoordEnd++) {
                    if(is_valid_move(board, startingLocationX, startingLocationY, xCoordEnd, yCoordEnd, currentTurn)) {
                        board[yCoordEnd][xCoordEnd].highlighted = true;
                    }
                }
            }

            //Draw the board
            draw_board(board);

            //free memory
            free(move);

            continue;
        }

        //Get x and y coordinates of the piece selected
        xCoord = move[0];
        yCoord = move[1];

        //Check if the piece selected is valid
        if(is_valid_move(board,startingLocationX, startingLocationY, xCoord, yCoord, currentTurn)) {
            move[0] = xCoord;
            move[1] = yCoord;
            break;
        }

        //free memory
        free(move);
    }

    return move;
}

//Plays a turn of the game
void play_turn(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {
    
    //Get selected piece location
    int * selectedPieceLocation = get_selected_piece_location(board, currentTurn);

    //Get move location
    int * moveLocation = get_move(board, selectedPieceLocation[0], selectedPieceLocation[1], currentTurn);

    //Move piece
    move_piece(board, selectedPieceLocation[0], selectedPieceLocation[1], moveLocation[0], moveLocation[1]);

    //Free memory
    free(selectedPieceLocation);
    free(moveLocation);
}

//Move a piece from one square to another
void move_piece(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd) {

    //Move the piece at starting location to end location
    board[yCoordEnd][xCoordEnd].piece = board[yCoordStart][xCoordStart].piece;

    //Set the piece at the starting location to empty and colour to empty
    board[yCoordStart][xCoordStart].piece.piece_ID = EMPTY_SQUARE;
    board[yCoordStart][xCoordStart].piece.colour = EMPTY_PIECE;

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

    if(is_stalemate(board, currentTurn)) return true;

    if(is_checkmate(board, currentTurn)) return true;

    return false;
}

//Determines the winner of the game
int get_winner(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {

    //Check if game is in checkmate
    if(is_checkmate(board, currentTurn)) {
        return !currentTurn;
    }

    //Check if game is in stalemate
    if(is_stalemate(board, currentTurn)) {
        return STALEMATE;
    }

    return 1;
}

//Checks if game ended in stalemate
bool is_stalemate(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {

    //Check if king is in not in check
    if(is_in_check(board, !currentTurn)) return false;

    //iterate through every piece and see if it has valid moves
    for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
        for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
            if(board[yCoord][xCoord].piece.piece_ID != EMPTY_SQUARE && board[yCoord][xCoord].piece.colour == currentTurn) {
                if(has_valid_moves(board, xCoord, yCoord, currentTurn)) {
                    return false;
                }
            }
        }
    }


    return true;
}

//Checks if the game is in checkmate
bool is_checkmate(GridSquare board[BOARD_SIZE][BOARD_SIZE], int currentTurn) {

    //Check if king is not in check
    if(!is_in_check(board, currentTurn)) return false;

    //Check if any move can prevent checkmate

    //Create a copy of the board
    GridSquare boardCopy[BOARD_SIZE][BOARD_SIZE];
    copy_board(board, boardCopy);

    //Iterate through every piece
    //Iterate through every possible move
    //Check if move is valid and would get the king out of check
    for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
        for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
            if(board[yCoord][xCoord].piece.piece_ID != EMPTY_SQUARE && board[yCoord][xCoord].piece.colour == currentTurn) {
                for(int xCoordEnd = 0; xCoordEnd < BOARD_SIZE; xCoordEnd++) {
                    for(int yCoordEnd = 0; yCoordEnd < BOARD_SIZE; yCoordEnd++) {
                        if(is_valid_move(board, xCoord, yCoord, xCoordEnd, yCoordEnd, currentTurn)) {

                            //Save piece in end location
                            GridSquare tempPiece = boardCopy[yCoordEnd][xCoordEnd];

                            move_piece(boardCopy, xCoord, yCoord, xCoordEnd, yCoordEnd);
                            if(!is_in_check(boardCopy, currentTurn)) {
                                return false;
                            }
                            move_piece(boardCopy, xCoordEnd, yCoordEnd, xCoord, yCoord);
                            boardCopy[yCoordEnd][xCoordEnd] = tempPiece;
                        }
                    }
                }
            }
        }
    }

    return true;
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

//Gets inputs from switches
//Returns x and y coordinates of the selected piece on indexes 0 and 1 respectively
//Returns if user sent the input to software in indexes 2
int* get_input_from_switches() {
    
    //Gets input from switches at DE1-SoC board
    volatile int userInput = *(volatile int *) (SW_BASE);

    //Create array to store the input
    int* userInputArray = (int*) malloc(sizeof(int) * 3);

    //Convert user input to x and y indexes
    userInputArray[0] = userInput & 0x00000007;
    userInputArray[1] = 7-((userInput & 0x00000038) >> 3);
    
    //Get if user sent input to software
    userInputArray[2] = (userInput & 0x00000200) >> 9;

    return userInputArray;
}

//Copy board to another board
void copy_board(GridSquare board[BOARD_SIZE][BOARD_SIZE], GridSquare copyBoard[BOARD_SIZE][BOARD_SIZE]) {

    //Copy board
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            copyBoard[i][j] = board[i][j];
        }
    }
}

//Gets king position
int* get_king_position(GridSquare board[BOARD_SIZE][BOARD_SIZE], int pieceColour) {

    //Initialize variables
    int * kingPosition = malloc(sizeof(int) * 2);

    //Loop through the board to find the king
    for(int yCoord = 0; yCoord < BOARD_SIZE; yCoord++) {
        for(int xCoord = 0; xCoord < BOARD_SIZE; xCoord++) {
            if(board[yCoord][xCoord].piece.piece_ID == KING && board[yCoord][xCoord].piece.colour == pieceColour) {
                kingPosition[0] = xCoord;
                kingPosition[1] = yCoord;
                break;
            }
        }
    }

    return kingPosition;

}

/////////////////////////////////////////////////////////////////////