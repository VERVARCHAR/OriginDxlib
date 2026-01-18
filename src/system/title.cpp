#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "system/title.hpp"
#include <algorithm>
#include <cmath>

Title::Title()
{
    state_ = State::MAIN;
    cursor_ = 0;
    subCursor_ = 0;
    t_ = 0;

    extraEnabled_ = false; // βでは false
    startMode_ = StartMode::Story;
    selectedDifficulty_ = Difficulty::EASY;
    selectedCharacterId_ = 0;

    menuFont = CreateFontToHandle(L"はんなり明朝", 32, 4, DX_FONTTYPE_ANTIALIASING);

    // 最低1キャラは居る想定（1体でもOK）
    // 既にどこかで追加するならここは不要
    // characters_.push_back({0, L"Player", -1});

    rebuildMainMenu();
}

Title::~Title()
{
    DeleteFontToHandle(menuFont);
}

void Title::addCharacter(const CharacterItem &c)
{
    characters_.push_back(c);
    if (characters_.size() == 1)
    {
        selectedCharacterId_ = characters_[0].id;
    }
}

bool Title::up() { return Key[KEY_INPUT_UP] == 1; }
bool Title::down() { return Key[KEY_INPUT_DOWN] == 1; }
bool Title::left() { return Key[KEY_INPUT_LEFT] == 1; }
bool Title::right() { return Key[KEY_INPUT_RIGHT] == 1; }
bool Title::ok() { return (Key[KEY_INPUT_Z] == 1) || (Key[KEY_INPUT_RETURN] == 1); }
bool Title::back() { return (Key[KEY_INPUT_X] == 1) || (Key[KEY_INPUT_ESCAPE] == 1); }

void Title::rebuildMainMenu()
{
    mainMenu_.clear();

    mainMenu_.push_back(MenuItem{MenuAction::StartStory, L"GameStart", -1, true});
    mainMenu_.push_back(MenuItem{MenuAction::StartExtra, L"ExtraStart", -1, extraEnabled_});
    mainMenu_.push_back(MenuItem{MenuAction::OpenOptions, L"Options", -1, true});
    mainMenu_.push_back(MenuItem{MenuAction::Exit, L"Exit", -1, true});

    // 将来増やす時は push_back するだけ
}

