// TanksGame.c

#include "TanksGame.h"
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

// Global const definition
const Location arrshDirections[] = {
									{-1, 0 },		// Up
									{0, 1  },		// Right
									{1, 0  },		// Down
									{0, -1 },		// Left
									{0, 0  },		// Not moving
									{-1, -1},		// Left Up
									{-1, 1 },		// Right Up
									{1, 1  },		// Right Down
									{1, -1 }		// Left Down
};


//-----------------------------------------------------------------------------------------------
//										 Tanks Game System
//										-------------------
//
// General	: The program perform tanks game between two users. Left tank to be first.
//
// Input	: In each turn one the users insert code of action he would like to perform:
//				1 - move on board
//				2 - normal shooting
//				3 - shooting in alternating direction
//				4 - dropping random bomb
//				STOP_SIGN_INPUT - stop the system
// 
//			  In a case of actions 1-3, the player whould need to insert direction:
//				1 - move up
//				2 - move right
//				3 - move down
//				4 - move left
//			  
//			  In a case the user enter action 4, he would need to insert location for the bomb.
//
// Process	: The game will go on until:
//				1. One of tanks get damaged.
//				2. The board is empty of breakable walls.
//				3. One of the users enter the STOP_SIGN_INPUT.
// 
// Output	: The program shows a board of the game.
//			  In each move of it updates it.
//			  In the end it prints the winner or draw. 
//
// Assumption: ROWS >= 3, COLS >= 3
//-----------------------------------------------------------------------------------------------
// Programmer   : Liel Buci.
// Date			: 25.03.2024
//-----------------------------------------------------------------------------------------------
int main()
{
	// Matrix definition
	char matcBoard[ROWS][COLS];
	
	// Variable definition
	Pos* pposFirstTank =  (Pos*)malloc(sizeof(Pos));
	Pos* pposSecondTank = (Pos*)malloc(sizeof(Pos));
	size_s ushAmountOfPlays;
	Bool bIsStillInGame = TRUE;
	size_s ushAmountOfWalls;
	size_s gameResult; /* 1 - draw, 2 - Right tank won, 3 - Left tank won */

	// Code Section

	if (pposFirstTank == NULL || pposSecondTank == NULL)
	{
		fprintf(stderr, "Could not allocate\n");
		exit(1);
	}

	InitBoard(matcBoard, pposFirstTank, pposSecondTank, &ushAmountOfPlays, &ushAmountOfWalls);

	/* The game loop */
	while (bIsStillInGame)
	{
		// Block variable definition
		size_s shPlayerChoice;
		Pos* pposCurrentPlayer;

		// Block Code Section

		PrintBoard(matcBoard, ushAmountOfPlays, ushAmountOfWalls);
		shPlayerChoice = Menu();

		// find the current player to play
		// If the amount of plays is even - it's the first one
		// Otherwise, it the second one
		
		pposCurrentPlayer = (ushAmountOfPlays & 1) ? pposSecondTank : pposFirstTank;

		switch (shPlayerChoice)
		{
		case 1:
			MoveTank(matcBoard, pposCurrentPlayer, &ushAmountOfPlays, ushAmountOfWalls);
			break;
		case 2:
		case 3:
			{
				// Variable definition
				size_s ushDirectionShoot = DirectionMenu();
				Pos* pposShootLoaction = (Pos*)malloc(sizeof(Pos));
				size_s ushShootStatus;

				// Code Section
				
				if (pposShootLoaction == NULL)
				{
					fprintf(stderr, "Could not allocate\n");
					exit(1);
				}

				pposShootLoaction->ushRow = pposCurrentPlayer->ushRow;
				pposShootLoaction->ushCol = pposCurrentPlayer->ushCol;

				// Calling the function that activates the normal shoot. In a case the shoot hit the other player (returned 2), the game is over
				bIsStillInGame = (ushShootStatus = NormalShoot(matcBoard, ushDirectionShoot, ushAmountOfPlays, pposShootLoaction, &ushAmountOfWalls)) != 2;

				// Otherwise, in the case playerChoice is 3, and shootStatus is 3, meaning that hit the edge of the board, we will continue to the right
				// As we designed our directions array, right from the last direction will be

				if (bIsStillInGame && shPlayerChoice == 3 && ushShootStatus == 3)
				{
					ushDirectionShoot = ((ushDirectionShoot + 1) == 5) ? 1: ushDirectionShoot + 1; // it will give us the new direction for the shoot

					// we already have the shoot location from the last call to the function
					
					// Calling the function that activates the normal shoot. In a case the shoot hit the other player (returned 2), the game is over
					bIsStillInGame = NormalShoot(matcBoard, ushDirectionShoot, ushAmountOfPlays, pposShootLoaction, &ushAmountOfWalls) != 2;
				}

				// If the game is over from shoot - odd amountOfPlays it's win of the right tank, and else the left tank won
				if (!bIsStillInGame)
				{
					gameResult = (ushAmountOfPlays & 1) ? 3 : 2;
				}

				// Free the shoot location allocated memory
				free(pposShootLoaction);
			}
			break;
		case 4:
			{
				// Variable definition
				size_s gameStatus;

				// Code Section

				gameStatus = RandomBomb(matcBoard, &ushAmountOfWalls, pposCurrentPlayer, ushAmountOfPlays);
				switch (gameStatus)
				{
				// the game still running
				case 1: 
					break;
				// current player won
				case 2: 
					gameResult = (ushAmountOfPlays & 1) ? 3 : 2;
					bIsStillInGame = FALSE;
					break;
				// other player won
				case 3: 
					bIsStillInGame = FALSE;
					gameResult = (ushAmountOfPlays & 1) ? 2 : 3;
					break;
				// the game ends with draw
				case 4:
					bIsStillInGame = FALSE;
					gameResult = 1;
				}
			}
			break;
		case STOP_GAME_SIGN:
			bIsStillInGame = FALSE;
			gameResult = 1; /* In that case it will be considered as draw between the players */
		}

		// If there is no more walls, the game over with draw, only if both tanks still alive
		if (bIsStillInGame && ushAmountOfWalls == 0)
		{
			bIsStillInGame = FALSE;
			gameResult = 1;
		}

		// Update amount of moves
		ushAmountOfPlays++;
	}

	// Free allocated memory
	free(pposFirstTank); free(pposSecondTank);

	// When the loop is ending there we want to print a message about the winner
	
	switch (gameResult)
	{
	case 1:
		printf("\033[0;36mThe game end with draw!!\033[0m\n");
		break;
	case 2:
		printf("\033[0;32mThe left tank won. Well done!\033[0m\n");
		break;
	case 3:
		printf("\033[0;34mThe right tank won. Well done!\033[0m\n");
	}

}

