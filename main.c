/*
In this program, we will use the C programming language to create a playable chess game in the DE1-SoC.

The game will be played on a chess board. The board will be represented by an 8 by 8 2D array of structs.
Those structs will contain the piece type, the color of the piece, and if a square is highlighted.
*/


#include <stdlib.h>
#include <time.h>


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
const int FALSE                 =0;
const int TRUE                  =1;
const int BOARD_SIZE            =8;
	

// location of the pixel buffer in SDRAM
volatile int pixel_buffer_start;



//Piece struct holds information about a piece
// x and y are the coordinates of the piece
// colour is the color of the piece (white or black)
// piece_ID is the id of the piece
struct Piece
{
    short int colour;
    PieceIdx piece_ID;
};
struct GridSquare
{
    struct Piece piece;
    int highlighted;
};


/////////////////////////////////////////////////////////////////////
// Function prototypes for drawing to the VGA display

//Plots a single pixel at the given coordinates with the given color
void plot_pixel(int x, int y, short int line_colour);

//Clears the screen with the colour black
void clear_screen();

//Draws the chess board in its current state
void draw_board(struct GridSquare board[BOARD_SIZE][BOARD_SIZE]);

//Synchronizes the double buffering of the VGA display
void wait_for_vsync();

//Draws the outline of the pieces on the chess board
void draw_outline();

//Draws a single piece on the chess board
void draw_piece(struct Piece piece);

//Draws a single square on the chess board
void draw_square(struct GridSquare square);

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


/////////////////////////////////////////////////////////////////////
// Function prototypes for the chess game

//Initializes the chess board to default state
void init_board(struct GridSquare board[BOARD_SIZE][BOARD_SIZE]);





int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    
    // declares the chessBoard
	struct GridSquare chessBoard[BOARD_SIZE][BOARD_SIZE];
	
	//initializes the chessBoard to default values
    //Loop through chessboard and sets GridSquare to default values
	for(int yCoord = 0; yCoord <= BOARD_SIZE-1; yCoord++){
        for(int xCoord = 0; xCoord <= BOARD_SIZE-1; xCoord++){
            if((yCoord == 0) ){
                chessBoard[xCoord][yCoord].piece.piece_ID = 4;
            }
            else{
                chessBoard[xCoord][yCoord].piece.piece_ID = 0;
            }


        }
    }


	for(int yCoord = 0; yCoord <= 7; yCoord++){

		//initialize the pieces
		if((i == 0) || (i = 7) || (i == 56) || (i == 63)){
			chessBoard[8.piece_ID = 4;
		}else if((i == 2) || (i == 5) || (i == 58) || (i == 61)){
			chessBoard[8.piece_ID = 3;
		}else if((i == 1) || (i == 6) || (i == 57) || (i = 62)){
			chessBoard[8.piece_ID = 2;
		}else if((i == 59) || (i == 3)){
			chessBoard[8.piece_ID = 5;
		}else if((i == 4) || (i == 60)){
			chessBoard[8.piece_ID = 6;
		}else if((i >= 8 && i <= 15) || (i >= 48 && i <= 55)){
			chessBoard[8.piece_ID = 1;
		}else{
			chessBoard[8.piece_ID = 0;
		}
		
		//initalize the y_coord
		if((i >= 8 && i <= 15)){
			chessBoard[8.chessBoard 845; //row 1
		}else if( i <=7 ){
			chessBoard[8.chessBoard 815; //row 0
		}else if((i >= 16 && i <= 23)){
			chessBoard[8.chessBoard 875; //row 2
		}else if((i >= 24 && i <= 31)){
			chessBoard[8.chessBoard 8105;//row 3
		}else if((i >= 32 && i <= 39)){
			chessBoard[8.chessBoard 8135; //row 4
		}else if((i >= 40 && i <= 47)){
			chessBoard[8.chessBoard 8165; //row 5
		}else if((i >= 48 && i <= 55)){
			chessBoard[8.chessBoard 8195; //row 6
		}else{
			chessBoard[8.chessBoard 8225; //row 7
		}
		
		//x_coord goes up by 8 each time
			if((i == 0) || (i = 8) || (i == 16) || (i == 24) || (i == 32) || (i == 40) || (i == 48) || (i == 56)){
			chessBoard[8.chessBoard 855;
		}else if((i == 1) || (i == 9) || (i == 17) || (i == 25) || (i == 33) || (i == 41) || (i == 49) || (i == 57)){
			chessBoard[8.chessBoard 885;
		}else if((i == 2) || (i == 10) || (i == 18) || (i == 26) || (i == 34) || (i == 42) || (i == 50) || (i == 58)){
			chessBoard[8.chessBoard 8115;
		}else if((i == 3) || (i == 11) || (i == 19) || (i == 27) || (i == 35) || (i == 43) || (i == 51) || (i == 59)){
			chessBoard[8.chessBoard 8145;
		}else if((i == 4) || (i == 12) || (i == 20) || (i == 28) || (i == 36) || (i == 44) || (i == 52) || (i == 60)){
			chessBoard[8.chessBoard 8175;
		}else if((i == 5) || (i == 13) || (i == 21) || (i == 29) || (i == 37) || (i == 45) || (i == 53) || (i == 61)){
			chessBoard[8.chessBoard 8205;
		}else if((i == 6) || (i == 14) || (i == 22) || (i == 30) || (i == 38) || (i == 46) || (i == 54) || (i == 62)){
			chessBoard[8.chessBoard 8235;
		}else{
			chessBoard[8.chessBoard 8265;
		}
		//colour of the pieces
		if(i <= 15){
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

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void wait_for_vsync(){
	volatile int *pixel_ctrl_ptr = (int *) 0xFF203020;
	int status;
	//launches the swap process
	*pixel_ctrl_ptr = 1; //sets the S bit to 1 
	//poll for the status bit
	status = *(pixel_ctrl_ptr + 3); //OxFF20302C
	while((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}//end while
	
}//end wait_for_vsync 

void clear_screen()
{
        int y, x;

        for (x = 0; x < 320; x++)
                for (y = 0; y < 240; y++)
                        plot_pixel (x, y, 0);
}

void draw_board(struct GridSquare board[BOARD_SIZE][BOARD_SIZE]){
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