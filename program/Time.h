#pragma once

// タイムストップ用のグローバル変数宣言
extern bool g_IsTimerStopped;

class GameTime
{
private:
    int frameCounter;   // 0.4秒（24フレーム）を計測するためのカウンタ

    bool isTimeStarted;    // 時間が進み始めたかどうかのフラグ
    bool isHurryBGMPlayed; // 残り100秒のBGMに切り替えたかどうかのフラグ
    bool isBGMStopped;     // タイムアップでBGMを停止したかどうかのフラグ

public:
    void Init();
    void Update();
    void Render();

    int count;          // 残り時間（400スタート）

    // 外部（マリオクラスなど）から時間切れを判定したい時のための関数
    int GetCount() const { return count; }
    bool IsTimeUp() const { return count <= 0; }
    void SetCount(int newCount) { count = newCount; }
};