#include <DxLib.h>
#include "Time.h"
#include "Game.h"
#include "Sound.h"

// 1カウント減少させるのに必要なフレーム数 (60FPS × 0.4秒 = 24フレーム)
const int UPDATE_INTERVAL_FRAME = 24;

void GameTime::Init()
{
	count = 200; // サウンド確認のため一旦200スタートにしてます
    frameCounter = 0;

    // フラグの初期化
    isTimeStarted = false;
    isHurryBGMPlayed = false;
    isBGMStopped = false;
}

void GameTime::Update()
{
    // 初回のUpdateが呼ばれた（時間が進み始めた）瞬間に通常のBGMを再生する
    if (!isTimeStarted)
    {
        isTimeStarted = true;
        SoundManager::GetInstance().PlayBGM("Stage1-1");
    }

    // 残り時間が100になった瞬間に、急ぎ版のBGMへ切り替える
    if (count <= 100 && count > 0 && !isHurryBGMPlayed)
    {
        isHurryBGMPlayed = true;
        SoundManager::GetInstance().PlayBGM("Stage1-1_Hurry");
    }

    // 0になったらストップ
    if (count <= 0)
    {
        count = 0;

        // タイムアップした瞬間にBGMを停止する
        if (!isBGMStopped)
        {
            isBGMStopped = true;
            SoundManager::GetInstance().StopBGM();
            // ここにタイムアップ時の効果音の再生を入れる
        }
        return;
    }

    frameCounter++;
    if (frameCounter >= UPDATE_INTERVAL_FRAME)
    {
        count--;
        frameCounter = 0; // カウンタをリセット
    }
}

void GameTime::Render()
{
    // 画面解像度が大きいため、文字が小さくならないようフォントサイズを設定
    SetFontSize(32);

    // 右上に表示するための座標計算（右端から少し左、上から少し下）
    int x = SCREEN_W - 100;
    int y = 40;

    // 白文字で「400」のように3桁（%03d）で描画
    DrawFormatString(x, y, GetColor(255, 255, 255), "%03d", count);

    // 他の描画処理に影響を及ぼさないよう、フォントサイズをデフォルトに戻しておく
    SetFontSize(16);
}