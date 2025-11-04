#include "system/utils.hpp"

// value of setup
static int mStartTime;            // time of starting measurement
static int mCount;                // conter
static float mFps;                // fps
static const int sample_fps = 60; // sample mean fps
static const int FPS = 60;        // setting fps
int Key[256];

void SetUp()
{
    // フルスクリーンモード時の解像度モードをモニターの最大解像度を使用するモードに設定
    SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_DESKTOP);

    SetGraphMode(1920, 1080, 32);
    SetWindowSize(1920, 1080);

    DxLib_Init();
    SetDrawScreen(DX_SCREEN_BACK);
}

bool Update()
{
    if (mCount == 0)
    {
        mStartTime = GetNowCount(); // save time of 1 flame
    }
    if (mCount == sample_fps)
    { // if 60 flame cacl mean
        int t = GetNowCount();
        mFps = 1000.f / ((t - mStartTime) / (float)sample_fps);
        mCount = 0;
        mStartTime = t;
    }
    mCount++;
    return true;
}

void Draw()
{
    DrawFormatString(1200, 680, GetColor(255, 255, 255), L"%.1fps", mFps);
}

void Wait()
{
    int tookTime = GetNowCount() - mStartTime;     // took time
    int waitTime = mCount * 1000 / FPS - tookTime; // must wait time
    if (waitTime > 0)
    {
        Sleep(waitTime); // wait
    }
}

int UpdateKey()
{
    char tmpKey[256];
    GetHitKeyStateAll(tmpKey); // Get now input state
    for (int i = 0; i < 256; i++)
    {
        if (tmpKey[i] != 0)
        {
            Key[i]++;
        }
        else
        {
            Key[i] = 0;
        }
    }
    return 0;
}

void Debug(int obj_size)
{
    DrawFormatString(100, 100, GetColor(255, 255, 255), L"size : %d", obj_size);
}