#include "Player.h"
#include "Game.h"

void Player::SetPos(int x, int y)
{
	_x = x;
	_y = y;
}

void Player::SetRot(int rot)
{
	_rot = rot;
}

int Player::GetRot()
{
	return _rot;
}

void Player::SetBrick(int brick)
{
	_nowBrick = brick;
}

int Player::GetBrickNum()
{
	return _nowBrick;
}

Type Player::GetBrickType()
{
	return static_cast<Type>(_nowBrick + 1);
}

POINT Player::GetPoint()
{
	return POINT{ _x, _y };
}

void Player::HandleInput(WPARAM keyCode)
{
    if (_nowBrick == -1) return;
    POINT p = GetPoint();
	switch (keyCode)
	{
    case VK_LEFT:
        p.x -= 1;
        if (_game->GetAround(p, _nowBrick, _rot) == Type::EMPTY)
        {
            _x--;
            _game->UpdateBoard();
        }
        break;
    case VK_RIGHT:
        p.x += 1;
        if (_game->GetAround(p, _nowBrick, _rot) == Type::EMPTY)
        {
            _x++;
            _game->UpdateBoard();
        }
        break;
    case VK_UP:
    {
        int targetRot = (_rot == 3 ? 0 : _rot + 1);
        if (_game->GetAround(p, _nowBrick, targetRot) == Type::EMPTY)
        {
            _rot = targetRot;
            _game->UpdateBoard();
        }
    }
        
        break;
    case VK_DOWN:
        if (MoveDown() == TRUE) //ÇÑÄ­ ³»·È´Âµ¥ ¹Ù´Ú¿¡ ´ê¾Ò´Ù¸é
        {
            _game->MakeNewBrick();
        }
        break;
    case VK_SPACE:
        while (MoveDown() == FALSE) { ; }
        _game->MakeNewBrick();
        break;
    }

}

BOOL Player::MoveDown()
{
    POINT p = GetPoint();
    p.y += 1;
    if (_game->GetAround(p, _nowBrick, _rot) != Type::EMPTY) {
        _game->TestFull();
        return TRUE;
    }
    _y++;
    _game->UpdateBoard();
    return FALSE;
}
