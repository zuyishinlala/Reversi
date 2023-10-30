#include <Keypad.h>
#include "pitch.h"

int col[8]={39,41,43,45,47,49,51,53};
int red[8]={36,34,32,30,28,26,24,22};
int grn[8]={37,35,33,31,29,27,25,23};
#define pin3 3///buzzer
const int LEN = 8;
const byte ROWS = 4;//four rows
const byte COLS = 4;//four columns
enum {EPT = 0, RED, GRN, MIX};
enum {NOHINT = 0,HINT = 1};

const int displaceX[8] = { 0, 1, 1, 1, 0, -1, -1, -1};
const int displaceY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
char hexaKeys[ROWS][COLS] =
                            { {'0','1','2','3'}, 
                              {'4','5','6','7'}, 
                              {'R','R','R','R'},
                              {'8','R','R','9'} };
byte rowPins[ROWS] = {4,5,6,7};
byte colPins[COLS] = {8,9,10,11};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, 4, 4);
int letterW[8][8] ={
    1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0, 
    0,0,1,0,0,1,0,0,
    1,0,1,0,0,1,0,1,
    1,0,0,0,0,0,0,1, 
    1,1,0,1,1,0,1,1
};

class Reversi {
    public:
    int board[8][8] =
    {   EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT,
        EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT, 
        EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT,
        EPT,EPT,EPT,GRN,RED,EPT,EPT,EPT, 
        EPT,EPT,EPT,RED,GRN,EPT,EPT,EPT, 
        EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT,
        EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT, 
        EPT,EPT,EPT,EPT,EPT,EPT,EPT,EPT, 
    };
    void show_board(int state,int place = -1);
    bool find_placeable_grid(int player);
    bool is_in_board(int x, int y); 
    void place_disk(int player); 
    void alertsound();
    void printL_W(int player); 
    int countdisk(); 
    void changedisk(int row,int col,int player);
    bool surrender = false; 
    void resetboard(); 
    void check_board();
};

