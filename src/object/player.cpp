#ifndef _PLAYER_HPP__
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

Player::Player()
{
    pos.x = 350;
    pos.y = 500;
    hp = 10;
    radius = 5;
    type = 0;
    strcpy(name, "Default");
}

Player::~Player()
{
    ;
}

void Player::playerUpdate(BombManager bMgr, BombInfo bombs[MAX_BOMBS])
{
    // 敵の状態更新ロジックをここに実装
    DrawBox(pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10, GetColor(0, 0, 255), TRUE);
    DrawCircle(pos.x, pos.y, radius, GetColor(255, 0, 255), TRUE);

    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].isUsing && !bombs[i].isPlayers && isHit(&bombs[i], pos, radius))
        {
            hp--;
        }
    }
}

Vec2d Player::getPosition()
{
    return pos;
}

void Player::getBMgrData(BombManager &_BombManager)
{
}
