#include "Game.h"

void Game::Init(HINSTANCE inst, HWND hwnd)
{
	_player = make_unique<Player>(BW / 2, 3, shared_from_this());

	_status = GameStatus::GAMEOVER; //게임오버 상태로 시작
	_hInst = inst;
	_hWndMain = hwnd;

	RECT cRect;
	SetRect(&cRect, 0, 0, (BW + 12) * TS, (BH + 2) * TS);
	AdjustWindowRect(&cRect, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, TRUE);
	SetWindowPos(_hWndMain, NULL, 0, 0, cRect.right - cRect.left, cRect.bottom - cRect.top, SWP_NOMOVE | SWP_NOZORDER);

	srand(GetTickCount());
	_blockSheet = LoadBitmap(_hInst, MAKEINTRESOURCE(IDB_BLOCKSHEET));
}

void Game::StartGame()
{
	//게임오버 상태가 아니라면 재시작 ㄴ
	if (_status != GameStatus::GAMEOVER) return; 

	for (int i = 0; i < BW + 2; i++)
	{
		for (int j = 0; j < BH + 2; j++)
		{
			_board[i][j] = (j == 0 || j == BH + 1 || i == 0 || i == BW + 1) ? Type::WALL : Type::EMPTY;
		}
	}
	_score = 0;
	_status = GameStatus::RUNNING;
	_nextBrick = rand() % blockCount;
	MakeNewBrick();
	_blockTime = 1000; //1초에 한번씩 블럭 내려온다.
}

void Game::PauseGame()
{
	if (_status == GameStatus::RUNNING)
		_status = GameStatus::PAUSE;
	else if (_status == GameStatus::PAUSE)
		_status = GameStatus::RUNNING;
}

void Game::Update(ULONGLONG tick)
{
	_currentTime += static_cast<int>(tick);
	if (_currentTime > _blockTime) {
		_currentTime = 0;
		if (_player->MoveDown() == TRUE) {
			MakeNewBrick();
		}
	}
}

void Game::Render(HDC hdc)
{
	TCHAR msgStr[128];

	//위아래 라인 그리고
	for (int x = 0; x < BW + 1; x++)
	{
		PrintTile(hdc, x, 0, Type::WALL);
		PrintTile(hdc, x, BH + 1, Type::WALL);
	}
	//좌우라인그리기
	for (int y = 0; y < BH + 2; y++)
	{
		PrintTile(hdc, 0, y, Type::WALL);
		PrintTile(hdc, BW + 1, y, Type::WALL);
	}

	//게임판과 플레이어 그리기
	for (int x = 1; x < BW + 1; x++)
	{
		for (int y = 1; y < BH + 1; y++)
		{
			if (CheckPlayerRender(x, y)) {
				PrintTile(hdc, x, y, _player->GetBrickType());
			}
			else {
				PrintTile(hdc, x, y, _board[x][y]);
			}
		}
	}

	//다음 모양 그리기
   //먼저 외곽선을 그리고
	for (int x = BW + 3; x <= BW + 11; x++)
	{
		for (int y = BH - 5; y <= BH + 1; y++)
		{
			if (x == BW + 3 || x == BW + 11 || y == BH - 5 || y == BH + 1)
			{
				PrintTile(hdc, x, y, Type::WALL);
			}
			else {
				PrintTile(hdc, x, y, Type::EMPTY);
			}
		}
	}
	//다음 블럭 그린다.
	if (_status != GameStatus::GAMEOVER)
	{
		for (int i = 0; i < 4; i++)
		{
			int x = BW + 7 + _Shape[_nextBrick][0][i].x;
			int y = BH - 2 + _Shape[_nextBrick][0][i].y;
			PrintTile(hdc, x, y, static_cast<Type>(_nextBrick + 1));
		}
	}

	//정보 출력
	lstrcpy(msgStr, _T("GGM Tetris"));
	TextOut(hdc, (BW + 4) * TS, 30, msgStr, lstrlen(msgStr));
	wsprintf(msgStr, _T("점수 : %d   "), _score);
	TextOut(hdc, (BW + 4) * TS, 60, msgStr, lstrlen(msgStr));
	wsprintf(msgStr, _T("벽돌 : %d 개"), _brickCount);
	TextOut(hdc, (BW + 4) * TS, 90, msgStr, lstrlen(msgStr));

}

