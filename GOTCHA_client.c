#include"GOTCHA_header.h"

ServerData *serverData;
PlayerData *playerData;

int ConnectServer();
int InitClient();
int SendMessageToServer(int message);
int ReceiveMessage();
int PerformMessage(Message msg);
int InputManager();
void DrawScreen();
void PrintLogo(int x, int y);
void PrintInstruction();
void MainScene();
void MapScene();
void ExplainScene();
void InGameScene();
void OverScene();

int main(int argc, char *argv[])
{
	InitClient();
	while(!serverData->isGameOver)
	{

	}
	endwin();
	return 0;
}

int InitClient()
{ 
	char msg[] = "플레이어의 이름을 무엇으로 하시겠습니까?(8글자 미만) : ";
	char inputID[20];
	int input = 0;

	//초기 화면 설정
	for(int y=0; y<SCREEN_HEIGHT - 4; y++)
	{
		for(int x=0; x<SCREEN_WIDTH; x++)
		{
			printf("■");
		}
		printf("\n");
	}
	printf("다음 사각형이 화면 안에 모두 들어오도록 Ctrl + 휠을 사용하여 글자 사이즈를 조정하세요.\n");
	printf(" 크기 조정을 마쳤다면 아무 키나 입력하세요.\n");
	scanf("%d", &input);	
	//화면 출력 관련 초기화
	setlocale(LC_ALL,"ko_KR.utf8"); 		
	initscr();
	start_color();//사용할 색 페어로 저장, 글자색 + 배경색
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_BLUE);
	keypad(stdscr, TRUE);
	//서버 관련 초기화
	ConnectServer();
	signal(SIGUSR1,ReceiveMessage);
	
	//초기 ID 입력
	mvprintw(LINES/2, 5,"%s \n",msg);
	mvscanw(LINES/2 + 1, 5, "%s", inputID);
	strcpy(playerData->name, inputID);
	mvprintw(LINES - 2, 0, "YOUR ID : %s", inputID);
	getch();
	cbreak();
	curs_set(0); // 세지나 이거 말하는거다 이거 iD입력후 게임 시작 전에 넣어야해!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	noecho();
	clear();
	printw("Test");
	//playerData 초기화
	playerData->pid = getpid();
	SendMessageToServer(ENTER_GAME);	
}

int ConnectServer()
{
	key_t shm_key;
	int shm_id;
	int playerNumber = -1;
	printw("Connect serverData...\n");
	shm_key = ftok("/home/g_202010948/TeamProject/",SHM_ID_SERVER);
	if(shm_key == -1)
	{
		printw("SHM - Key error");
		endwin();
		exit(1);
	}
	shm_id = shmget(shm_key, sizeof(ServerData), 0);
	if(shm_id == -1)
	{
		printw("SHM - ID error");
		endwin();	
		exit(2);
	}
	serverData = (ServerData*)shmat(shm_id, NULL, 0);
	if(serverData == (ServerData*)-1)
	{
		printw("SHM - Address error");
		endwin();
		exit(3);
	}
	printw("Connect serverData Success\n");
	
	//만약 플레이어가 가득 찰 경우
	if(serverData->playerCount >= MAX_PLAYER_COUNT)
	{
		printw("Full Player!");
		getch();
		endwin();
		exit(6);
	}
	shm_key = ftok("/home/g_202010948/TeamProject/", SHM_ID_PLAYER + serverData->playerCount);
	serverData->playerCount++;
	if(shm_key == -1)
	{
		printw("SHM_Player - Key error");
		endwin();
		exit(4);
	}
	shm_id = shmget(shm_key, sizeof(PlayerData), 0);
	if(shm_id == -1)
	{
		printw("SHM - ID error");
		getch();
		endwin();	
		exit(7);
	}
	playerData = (PlayerData*)shmat(shm_id, NULL, 0);
	if(playerData == (PlayerData*)-1)
	{
		printw("SHM - Address error");
		getch();
		endwin();
		exit(8);
	}
	printw("Connect playerData Success\n");
	getch();
	clear();
}

