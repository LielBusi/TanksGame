// TanksGame.h

#pragma once
#pragma warning(disable: 4996)

// Const definition
#define ROWS						11
#define COLS						11
#define TANK_SYMBOL					'O'
#define WALLS_SYMBOL				'X'
#define EMPTY_SYMBOL				'_'
#define SHOOT_SIGN					'!'
#define STOP_GAME_SIGN				999
#define UPDATE_BOARD_WAITING		400
#define BOMB_DIMENSION				3
#define DIRECTIONS_LENGTH			9

// Typedef declaration
typedef enum Boolean				Bool;
typedef unsigned short				size_s;
typedef struct Direction			Location;
typedef struct PositionPlayer		Pos;

// Struct declaration
struct Direction {
	// optimal allignment
	short shRow;
	short shCol;
}; 

struct PositionPlayer {
	// optimal allignment
	size_s ushRow;
	size_s ushCol;
};

// Enum	declaration
enum Boolean
{
	FALSE = 0,
	TRUE = 1
};

// Function prototypes

void InitBoard(char imatcBoard[ROWS][COLS], Pos* ipposFirstPlayer, Pos* ipposSecondPlayer, size_s* ipushAmountOfPlays, size_s* ipushAmountOfWalls);
void PrintBoard(char imatcBoard[ROWS][COLS], size_s iushAmountOfPlays, size_s iushAmountOfWalls);
size_s Menu();
size_s DirectionMenu();
void MoveTank(char imatcBoard[ROWS][COLS], Pos* ipposCurrentPlayer, size_s* ipushAmountOfPlays, size_s iushAmountOfWalls);
void MovingTank(char imatcBoard[ROWS][COLS], Pos* ipposCurrentPlayer, size_s iushDirection, size_s iushPlacesToMove, size_s iushAmountOfPlays, size_s iushAmountOfWalls);
Bool IsFreeSquare(char imatcBoard[ROWS][COLS], size_s iushRow, size_s iushCol);
void ClearInputBuffer();
size_s NormalShoot(char imatcBoard[ROWS][COLS], size_s iushDirection, size_s iushAmountOfPlays, Pos* ipposShootLocation, size_s* ipushAmountOfWalls);
size_s RandomBomb(char imatcBoard[ROWS][COLS], size_s* ipushAmountOfWalls, Pos* ipposCurrentPlayer, size_s iushAmountOfPlays);
