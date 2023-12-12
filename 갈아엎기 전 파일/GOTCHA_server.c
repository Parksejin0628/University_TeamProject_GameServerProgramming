#include"GOTCHA_header.h"

ServerData *serverData;
PlayerData *playerData[MAX_PLAYER_COUNT];

int InitServer();	
int CreateSharedMemory();
int SendMessageToClient(int playerIndex, Message msg);
int ReceiveMessage();
int PerformMessage(Message msg);
int PlayerMove();
int LoadMap();
void PrintLog(char *log);
int DeleteSharedMemory();

int main(void)
{
	char nowTime[20];
	InitServer();

	for(int i=1; i<=10000; i++)
	{
		sprintf(nowTime, "정기 로그 %d번째", i);	//로그에 들어갈 문자열 
		//PrintLog(nowTime);
		sleep(1);
	}

	return 0;
}

int InitServer()
{
	signal(SIGUSR1, ReceiveMessage);
	CreateSharedMemory();
	serverData->pid = getpid();
	serverData->mapIndex = 1;
	serverData->playerCount = 0;
	serverData->turn = 0;
	serverData->isGameOver = false;
	serverData->canInput = true;
	serverData->nowScene = LOBBY;
	for(int i=0; i<MAX_PLAYER_COUNT; i++)
	{
		serverData->message[i] = NO_MESSAGE;
		playerData[i]->x = 0;
		playerData[i]->y = 0;
		playerData[i]->direction = NONE;
		strcpy(playerData[i]->name, "unnamed");
		if((i&1) == 0) // i가짝수인 경우 
		{
			playerData[i]->role = CHASER;
		}
		else
		{
			playerData[i]->role = RUNNER;
		}
	}

	printf("Init ServerSucess\n");
}

int CreateSharedMemory()
{
	key_t shm_key;
	int shm_id;
	/******Create Server Shared Memory*****/
	printf("Create SharedData_server...\n");
	shm_key = ftok("/home/g_202010948/TeamProject/", SHM_ID_SERVER);
	if(shm_key == -1)
	{
		perror("SHM - Key error");
		exit(1);
	}
	shm_id = shmget(shm_key, sizeof(ServerData), IPC_CREAT|0666);
	printf("%d\n", sizeof(ServerData));
	if(shm_id == -1)
	{
		perror("SHM - ID error");
		exit(2);
	}
	serverData = (ServerData*)shmat(shm_id, NULL, 0);
	if(serverData == (ServerData*)-1)
	{
		perror("SHM - Address error");
		exit(3);
	}
	serverData->shm_id = shm_id;
	printf("ServerData succecc\n");
	

	/***** Create Player Shared Memory*****/
	for(int i=0; i<MAX_PLAYER_COUNT; i++)
	{
		printf("Create player%dData...\n", i+1);
		shm_key = ftok("/home/g_202010948/TeamProject/", SHM_ID_PLAYER + i);
		if(shm_key == -1)
		{
			perror("Player, SHM - Key error");
			exit(1);
		}
		shm_id = shmget(shm_key, sizeof(PlayerData), IPC_CREAT|0666);
		if(shm_id == -1)
		{
			perror("Player, SHM - ID error");
			exit(2);
		}
		playerData[i] = (PlayerData*)shmat(shm_id, NULL, 0);
		if(playerData[i] == (PlayerData*)-1)
		{
			perror("Player, SHM - Address error");
			exit(3);
		}
		printf("Player%dData Success\n", i + 1);
		playerData[i]->shm_id = shm_id;
	}
}

