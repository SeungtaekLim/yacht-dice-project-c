#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <process.h>
#pragma comment(lib,"winmm.lib")

#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define DICE_N 5
#define DICE_PX 72	
#define DICE_PY 3	

#define ESC 0x1b //  ESC

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define RESET   '0'

#define UP  0x48 // 0xe0 + 0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define SPACE 0x20
#define ENTER 0x0d

#define UP2		'w' // p2 방향키
#define DOWN2	's'
#define LEFT2	'a'
#define RIGHT2	'd'

#define WIDTH 164
#define HEIGHT 40

int dice_flag[DICE_N]; // 주사위 보관 배열
int p1_score_flag[12]; // 플레이어 1 스코어 플래그
int p2_score_flag[12]; // 플레이어 2 스코어 플래그
int p1_score[13]; // 플레이어 1 스코어
int p2_score[13]; // 플레이어 2 스코어
int select_chance = 1; // 선택기회
int select_n; //주사위 선택 숫자
int game_type; // 1인용 2인용 선택

void removeCursor(void) { // 커서를 안보이게 한다

    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}

void textcolor(int fg_color, int bg_color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void cls(int text_color, int bg_color) // 화면 지우기고 원하는 배경색으로 설정한다.
{
    char cmd[100];
    system("cls");
    sprintf(cmd, "COLOR %x%x", bg_color, text_color);
    system(cmd);
}

// 주사위 판 그리기
void dice_box(int x1, int y1, int x2, int y2)
{
    int x, y;
    textcolor(BLACK, BLACK);
    for (y = y1; y <= y2; y++) {
        for (x = x1; x <= x2; x += 2) {
            textcolor(RED1,RED1);
            gotoxy(x, y);
            printf("  ");
            textcolor(BLACK, BLACK);
        }
        textcolor(BLACK, GRAY1);
        gotoxy(x1, y);
        printf("▥");
        textcolor(BLACK, BLACK);
        gotoxy(x2, y);
        printf("  ");
    }
    for (x = x1; x <= x2; x += 2) {
        textcolor(BLACK, GRAY1);
        gotoxy(x, y1);
        printf("▥");
        textcolor(BLACK, BLACK);
        gotoxy(x, y2);
        printf("  ");
    }
    textcolor(WHITE, BLACK);
}

// 주사위 판 지우기
void d_dice_box()
{
    int i,j;
    textcolor(YELLOW1, YELLOW1);
    for (i = 60; i <= 157; i++) {
        for (j = 1; j <= 38; j ++) {
            gotoxy(i, j);
            printf(" ");
        }
    }
    textcolor(WHITE, YELLOW1);
}

// 스코어 보드 그리기
void score_board_box() 
{
    textcolor(BLACK,WHITE);
    gotoxy(2, 1); printf("                    ");
    gotoxy(2, 2); printf("                    ");
    gotoxy(2, 3); printf("                    ");
    gotoxy(2, 4); printf("                    ");

    gotoxy(22, 1); printf("┌─────┬─────┐");
    gotoxy(22, 2); printf("│   Play   │   Play   │");
    gotoxy(22, 3); printf("│    1     │    2     │");
    gotoxy(22, 4); printf("│          │          │");

    gotoxy(2, 5); 
    textcolor (WHITE, BLACK);   printf("┌─────────"); 
    textcolor(BLACK, WHITE);    printf("┼─────┼─────┤");
    gotoxy(2, 6); 
    textcolor (WHITE, BLACK);   printf("│                  "); 
    textcolor(BLACK, WHITE);    printf("│          │          │");

    for (int i = 0; i < 22; i+=2) {
        gotoxy(2, 7 + i); 
        textcolor(WHITE, BLACK); printf("├─────────"); 
        textcolor(BLACK, WHITE); printf("┼─────┼─────┤");
        gotoxy(2, 8 + i); 
        textcolor(WHITE, BLACK); printf("│                  "); 
        textcolor(BLACK, WHITE); printf("│          │          │");
    }

    gotoxy(2, 29);
    textcolor(WHITE, BLACK); printf("└─────────"); 
    textcolor(BLACK, WHITE); printf("┴─────┴─────┘");
    gotoxy(2, 30); 
    printf("                                              ");
    gotoxy(2, 31); 
    textcolor(WHITE, BLACK); printf("┌─────────"); 
    textcolor(BLACK, WHITE); printf("┬─────┬─────┐");
    gotoxy(2, 32); 
    textcolor(WHITE, BLACK); printf("│ Total            "); 
    textcolor(BLACK, WHITE); printf("│          │          │");
    gotoxy(2, 33); 
    textcolor(WHITE, BLACK); printf("└─────────"); 
    textcolor(BLACK, WHITE); printf("┴─────┴─────┘");
    gotoxy(2, 34); printf("                                              ");
    
    for (int y = 1; y < 36; y++) {
        gotoxy(48, y);
        textcolor(GRAY2, GRAY2);
        printf(" ");
    }

    gotoxy(3,35);
    printf("                                              ");

    textcolor(WHITE, BLACK);
    gotoxy(5, 6);
    printf("Aces       \n"); //aces_score(dice));
    gotoxy(5, 6 + 2);
    printf("Twos       \n"); //twos_score(dice));
    gotoxy(5, 6 + 4);
    printf("Threes     \n"); //threes_score(dice));
    gotoxy(5, 6 + 6);
    printf("Fours      \n"); //fours_score(dice));
    gotoxy(5, 6 + 8);
    printf("Fives      \n"); //fives_score(dice));
    gotoxy(5, 6 + 10);
    printf("Sixes      \n"); //sixes_score(dice));
    gotoxy(5, 6 + 12);
    printf("Choice     \n"); //choice_score(dice));
    gotoxy(5, 6 + 14);
    printf("4 of a kind\n"); //four_of_a_kind_score(dice));
    gotoxy(5, 6 + 16);
    printf("Full_house \n"); //full_house_score(dice));
    gotoxy(5, 6 + 18);
    printf("S_straight \n"); //small_straight_score(dice));
    gotoxy(5, 6 + 20);
    printf("L_straight \n"); //lage_straight_score(dice));
    gotoxy(5, 6 + 22);
    printf("Yacht      \n"); //yacht_score(dice));

    textcolor(BLACK, WHITE);
}

// 주사위 모양
void dice_art(int n, int x, int y) 
{
    textcolor(BLACK, WHITE);
    switch (n) {
    case 0:
    case 1:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("          ");
        gotoxy(x, y+2); printf("    ●    ");
        gotoxy(x, y+3); printf("          ");
        gotoxy(x, y+4); printf("          ");
        break;
    case 2:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("    ●    ");
        gotoxy(x, y+2); printf("          ");
        gotoxy(x, y+3); printf("    ●    ");
        gotoxy(x, y+4); printf("          ");
        break;
    case 3:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("      ●  ");
        gotoxy(x, y+2); printf("    ●    ");
        gotoxy(x, y+3); printf("  ●      ");
        gotoxy(x, y+4); printf("          ");
        break;
    case 4:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("  ●  ●  ");
        gotoxy(x, y+2); printf("          ");
        gotoxy(x, y+3); printf("  ●  ●  ");
        gotoxy(x, y+4); printf("          ");
        break;
    case 5:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("  ●  ●  ");
        gotoxy(x, y+2); printf("    ●    ");
        gotoxy(x, y+3); printf("  ●  ●  ");
        gotoxy(x, y+4); printf("          ");
        break;
    case 6:
        gotoxy(x, y);   printf("          ");
        gotoxy(x, y+1); printf("  ●  ●  ");
        gotoxy(x, y+2); printf("  ●  ●  ");
        gotoxy(x, y+3); printf("  ●  ●  ");
        gotoxy(x, y+4); printf("          ");
        break;
    }
    textcolor(WHITE, BLACK);
}

void p_yatch(int x, int y) 
{
    gotoxy(x, y);     printf("###  ###   ###     ######    ####    ##  ##\n");
    gotoxy(x, y + 1); printf(" ##  ##   ## ##   ## ## ##  ##  ##   ##  ##\n");
    gotoxy(x, y + 2); printf("  ####   ##   ##     ##    ##        ##  ##\n");
    gotoxy(x, y + 3); printf("   ##    ##   ##     ##    ##        ######\n");
    gotoxy(x, y + 4); printf("   ##    #######     ##    ##        ##  ##\n");
    gotoxy(x, y + 5); printf("   ##    ##   ##     ##     ##  ##   ##  ##\n");
    gotoxy(x, y + 6); printf("  ####   ##   ##    ####     ####    ##  ##\n");
}

void p_winner(int x, int y) 
{
    textcolor(WHITE, YELLOW1);
    gotoxy(x, y);     printf("                    ■■                                                            \n");
    gotoxy(x, y + 1); printf("■■      ■■      ■■                                                            \n");
    gotoxy(x, y + 2); printf("■■      ■■                                                                      \n");
    gotoxy(x, y + 3); printf("■■      ■■  ■■■■      ■■■■■    ■■■■■      ■■■■    ■■    ■■\n");
    gotoxy(x, y + 4); printf("■■  ■  ■■      ■■      ■■    ■■  ■■    ■■  ■■    ■■  ■■  ■■■\n");
    gotoxy(x, y + 5); printf("■■  ■  ■■      ■■      ■■    ■■  ■■    ■■  ■■    ■■  ■■■      \n");
    gotoxy(x, y + 6); printf("■■  ■  ■■      ■■      ■■    ■■  ■■    ■■  ■■■■■■  ■■        \n");
    gotoxy(x, y + 7); printf("  ■■  ■■        ■■      ■■    ■■  ■■    ■■  ■■          ■■        \n");
    gotoxy(x, y + 8); printf("  ■■  ■■        ■■      ■■    ■■  ■■    ■■  ■■          ■■        \n");
    gotoxy(x, y + 9); printf("  ■■  ■■    ■■■■■■  ■■    ■■  ■■    ■■    ■■■■    ■■        \n");
}

// 주사위 정렬
void dice_sort(int dice[]) 
{
    int temp = 0;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 5; j++) {
            if (dice[j] > dice[j + 1]) {
                temp = dice[j];
                dice[j] = dice[j + 1];
                dice[j + 1] = temp;
            }
        }
    }
}

