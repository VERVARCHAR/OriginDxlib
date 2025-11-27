#ifndef _UTILS_HPP_
#define _UTILS_HPP_
#include "system/utils.hpp"
#endif

#ifndef _BOMB_HPP_
#define _BOMB_HPP_
#include "object/bomb.hpp"
#endif // _BOMB_MANAGER_HPP_

#ifndef _ENEMY_HPP_
#define _ENEMY_HPP_
#include "object/enemy.hpp"
#endif // _ENEMY_HPP_s

#ifndef _ENEMY_SHOOT_SCRIPT_01_HPP_
#define _ENEMY_SHOOT_SCRIPT_01_HPP_
#include "object/enemyShootScripts/01.hpp"
#endif // _ENEMY_SHOOT_SCRIPT_01_HPP_

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_
#include "object/player.hpp"
#endif // _PLAYER_HPP_

#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include "object/stage.hpp"
#endif

#ifndef _UI_HPP_
#define _UI_HPP_
#include "system/UI.hpp"
#endif

#ifndef _EFFECTER_HPP_
#define _EFFECTER_HPP_
#include "system/effecter.hpp"
#endif

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetUp();

    // 初期ローディング
    Scene scene = PRELOADING;
    Difficulty difficulty = Difficulty::EASY;

    // 最低限のクラス生成
    UI ui;
    Effecter effecter;

    // タイトル画面用の画像読み込み
    ui.startLoading();
    SetUseASyncLoadFlag(TRUE);
    ui.getImage();
    effecter.loadEffecter();
    SetUseASyncLoadFlag(FALSE);

    // ローディング画面用
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        if (!Update())
        {
            break;
        }
        ui.loadingScreen();
        if (GetASyncLoadNum() == 0 && ui.minLoadingTime > 120)
        {
            scene = TITLE;
            break;
        }
        Wait();
        Draw();
    }

    // 各クラス生成
    BombInfo bombs[MAX_BOMBS];
    BombManager bMgr(bombs);

    StageManager sMgr(1, 0, difficulty);

    Player player;
    EnemyShootScript *enemyShootScript;

    int time = 0; // いらないかも??
    // int difficulty = 4;

    bool title = true;
    bool flag = false;

    // マウスを表示状態にする
    SetMouseDispFlag(TRUE);
    int x, y;

    EnemyStatus bossStatus;

    // TODO :Loading
    // Title
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0 && UpdateKey() == 0)
    {
        clsDx();

        if (!Update())
        {
            break;
        }
        switch (scene)
        {
        case TITLE: // タイトル処理

            // TODO タイトルの作成
            // 1が押されたらゲームスタート用のローディング
            if (Key[KEY_INPUT_1] == 1)
            {
                ui.startLoading();
                scene = LOADING;
                break;
            }

            // [DEBUG]
            if (Key[KEY_INPUT_TAB] == 1)
            {
                scene = DEBUG;
                break;
            }
            DrawFormatString(30, 30, GetColor(255, 255, 255), L"Title");
            break;

        case LOADING: // ローディング

            // 各必要要素ローディング
            if (ui.minLoadingTime == 0)
            {
                SetUseASyncLoadFlag(TRUE);
                ui.getInGameImage();
                player.init();
                bMgr.init(bombs);
                sMgr.init(1, 0, difficulty);
                player.loadPlayerImage();
                sMgr.loadEnemy();
                bMgr.setBombsHandle(ui.bombsImageHandle);
                effecter.init();
                SetUseASyncLoadFlag(FALSE);
            }
            ui.loadingScreen();
            if (GetASyncLoadNum() == 0 && ui.minLoadingTime > 120)
            {
                scene = INGAME;
                break;
            }
            break;
        case INGAME: // ゲーム中処理

            // UIの表示
            ui.drawUI(sMgr.getStageInfo());

            bossStatus = sMgr.enemies[sMgr.bossIndex]->getStatus();

            if (bossStatus.isAlive)
            {
                ui.drawBossStatus(bossStatus);
                ui.drawSpellCardText(bossStatus, sMgr.enemies[sMgr.bossIndex]->getSpellInfo(), time);
            }

            GetMousePoint(&x, &y);
            DrawFormatString(500, 500, GetColor(255, 0, 255), L"Mouse : %d,%d", x, y);

            // メニュー画面のキー入力
            if (Key[KEY_INPUT_ESCAPE] == 1)
            {
                sMgr.isPause = !sMgr.isPause;
            }

            // [DEBUG] パワーを1増やす
            if (Key[KEY_INPUT_0] == 1)
            {
                player.setpower();
            }

            // 会話中の処理
            if (sMgr.isTalk)
            {
                ui.talkUI(sMgr.getTalkString(sMgr.talkCount), sMgr.getTalkWho(sMgr.talkCount));
                if (Key[KEY_INPUT_RETURN] == 1)
                {
                    sMgr.talkCount++;
                }
                if (sMgr.talkCount == 7)
                {
                    sMgr.endTalk();
                }
            }

            // 基本的なゲーム処理
            sMgr.updateStage(&bMgr, bombs, &player, &effecter);

            // ゲームオーバーでなく，ポーズ中ならばポーズ画面を表示
            // TODO ポーズ処理
            if (sMgr.isPause && !sMgr.isGameOver)
            {
                DrawString(200, 200, L"PAUSE", GetColor(255, 255, 0));
            }

            // ゲームオーバー時の処理
            if (sMgr.isGameOver)
            {
                DrawString(200, 200, L"Game Over...", GetColor(255, 255, 0));
                if (Key[KEY_INPUT_R] == 1)
                {
                    ui.startLoading();
                    scene = LOADING;
                }
            }

            // [DEBUG]
            bMgr.DEBUG_printAllBombs(bombs);
            // sMgr.DEBUG_print_enemies();

            break;
        case RESULT: // TODO リザルトの作成
            break;
        case DEBUG: // [DEBUG] 以下デバッグ用
            effecter.effecterUpdate();
            effecter.effecterDraw();
            DrawFormatString(10, 10, GetColor(255, 255, 255), L"[DEBUG] Effect");
            if (Key[KEY_INPUT_0] == 1)
            {
                Vec2d p = {100, 100};
                effecter.playEnemyExplode(p);
            }
            if (Key[KEY_INPUT_1] == 1)
            {
                Vec2d p = {200, 100};
                effecter.playPlayerExplode(p);
            }
            break;
        default:
            break;
        }

        Draw();
        Wait();
    }

    DxLib_End();
    return 0;
}
