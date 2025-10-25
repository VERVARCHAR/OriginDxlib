#ifndef __ENEMY_HPP_
#include "object/enemy.hpp"
#endif // __ENEMY_HPP_

#ifndef _BOMB_MANAGER_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

void BombType01(BombManager &bombManager, Enemy &enemy, int time,int dificulty)
{
    for(int i = 0;i < dificulty * 4;i++){
        enemy.usingBombs[bombManager.getEmptyIndex()];
    }

    if (index != -1)
    {
        // 敵の位置に向かって真っ直ぐ進む弾を発射
        int enemyX = 0; // 敵のX座標を取得する関数を呼び出す
        int enemyY = 0; // 敵のY座標を取得する関数を呼び出す

        int startX = enemyX;
        int startY = enemyY;

        int targetX = 500; // 画面中央のX座標
        int targetY = 300; // 画面中央のY座標

        // 速度計算
        int speed = 5;
        int dx = targetX - startX;
        int dy = targetY - startY;
        float length = sqrtf(dx * dx + dy * dy);
        dx = static_cast<int>(dx / length * speed);
        dy = static_cast<int>(dy / length * speed);

        bombManager.setBomb(index, startX, startY, dx, dy, 10, 1);
    }
}