//-----------------------------------------------------------------------------------------------
//										 Init Board
//										-------------
//
// General		: This procedure init the game board by putting breakable walls in each 
//				  (odd row, odd col) location, and also puts the tanks on board.
//
// Parametes	: 
//		imatcBoard		   - The game board. (In)
//		ipposFirstPlayer   - A pointer to the position of the first tank to play. (In)
//		ipposSecondPlayer  - A pointer to the position of the second tank to play. (In)
//		ipushAmountOfPlays - A pointer to a variable contains amount of plays until now. (In)
//		ipushAmountOfWalls - A pointer to a variable contains amount of walls left in the board. (In)
//
// Return Value : None.
//
//-----------------------------------------------------------------------------------------------

void InitBoard(char imatcBoard[ROWS][COLS], Pos* ipposFirstPlayer, Pos* ipposSecondPlayer, size_s* ipushAmountOfPlays, size_s* ipushAmountOfWalls)
{
	// Variable definition
	size_s i, j;

	// Code Section

	// initial position of tanks (edges of board, in the middle row - rounded down)
	
	ipposFirstPlayer->ushCol = 0;
	ipposSecondPlayer->ushCol = COLS - 1;

	ipposFirstPlayer->ushRow = ipposSecondPlayer->ushRow = ROWS / 2;

	// Fulling the board with empty signs EMPTY_SYMBOL
	memset(imatcBoard, EMPTY_SYMBOL, ROWS * COLS);

	// Now, we will put breakable walls (WALLS_SYMBOL) in each (odd row, odd col) location
	// Assumption: ROWS >= 3, COLS >= 3

	// initialize the current amount of walls on board to be 0
	*ipushAmountOfWalls = 0;

	for (i = 1; i < ROWS; i += 2)
	{
		for (j = 1; j < COLS; j += 2)
		{
			imatcBoard[i][j] = WALLS_SYMBOL;
			(*ipushAmountOfWalls)++;
		}
	}

	// Putting the tanks on board
	imatcBoard[ipposFirstPlayer->ushRow][ipposFirstPlayer->ushCol] = TANK_SYMBOL;
	imatcBoard[ipposSecondPlayer->ushRow][ipposSecondPlayer->ushCol] = TANK_SYMBOL;

	// Initialize Plays in play to be 0
	*ipushAmountOfPlays = 0;
}


