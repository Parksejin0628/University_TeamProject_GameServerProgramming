#ifndef _GOTCHA_HEADER
#define _GOTCHA_HEADER

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<ncurses.h>
#include<locale.h>
#include<stdbool.h>

#define SHM_ID_SERVER 200
#define SHM_ID_PLAYER 201

#define MAX_PLAYER_COUNT 2 
#define MAX_TURN 20
#define MAX_NAME_LENGTH 20
#define SCREEN_WIDTH 60
#define SCREEN_HEIGHT 40
#define MAP_COUNT 3

typedef enum FieldData
{
	EMPTY = -3 , BLOCK, SPIKE, CHASER = 0, RUNNER = 1
}FieldData;

typedef enum Direction
{
	NONE, UP, DOWN, RIGHT, LEFT
}Direction;

typedef enum Scene
{
	LOBBY, INSTRUCTION, SELECTMAP, INGAME, WIN_CHASER, WIN_RUNNER
}Scene;

typedef enum Message
{
	NO_MESSAGE,
	/*server -> client*/ UPDATE_SCREEN, UPDATE_SCREEN_AND_INPUT
	/*client -> server*/ , ENTER_GAME, END_INPUT, PRINT_LOG
}Message;

typedef struct ServerData
{
	int shm_id;
	int pid;
	int mapIndex;
	int playerCount;
	int turn;
	bool isGameOver;
	bool canInput;
	Message message[MAX_PLAYER_COUNT];
	FieldData fieldData[SCREEN_HEIGHT][SCREEN_WIDTH];
	Scene nowScene;
}ServerData;

typedef struct PlayerData
{
	int shm_id;
	int pid;
	int x;
	int y;
	int role; //매크로 CHASER 혹은 RUNNER
	Message message;
	char name[20];
	Direction direction;
}PlayerData;


#endif

