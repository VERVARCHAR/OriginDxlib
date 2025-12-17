#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <vector>
#include <string>
#include "DxLib.h"
#include "system/UI.hpp"
#include "object/stage.hpp" // Scene, Difficulty など（あなたの環境に合わせて）

extern int Key[256];

enum class MenuAction
{
    StartStory,
    StartExtra,
    OpenOptions,
    Exit
};

enum class StartMode
{
    Story,
    Extra
};

struct MenuItem
{
    MenuAction action;
    std::wstring text;
    int labelGraph = -1; // 画像を使う場合（無いなら -1）
    bool enabled = true;
};

struct CharacterItem
{
    int id = 0;
    std::wstring name;
    int portraitGraph = -1; // 立ち絵/アイコンに使える
};

class Title
{
public:
    Title();

    // βでは Extra を無効にしたい → false のまま
    void setExtraEnabled(bool v) { extraEnabled_ = v; }

    // キャラ追加（後で増やせる）
    void addCharacter(const CharacterItem &c);

    // main.cpp の case TITLE ではこれだけ
    // ※必要なら playerType / mode なども参照で渡す
    void drawTitle(UI &ui, Scene &scene, Difficulty &difficulty, int &sceneChangeMinimumTime);

    // スタート時にゲーム側に渡したい情報（必要なら getter を追加）
    int selectedCharacterId() const { return selectedCharacterId_; }
    StartMode startMode() const { return startMode_; }

private:
    enum class State
    {
        MAIN,
        SELECT_DIFFICULTY,
        SELECT_CHARACTER,
        OPTIONS,
        EXIT_CONFIRM
    };

    State state_;
    int cursor_;
    int subCursor_;
    int t_;

    bool extraEnabled_;
    StartMode startMode_; // Story / Extra
    Difficulty selectedDifficulty_;
    int selectedCharacterId_;

    std::vector<MenuItem> mainMenu_;
    std::vector<CharacterItem> characters_;

    // 入力
    static bool up();
    static bool down();
    static bool left();
    static bool right();
    static bool ok();
    static bool back();

    // 構築
    void rebuildMainMenu();

    // 更新
    void updateMain(UI &ui, Scene &scene, int &sceneChangeMinimumTime);
    void updateSelectDifficulty();
    void updateSelectCharacter(UI &ui, Scene &scene, int &sceneChangeMinimumTime);
    void updateOptions();
    void updateExit(Scene &scene);

    // 描画（テキスト/画像切替を内包）
    void drawPanel(int l, int t, int r, int b) const;
    void drawMenuItem(int x, int y, bool selected, const MenuItem &item) const;
    void drawTextItem(int x, int y, bool selected, const wchar_t *text, bool enabled) const;
    void drawImageItem(int x, int y, bool selected, int graphHandle, bool enabled) const;
};