// 주사위 출력
void print_dice(int dice[]) 
{
    int dice_x = DICE_PX-2;
    int dice_y = 4;

    for (int i = 0; i < DICE_N; i++) {
        dice_art(dice[i], dice_x + i*18, dice_y);
    }
    printf("\n");
}

// 주사위를 굴리기
void roll_dice(int dice[], unsigned char a) {
    int dice_x = DICE_PX-2; //70
    int dice_y = 4;

    switch (a)
    {
    case RESET:
        break;
    case UP :
    case UP2 :
        dice_x = 70;
        dice_y = 3;
        break;
    case DOWN:
    case DOWN2:
        dice_x = 70;
        dice_y = 5;
        break;
    case LEFT:
    case LEFT2:
        dice_x = 68;
        dice_y = 4;
        break;
    case RIGHT:
    case RIGHT2:
        dice_x = 72;
        dice_y = 4;
        break;
    }
    for (int i = 0; i < DICE_N; i++) {
        if (dice_flag[i] == 0) {
            dice[i] = rand() % 6 + 1;
            dice_art(dice[i], dice_x + i * 18, dice_y);
            switch (a) // 주사위 움직임
            {
            case RESET:
                textcolor(RED1, RED1);
                gotoxy(68 + i * 18, 4);  printf("  ");
                gotoxy(68 + i * 18, 5);  printf("  ");
                gotoxy(68 + i * 18, 6);  printf("  ");
                gotoxy(68 + i * 18, 7);  printf("  ");
                gotoxy(68 + i * 18, 8);  printf("  ");

                gotoxy(80 + i * 18, 4);  printf("  ");
                gotoxy(80 + i * 18, 5);  printf("  ");
                gotoxy(80 + i * 18, 6);  printf("  ");
                gotoxy(80 + i * 18, 7);  printf("  ");
                gotoxy(80 + i * 18, 8);  printf("  ");

                gotoxy(70 + i * 18, 3);  printf("          ");
                gotoxy(70 + i * 18, 9);  printf("          ");
                break;
            case UP:
            case UP2:
                textcolor(RED1, RED1);
                gotoxy(70 + i * 18, dice_y + 5);  printf("          ");
                gotoxy(70 + i * 18, dice_y + 6);  printf("          ");

                gotoxy(80 + i * 18, 4);  printf("  ");
                gotoxy(80 + i * 18, 5);  printf("  ");
                gotoxy(80 + i * 18, 6);  printf("  ");
                gotoxy(80 + i * 18, 7);  printf("  ");
                gotoxy(80 + i * 18, 8);  printf("  ");

                gotoxy(68 + i * 18, 4);  printf("  ");
                gotoxy(68 + i * 18, 5);  printf("  ");
                gotoxy(68 + i * 18, 6);  printf("  ");
                gotoxy(68 + i * 18, 7);  printf("  ");
                gotoxy(68 + i * 18, 8);  printf("  ");
                break;
            case DOWN:
            case DOWN2:
                textcolor(RED1, RED1);
                gotoxy(70+ i * 18, 3);  printf("          ");
                gotoxy(70+ i * 18, 4);  printf("          ");
                gotoxy(68+ i * 18, 4);  printf("  ");
                gotoxy(68+ i * 18, 5);  printf("  ");
                gotoxy(68+ i * 18, 6);  printf("  ");
                gotoxy(68+ i * 18, 7);  printf("  ");
                gotoxy(68+ i * 18, 8);  printf("  ");

                gotoxy(80 + i * 18, 4);  printf("  ");
                gotoxy(80 + i * 18, 5);  printf("  ");
                gotoxy(80 + i * 18, 6);  printf("  ");
                gotoxy(80 + i * 18, 7);  printf("  ");
                gotoxy(80 + i * 18, 8);  printf("  ");
                break;
            case LEFT:
            case LEFT2:
                textcolor(RED1, RED1);
                gotoxy(78+ i * 18, 4);  printf("    ");
                gotoxy(78+ i * 18, 5);  printf("    ");
                gotoxy(78+ i * 18, 6);  printf("    ");
                gotoxy(78+ i * 18, 7);  printf("    ");
                gotoxy(78+ i * 18, 8);  printf("    ");
                gotoxy(70+ i * 18, 3);  printf("          ");
                gotoxy(70+ i * 18, 9);  printf("          ");

                break;
            case RIGHT:
            case RIGHT2:
                textcolor(RED1, RED1);
                gotoxy(68+ i * 18, 4);  printf("    ");
                gotoxy(68+ i * 18, 5);  printf("    ");
                gotoxy(68+ i * 18, 6);  printf("    ");
                gotoxy(68+ i * 18, 7);  printf("    ");
                gotoxy(68+ i * 18, 8);  printf("    ");
                gotoxy(70+ i * 18, 3);  printf("          ");
                gotoxy(70+ i * 18, 9);  printf("          ");
                break;
            }
        }
    }
}