int SendMessageToServer(int message)
{
	playerData->message = message;
	kill(serverData->pid, SIGUSR1);
}

int ReceiveMessage()
{
	static Message temp;
	temp = serverData->message[playerData->role];
	serverData->message[playerData->role] = NO_MESSAGE;
	PerformMessage(temp);
}

int PerformMessage(Message msg)
{
	if(msg == UPDATE_SCREEN)
	{
		DrawScreen();
	}
	else if(msg == UPDATE_SCREEN_AND_INPUT)
	{
		DrawScreen();
		InputManager(); 
	}
}

int InputManager()
{
	int key;
	SendMessageToServer(PRINT_LOG);
	while(serverData->canInput)
	{
		key = getch();
		if(serverData->nowScene == LOBBY)
		{
			if(key == 'f' || key == ' ')
			{
				SendMessageToServer(END_INPUT);
				break;
			}
			key = 0;
		}
		else if(serverData->nowScene == INSTRUCTION)
		{
			if(key == 'f' || key == ' ')
			{
				SendMessageToServer(END_INPUT);
				break;
			}
			key = 0;
		}
		else if(serverData->nowScene == SELECTMAP)
		{
			if(key == '1')
			{
				serverData->mapIndex = 1;
				SendMessageToServer(END_INPUT);
				break;
			}
			else if(key == '2')
			{
				serverData->mapIndex = 2;
				SendMessageToServer(END_INPUT);
				break;
			}
			else if(key == '3')
			{
				serverData->mapIndex = 3;
				SendMessageToServer(END_INPUT);
				break;
			}
			key = 0;
		}
		else if(serverData->nowScene == INGAME)
		{
			if(key == KEY_LEFT)
			{
				playerData->direction = LEFT;
				SendMessageToServer(END_INPUT);
				break;
			}
			else if(key == KEY_RIGHT)
			{
				playerData->direction = RIGHT;
				SendMessageToServer(END_INPUT);
				break;
			}
			else if(key == KEY_UP)
			{
				playerData->direction = UP;
				SendMessageToServer(END_INPUT);
				break;
			}
			else if(key == KEY_DOWN)
			{
				playerData->direction = DOWN;
				SendMessageToServer(END_INPUT);
				break;
			}
			key = 0;
		}
	}
}

void DrawScreen()
{
	switch(serverData->nowScene)
	{
		case LOBBY:
			MainScene();
			break;
		case INSTRUCTION:
			clear();
			ExplainScene();
			break;
		case SELECTMAP:
			clear();
			MapScene();
			break;
		case INGAME:
			InGameScene();
			break;
		case WIN_CHASER:
			OverScene(CHASER);
			break;
		case WIN_RUNNER:
			OverScene(RUNNER);
			break;
	}
}

void PrintLogo(int starty, int startx){
	FILE* logo;
	char ch;
	int x, y;
			
	logo = fopen("Logo.txt", "r");	

	if(logo == NULL){
		printw(" error - file");
		getch();
		endwin();
		return;
	}
	move(starty,startx);
	
	while((ch = getc(logo)) != EOF){
		if(ch == '0'){
			printw(" ");
		}else if(ch == '1'){
			addch(ACS_CKBOARD);
		} else if (ch == '\n') {
            printw("\n");
			move(starty++,startx);
        } else {
            printw("%c", ch);  // 다른 문자 출력
        }
    }

    fclose(logo);
    refresh();

	for(y = 0; y < SCREEN_HEIGHT; y++){
		mvaddch(y, 0, ACS_CKBOARD);
		mvaddch(y, SCREEN_WIDTH - 1, ACS_CKBOARD);
		refresh();
	}
	for(x = 0; x < SCREEN_WIDTH; x++){
		mvaddch(0, x, ACS_CKBOARD);
		mvaddch(SCREEN_HEIGHT - 1, x, ACS_CKBOARD);
		refresh();
	}
}

