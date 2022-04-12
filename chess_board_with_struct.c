
	/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
	
// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable

void plot_pixel(int x, int y, short int line_colour);
void clear_screen();
void draw_board();
void wait_for_vsync();
void draw_outline();
//have seperate functions for drawing each of the pieces
//set a centre point for each box to know where to draw

struct Box
{
	int xbox, ybox;
	short int colour;
	int piece_ID;
}Box;

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
	struct Box box[63];
	int i;
	
	//im so sorry thomas im just hardcoding this
	for(i = 0; i <= 63; i++){
		//initialize the pieces
		if((i == 0) || (i = 7) || (i == 56) || (i == 63)){
			box[i].piece_ID = 4;
		}else if((i == 2) || (i == 5) || (i == 58) || (i == 61)){
			box[i].piece_ID = 3;
		}else if((i == 1) || (i == 6) || (i == 57) || (i = 62)){
			box[i].piece_ID = 2;
		}else if((i == 59) || (i == 3)){
			box[i].piece_ID = 5;
		}else if((i == 4) || (i == 60)){
			box[i].piece_ID = 6;
		}else if((i >= 8 && i <= 15) || (i >= 48 && i <= 55)){
			box[i].piece_ID = 1;
		}else{
			box[i].piece_ID = 0;
		}
		
		//initalize the y_coord
		if((i >= 8 && i <= 15)){
			box[i].ybox = 45; //row 1
		}else if( i <=7 ){
			box[i].ybox = 15; //row 0
		}else if((i >= 16 && i <= 23)){
			box[i].ybox = 75; //row 2
		}else if((i >= 24 && i <= 31)){
			box[i].ybox = 105;//row 3
		}else if((i >= 32 && i <= 39)){
			box[i].ybox = 135; //row 4
		}else if((i >= 40 && i <= 47)){
			box[i].ybox = 165; //row 5
		}else if((i >= 48 && i <= 55)){
			box[i].ybox = 195; //row 6
		}else{
			box[i].ybox = 225; //row 7
		}
		
		//x_coord goes up by 8 each time
			if((i == 0) || (i = 8) || (i == 16) || (i == 24) || (i == 32) || (i == 40) || (i == 48) || (i == 56)){
			box[i].xbox = 55;
		}else if((i == 1) || (i == 9) || (i == 17) || (i == 25) || (i == 33) || (i == 41) || (i == 49) || (i == 57)){
			box[i].xbox = 85;
		}else if((i == 2) || (i == 10) || (i == 18) || (i == 26) || (i == 34) || (i == 42) || (i == 50) || (i == 58)){
			box[i].xbox = 115;
		}else if((i == 3) || (i == 11) || (i == 19) || (i == 27) || (i == 35) || (i == 43) || (i == 51) || (i == 59)){
			box[i].xbox = 145;
		}else if((i == 4) || (i == 12) || (i == 20) || (i == 28) || (i == 36) || (i == 44) || (i == 52) || (i == 60)){
			box[i].xbox = 175;
		}else if((i == 5) || (i == 13) || (i == 21) || (i == 29) || (i == 37) || (i == 45) || (i == 53) || (i == 61)){
			box[i].xbox = 205;
		}else if((i == 6) || (i == 14) || (i == 22) || (i == 30) || (i == 38) || (i == 46) || (i == 54) || (i == 62)){
			box[i].xbox = 235;
		}else{
			box[i].xbox = 265;
		}
		//colour of the pieces
		if(i <= 15){
			box[i].colour = ORANGE;//just use yello and orange for now
			//orange = black, yellow = white
		}else if(i >= 48){
			box[i].colour = YELLOW;
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
        /* Erase any boxes and lines that were drawn in the last iteration */
        

        // code for drawing the boxes and lines (not shown)
        // code for updating the locations of boxes (not shown)

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

        for (x = 0; x < RESOLUTION_X; x++)
                for (y = 0; y < RESOLUTION_Y; y++)
                        plot_pixel (x, y, 0);
}

void draw_board(){
	int y,x;
	for (x = 40; x < 280; x++){
		for (y = 0; y < RESOLUTION_Y; y++){
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
	for(x = 0; x <=RESOLUTION_X; x++){
		for(y = 0; y < RESOLUTION_Y; y++){
			if((x <= 40) || (x >= 280)){
			plot_pixel(x,y,GREY);
			}
		}
	}
}