// 주사위 숫자 갯수
int countDice(int dice[], int number) {
    int count = 0;
    for (int i = 0; i < DICE_N; i++) {
        if (dice[i] == number) {
            count++;
        }
    }
    return count;
}

// 스코어 계산
int aces_score(int dice[]) // 0
{
    return countDice(dice, 1) * 1;
}

int twos_score(int dice[]) //1
{
    return countDice(dice, 2) * 2;
}

int threes_score(int dice[]) //2
{
    return countDice(dice, 3) * 3;
}

int fours_score(int dice[]) //3
{
    return countDice(dice, 4) * 4;
}

int fives_score(int dice[]) //4
{
    return countDice(dice, 5) * 5;
}

int sixes_score(int dice[]) //5
{
    return countDice(dice, 6) * 6;
}

int choice_score(int dice[]) //6
{
    return dice[0] + dice[1] + dice[2] + dice[3] + dice[4];
}

int four_of_a_kind_score(int dice[]) //7
{
    for (int i = 1; i <= 6; i++) {
        if (countDice(dice, i) >= 4) {
            return i * 4;
        }
    }
    return 0;
}

int full_house_score(int dice[]) //8
{
    for (int i = 1; i <= 6; i++) {
        if (countDice(dice, i) == 3) {
            for (int j = 1; j <= 6; j++) {
                if (i != j && countDice(dice, j) == 2) {
                    return dice[0] + dice[1] + dice[2] + dice[3] + dice[4];
                }
            }
        }
    }
    return 0;
}

int small_straight_score(int dice[]) //9
{
    for (int i = 1; i < 4; i++) {
        if (countDice(dice, i) >= 1 && countDice(dice, i+1) >= 1 && 
            countDice(dice, i + 2) >= 1 && countDice(dice, i + 3) >= 1) {
            return 30;
        }
    }
    return 0;
}

int lage_straight_score(int dice[]) //10
{

    for (int i = 1; i < 3; i++) {
        if (countDice(dice, i) == 1 && countDice(dice, i + 1) == 1 && 
            countDice(dice, i + 2) == 1 && countDice(dice, i + 3) == 1 && 
            countDice(dice, i + 4) == 1) {
            return 30;
        }
    }
    return 0;
}

int yacht_score(int dice[]) //11
{
    for (int i = 1; i <= 6; i++) {
        if (countDice(dice, i) == 5) {
            return 50;
        }
    }
    return 0;
}

// 스코어 관리
int score_num(int dice[],int n) 
{
    if (n == 0) {
        n = 111;
    }
    switch (n) {
    case 111:
        return aces_score(dice);
    case 1:
        return twos_score(dice);
    case 2:
        return threes_score(dice);
    case 3:
        return fours_score(dice);
    case 4:
        return fives_score(dice);
    case 5:
        return sixes_score(dice);
    case 6:
        return choice_score(dice);
    case 7:
        return four_of_a_kind_score(dice);
    case 8:
        return full_house_score(dice);
    case 9:
        return small_straight_score(dice);
    case 10:
        return lage_straight_score(dice);
    case 11:
        return yacht_score(dice);
    }
}

