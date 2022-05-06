#pragma once
#include "Tetris2.h"

class Game; //��ȯ���� ����Ű�� �ʵ��� ���ݼ��� ���ش�.

class Player
{
public:
	Player(int x, int y, shared_ptr<Game> game) : _x(x), _y(y), _game(game){}
	void SetPos(int x, int y);
	void SetRot(int rot);
	int GetRot();
	void SetBrick(int brick);
	int GetBrickNum();
	Type GetBrickType();
	POINT GetPoint();
	void HandleInput(WPARAM keyCode);
	BOOL MoveDown();

private:
	int _nowBrick = -1;
	int _x;
	int _y;
	int _rot = 0;
	shared_ptr<Game> _game;
};

