# DE1-SoC-Chess
Chess implemented on the DE1-SoC hardware using C. VGA Display is used to display the game, while the switches are used to move the pieces.

Switches 0-2 determine the column of the selected square
Switches 3-5 determine the row of the selected square
Switch 9 confirms the selection of the square

Selected squares are outlined in purple
Possible moves for a piece are highlighted yellow

Winner will be displayed on the LEDs and HEX displays:
  1 represents a win for the white pieces, -1 for the black pieces
  5 rightmost LEDs will be turned on if the white pieces win
  5 leftmost LEDs will be turned on if the black pieces win

In case of a stalemate, All LEDs will turn on, and the HEX display will show a zero