// 플레이어 1 점수 기록
void p1_score_board(int dice[]) 
{
    int i;
    int x = 5, y = 6;

    for (i = 0; i < 12; i++) {
        gotoxy(x+17, y+2*i);
        if (p1_score_flag[i] == 0) {
            textcolor(BLACK, WHITE);
            printf("│    %02d    │\n", score_num(dice, i));
            if (score_num(dice, i) == 50) {
                textcolor(WHITE, RED1);
                p_yatch(90, 20);
                PlaySound(TEXT("3.wav"), 0, SND_FILENAME | SND_ASYNC);
                Sleep(700);
                textcolor(RED1, RED1);
                p_yatch(90, 20);
                Sleep(700);
                textcolor(WHITE, RED1);
                p_yatch(90, 20);
                Sleep(700);
                textcolor(RED1, RED1);
                p_yatch(90, 20);
                textcolor(BLACK, WHITE);
            }
        }
        else {
            textcolor(WHITE, BLACK);
            printf("│    %02d    │\n", p1_score[i]);
        }
    }
}

// 플레이어 2 점수 기록
void p2_score_board(int dice[]) 
{
    int i;
    int x = 17, y = 6;

    for (i = 0; i < 12; i++) {
        gotoxy(x + 19, y + 2 * i);
        if (p2_score_flag[i] == 0) {
            textcolor(BLACK, WHITE);
            printf("    %02d    \n", score_num(dice, i));
            if (score_num(dice, i) == 50) {
                textcolor(WHITE, RED1);
                p_yatch(90, 20);
                PlaySound(TEXT("3.wav"), 0, SND_FILENAME | SND_ASYNC);
                Sleep(700);
                textcolor(RED1, RED1);
                p_yatch(90, 20);
                Sleep(700);
                textcolor(WHITE, RED1);
                p_yatch(90, 20);
                Sleep(700);
                textcolor(RED1, RED1);
                p_yatch(90, 20);
                textcolor(BLACK, WHITE);
            }
        }
        else {
            textcolor(WHITE, BLACK);
            printf("    %02d    │\n", p2_score[i]);
        }
    }
}

// 주사위 keep 화살표
void p_select_dice(int n) 
{
    textcolor(GREEN2, GREEN2);
    int x = 68 + n * 18, y = DICE_PY;
    int i;
    for (i = 0; i <= 12; i += 2) {
        gotoxy(x + i, y);
        printf("%s", "▨");
        gotoxy(x + i, y + 6);
        printf("%s", "▨");
    }
    for (i = 0; i <= 6; i++) {
        gotoxy(x, y + i);
        printf("%s", "▨");
        gotoxy(x+12, y + i);
        printf("%s", "▨");
    }
    textcolor(WHITE, GREEN2);
    gotoxy(x+1, y);
    printf("Keep");
    textcolor(WHITE, BLACK);
}

// 주사위 keep 화살표 지우기
void r_select_dice(int n) 
{
    textcolor(RED1, RED1);
    int x = 68 + n * 18, y = DICE_PY;
    int i;
    for (i = 0; i <= 12; i += 2) {
        gotoxy(x + i, y);
        printf("%s", "  ");
        gotoxy(x + i, y + 6);
        printf("%s", "  ");
    }
    for (i = 0; i <= 6; i++) {
        gotoxy(x, y + i);
        printf("%s", "  ");
        gotoxy(x + 12, y + i);
        printf("%s", "  ");
    }
    textcolor(WHITE, BLACK);
}

// 스코어 화살표 지우기
void p_select_score(int n) 
{
    gotoxy(50, n);
    textcolor(YELLOW1, YELLOW1);
    printf("  ");
    textcolor(WHITE, BLACK);
}

// p1 주사위 선택
void p1_select_dice(unsigned char ch)
{
    int s_number = 0;
    static int oldx = DICE_PX+2, oldy = DICE_PY+8, newx = DICE_PX+2, newy = DICE_PY+8;
    int move_flag = 0;

    static int called = 0;

    switch (ch) {
    case LEFT:
        if (oldx > DICE_PX+4) {
            newx = oldx - 18;
        }
        move_flag = 1;
        break;
    case RIGHT:
        if (oldx < DICE_PX+2 + 18 * 4) {
            newx = oldx + 18;
        }
        move_flag = 1;
        break;
    case SPACE:
        s_number = (oldx - DICE_PX+2) / 18;
        if (dice_flag[s_number] == 0) {
            p_select_dice(s_number);
            dice_flag[s_number] = 1;
        }
        else {
            r_select_dice(s_number);
            dice_flag[s_number] = 0;
        }
        break;
    case RESET: 
        gotoxy(newx, newy);
        textcolor(WHITE, RED1);
        printf("  ");
        textcolor(WHITE, BLACK);
        oldx = DICE_PX+2, oldy = DICE_PY+8, newx = DICE_PX+2, newy = DICE_PY+8;
        break;
    }
    if (move_flag) {
        textcolor(WHITE,RED1);
        gotoxy(oldx, oldy); // 마지막 위치지우기
        printf("  ");
        gotoxy(newx, newy); // 새로운 위치
        printf("▲");
        textcolor(WHITE,BLACK);
        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;
    }
}

// p2 주사위 선택
void p2_select_dice(unsigned char ch)
{
    int s_number = 0;
    static int oldx = DICE_PX + 2, oldy = DICE_PY + 8, newx = DICE_PX + 2, newy = DICE_PY + 8;
    int move_flag = 0;

    static int called = 0;

    switch (ch) {
    case LEFT2:
        if (oldx > DICE_PX + 4) {
            newx = oldx - 18;
        }
        move_flag = 1;
        break;
    case RIGHT2:
        if (oldx < DICE_PX + 2 + 18 * 4) {
            newx = oldx + 18;
        }
        move_flag = 1;
        break;
    case SPACE:
        s_number = (oldx - DICE_PX + 2) / 18;
        if (dice_flag[s_number] == 0) {
            p_select_dice(s_number);
            dice_flag[s_number] = 1;
        }
        else {
            r_select_dice(s_number);
            dice_flag[s_number] = 0;
        }
        break;
    case RESET:
        gotoxy(newx, newy);
        textcolor(WHITE, RED1);
        printf("  ");
        textcolor(WHITE, BLACK);
        oldx = DICE_PX + 2, oldy = DICE_PY + 8, newx = DICE_PX + 2, newy = DICE_PY + 8;
        break;
    }
    if (move_flag) {
        textcolor(WHITE, RED1);
        gotoxy(oldx, oldy); // 마지막 위치지우기
        printf("  ");
        gotoxy(newx, newy); // 새로운 위치
        printf("▲");
        textcolor(WHITE, BLACK);
        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;
    }
}