void PrintInstruction(){	
	FILE *fp;
	char buffer[80];
	int line = 0;

	fp = fopen("instruction.txt", "r");
	while(fgets(buffer, 80, fp)!=NULL){
		mvprintw(14 + line, 3, buffer);
		line++;
	}
	return;
}

void MainScene(){
	char* msg = "[ 2인용 추격 & 생존 미니게임 ]";
	PrintLogo(5,0);

	mvprintw(13,(SCREEN_WIDTH - strlen(msg))/2 + 5,"%s", msg);

	msg = "-PRESS ANY KEY TO STRRT-";
    mvprintw(22,(SCREEN_WIDTH - strlen(msg))/2 ,"%s", msg);
	refresh();
}

void ExplainScene(){
	char* msg = "[ Instruction ]";
	mvprintw(13,(SCREEN_WIDTH - strlen(msg))/2,"%s", msg);

	PrintInstruction();
	//msg = "플레이어는 ‘방향키’ 입력 시,\n    해당 방향으로 오브젝트가 없으면 계속 직진함.";
	//mvprintw(17,3 ,"%s", msg);

	PrintLogo(5,0);
	refresh();
}

void MapScene(){
	char* msg = "[ 게임을 진행할 ‘Map’을 고르세요! ]";
	mvprintw(13,(SCREEN_WIDTH - strlen(msg))/2 + 7 ,"%s", msg);

	msg = "[ Press ‘1’: 점선 ]";
	mvprintw(17,3 ,"%s", msg);	
	msg = "[ Press ‘2’: 데칼코마니 ]";
	mvprintw(19,3 ,"%s", msg);
	msg = "[ Press ‘3’: 흩뿌려진 강박증 ]";
	mvprintw(21,3 ,"%s", msg);
	PrintLogo(5,0);
	refresh();
}

void InGameScene()
{
	int x, y;
	static bool is_init = false;
	static int stageBefore[SCREEN_HEIGHT][SCREEN_WIDTH];
	
	if(!is_init){
		for(y = 0; y < SCREEN_HEIGHT; y++){
			for(x = 0; x < SCREEN_WIDTH; x++){
				stageBefore[y][x] = 100;
			}
		}
		is_init = true;
	}
	for(y = 0; y < SCREEN_HEIGHT; y++){
		for(x = 0; x < SCREEN_WIDTH; x++){
			if(serverData->fieldData[y][x] != stageBefore[y][x]){
				move(y,x);
				switch(serverData->fieldData[y][x]){
					case EMPTY:
						printw(" ");
						refresh();
						break;
					case BLOCK:
						addch(ACS_CKBOARD);
						refresh();
						break;
					case CHASER:
						attron(COLOR_PAIR(1));
						addch(ACS_DIAMOND);
						attroff(COLOR_PAIR(1));
						refresh();
						break;
					case RUNNER:
						attron(COLOR_PAIR(2));
						addch(ACS_DIAMOND);
						attroff(COLOR_PAIR(2));
						refresh();
						break;
					case SPIKE:
						addch(ACS_DIAMOND);
						refresh();
						break;
				}
			}
			stageBefore[y][x] = serverData->fieldData[y][x]; 
		}
	}
	refresh();
}

void OverScene(int n) {
    const char* msg1 = "※ 아무 키나 눌러 메인화면으로 이동 ※";
    const char* msg2 = (n == CHASER) ? "[ 1ND PLAYER 승리! ]" : "[ 2ND PLAYER 승리! ]";
	clear();
	
    mvprintw(21, (SCREEN_WIDTH - strlen(msg1))/2 + 9, "%s", msg1);
	
	move(19, (SCREEN_WIDTH - strlen(msg2))/2 + 2);
    // 승리 메시지 출력
    attron(COLOR_PAIR(n));
    addch(ACS_DIAMOND);
    attroff(COLOR_PAIR(n));
	printw("%s", msg2); 
	refresh();

    PrintLogo(11, 0);
}