//-----------------------------------------------------------------------------------------------
//										 Print Board
//										-------------
//
// General		: This procedure print the current game board.
//
// Parametes	: 
//				imatcBoard  - The game board. (In)
//				iushAmountOfPlays - A variable that contains amount of plays until now. (In)
//				iushAmountOfWalls - A variable that contains amount of walls left in the board. (In)
//
// Return Value : None. The procedure print the current board.
//
//-----------------------------------------------------------------------------------------------
void PrintBoard(char imatcBoard[ROWS][COLS], size_s iushAmountOfPlays, size_s iushAmountOfWalls)
{
	// Variable definition
	size_s i, j;

	// Code Section

	Sleep(UPDATE_BOARD_WAITING);
	system("cls");

	/* print indexes of columns for the user's convenience */
	printf("  ");
	for (i = 0; i < COLS; i++)
	{
		/* 2 - for a good display, each character/number we print will occupy two places in the print*/
		printf("\033[0;36m %-4hu\033[0m", i);
	}
	puts("");

	/* print game board */
	for (i = 0; i < ROWS; i++)
	{
		printf("\033[0;36m%-2hu \033[0m", i); /* index of row */

		for (j = 0; j < COLS; j++)
		{
			/* tanks with green color */
			if (imatcBoard[i][j] == TANK_SYMBOL)
			{
				printf("\033[0;32m%-2c\033[0m | ", TANK_SYMBOL);
			}

			/* breakable walls with pink color */
			else if (imatcBoard[i][j] == WALLS_SYMBOL)
			{
				printf("\033[0;35m%-2c\033[0m | ", WALLS_SYMBOL);
			}
			/* shoot with red color */
			else if(imatcBoard[i][j] == SHOOT_SIGN)
			{
				printf("\033[0;31m%-2c\033[0m | ", SHOOT_SIGN);
			}
			/* free square - no color */
			else
			{
				printf("%-2c | ", imatcBoard[i][j]);
			}
		}
		puts("");

		/* print line just for visualization of the board */
		printf("  ");
		for (j = 0; j < COLS; j++)
		{
			printf("----+");
		}
		puts("");
	}

	/* Print amount of walls left in the game board */
	printf("There are %hu walls left in the game! \n\n\n", iushAmountOfWalls);

	/* print whos turn it is now */
	if (iushAmountOfPlays & 1)
	{
		printf("\033[0;34mIt's right tank turn!\033[0m\n");
	}
	else
	{
		printf("\033[0;32mIt's left tank turn!\033[0m\n");
	}
	printf("\n\n");
}