// 점수 선택
void p1_select_score(unsigned char ch)
{
    int s_number;
    static int oldx = 50, oldy = 6, newx = 50, newy = 6;
    int move_flag = 0;

    if (ch == RESET) {
        textcolor(WHITE, YELLOW1);
        gotoxy(newx, newy);
        printf("  ");
        oldx = 50, oldy = 6, newx = 50, newy = 6;
        return;
    }
    switch (ch) {
    case UP:
        if (oldy > 6) {
            newy = oldy - 2;
        }
        move_flag = 1;
        break;
    case DOWN:
        if (oldy < 6 + 2 * 11) {
            newy = oldy + 2;
        }
        move_flag = 1;
        break;
    case ENTER:
        s_number = (oldy-6)/2;
        if (p1_score_flag[s_number] == 0 && select_chance == 1 ) {
            p_select_score(s_number);
            p1_score_flag[s_number] = 1;
            select_chance = 0;
            select_n = s_number;
        }
        break;
    }

    if (move_flag) {
        gotoxy(oldx, oldy); // 마지막 위치지우기
        textcolor(WHITE, YELLOW1);
        printf("  ");
        gotoxy(newx, newy); // 새로운 위치
        printf("◀");
        textcolor(BLACK,WHITE);
        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;
    }
}

// p2 점수 선택
void p2_select_score(unsigned char ch)
{
    int s_number = 0;
    static int oldx = 50, oldy = 6, newx = 50, newy = 6;
    int move_flag = 0;

    if (ch == RESET) {
        textcolor(WHITE, YELLOW1);
        gotoxy(newx, newy);
        printf("  ");
        oldx = 50, oldy = 6, newx = 50, newy = 6;
        return;
    }

    switch (ch) {
    case UP2:
        if (oldy > 6) {
            newy = oldy - 2;
        }
        move_flag = 1;
        break;
    case DOWN2:
        if (oldy < 6 + 2 * 11) {
            newy = oldy + 2;
        }
        move_flag = 1;
        break;
    case ENTER:
        s_number = (oldy - 6) / 2;
        if (p2_score_flag[s_number] == 0 && select_chance == 1) {
            p_select_score(s_number);
            p2_score_flag[s_number] = 1;
            select_chance = 0;
            select_n = s_number;
        }
        break;
    }

    if (move_flag) {
        gotoxy(oldx, oldy); // 마지막 위치지우기
        textcolor(WHITE, YELLOW1);
        printf("  ");
        gotoxy(newx, newy); // 새로운 위치
        printf("◀");
        textcolor(BLACK, WHITE);
        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;
    }
}

void select_game(unsigned char ch) // 게임 선택
{
    static int oldx = 86, oldy = 24, newx = 86, newy = 24;
    int move_flag = 0;

    switch (ch) {
    case UP:
        if (oldy > 24) {
            newy = oldy - 4;
        }
        move_flag = 1;
        break;
    case DOWN:
        if (oldy < 24 + 4) {
            newy = oldy + 4;
        }
        move_flag = 1;
        break;
    case ENTER:
        if (oldy == 24) {
            game_type = 1;
            break;
        }
        else {
            game_type = 2;
            break;
        }
    case RESET:
        textcolor(WHITE, BLACK);
        gotoxy(newx, newy);
        printf("  ");
        break;
    }

    if (move_flag) {
        gotoxy(oldx, oldy); // 마지막 위치지우기
        textcolor(WHITE, BLACK);
        printf("  ");
        gotoxy(newx, newy); // 새로운 위치
        printf("◀");
        textcolor(BLACK, WHITE);
        oldx = newx; // 마지막 위치를 기억한다.
        oldy = newy;
    }
}

void yatch_effect() {
    textcolor(WHITE, RED1);
    p_yatch(90, 20);
    PlaySound(TEXT("3.wav"), 0, SND_FILENAME | SND_ASYNC);
    Sleep(700);
    textcolor(RED1, RED1);
    p_yatch(90, 20);
    Sleep(700);
    textcolor(WHITE, RED1);
    p_yatch(90, 20);
    Sleep(700);
    textcolor(RED1, RED1);
    p_yatch(90, 20);
    textcolor(BLACK, WHITE);
}

void p1_one_turn() { // 한 턴
    unsigned char ch;
    int count = 0;
    int dice[DICE_N] = { 0, };
    int i;
    int slow_n = 0;
  
    for (i = 0; i < 5; i++) { // 주사위 초기화
        r_select_dice(i);
        dice_flag[i] = 0;
    }

    select_chance = 1; // 선택 기회 초기화

    p1_score_board(dice);
    ch = RESET;
    while (1) {
        if (kbhit() == 1) {
            ch = _getch();
            if (ch == ENTER) {
                roll_dice(dice, RESET);
                break;
            }
        }
        if (slow_n == 0) {
            slow_n = 5000;
        }
        if (slow_n == 5000) {
            roll_dice(dice,ch);
            PlaySound(TEXT("1.wav"), 0, SND_FILENAME | SND_ASYNC);
        }
        slow_n--;
    }

    count = 2;
    while (count--) {
        p1_score_board(dice);
        gotoxy(DICE_PX+2, DICE_PY+8); // 초기 화살표
        textcolor(WHITE, RED1);
        printf("▲");
        textcolor(WHITE, BLACK);
        while (1) {
            if (kbhit() == 1) {
                ch = getch();
                if (ch == ENTER) // ENTER 누르면 다음
                    break;
                if (ch == 'o')
                    yatch_effect();
                if (ch == SPACE) // SPACE 누르면 고정
                    p1_select_dice(ch);

                if (ch == SPECIAL1 || ch == SPECIAL2) {
                    ch = getch();
                    switch (ch) {
                    case LEFT:
                    case RIGHT:
                        p1_select_dice(ch);
                        break;
                    }
                }
            }
        }

        p1_score_board(dice);

        ch = RESET;
        while (1) {
            if (kbhit() == 1) {
                ch = getch();
                if (ch == ENTER) {
                    roll_dice(dice, RESET);
                    p1_select_dice(RESET);
                    break;
                }
            }
            if (slow_n == 0) {
                slow_n = 5000;
            }
            if (slow_n == 5000) {
                roll_dice(dice,ch);
                PlaySound(TEXT("1.wav"), 0, SND_FILENAME | SND_ASYNC);
            }
            slow_n--;
        }
    }
    p1_score_board(dice);

    for (i = 0; i < 5; i++) {
        p_select_dice(i);
        dice_flag[i] = 1;
    }

    textcolor(WHITE, YELLOW1);
    gotoxy(50, 6); //초기 화살표
    printf("◀");

    while (1) {
        if (kbhit() == 1) {
            ch = getch();
            if (ch == ENTER) { // ENTER 누르면 다음
                p1_select_score(ch);
                p1_select_score(RESET);
                break;
            }
            if (ch == SPECIAL1 || ch == SPECIAL2) {
                ch = getch();
                switch (ch) {
                case UP:
                case DOWN:
                    p1_select_score(ch);
                    break;
                }
            }
        }
    }
    p1_score[select_n] = score_num(dice, select_n);
    select_n = 12;
    textcolor(BLACK, WHITE);
    p1_score_board(dice);
}

