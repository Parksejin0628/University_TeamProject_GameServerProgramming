#include <ncurses.h>
#include <locale.h>
#include <string.h>

#define HEIGHT 34
#define WIDTH 52
void PrintLogo(int x, int y);
void PrintInstruction();

void MainScene();
void MapScene();
void ExplainScene();
void OverScene();

int main(int argc, char *argv[])
{	
	int startx, starty, width, height, x, y;
	int ch;
	char msg[] = "Game start";
	
	setlocale(LC_ALL,"ko_KR.utf8");

	initscr();			
	cbreak();			
	curs_set(0);
	start_color();	
	init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
	
	keypad(stdscr, TRUE);		

	MainScene();

	getch();	
	//printlogo(0,10);

	endwin();
	return 0;
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

	for(y = 0; y < HEIGHT; y++){
		mvaddch(y, 0, ACS_CKBOARD);
		mvaddch(y, WIDTH - 1, ACS_CKBOARD);
		refresh();
	}
	for(x = 0; x < WIDTH; x++){
		mvaddch(0, x, ACS_CKBOARD);
		mvaddch(HEIGHT - 1, x, ACS_CKBOARD);
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

	mvprintw(13,(WIDTH - strlen(msg))/2 + 5,"%s", msg);

	msg = "-PRESS ANY KEY TO STRRT-";
    mvprintw(22,(WIDTH - strlen(msg))/2 ,"%s", msg);
	refresh();
	getch();
	clear();
	ExplainScene();
}

void ExplainScene(){
	char* msg = "[ Instruction ]";
	mvprintw(13,(WIDTH - strlen(msg))/2,"%s", msg);

	PrintInstruction();
	//msg = "플레이어는 ‘방향키’ 입력 시,\n    해당 방향으로 오브젝트가 없으면 계속 직진함.";
	//mvprintw(17,3 ,"%s", msg);

	PrintLogo(5,0);

	getch();
	clear();
	MapScene();
}

void MapScene(){
	char* msg = "[ 게임을 진행할 ‘Map’을 고르세요! ]";
	mvprintw(13,(WIDTH - strlen(msg))/2 + 7 ,"%s", msg);

	msg = "[ Press ‘1’: 점선 ]";
	mvprintw(17,3 ,"%s", msg);	
	msg = "[ Press ‘2’: 데칼코마니 ]";
	mvprintw(19,3 ,"%s", msg);
	msg = "[ Press ‘3’: 흩뿌려진 강박증 ]";
	mvprintw(21,3 ,"%s", msg);
	PrintLogo(5,0);

	getch();
	clear();
	OverScene(1);
}

void OverScene(int n) {
    const char* msg1 = "※ 아무 키나 눌러 메인화면으로 이동 ※";
    const char* msg2 = (n == 1) ? "[ 1ND PLAYER 승리! ]" : "[ 2ND PLAYER 승리! ]";

    mvprintw(21, (WIDTH - strlen(msg1))/2 + 9, "%s", msg1);
	
	move(19, (WIDTH - strlen(msg2))/2 + 2);
    // 승리 메시지 출력
    attron(COLOR_PAIR(n));
    addch(ACS_DIAMOND);
    attroff(COLOR_PAIR(n));
	printw("%s", msg2); 
	refresh();

    PrintLogo(11, 0);

    getch();  // 사용자 입력 대기

    clear();
    refresh();
}