//-----------------------------------------------------------------------------------------------
//											Menu
//										-------------
//
// General		: This procedure print the menu available to the player in his turn.
//					1 - move on board
//					2 - normal shooting
//					3 - shooting in alternating direction
//					4 - dropping random bomb
//					STOP_SIGN_INPUT - stop the system
//
// Parametes	: None.
//
// Return Value : 
//				  oshPlayerChoice - The move the player wants to play. (out)
//
//-----------------------------------------------------------------------------------------------
size_s Menu()
{
	// Variable definition
	size_s oshPlayerChoice;
	Bool bIsValidInput;

	// Code Section
	printf("\nPlease choose one of the following actions to play.\n\n");
	printf("\t 1 - move on board \n");
	printf("\t 2 - normal shooting \n");
	printf("\t 3 - shooting in alternating direction \n");
	printf("\t 4 - dropping random bomb \n");
	printf("\t %hu - stop the system \n\n", STOP_GAME_SIGN);

	do
	{
		printf("Your choice: ");
		// check if the input success and the correctness (if it was one of the option)
		// if the input didn't success, it will stop there (this is the first phrase to be tested)
		bIsValidInput = scanf("%hu", &oshPlayerChoice) == 1 && ((oshPlayerChoice >= 1 && oshPlayerChoice <= 4) || oshPlayerChoice == STOP_GAME_SIGN);
		ClearInputBuffer();  // Clear the input buffer
	} while (!bIsValidInput);

	// return the player legal choice
	return oshPlayerChoice;
}

//-----------------------------------------------------------------------------------------------
//										Direction Menu
//										-------------
//
// General		: This procedure print the menu of directions the player can move to.
//					1 - up
//					2 - right
//					3 - down
//					4 - left
//
// Parametes	: None.
//
// Return Value : 
//				  oshPlayerChoice - The direction the player wants to move. (out)
//
//-----------------------------------------------------------------------------------------------
size_s DirectionMenu()
{
	// Variable definition
	size_s oshPlayerChoice;
	Bool bIsValidInput;

	// Code Section
	printf("\nPlease choose one of the following directions to play.\n\n");
	printf("\t 1 - up \n");
	printf("\t 2 - right \n");
	printf("\t 3 - down \n");
	printf("\t 4 - left \n\n");

	do
	{
		printf("Your choice: ");
		// check if the input success and the correctness (if it was one of the option)
		// if the input didn't success, it will stop there (this is the first phrase to be tested)
		bIsValidInput = scanf("%hu", &oshPlayerChoice) == 1 && (oshPlayerChoice >= 1 && oshPlayerChoice <= 4);
		ClearInputBuffer();  // Clear the input buffer
	} while (!bIsValidInput);

	// return the player legal choice
	return oshPlayerChoice;
}


//-----------------------------------------------------------------------------------------------
//										  Move Tank
//										-------------
//
// General		: This procedure gets from current player the move direction and amount of places to move.
//				  It will update the tank place on board.
//				  The tank will move as he want unless he had previously reached a wall, 
//				  the second tank, or one of the edges of the board.
//
// Parametes	: 
//				imatcBoard		    - The game board. (In)
//				ipposCurrentPlayer   - A pointer to the position of the current tank to play. (In)
//				ipushAmountOfPlays  - A pointer to a variable contains amount of plays until now. (In)
//				iushAmountOfWalls   - A variable that contains amount of walls left in the board. (In)
//
// Return Value : None.
//
//-----------------------------------------------------------------------------------------------
void MoveTank(char imatcBoard[ROWS][COLS], Pos* ipposCurrentPlayer, size_s* ipushAmountOfPlays, size_s iushAmountOfWalls)
{
	// Variable definition
	size_s ushDirection, ushPlacesToMove;

	// Code Section

	// get the direction the player want to move into
	ushDirection = DirectionMenu();

	// get the places he want to move in that direction
	do 
	{
		printf("Please enter the amount of squares you would like to move on the board: ");
	} while (scanf("%hu", &ushPlacesToMove) != 1);

	ClearInputBuffer();  // Clear the input buffer

	// Moving the current player
	MovingTank(imatcBoard, ipposCurrentPlayer, ushDirection, ushPlacesToMove, *ipushAmountOfPlays, iushAmountOfWalls);
}