void p2_one_turn() { // 한 턴
    unsigned char ch;
    int count = 0;
    int dice[DICE_N] = { 0, };
    int i;
    int slow_n = 0;

    for (i = 0; i < 5; i++) { // 주사위 초기화
        r_select_dice(i);
        dice_flag[i] = 0;
    }

    select_chance = 1; // 선택 기회 초기화

    p2_score_board(dice);
    ch = RESET;
    while (1) {
        if (kbhit() == 1) {
            ch = getch();
            if (ch == ENTER) {
                roll_dice(dice, RESET);
                break;
            }
        }
        if (slow_n == 0) {
            slow_n = 5000;
        }
        if (slow_n == 5000) {
            roll_dice(dice,ch);
            PlaySound(TEXT("1.wav"), 0, SND_FILENAME | SND_ASYNC);
        }
        slow_n--;
    }

    count = 2;
    while (count--) {
        p2_score_board(dice);
        gotoxy(DICE_PX + 2, DICE_PY + 8); // 초기 화살표
        textcolor(WHITE, RED1);
        printf("▲");
        textcolor(WHITE, BLACK);
        while (1) {
            if (kbhit() == 1) {
                ch = getch();
                if (ch == ENTER) // ENTER 누르면 다음
                    break;
                p2_select_dice(ch);
            }
        }
        p2_score_board(dice);
        ch = RESET;
        while (1) {
            if (kbhit() == 1) {
                ch = _getch();
                if (ch == ENTER) {
                    roll_dice(dice, RESET);
                    p2_select_dice(RESET);
                    break;
                }
            }
            if (slow_n == 0) {
                slow_n = 5000;
            }
            if (slow_n == 5000) {
                roll_dice(dice,ch);
                PlaySound(TEXT("1.wav"), 0, SND_FILENAME | SND_ASYNC);
            }
            slow_n--;
        }
    }
    p2_score_board(dice);

    for (i = 0; i < 5; i++) {
        p_select_dice(i);
        dice_flag[i] = 1;
    }

    textcolor(WHITE, YELLOW1);
    gotoxy(50, 6); //초기 화살표
    printf("◀");

    while (1) {
        if (kbhit() == 1) {
            ch = getch();
            if (ch == ENTER) { // ENTER 누르면 다음
                p2_select_score(ch);
                p2_select_score(RESET);
                break;
            }
            switch (ch) {
            case ENTER:
                p2_select_score(ch);
                p2_select_score(RESET);
                break;
            case UP2:
            case DOWN2:
                p2_select_score(ch);
                break;
            }
        }
    }

    p2_score[select_n] = score_num(dice, select_n);
    select_n = 12;

    textcolor(BLACK, WHITE);
    p2_score_board(dice);
}

void start_logo(int x,int y) 
{
    gotoxy(x, y    ); printf("■■■          ■■■                                    ■■■                                ■■■■■■■        ■■■                                  \n");
    gotoxy(x, y + 1); printf("■■■          ■■■                                    ■■■            ■■■              ■■■    ■■■      ■■■                                  \n");
    gotoxy(x, y + 2); printf("  ■■■      ■■■                                      ■■■            ■■■              ■■■      ■■■                                            \n");
    gotoxy(x, y + 3); printf("    ■■■  ■■■        ■■■■■        ■■■■■    ■■■■■■    ■■■■■            ■■■      ■■■    ■■■      ■■■■■      ■■■■■  \n");
    gotoxy(x, y + 4); printf("      ■■■■■        ■■■  ■■■    ■■■  ■■■  ■■■  ■■■    ■■■              ■■■      ■■■    ■■■    ■■■  ■■■  ■■■  ■■■\n");
    gotoxy(x, y + 5); printf("        ■■■              ■■■■■    ■■■          ■■■  ■■■    ■■■              ■■■      ■■■    ■■■    ■■■          ■■■■■■■\n");
    gotoxy(x, y + 6); printf("        ■■■            ■■■■■■    ■■■          ■■■  ■■■    ■■■              ■■■      ■■■    ■■■    ■■■          ■■■        \n");
    gotoxy(x, y + 7); printf("        ■■■          ■■■  ■■■    ■■■          ■■■  ■■■    ■■■              ■■■      ■■■    ■■■    ■■■          ■■■        \n");
    gotoxy(x, y + 8); printf("        ■■■          ■■■  ■■■    ■■■  ■■■  ■■■  ■■■    ■■■              ■■■    ■■■      ■■■    ■■■  ■■■  ■■■  ■■■\n");
    gotoxy(x, y + 9); printf("        ■■■            ■■■■■■      ■■■■■    ■■■  ■■■      ■■■            ■■■■■■■        ■■■      ■■■■■      ■■■■■  \n");
}

