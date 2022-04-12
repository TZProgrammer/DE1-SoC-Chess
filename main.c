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
const int BOARD_SIZE            =8;
const int WHITE_PIECE           =1;
const int BLACK_PIECE           =0;
const int EMPTY_PIECE          =-1;
	

// location of the pixel buffer in SDRAM
volatile int pixel_buffer_start;



//Piece struct holds information about a piece
// x and y are the coordinates of the piece
// colour is the color of the piece (white or black)
// piece_ID is the id of the piece
typedef struct Piece
{
    short int colour;
    PieceIdx piece_ID;
} Piece;
typedef struct GridSquare
{
    Piece piece;
    int highlighted;
} GridSquare;


// Function prototypes for drawing to the VGA display
/////////////////////////////////////////////////////////////////////

//Plots a single pixel at the given coordinates with the given color
void plot_pixel(int x, int y, short int line_colour);

//Clears the screen with the colour black
void clear_screen();

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Synchronizes the double buffering of the VGA display
void wait_for_vsync();

//Draws the outline of the pieces on the chess board
void draw_outline();

//Draws a single piece on the chess board
void draw_piece(Piece piece);

//Draws a single square on the chess board
void draw_square(GridSquare square);

//Draws a pawn on the chess board
void draw_pawn(int xCoord, int yCoord);

//Draws a knight on the chess board
void draw_knight(int xCoord, int yCoord);

//Draws a bishop on the chess board
void draw_bishop(int xCoord, int yCoord);

//Draws a rook on the chess board
void draw_rook(int xCoord, int yCoord);

//Draws a queen on the chess board
void draw_queen(int xCoord, int yCoord);

//Draws a king on the chess board
void draw_king(int xCoord, int yCoord);

/////////////////////////////////////////////////////////////////////


// Function prototypes for the chess game
/////////////////////////////////////////////////////////////////////

//Enters chess game loop
void play_chess();

//Initializes the chess board to default state
void init_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Initializes highlights
void init_highlights(GridSquare board[BOARD_SIZE][BOARD_SIZE]);

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
int is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd);

/////////////////////////////////////////////////////////////////////





int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    
    // declares the chessBoard
	GridSquare chessBoard[BOARD_SIZE][BOARD_SIZE];
	
	//initializes the chessBoard to default values
    init_board(chessBoard);

    


	for(int yCoord = 0; yCoord <= 7; yCoord++){

		//initialize the pieces
		if((
			chessBoard[8.piece_ID = 4;
		}else if((
			chessBoard[8.piece_ID = 3;
		}else if((
			chessBoard[8.piece_ID = 2;
		}else if((
			chessBoard[8.piece_ID = 5;
		}else if((
			chessBoard[8.piece_ID = 6;
		}else if((
			chessBoard[8.piece_ID = 1;
		}else{
			chessBoard[8.piece_ID = 0;
		}
		
		//initalize the y_coord
		if((
			chessBoard[8.chessBoard 845; //row 1
		}else if( 
			chessBoard[8.chessBoard 815; //row 0
		}else if((
			chessBoard[8.chessBoard 875; //row 2
		}else if((
			chessBoard[8.chessBoard 8105;//row 3
		}else if((
			chessBoard[8.chessBoard 8135; //row 4
		}else if((
			chessBoard[8.chessBoard 8165; //row 5
		}else if((
			chessBoard[8.chessBoard 8195; //row 6
		}else{
			chessBoard[8.chessBoard 8225; //row 7
		}
		
		//x_coord goes up by 8 each time
			if((
			chessBoard[8.chessBoard 855;
		}else if((
			chessBoard[8.chessBoard 885;
		}else if((
			chessBoard[8.chessBoard 8115;
		}else if((
			chessBoard[8.chessBoard 8145;
		}else if((
			chessBoard[8.chessBoard 8175;
		}else if((
			chessBoard[8.chessBoard 8205;
		}else if((
			chessBoard[8.chessBoard 8235;
		}else{
			chessBoard[8.chessBoard 8265;
		}
		//colour of the pieces
		if(
			chessBoard[8.colour = ORANGE;//just use yello and orange for now
			//orange = black, yellow = white
		}else if(i >= 48){
			chessBoard[8.colour = YELLOW;
		}
		

	}//end for
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    while (1){
        /* Erase any chessBoard 8d lines that were drawn in the last iteration */
        

        // code for drawing the chessBoard 8d lines (not shown)
        // code for updating the locations of chessBoard 8ot shown)

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		draw_outline();
		draw_board();
    }
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

        for (x = 0; x < 320; x++)
                for (y = 0; y < 240; y++)
                        plot_pixel (x, y, 0);
}

//Draws the chess board in its current state
void draw_board(GridSquare board[BOARD_SIZE][BOARD_SIZE]){
	int y,x;
	for (x = 40; x < 280; x++){
		for (y = 0; y < 240; y++){
		if((x < 70 && x > 40) || (x > 100 && x < 130) || (x > 160 && x < 190) || (x > 220 && x < 250)){//first column
			if(y < 30 || (y > 60 && y < 90) || (y > 120 && y < 150) || (y > 180 && y < 210)){
				plot_pixel(x,y, WHITE);
			}
		}
		if((x > 70 && x < 100) || (x > 130 && x < 160) || (x > 190 && x < 220) || (x > 250 && x < 280)){
			if((y > 30 && y < 60) || (y > 90 && y < 120) || (y > 150 && y < 180) || (y > 210)){
			plot_pixel(x,y, WHITE);
			}
		}
		}

	}
}

//Synchronizes the double buffering of the VGA display
void wait_for_vsync(){
	
    volatile int *pixel_ctrl_ptr = (int *) 0xFF203020;
	int status;
	
    //launches the swap process
	*pixel_ctrl_ptr = 1; //sets the S bit to 1 
	
    //poll for the status bit
	status = *(pixel_ctrl_ptr + 3); //OxFF20302C
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}
	
}

//Draws the outline of the pieces on the chess board
void draw_outline(){
	int x, y;
	for(x = 0; x <=320; x++){
		for(y = 0; y < 240; y++){
			if((x <= 40) || (x >= 280)){
			plot_pixel(x,y,GREY);
			}
		}
	}
}

//Draws a single piece on the chess board
void draw_piece(Piece piece) {
    return ;
}

//Draws a single square on the chess board
void draw_square(GridSquare square) {
    return ;
}

//Draws a pawn on the chess board
void draw_pawn(int xCoord, int yCoord) {
    return ;
}

//Draws a knight on the chess board
void draw_knight(int xCoord, int yCoord) {
    return ;
}

//Draws a bishop on the chess board
void draw_bishop(int xCoord, int yCoord) {
    return ;
}

//Draws a rook on the chess board
void draw_rook(int xCoord, int yCoord) {
    return ;
}

//Draws a queen on the chess board
void draw_queen(int xCoord, int yCoord) {
    return ;
}

//Draws a king on the chess board
void draw_king(int xCoord, int yCoord) {
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
            board[yCoord][xCoord].highlight = false;
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
            board[yCoord][xCoord].piece.piece_color = EMPTY_PIECE;
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
int is_valid_move(GridSquare board[BOARD_SIZE][BOARD_SIZE], int xCoordStart, int yCoordStart, int xCoordEnd, int yCoordEnd) {
    return ;
}

/////////////////////////////////////////////////////////////////////