void Title::drawPanel(int l, int t, int r, int b) const
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
    DrawBox(l, t, r, b, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Title::drawTextItem(int x, int y, bool selected, const wchar_t *text, bool enabled) const
{
    int bounce = (int)(2.0 * std::sin(t_ * 0.15));
    int col = enabled ? (selected ? GetColor(255, 255, 255) : GetColor(180, 180, 180))
                      : GetColor(120, 120, 120);

    if (selected)
        DrawFormatStringToHandle(x - 28, y + bounce, col, menuFont, L"▶");
    DrawFormatStringToHandle(x, y, col, menuFont, L"%s", text);

    if (!enabled)
    {
        // DrawFormatString(x + 220, y, GetColor(140, 140, 140), L"(Locked)");
        DrawFormatStringToHandle(
            x + 220,
            y,
            GetColor(140, 140, 140),
            menuFont,
            L"(Locked)");
    }
}

void Title::drawImageItem(int x, int y, bool selected, int graphHandle, bool enabled) const
{
    int bounce = (int)(2.0 * std::sin(t_ * 0.15));
    int alpha = enabled ? (selected ? 255 : 200) : 120;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    // 画像はサイズがバラつくので、基準位置だけ固定しておく（後で調整しやすい）
    DrawGraph(x, y + (selected ? bounce : 0), graphHandle, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (selected)
    {
        DrawFormatStringToHandle(x - 28, y + bounce, GetColor(255, 255, 255), menuFont, L"▶");
    }
}

void Title::drawMenuItem(int x, int y, bool selected, const MenuItem &item) const
{
    if (item.labelGraph != -1)
    {
        drawImageItem(x, y, selected, item.labelGraph, item.enabled);
    }
    else
    {
        drawTextItem(x, y, selected, item.text.c_str(), item.enabled);
    }
}

void Title::updateMain(UI &ui, Scene &scene, int &sceneChangeMinimumTime)
{
    // カラム数が増えてもOK
    const int n = (int)mainMenu_.size();
    if (n <= 0)
        return;

    if (up())
        cursor_ = (cursor_ + n - 1) % n;
    if (down())
        cursor_ = (cursor_ + 1) % n;

    if (!ok())
        return;

    // 選択不可は弾く
    if (!mainMenu_[cursor_].enabled)
        return;

    switch (mainMenu_[cursor_].action)
    {
    case MenuAction::StartStory:
        startMode_ = StartMode::Story;
        state_ = State::SELECT_DIFFICULTY;
        subCursor_ = 0;
        break;

    case MenuAction::StartExtra:
        startMode_ = StartMode::Extra;
        state_ = State::SELECT_DIFFICULTY;
        subCursor_ = 0;
        break;

    case MenuAction::OpenOptions:
        state_ = State::OPTIONS;
        subCursor_ = 0;
        break;

    case MenuAction::Exit:
        scene = Scene::EXIT;
        break;
    }
}

void Title::updateSelectDifficulty()
{
    const int n = 3;
    if (up())
        subCursor_ = (subCursor_ + n - 1) % n;
    if (down())
        subCursor_ = (subCursor_ + 1) % n;

    if (ok())
    {
        if (subCursor_ == 0)
            selectedDifficulty_ = Difficulty::EASY;
        if (subCursor_ == 1)
            selectedDifficulty_ = Difficulty::NORMAL;
        if (subCursor_ == 2)
            selectedDifficulty_ = Difficulty::HARD;

        state_ = State::SELECT_CHARACTER;
        subCursor_ = 0;
    }
    if (back())
        state_ = State::MAIN;
}

void Title::updateSelectCharacter(UI &ui, Scene &scene, int &sceneChangeMinimumTime)
{
    if (characters_.empty())
    {
        // キャラが未登録ならとりあえず即開始でも良い
        // ただ、βは1キャラのはずなので通常はここに来ない
        ui.startLoading();
        scene = LOADING;
        sceneChangeMinimumTime = 0;
        return;
    }

    const int n = (int)characters_.size();
    if (up())
        subCursor_ = (subCursor_ + n - 1) % n;
    if (down())
        subCursor_ = (subCursor_ + 1) % n;

    if (ok())
    {
        selectedCharacterId_ = characters_[subCursor_].id;
        // ここでゲーム開始へ
        ui.startLoading();
        scene = LOADING;
        sceneChangeMinimumTime = 0;
    }
    if (back())
        state_ = State::SELECT_DIFFICULTY;
}

void Title::updateOptions()
{
    // まずはβは空でもOK。必要なら音量やキーコンフィグ等
    if (back())
        state_ = State::MAIN;
}

void Title::updateExit(Scene &scene)
{
    // βでは "戻る" のみでもOK
    if (back())
        state_ = State::MAIN;

    // 「本当に終了」までやるならここで ok() を拾って終了フラグを返す設計にする
    if (ok())
    {
        state_ = State::EXIT_CONFIRM;
    }
}

void Title::drawTitle(UI &ui, Scene &scene, Difficulty &difficulty, int &sceneChangeMinimumTime)
{
    t_++;

    // 背景（今のタイトル画像）
    ui.drawTitle();

    // 連打防止（あなたの既存仕様を尊重）
    if (sceneChangeMinimumTime <= 20)
    {
        sceneChangeMinimumTime++;
        return;
    }

    // MAINの enabled はフラグで変わるので、毎フレームでも再構築OK（軽い）
    rebuildMainMenu();

    // 右パネル
    const int panelL = 720;
    const int panelT = 250;
    const int panelR = WINDOW_WIDTH - 60;
    const int panelB = WINDOW_HEIGHT - 40;

    drawPanel(panelL, panelT, panelR, panelB);

    const int x = panelL + 60;
    const int y0 = panelT + 100;
    const int dy = 52;

    // 更新
    if (state_ == State::MAIN)
        updateMain(ui, scene, sceneChangeMinimumTime);
    else if (state_ == State::SELECT_DIFFICULTY)
        updateSelectDifficulty();
    else if (state_ == State::SELECT_CHARACTER)
        updateSelectCharacter(ui, scene, sceneChangeMinimumTime);
    else if (state_ == State::OPTIONS)
        updateOptions();
    else if (state_ == State::EXIT_CONFIRM)
        updateExit(scene);

    // 描画
    if (state_ == State::MAIN)
    {
        DrawFormatStringToHandle(panelL + 30, panelT + 30, GetColor(230, 230, 230), menuFont, L"MENU");

        for (int i = 0; i < (int)mainMenu_.size(); ++i)
        {
            drawMenuItem(x, y0 + dy * i, (cursor_ == i), mainMenu_[i]);
        }
    }
    else if (state_ == State::SELECT_DIFFICULTY)
    {
        DrawFormatStringToHandle(panelL + 30, panelT + 30, GetColor(230, 230, 230), menuFont,
                                 (startMode_ == StartMode::Story) ? L"DIFFICULTY (Story)" : L"DIFFICULTY (Extra)");

        // 3段固定。将来増やしても良い
        drawTextItem(x, y0 + dy * 0, subCursor_ == 0, L"EASY", true);
        drawTextItem(x, y0 + dy * 1, subCursor_ == 1, L"NORMAL", true);
        drawTextItem(x, y0 + dy * 2, subCursor_ == 2, L"HARD", true);

        DrawFormatStringToHandle(panelL + 30, panelB - 60, GetColor(200, 200, 200), menuFont, L"Z: OK   X: Back");
    }
    else if (state_ == State::SELECT_CHARACTER)
    {
        DrawFormatStringToHandle(panelL + 30, panelT + 30, GetColor(230, 230, 230), menuFont, L"CHARACTER");

        for (int i = 0; i < (int)characters_.size(); ++i)
        {
            bool sel = (subCursor_ == i);
            // 画像で表示したい場合は portraitGraph を使うなど拡張できる
            drawTextItem(x, y0 + dy * i, sel, characters_[i].name.c_str(), true);
        }
        DrawFormatStringToHandle(panelL + 30, panelB - 60, GetColor(200, 200, 200), menuFont, L"Z: Start   X: Back");
    }
    else if (state_ == State::OPTIONS)
    {
        DrawFormatStringToHandle(panelL + 30, panelT + 30, GetColor(230, 230, 230), menuFont, L"OPTIONS");
        DrawFormatStringToHandle(panelL + 30, panelT + 90, GetColor(200, 200, 200), menuFont, L"(beta) Not implemented yet");
        DrawFormatStringToHandle(panelL + 30, panelB - 60, GetColor(200, 200, 200), menuFont, L"X: Back");
    }
    else if (state_ == State::EXIT_CONFIRM)
    {
        DrawFormatStringToHandle(panelL + 30, panelT + 30, GetColor(230, 230, 230), menuFont, L"EXIT");
        DrawFormatStringToHandle(panelL + 30, panelT + 100, GetColor(200, 200, 200), menuFont, L"X: Back");
    }

    // 決定済み difficulty をゲーム側へ反映（“選択中”は Title が持つ）
    difficulty = selectedDifficulty_;

    DrawFormatStringToHandle(panelL + 30, panelB - 30, GetColor(128, 128, 128), menuFont, L"beta 0.1");
}