void start() // 시작화면 출력
{
    unsigned char ch;

    int x1 = 0, y1 = 0;
    int x2 = 0, y2 = 0;
    int x3 = 0, y3 = 0;
    int x4 = 0, y4 = 0;

    int slow_n = 30000;

    start_logo(4, 6);
    textcolor(YELLOW1, BLACK);
    start_logo(2, 4);
    textcolor(WHITE, BLACK);
    gotoxy(78, 24);
    printf("1 play");
    gotoxy(86, 24);
    printf("◀");
    gotoxy(78, 28);
    printf("2 play");
    while (1) {
        if (slow_n == 0) {
            slow_n = 30000;
        }
        if (slow_n == 30000) {
            textcolor(BLACK, BLACK);
            gotoxy(x1, y1);
            printf("  ");
            gotoxy(x2, y2);
            printf("  ");
            gotoxy(x3, y3);
            printf("  ");
            gotoxy(x4, y4);
            printf("  ");

            x1 = rand() % 150;
            y1 = rand() % 3;
            x2 = rand() % 150;
            y2 = 16 + rand() % 4;
            x3 = rand() % 150;
            y3 = rand() % 3;
            x4 = rand() % 150;
            y4 = 16 + rand() % 4;


            textcolor(YELLOW1, BLACK);
            gotoxy(x1, y1);
            printf("■");
            gotoxy(x2, y2);
            printf("■");
            gotoxy(x3, y3);
            printf("■");
            gotoxy(x4, y4);
            printf("■");
        }
        slow_n--;

        if (kbhit() == 1) {
            ch = getch();
            if (ch == ENTER) { // ENTER 누르면 다음
                select_game(ch);
                select_game(RESET);
                textcolor(YELLOW1, YELLOW1);
                for (int i = 0; i <= 82; i++) {
                    for (int j = 0; j <= 40; j++) {
                        gotoxy(82 - i, j);
                        printf(" ");
                        gotoxy(i + 82, j);
                        printf(" ");
                    }
                }
                textcolor(BLACK, WHITE);
                break;
            }
            if (ch == SPECIAL1 || ch == SPECIAL2) {
                ch = getch();
                switch (ch) {
                case UP:
                case DOWN:
                    select_game(ch);
                    break;
                }
            }
        }
    }
}

void p_pencil() // 연필 배경 그리기
{
    for (int y = 4; y < 34; y++) {
        gotoxy(53, y);
        textcolor(BLUE1, BLUE1);
        printf("    ");
    }
    gotoxy(54, 2);
    textcolor(YELLOW2, YELLOW2);
    printf("  ");
    textcolor(GRAY2, GRAY2);
    printf(" ");
    gotoxy(53, 3);
    textcolor(YELLOW2, YELLOW2);
    printf("    ");
    gotoxy(54, 1);
    textcolor(BLACK, BLACK);
    printf("  ");
    gotoxy(53, 34);
    textcolor(WHITE, WHITE);
    printf("    ");
    gotoxy(53, 35);
    textcolor(RED2, RED2);
    printf("    ");
    gotoxy(53, 36);
    printf("    ");

    for (int y = 4; y < 38; y++) {
        gotoxy(57, y);
        textcolor(GRAY2, GRAY2);
        printf(" ");
    }
    gotoxy(54, 37);
    printf("   ");
    textcolor(BLACK, WHITE);
}

void init_turn() // 턴 초기화
{
    gotoxy(11, 2);
    printf("Turn");
    gotoxy(10, 3);
    printf(" 0/12");
}

void p2_init_score() // 2p 스코어 보드 초기화
{
    int x = 17, y = 6;

    for (int i = 0; i < 12; i++) {
        gotoxy(x + 19, y + 2 * i);
        if (p2_score_flag[i] == 0) {
            textcolor(BLACK, WHITE);
            printf("    00    \n");
        }
        else {
            textcolor(WHITE, BLACK);
            printf("    00    │\n");
        }
    }
}

void whoes_turn(int n) // 턴표시
{
    switch (n) {
    case 1:
        textcolor(WHITE, BLACK);
        gotoxy(24, 2); 
        printf("   Play   ");
        textcolor(BLACK, WHITE);
        gotoxy(36, 2);
        printf("   Play   ");
        break;
    case 2:
        textcolor(BLACK, WHITE);
        gotoxy(24, 2);
        printf("   Play   ");
        textcolor(WHITE, BLACK);
        gotoxy(36, 2);
        printf("   Play   ");
        textcolor(BLACK, WHITE);
        break;
    case 3:
        textcolor(BLACK, WHITE);
        gotoxy(24, 2);
        printf("   Play   ");
        gotoxy(36, 2);
        printf("   Play   ");
        break;
    case 4:
        textcolor(WHITE, WHITE);
        gotoxy(36, 2);
        printf("          ");
        gotoxy(36, 3);
        printf("          ");
        textcolor(BLACK, WHITE);
        break;
    }

}

void init_p1_flag() // p1 플레그 스코어 초기화
{
    for (int i = 0; i < 12; i++) {
        p1_score_flag[i] = 0;
        p1_score[i] = 0;
    }
}

void init_p2_flag() // p2 플레그 스코어 초기화
{
    for (int i = 0; i < 12; i++) {
        p2_score_flag[i] = 0;
        p2_score[i] = 0;
    }
}

