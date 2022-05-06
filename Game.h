#pragma once

#include "Tetris2.h"
#include "Player.h"

#define BW 10
#define BH 20
#define TS 24

//자기 자신을 sharedPtr로 보내기 위해서 사용
class Game : public enable_shared_from_this<Game>
{
public:
    ~Game() {
        DeleteObject(_blockSheet);
    }
	void Init(HINSTANCE inst, HWND hwnd);
	void Update(ULONGLONG tick);
	void Render(HDC hdc);
    void PrintTile(HDC hdc, int x, int y, Type t);
    void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitSheet, int idx);

	void GetInput(WPARAM keyCode);
    void StartGame();
    void PauseGame();

    void MakeNewBrick(); //새로운 블럭 만들어주는 함수
    void UpdateBoard(); //지정된 게임판만을 새로 그리는 함수
    Type GetAround(POINT p, int brick, int rot);
    void TestFull(); 

    BOOL CheckPlayerRender(int x, int y); 
    //플레이어가 그려져야하는지 체크
private:
    const int blockCount = 7;
    HINSTANCE _hInst;
    HWND _hWndMain;

	Type _board[BW + 2][BH + 2];
	unique_ptr<Player> _player;
	
	int _nextBrick;

	int _score = 0;
	int _brickCount;
	GameStatus _status;
	int _blockTime;
    int _currentTime = 0;

    HBITMAP _blockSheet;
    
    Point _Shape[7][4][4] = {
    { {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
    { {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
    { {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
    { {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
    { {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
    { {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
    { {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },
    };
};