BOOL Game::CheckPlayerRender(int x, int y)
{
	if (_status != GameStatus::GAMEOVER && _player->GetBrickNum() != -1) {
		for (int idx = 0; idx < 4; idx++)
		{
			POINT p = _player->GetPoint();
			int rot = _player->GetRot();
			int nowBrick = _player->GetBrickNum();
			if (x == p.x + _Shape[nowBrick][rot][idx].x && y == p.y + _Shape[nowBrick][rot][idx].y) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

void Game::PrintTile(HDC hdc, int x, int y, Type t)
{
	DrawBitmap(hdc, x * TS, y * TS, _blockSheet, static_cast<int>(t) );
}

void Game::DrawBitmap(HDC hdc, int x, int y, HBITMAP hBitSheet, int idx)
{
	int sx = idx % 4;
	int sy = idx / 4;

	HDC hMemDC;
	HBITMAP oldBitmap;

	hMemDC = CreateCompatibleDC(hdc);
	oldBitmap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitSheet));

	StretchBlt(hdc, x, y, TS, TS, hMemDC, sx * TS, sy * TS, TS, TS, SRCCOPY);

	SelectObject(hMemDC, oldBitmap);
	DeleteDC(hMemDC);
}


void Game::GetInput(WPARAM keyCode)
{
	if(_status == GameStatus::RUNNING)
		_player->HandleInput(keyCode);
}

Type Game::GetAround(POINT p, int brick, int rot)
{
	for (int i = 0; i < 4; i++)
	{
		Type t = _board[p.x + _Shape[brick][rot][i].x][p.y + _Shape[brick][rot][i].y];
		if (Type::EMPTY != t) return t;
	}
	return Type::EMPTY;
}

void Game::TestFull()
{
	int count = 0;
	//가지고 있는 벽돌을 내려놓는다.
	POINT p = _player->GetPoint();
	int nowBrick = _player->GetBrickNum();
	int rot = _player->GetRot();
	Type t = _player->GetBrickType();
	for (int i = 0; i < 4; i++)
	{
		_board[p.x + _Shape[nowBrick][rot][i].x][p.y + _Shape[nowBrick][rot][i].y] = t;
	}

	//요기는 작성하게 해보자
	for (int y = 1; y < BH + 1; y++)
	{
		BOOL isClear = TRUE;
		for (int x = 1; x < BW + 1; x++)
		{
			if (_board[x][y] == Type::EMPTY) {
				isClear = FALSE;
				break;
			}
		}

		if (isClear == TRUE)
		{
			count++;
			//지워지는 줄에서부터 위쪽으로 전부 
			for (int ty = y; ty > 1; ty--)
			{
				for (int x = 1; x < BW + 1; x++)
				{
					_board[x][ty] = _board[x][ty - 1]; //윗줄 데이터를 아래로 복사함
				}
			}
			UpdateBoard();
		}
	}

	//요기까지오면 지워진 줄수가 count에 담긴다.
	_score += count * 10; //줄수에 따라 다르게 점수 올리는것도 가능
	if (_brickCount % 10 == 0 && _blockTime > 200)
	{
		_blockTime -= 50;
	}
}

void Game::MakeNewBrick()
{
	_brickCount++;
	_player->SetBrick(_nextBrick);
	
	_player->SetPos(BW / 2, 3);
	_player->SetRot(0);
	
	_nextBrick = rand() % blockCount;
	//UpdateBoard();
	InvalidateRect(_hWndMain, NULL, FALSE);

	Type t = GetAround(_player->GetPoint(), _player->GetBrickNum(), _player->GetRot());
	if (t != Type::EMPTY)
	{
		_status = GameStatus::GAMEOVER;
		MessageBox(_hWndMain, _T("GAME OVER"), _T("알림"), MB_OK);
	}

}

void Game::UpdateBoard()
{
	RECT rt;
	SetRect(&rt, TS, TS, (BW + 1) * TS, (BH + 1) * TS);
	InvalidateRect(_hWndMain, &rt, FALSE);

	SetRect(&rt, (BW + 4) * TS, (BH - 6) * TS, (BW + 10) * TS, (BH)*TS);
	InvalidateRect(_hWndMain, &rt, FALSE);
}