int main(int argc, char* argv[]) 
{
    unsigned char ch_m;
    int i;
    int top_score = 0;
    int player1_score; // 플레이어 1 점수 종합
    int player2_score; // 플레이어 2 점수 종합
    int slow_n = 0;

    // 처음 실행할 때는 argc == 1로 시작되고 
    // 아래에서 spawnl() 으로 자기 자신을 다시 실행하면
    // argc == 2 로 내려오기 때문에 이 부분을 pass 한다.
    if (argc == 1) {
        // bgm 재생
        PlaySound(TEXT("mainbgm.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP);
        // 게임 실행하고 게임이 종료될때까지 기다린다.
        _spawnl(P_WAIT, argv[0], argv[0], "gamerun", NULL);
        // 게임이 종료되면 bgm process도 종료
        exit(0);
    }

    removeCursor(); // 커서 삭제
    system("mode con:cols=164 lines=40"); // 가로 164 세로 40
    srand(time(NULL));

START:
    cls(WHITE, BLACK); // 배경 초기화
    start(); // 초기화면

    player1_score = 0; // 플레이어 1 점수 초기화
    player2_score = 0; // 플레이어 2 점수 초기화

    if (game_type == 1) { // 1인용
    RESTART1:
        init_p1_flag(); // 플레이어 1 스코어 플레그 초기화
        player1_score = 0; // 플레이어 1 점수 초기화
        dice_box(60, 1, 156, 38); // 주사위 상자 그리기
        score_board_box(); // 스코어보드 그리기
        whoes_turn(4);
        init_turn(); // 턴 초기화 
        p_pencil(); //배경 그리기

        for (i = 0; i < 12; i++) { // 12턴 진행
            p1_one_turn();
            textcolor(BLACK, WHITE);
            gotoxy(10, 3);
            printf("%2d/12", i + 1);

        }
        for (i = 0; i < 12; i++) { // 1인용 점수종합
            if (i != 0) {
                textcolor(YELLOW1, YELLOW1);
                gotoxy(50, 4 + 2 * i);
                printf("  ");
            }
            gotoxy(50, 6 + 2 * i);
            textcolor(WHITE, YELLOW1);
            printf("◀");
            gotoxy(22, 32);
            textcolor(BLACK, WHITE);
            printf("│    %03d   │\n", player1_score += p1_score[i]);
            PlaySound(TEXT("2.wav"), 0, SND_FILENAME | SND_ASYNC);
            Sleep(500);
            if (i == 11) {
                textcolor(YELLOW1, YELLOW1);
                gotoxy(50, 6 + 22);
                printf("  ");
                textcolor(WHITE, BLACK);
                gotoxy(22, 32);
                printf("│    %03d   │\n", player1_score);
            }
        }
        if (top_score < player1_score) { // 최고기록
            top_score = player1_score;
        }
        PlaySound(TEXT("3.wav"), 0, SND_FILENAME | SND_ASYNC);
        textcolor(GREEN2, BLACK);
        gotoxy(30 + 50, 7 + 10); 
        printf(" RESULT SCORE : %03d ", player1_score);
        Sleep(300);
        gotoxy(32 + 50, 10 + 10); 
        printf(" TOP SCORE : %03d ", top_score);
        Sleep(300);
        gotoxy(32 + 50, 13 + 10); 
        printf("PLAY AGAIN : (Q)");
        gotoxy(32 + 50, 14 + 10); 
        printf("OTHER GAME : (W)");
        gotoxy(32 + 50, 15 + 10); 
        printf("EXIT       : (E)");
        while (1) {
            if (kbhit()) {
                ch_m = getch();
                if (ch_m == 'q' || ch_m == 'w' || ch_m == 'e')
                    break;
            }
        }
        if (ch_m == 'q')
            goto RESTART1;
        if (ch_m == 'w')
            goto START;
    }
    else { // 2인용
    RESTART2:
        dice_box(60, 1, 156, 38); // 주사위 상자 그리기
        init_p1_flag(); // 플레이어 1 스코어 플레그 초기화
        init_p2_flag(); // 플레이어 2 스코어 플레그 초기화
        player1_score = 0; // 플레이어 1 점수 초기화
        player2_score = 0; // 플레이어 2 점수 초기화
        score_board_box(); // 스코어보드 그리기
        init_turn(); // 턴 초기화 
        p_pencil(); //배경 그리기
        p2_init_score(); // 점수판 초기화 

        for (i = 0; i < 12; i++){ // 2명 12턴 진행
            whoes_turn(1);
            p1_one_turn();
            whoes_turn(2);
            p2_one_turn();
            textcolor(BLACK, WHITE);
            gotoxy(10, 3);
            printf("%2d/12", i + 1);
        }

        whoes_turn(3);
        for (i = 0; i < 12; i++){ // 2인용 점수종합
            if (i != 0) {
                textcolor(YELLOW1, YELLOW1);
                gotoxy(50, 4 + 2 * i);
                printf("  ");
            }
            gotoxy(50, 6 + 2 * i);
            textcolor(WHITE, YELLOW1);
            printf("◀");
            gotoxy(22, 32);
            textcolor(BLACK, WHITE);
            printf("│    %03d   │    %03d   \n", player1_score += p1_score[i], player2_score += p2_score[i]);
            PlaySound(TEXT("2.wav"), 0, SND_FILENAME | SND_ASYNC);
            Sleep(500);
            if (i == 11) {
                textcolor(YELLOW1, YELLOW1);
                gotoxy(50, 6 + 22);
                printf("  ");
                textcolor(WHITE, BLACK);
                gotoxy(22, 32);
                printf("│    %03d   │    %03d   │\n", player1_score, player2_score);
            }
        }
        d_dice_box();
        p_winner(68, 4);
        PlaySound(TEXT("3.wav"), 0, SND_FILENAME | SND_ASYNC);
        gotoxy(30 + 66, 7 + 8);
        textcolor(WHITE, YELLOW1);
        if (player1_score > player2_score) {
            printf(" WINNER IS PLAYER 1 ");
        }
        else if (player1_score < player2_score) {
            printf(" WINNER IS PLAYER 2 ");
        }
        else {
            printf("       DRAW!!        ");
        }

        textcolor(WHITE,YELLOW1);
        gotoxy(30 + 66, 12 + 10); 
        printf(" PLAYER 1 SCORE : %03d ", player1_score);
        Sleep(300);
        gotoxy(30 + 66, 12 + 12); 
        printf(" PLAYER 2 SCORE : %03d ", player2_score);
        Sleep(300);
        gotoxy(32 + 66, 18 + 10); 
        printf("PLAY AGAIN : (Q)");
        gotoxy(32 + 66, 19 + 10); 
        printf("OTHER GAME : (W)");
        gotoxy(32 + 66, 20 + 10); 
        printf("EXIT       : (E)");
        while (1) {
            if (slow_n == 0) {
                slow_n = 10000;
            }
            if (slow_n >= 10000/2) {
                textcolor(WHITE, YELLOW1);
                gotoxy(30 + 66, 12 + 10); 
                printf(" PLAYER 1 SCORE : %03d ", player1_score);
                gotoxy(30 + 66, 12 + 12); 
                printf(" PLAYER 2 SCORE : %03d ", player2_score);
            }
            else {
                textcolor(YELLOW1, YELLOW1);
                gotoxy(30 + 66, 12 + 10); 
                printf(" PLAYER 1 SCORE : %03d ", player1_score);
                gotoxy(30 + 66, 12 + 12); 
                printf(" PLAYER 2 SCORE : %03d ", player2_score);
                textcolor(WHITE, YELLOW1);
            }
            if (kbhit()) {
                ch_m = getch();
                if (ch_m == 'q' || ch_m == 'w' || ch_m == 'e')
                    break;
            }
            slow_n--;
        }
        if (ch_m == 'q')
            goto RESTART2;
        if (ch_m == 'w')
            goto START;
    }
    printf("\n\n\n");
    return 0;
}