void PrintLog(char* log)
{
	static int logNumber;
	logNumber++;
	printf("\n-----------------[LOG%d]-------------------\n", logNumber);
	printf("%s\n", log);
	printf("[Server]\n");
	printf("pid : %d\n", serverData->pid);
	printf("mapIndex : %d\n", serverData->mapIndex);
	printf("playerCount : %d\n", serverData->playerCount);
	printf("turn : %d\n", serverData->turn);
	printf("isGameOver : %s\n", serverData->isGameOver ? "True" : "False");
	printf("canInput : %s\n", serverData->canInput ? "True" : "False");
	for(int i=0; i<MAX_PLAYER_COUNT; i++)
	{
		switch(serverData->message[i])
		{
			case NO_MESSAGE:
				printf("message%d : NO_MESSAGE\n", i+1);
				break;
			case UPDATE_SCREEN:
				printf("message%d : UPDATE_SCREEN\n", i+1);
				break;
			case UPDATE_SCREEN_AND_INPUT:
				printf("message%d : UPDATE_SCREEN_AND_INPUT\n", i+1);
				break;
			default:
				printf("message%d : UNKNOWN_MESSAGE\n", i+1);
		}
	}
	switch(serverData->nowScene)
	{
		case LOBBY:
			printf("Scene : LOBBY\n");
			break;
		case INSTRUCTION:
			printf("Scene : INSTRUCTION");
			break;
		case SELECTMAP:
			printf("Scene : SELECTMAP");
			break;
		case INGAME:
			printf("Scene : INGAME");
			break;
		case WIN_CHASER:
			printf("Scene : WIN_CHASER");
			break;
		case WIN_RUNNER:
			printf("Scene : WIN_RUNNER");
			break;
		default:
			printf("Scene : UNKNOWN");
			break;
	}
	printf("\n");	


	for(int i=0; i<MAX_PLAYER_COUNT; i++)
	{
		printf("[player%d]\n", i+1);
		if(serverData->playerCount > i)
		{
			printf("isConnect : True\n");
		}
		else
		{
			printf("isConnect : false\n");
		}
		switch(playerData[i]->role)
		{
			case CHASER:
				printf("role : CHASER\n");
				break;
			case RUNNER:
				printf("role : RUNNER\n");
				break;
			default:
				printf("role : UNKNOWN ROLE\n");
				break;
		}
		printf("shm_id : %d, pid : %d\n", playerData[i]->shm_id, playerData[i]->pid);
		printf("x : %d, y : %d\n", playerData[i]->x, playerData[i]->y);
		switch(playerData[i]->message)
		{
			case NO_MESSAGE:
				printf("message : NO_MESSAGE\n");
				break;
			case ENTER_GAME:
				printf("message : ENTER_GAME\n");
				break;
			case END_INPUT:
				printf("message : END_INPUT\n");
				break;
			default:
				printf("message : UNKNOWN_MESSAGE\n");
				break;
		}
		switch(playerData[i]->direction)
		{
			case NONE:
				printf("direction : NONE\n");
				break;
			case UP:
				printf("direction : UP\n");
				break;
			case DOWN:
				printf("direction : DOWN\n");
				break;
			case RIGHT:
				printf("direction : RIGHT\n");
				break;
			case LEFT:
				printf("direction : LEFT\n");
				break;
			default:
				printf("direction : UNKNOWN\n");
				break;
		}
		printf("\n");	
	}

}

int SendMessageToClient(int playerIndex, Message msg)
{
	
	serverData->message[playerIndex] = msg;
	PrintLog("클라이언트로 메세지를 보냄");
	kill(playerData[playerIndex]->pid, SIGUSR1);
}

int ReceiveMessage()
{
	Message temp;
	temp = NO_MESSAGE;
	for(int i=0; i<serverData->playerCount; i++)
	{
		if(playerData[i]->message != NO_MESSAGE)
		{
			if(temp != NO_MESSAGE)
			{
				printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
				printf("시그널 중복 문제 발생!\n");
				printf("!!!!!!!!!!!!!!!!!!!!!!!!\n");
			}
			temp = playerData[i]->message;
			playerData[i]->message = NO_MESSAGE;
			PerformMessage(temp);
		}
	}
}

int PerformMessage(Message msg)
{
	static int enterGame;
	if(msg == ENTER_GAME)
	{
		enterGame++;
		if(enterGame == 1)
		{
			SendMessageToClient(CHASER, UPDATE_SCREEN);
		}
		else if(enterGame == MAX_PLAYER_COUNT)
		{
			SendMessageToClient(RUNNER, UPDATE_SCREEN);
			SendMessageToClient(CHASER, UPDATE_SCREEN_AND_INPUT);
		}
	}
	else if(msg == END_INPUT)
	{
		switch(serverData->nowScene)
		{
			case LOBBY:
				serverData->nowScene = INSTRUCTION;
				SendMessageToClient(RUNNER, UPDATE_SCREEN);
				SendMessageToClient(CHASER, UPDATE_SCREEN_AND_INPUT);
				break;
			case INSTRUCTION:
				serverData->nowScene = SELECTMAP;
				SendMessageToClient(RUNNER, UPDATE_SCREEN);
				SendMessageToClient(CHASER, UPDATE_SCREEN_AND_INPUT);
				break;
			case SELECTMAP:
				serverData->nowScene = INGAME;
				LoadMap();
				SendMessageToClient(RUNNER, UPDATE_SCREEN_AND_INPUT);
				SendMessageToClient(CHASER, UPDATE_SCREEN_AND_INPUT);
				break;
			case INGAME:
				if(playerData[CHASER]->direction == NONE || playerData[RUNNER]->direction == NONE)
				{
					break;
				}
				serverData->turn++;
				PlayerMove();
				playerData[CHASER]->direction = NONE;
				playerData[RUNNER]->direction = NONE;
				SendMessageToClient(RUNNER, UPDATE_SCREEN_AND_INPUT);
				SendMessageToClient(CHASER, UPDATE_SCREEN_AND_INPUT);
				break;
		}
	}
	else if(msg == PRINT_LOG)
	{
		PrintLog("클라이언트로 부터 로그 요청");
	}
}