//-----------------------------------------------------------------------------------------------
//										Moving Tank
//										-------------
//
// General		: This procedure gets the board game, current player, direction to move and places
//				  on board. It updates the game board and current player position.
//
// Parametes	: 
//				imatcBoard		     - The game board. (In)
//				ipposCurrentPlayer   - A pointer to the position of the current tank to play. (In)
//				iushDirection		 - Direction the player want to move on. (In)
//				iushPlacesToMove	 - Squares to move on board. (In)
//				iushAmountOfPlays	 - A variable that contains amount of plays until now. (In)
//				iushAmountOfWalls    - A variable that contains amount of walls left in the board. (In)
//
// Return Value : None.
//
//-----------------------------------------------------------------------------------------------
void MovingTank(char imatcBoard[ROWS][COLS], Pos* ipposCurrentPlayer, size_s iushDirection, size_s iushPlacesToMove, size_s iushAmountOfPlays, size_s iushAmountOfWalls)
{
	// Variable definition
	size_s ushCheckedRow, ushCheckedCol;

	// Code Section

	// Initialize checked row and col to the current position of row and col
	ushCheckedRow = ipposCurrentPlayer->ushRow;
	ushCheckedCol = ipposCurrentPlayer->ushCol;

	// look for new place for the tank until blocked or finish places to move
	// we will do it with animation to upgrade the game experienced

	while (iushPlacesToMove-- && IsFreeSquare(imatcBoard, ushCheckedRow + arrshDirections[iushDirection - 1].shRow, ushCheckedCol + arrshDirections[iushDirection - 1].shCol))
	{
		// for the animation, for each legal square we will update the board and print it
		
		// delete the last place of the tank
		imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;

		// update to new place the tank can go
		ushCheckedRow += arrshDirections[iushDirection - 1].shRow;
		ushCheckedCol += arrshDirections[iushDirection - 1].shCol;

		// update the game board too and print it
		imatcBoard[ushCheckedRow][ushCheckedCol] = TANK_SYMBOL;
		PrintBoard(imatcBoard, iushAmountOfPlays, iushAmountOfWalls);

	}

	// Eventually we will update the position of the player also in the variable
	ipposCurrentPlayer->ushRow = ushCheckedRow;
	ipposCurrentPlayer->ushCol = ushCheckedCol;
}
//-----------------------------------------------------------------------------------------------
//										 Is On Board
//										-------------
// General		: The function checks whether a position is on the board or not.
//
// Parametes	: 
//			iushRow		- row of the position (In)
//			iushCol		- col of the position (In)
//
// Return Value : It returns true if (row, col) is on the game board.
//				  Otherwise, it returns false.
//
//-----------------------------------------------------------------------------------------------
Bool IsOnBoard(size_s iushRow, size_s iushCol)
{
	if (iushRow < 0 || iushCol < 0 || iushRow >= ROWS || iushCol >= COLS)
	{
		return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------------------------
//										 Is Free Square
//										----------------
// General		: The function checks whether a position is on the board or not, and also if 
//				  it's an empty square.
//
// Parametes	: 
//			imatcBoard  - The game board. (In)
//			iushRow		- row of the position (In)
//			iushCol		- col of the position (In)
//
// Return Value : It returns true if (row, col) is on the game board and empty square.
//				  Otherwise, it returns false.
//
//-----------------------------------------------------------------------------------------------
Bool IsFreeSquare(char imatcBoard[ROWS][COLS], size_s iushRow, size_s iushCol)
{
	// Code Section

	// check board limits and if the square is empty
	if (!IsOnBoard(iushRow, iushCol) || imatcBoard[iushRow][iushCol] != EMPTY_SYMBOL)
	{
		return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------------------------
//										 Clear Input
//										-------------
//
// General		: This procedure clear the input buffer, to avoid errors in reading the input.
//
// Parametes	: None.
//
// Return Value : None. 
//
//-----------------------------------------------------------------------------------------------
void ClearInputBuffer()
{
	// Variable definition
	char c;

	// Code section 

	while ((c = getchar()) != '\n' && c != EOF);
}

//-----------------------------------------------------------------------------------------------
//										   Normal Shoot
//										-----------------
// General		: The function activate normal shoot.
//
// Parametes	: 
//			imatcBoard			- The game board. (In)
//			iushDirection		- Direction of the shoot. (In)
//			iushAmountOfPlays	- A variable that contains amount of plays until now. (In)
//			ipposShootLocation	- A pointer to the current location of the shoot. (In)
//			ipushAmountOfWalls - A pointer to a variable contains amount of walls left in the board. (In)
//
// Return Value : It returns the status of the shoot:
//					1 - The shoot hit a breakable wall
//					2 - The shoot hit the other player
//					3 - The shoot hit one of the edges
//
//-----------------------------------------------------------------------------------------------
size_s NormalShoot(char imatcBoard[ROWS][COLS], size_s iushDirection, size_s iushAmountOfPlays, Pos* ipposShootLocation, size_s* ipushAmountOfWalls)
{
	// Variable definition
	size_s ushCheckedRow, ushCheckedCol;

	// Code Section

	// Initialize checked row and col to the current position of row and col
	ushCheckedRow = ipposShootLocation->ushRow;
	ushCheckedCol = ipposShootLocation->ushCol;

	// Continue until hit something

	while (IsFreeSquare(imatcBoard, ushCheckedRow + arrshDirections[iushDirection - 1].shRow, ushCheckedCol + arrshDirections[iushDirection - 1].shCol))
	{
		// for the animation, for each legal square we will update the board and print it

		// delete the last place of the shoot
		if (imatcBoard[ushCheckedRow][ushCheckedCol] == SHOOT_SIGN)
		{
			imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
		}

		// update to new place the shoot can go
		ushCheckedRow += arrshDirections[iushDirection - 1].shRow;
		ushCheckedCol += arrshDirections[iushDirection - 1].shCol;

		// update the game board too and print it
		imatcBoard[ushCheckedRow][ushCheckedCol] = SHOOT_SIGN;
		PrintBoard(imatcBoard, iushAmountOfPlays, *ipushAmountOfWalls);
	}

	// We will update the position of the shoot also in the variable
	// We are doing it in a case 3, so we will be able to use again this method in alternating direction
	ipposShootLocation->ushRow = ushCheckedRow;
	ipposShootLocation->ushCol = ushCheckedCol;

	// Delete the shoot sign from the board in a case it was there
	if (imatcBoard[ushCheckedRow][ushCheckedCol] == SHOOT_SIGN)
	{
		imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
	}

	// Update the variables for check more easily the conditions
	ushCheckedRow += arrshDirections[iushDirection - 1].shRow;
	ushCheckedCol += arrshDirections[iushDirection - 1].shCol;

	// If the reason the loop stop is because we out of bounds we will return 3
	if (!IsOnBoard(ushCheckedRow, ushCheckedCol))
	{
		return 3;
	}

	// If the next square was the other player
	if (imatcBoard[ushCheckedRow][ushCheckedCol] == TANK_SYMBOL)
	{
		imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
		PrintBoard(imatcBoard, iushAmountOfPlays, *ipushAmountOfWalls);
		return 2;
	}

	// Otherwise, it hits a breakable wall 
	(*ipushAmountOfWalls)--;
	imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
	PrintBoard(imatcBoard, iushAmountOfPlays, *ipushAmountOfWalls);
	return 1;
}

//-----------------------------------------------------------------------------------------------
//										  Random Bomb
//										-----------------
// General		: The function gets from the user row and col to drop a bomb at this location.
//
// Parametes	: 
//			imatcBoard			- The game board. (In)
//			ipushAmountOfWalls  - A pointer to a variable contains amount of walls left in the board. (In)
//			ipposCurrentPlayer  - A pointer to the position of the current tank to play. (In)
//			iushAmountOfPlays	- A variable that contains amount of plays until now. (In)
//
// Return Value : It returns the status of the game:
//					1 - The bomb didn't damage any tank
//					2 - The bomb hit the other tank    (win)
//					3 - The bomb hit the current tank  (lose)
//					4 - The bomb hit both tanks		   (draw)
//
//-----------------------------------------------------------------------------------------------
size_s RandomBomb(char imatcBoard[ROWS][COLS], size_s* ipushAmountOfWalls, Pos* ipposCurrentPlayer, size_s iushAmountOfPlays)
{
	// Matrix definition
	size_s bomb[BOMB_DIMENSION][BOMB_DIMENSION];
	
	// Variable definition
	size_s i, j, validateInput, gameStatus = 1;
	Pos initialBombLocation, initialPlayerLocation;

	// Random either 1 or 0 for any square in the bomb
	for (i = 0; i < BOMB_DIMENSION; i++)
	{
		for (j = 0; j < BOMB_DIMENSION; j++)
		{
			bomb[i][j] = (size_s)(rand() & 1);
		}
	}

	// Getting from the user row and col for bomb. There is no need to check limits, because we allow the bomb to get out of board
	do
	{
		validateInput = 0;
		printf("Please enter row for middle of the bomb: ");
		validateInput += scanf("%hu", &initialPlayerLocation.ushRow);
		printf("Please enter col for middle of the bomb: ");
		validateInput += scanf("%hu", &initialPlayerLocation.ushCol);
	} while (validateInput != 2);

	// Initializing the first bomb location to the middle
	initialBombLocation.ushRow = 1;
	initialBombLocation.ushCol = 1;

	// Starting to scan each cell in bomb versus each cell on board for each direction (bomb dimensions are always 3*3)
	for (i = 0; i < DIRECTIONS_LENGTH; i++)
	{	
		// If that cell in bomb is on, we will check the relative cell on board
		if (bomb[initialBombLocation.ushRow + arrshDirections[i].shRow][initialBombLocation.ushCol + arrshDirections[i].shCol])
		{
			// Variable definition
			
			// save values in variables for readability of the code
			size_s ushCheckedRow, ushCheckedCol;

			ushCheckedRow = initialPlayerLocation.ushRow + arrshDirections[i].shRow;
			ushCheckedCol = initialPlayerLocation.ushCol + arrshDirections[i].shCol;

			if (IsOnBoard(ushCheckedRow, ushCheckedCol) && imatcBoard[ushCheckedRow][ushCheckedCol] != EMPTY_SYMBOL)
			{
				// In a case of breakable wall
				if (imatcBoard[ushCheckedRow][ushCheckedCol] == WALLS_SYMBOL)
				{
					imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
					(*ipushAmountOfWalls)--;
				}

				// Otherwise, check if the gameStatus isn't one, meaning we hit both tanks
				else if (gameStatus != 1)
				{
					imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;
					gameStatus = 4;
				}

				// Otherwise, we must check which tank the bomb hit
				else
				{
					imatcBoard[ushCheckedRow][ushCheckedCol] = EMPTY_SYMBOL;

					// If the bomb hit the current player
					if (ushCheckedRow == ipposCurrentPlayer->ushRow && ushCheckedCol == ipposCurrentPlayer->ushCol)
					{
						gameStatus = 3;
					}
					else
					{
						gameStatus = 2;
					}
				}
			}
		}
	}

	PrintBoard(imatcBoard, iushAmountOfPlays, *ipushAmountOfWalls);
	return gameStatus;
}