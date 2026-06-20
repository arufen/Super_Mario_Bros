#include <DxLib.h>
#include "Time.h"
#include "Game.h"
#include "Sound.h"
#include "StageManager.h"
#include "Mario.h"
#include "Fonts.h"

// 外部にあるマリオの実体を参照する
extern Mario MainMario;

// タイムストップ用のグローバル変数の実体
bool g_IsTimerStopped = false;

// 1カウント減少させるのに必要なフレーム数
const int UPDATE_INTERVAL_FRAME = 25;

static std::string currentPlayingBGMKey = "";

void GameTime::Init()
{
	count = WORLD1_1TIME; // サウンド確認のため一旦200スタートにしてます
    frameCounter = 0;

    // フラグの初期化
    isTimeStarted = false;
    isHurryBGMPlayed = false;
    isBGMStopped = false;
    g_IsTimerStopped = false;

    currentPlayingBGMKey = "";

    // リスタート（再初期化）時に前回のBGMが残らないように一度完全に止める
    SoundManager::GetInstance().StopBGM();
}

void GameTime::Update()
{
    if (isConvertingTimeToScore)
    {
        if (count > 0)
        {
            count--; // Tick down 1 second unit
            MainScore.AddScore(50); // Formula: 50 points per remaining second

            // Optional: Play a repetitive high-pitched tick sound effect
            // SoundManager::GetInstance().PlaySE("Tick"); 
        }
        else
        {
            isConvertingTimeToScore = false; // Countdown complete
        }
        return; // Bypass normal gameplay timing logic while converting
    }
    // マリオがすでに死亡状態なら、時間の進行もBGMの自動切り替えもすべて停止する
    if (MainMario.GetState() == MarioState::DEAD)
    {
        return;
    }

    // 0になったらストップ＆死亡処理
    if (count <= 0)
    {
        count = 0;
        if (MainMario.GetState() != MarioState::GOAL) {
            // タイムアップした瞬間にBGMを停止する
            if (!isBGMStopped)
            {
                isBGMStopped = true;

                // マリオを死亡状態へ移行させる
                MainMario.ToDeadState();
            }
        }
        return;
    }

    // ゴールに到達してタイマーが停止フラグになったら以降の処理を行わない
    if (g_IsTimerStopped)
    {
        return;
    }

    // ----------------------------------------------------
    // 現在の状態（ゾーン × 残り時間）に応じたBGM自動切り替え
    // ----------------------------------------------------
    // 1. 残り時間から「急ぎ（Hurry）」状態かどうかを判定
    bool isHurry = (count <= 100);

    if (isHurry && !isHurryBGMPlayed)
    {
        isHurryBGMPlayed = true;
        SoundManager::GetInstance().StopBGM();
        SoundManager::GetInstance().PlayBGMOnce("HurryAction");
    }

    // 2. StageManagerから現在のゾーンを取得
    // ※もしコンパイルエラーが出る場合は、StageManager.hにある「現在地を返す関数」の名前に差し替えてください
    WorldZone currentZone = StageManager::GetInstance().GetWorldZone();

    // 3. 条件に合わせて「次に再生すべきBGM」のキー名を決める
    std::string targetBGM = "";
    if (currentZone == WorldZone::OVERWORLD)
    {
        targetBGM = isHurry ? "Stage1-1_Hurry" : "Stage1-1";
    }
    else if (currentZone == WorldZone::UNDERWORLD)
    {
        targetBGM = isHurry ? "Stage1-1_Underground_Hurry" : "Stage1-1_Underground";
    }

    if (!SoundManager::GetInstance().IsPlayingBGM("HurryAction"))
    {
        if (targetBGM != "" && targetBGM != currentPlayingBGMKey)
        {
            currentPlayingBGMKey = targetBGM; 
            SoundManager::GetInstance().PlayBGM(targetBGM);
        }
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
    //// 画面解像度が大きいため、文字が小さくならないようフォントサイズを設定
    //SetFontSize(32);

    //// 右上に表示するための座標計算（右端から少し左、上から少し下）
    //int x = SCREEN_W - 100;
    //int y = 40;

    //// 白文字で「400」のように3桁（%03d）で描画
    //DrawFormatString(x, y, GetColor(255, 255, 255), "%03d", count);

    //// 他の描画処理に影響を及ぼさないよう、フォントサイズをデフォルトに戻しておく
    //SetFontSize(16);
    /*MarioFont::GetInstance().DrawMarioLabel(80, 20);
    MarioFont::GetInstance().DrawNumber(80, 52, MainMario.GetScore(), 6);

    MarioFont::GetInstance().DrawTimeLabel(SCREEN_W - 232, 20);
    MarioFont::GetInstance().DrawWorldLabel(SCREEN_W - 482, 20);
    MarioFont::GetInstance().DrawNumber(SCREEN_W - 200, 60, count, 3);*/
}