int LoadMap()
{
	FILE* map;
	char line[SCREEN_WIDTH+2];
	if(serverData->mapIndex == 1)
	{
		map = fopen("map1.txt", "r");
	}
	else if(serverData->mapIndex == 2)
	{
		map = fopen("map2.txt", "r");
	}
	else if(serverData->mapIndex == 3)
	{
		map = fopen("map3.txt", "r");
	}
	if(map == (FILE*)-1)
	{
		printf("fopen error\n");
		return -1;
	}
	printf("check1\n");
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		fgets(line, SCREEN_WIDTH+2, map);
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			if(line[x] == '0')
			{
				serverData->fieldData[y][x] = EMPTY;
			}
			else if(line[x] == '1')
			{
				serverData->fieldData[y][x] = BLOCK;
			}
			else if(line[x] == '8')
			{
				serverData->fieldData[y][x] = CHASER;
				for(int i=0; i<serverData->playerCount; i++)
				{
					if(playerData[i]->role == CHASER)
					{
						playerData[i]->x = x;
						playerData[i]->y = y;
					}
				}
			}
			else if(line[x] == '9')
			{
				serverData->fieldData[y][x] = RUNNER;
				for(int i=0; i<serverData->playerCount; i++)
				{
					if(playerData[i]->role == RUNNER)
					{
						playerData[i]->x = x;
						playerData[i]->y = y;
					}
				}
			}	
		}
	}
	fclose(map);
	/*	
	for(int y=0; y<SCREEN_HEIGHT; y++)
	{
		for(int x=0; x<SCREEN_WIDTH; x++)
		{
			if(serverData->fieldData[y][x] == EMPTY)
			{
				printf(" ");
			}
			else if(serverData->fieldData[y][x] == BLOCK)
			{
				printf("1");
			}
			else if(serverData->fieldData[y][x] == CHASER)
			{
				printf("8");
			}
			else if(serverData->fieldData[y][x] == RUNNER)
			{
				printf("9");
			}
		}
		printf("\n");
	}
		*/	
}

int PlayerMove()
{
	int dirX = 0;
	int dirY = 0;
	printf("playerMove Check\n");
	for(int i=0; i<serverData->playerCount; i++)
	{
		switch(playerData[i]->direction)
		{
			case UP:
				dirY = -1;
				break;
			case DOWN:
				dirY = 1;
				break;
			case LEFT:
				dirX = -1;
				break;
			case RIGHT:
				dirX = 1;
				break;
		}
		for(int j=1; j<SCREEN_WIDTH; j++)
		{
			if(serverData->fieldData[playerData[i]->y+dirY*j][playerData[i]->x+dirX*j] == BLOCK)
			{
				serverData->fieldData[playerData[i]->y][playerData[i]->x] = EMPTY;
				if(playerData[i]->role == CHASER)
				{
					serverData->fieldData[playerData[i]->y][playerData[i]->x] = SPIKE;
				}
				serverData->fieldData[playerData[i]->y+dirY*(j-1)][playerData[i]->x+dirX*(j-1)] = playerData[i]->role;
				playerData[i]->x = playerData[i]->x + dirX*(j-1);
				playerData[i]->y = playerData[i]->y + dirY*(j-1);
				break;
			}
			if(serverData->fieldData[playerData[i]->y+dirY*j][playerData[i]->x+dirX*j] == SPIKE)
			{
				serverData->fieldData[playerData[i]->y][playerData[i]->x] = EMPTY;
				if(playerData[i]->role == RUNNER)
				{
					//패배
					serverData->nowScene = WIN_CHASER;
				}
				if(playerData[i]->role == CHASER)
				{
					serverData->fieldData[playerData[i]->y][playerData[i]->x] = SPIKE;
				}
				serverData->fieldData[playerData[i]->y+dirY*(j-1)][playerData[i]->x+dirX*(j-1)] = playerData[i]->role;
				playerData[i]->x = playerData[i]->x + dirX*(j-1);
				playerData[i]->y = playerData[i]->y + dirY*(j-1);
				break;
			}
		}
	}
	for(int i=0; i<serverData->playerCount; i++)
	{
		for(int j=0; j<serverData->playerCount; j++)
		{
			if(i==j)
			{
				continue;
			}
			if(playerData[i]->x == playerData[j]->x && playerData[i]->y == playerData[j]->y)
			{
				if(playerData[i]->role != playerData[i]->role)
				{
					 //패배
					serverData->nowScene = WIN_CHASER;
				}
			}
		}
	}
	if(serverData->turn >= MAX_TURN)
	{
		serverData->nowScene = WIN_RUNNER;
	}
}