void Reversi::resetboard(){ 
    for(int r = 0 ; r < 8 ; ++r){
        for(int c = 0 ; c < 8 ; ++c){
            if(board[r][c] == MIX) board[r][c] = EPT; 
        }
    }
}
void Reversi::show_board(int state,int place = -1) {
    for (int c = 0 ; c < LEN; c++) { 
        digitalWrite(col[c],LOW);
        for (int r = 0; r < LEN; r++) {
            if(state == NOHINT){ // print normal 
                grid switch (board[r][c]) { 
                    case RED: digitalWrite(red[r], LOW); break; 
                    case GRN: digitalWrite(grn[r], LOW); break; 
                }
            }else if(state == HINT){ // print placeable 
                grid switch (board[r][c]) { 
                    case RED: digitalWrite(red[r], LOW); break;
                    case GRN: digitalWrite(grn[r], LOW); break;
                    case MIX: digitalWrite(red[r], LOW); 
                              digitalWrite(grn[r], LOW);
                }
            }else{ // flicker grid 
                int row = place/8; 
                int col = place%8; 
                int i = state - 2; 
                if(row == r && col == c){ 
                    if(i/4%2 == 0){ // flick effect
                        digitalWrite(red[r],LOW);
                        digitalWrite(grn[r],LOW); 
                    }
                }else{ 
                    switch (board[r][c]) { 
                        case RED: digitalWrite(red[r], LOW); break;
                        case GRN: digitalWrite(grn[r], LOW); break; 
                    } 
                }
            } 
        } 
        delay(1);
        for (int r = 0; r < LEN; r++) { 
            digitalWrite(red[r], HIGH); 
            digitalWrite(grn[r], HIGH);
        } 
        digitalWrite(col[c],HIGH);
    }
}
bool Reversi::find_placeable_grid(int player) {
    bool canplace = false;
    int opposite = 3 - player; 
    for (int r = 0; r < LEN; r++) { 
        for (int c = 0; c < LEN; c++) { 
            if (board[r][c] == EPT) {
                for (int dir = 0; dir < 8; dir++) { 
                    bool findSelf = false;
                    bool findOppo = false;
                    int curr = r;
                    int curc = c; 
                    while (is_in_board(curr+=displaceX[dir],curc+=displaceY[dir])) { 
                        if(board[curr][curc] == opposite){ 
                            findOppo = true; 
                        }else if(board[curr][curc] == player){
                            findSelf = true;
                            break; 
                        }else{
                        break; 
                        } 
                    }
                if (findSelf && findOppo) { 
                    canplace = true;
                    board[r][c] = MIX;
                    break; 
                    } 
                }
            } 
        }
    } 
    return canplace;
}
bool Reversi::is_in_board(int x, int y) {
 return (0 <= x && x < LEN && 0 <= y && y < LEN);
}
void Reversi::alertsound(){ for (int i = 0 ; i < 4 ; i++) {
    tone(pin3, NOTE_C6, 200); delay(250); }
}
int Reversi::countdisk(){
    int red = 0;
    int grn = 0; 
    for(int r = 0 ; r < 8 ; ++r){ 
        for(int c = 0 ; c < 8 ; ++c){
            if(board[r][c] == RED) ++red; 
            if(board[r][c] == GRN) ++grn; 
        }
    } 
    Serial.println(red); 
    Serial.println(grn);
    if(red > grn){ 
        return RED; 
    }else if(grn > red){
        return GRN; 
    }else{ 
        return MIX;
    }
}
void Reversi::printL_W(int player){
    for(int i = 0 ; i < 1000 ; ++i){ 
        for (int c = 0 ; c < LEN; c++) {
            digitalWrite(col[c],LOW);
            for (int r = 0; r < LEN; r++) { 
                if(!letterW[r][c]){ 
                    switch (player){ 
                        case RED: digitalWrite(red[r], LOW); break;
                        case GRN: digitalWrite(grn[r], LOW); break;
                        case MIX: digitalWrite(red[r], LOW); 
                        digitalWrite(grn[r], LOW); 
                    } 
                }
            } 
            delay(1);
            for (int r = 0; r < LEN; r++) {
                digitalWrite(red[r], HIGH); 
                digitalWrite(grn[r], HIGH); 
            } 
            digitalWrite(col[c],HIGH); 
        } 
    }
}
void Reversi::changedisk(int row,int col,int player){ 
    int opposite = 3 - player;
    board[row][col] = player; 
    for(int dir = 0 ; dir < 8 ; ++dir){
        bool findsame = false;
        int count = 0; int r = row; int c = col;
        while(is_in_board(r+=displaceX[dir],c+=displaceY[dir])){
            if(board[r][c] == opposite) ++count; // continue 
            else if(board[r][c] == player){ // stop to find
                findsame = true; 
                break; 
            }else break; // mix or empt
        } 
        if(findsame){ 
            for(int i = 0 ; i < count ; ++i) board[r-=displaceX[dir]][c- =displaceY[dir]] = player;
        }
    }
}
void Reversi::place_disk(int player) {
    char place[3] = {0};
    bool re_enter = false; 
    int index = 0; 
    int count = 0;
    while(index < 3){ 
        char input = customKeypad.getKey();
        if (input){
            re_enter = false; 
            if(input == 'R'){ 
                index = 0;
                continue; 
                } 
            if(index == 2){
                if(input == '9') break; 
                else re_enter = true; 
            }
            if(index == 1){ 
                place[1] = input -'0'; 
                if(is_in_board(place[0],place[1])){
                if(board[place[0]][place[1]] != MIX)
                    re_enter = true; 
                }else if(place[0] == 8 && place[1] == 8){
                    alertsound();
                    surrender = true;
                    return;
                }else re_enter = true;
            } 
            if(re_enter){
                alertsound();
                index = 0; 
            }else{ // index is 0 or could place on disk 
                place[index] = input - '0'; ++index; 
            }
        } 
        if(index == 2){ 
            show_board(count%=10000+2,place[0]*8+place[1]); 
        }else show_board(1);
        ++count; 
    }
    changedisk(place[0],place[1],player);
}
void Reversi::check_board(){
    while(true){ 
        char input = customKeypad.getKey(); 
        if(input){ 
            if(input == '9') break;
        } 
        show_board(0); 
    }
}
void setup() {
    // put your setup code here, to run once: 
    Serial.begin(9600); // 設定 Serial Monitor 的傳輸鮑率 
    for(int i=0;i<=7;i++){ 
        pinMode(col[i],OUTPUT); 
        digitalWrite(col[i],HIGH);
        pinMode(grn[i],OUTPUT); 
        digitalWrite(grn[i],HIGH); 
        pinMode(red[i],OUTPUT);
        digitalWrite(red[i],HIGH); 
    }
}
void loop() {
    Reversi reversi;
    for(int game = 0 ; game < 60 ; ++game){
        bool canplace = reversi.find_placeable_grid(game%2+1); 
        if(!canplace){
            continue; 
        } 
        reversi.place_disk(game%2+1);
        reversi.resetboard(); if(reversi.surrender){
        reversi.printL_W(3-(game%2+1));
        break; 
        } 
    }
    if(!reversi.surrender){ 
        int winner = reversi.countdisk();
        reversi.check_board();
        reversi.printL_W(winner); 
    }
}