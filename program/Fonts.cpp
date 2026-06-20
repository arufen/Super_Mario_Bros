#include "DxLib.h"
#include "Fonts.h"
#include <stdio.h>

void MarioFont::Init()
{
    // 0 から 9 までの10個の独立した画像をロードして配列（ハンドル）に格納
    for (int i = 0; i < 10; ++i)
    {
        char path[64];
        // 実際の画像のフォルダパスに合わせて変更してください (例: "data/image/0.png")
        sprintf_s(path, "data/image/Fonts/%d.png", i);

        fontGraphHandles[i] = LoadGraph(path);

        // --- DEBUG BLOCK TO CHECK IF LOADING FAILS ---
        if (fontGraphHandles[i] == -1)
        {
            // This prints a message directly on your game window
            printfDx("Failed to load image: %s\n", path);
        }
    }

    timeLabelHandle = LoadGraph("data/image/Fonts/TIME.png");
    worldLabelHandle = LoadGraph("data/image/Fonts/WORLD.png");
    marioLabelHandle = LoadGraph("data/image/Fonts/MARIO.png");

}
void MarioFont::DrawTimeLabel(int x, int y)
{
    if (timeLabelHandle != -1)
    {
        // Draws the static "TIME" image asset at specified coordinates
        DrawGraph(x, y, timeLabelHandle, TRUE);
    }
}
void MarioFont::DrawWorldLabel(int x, int y)
{
    if (worldLabelHandle != -1)
    {
        // Draws the static "TIME" image asset at specified coordinates
        DrawGraph(x, y, worldLabelHandle, TRUE);
    }
}

void MarioFont::DrawMarioLabel(int x, int y)
{
    if (marioLabelHandle != -1) DrawGraph(x, y, marioLabelHandle, TRUE);
}

void MarioFont::DrawWorldNumberLabel(int x, int y, int graphHandle)
{
        if (graphHandle != -1) DrawGraph(x, y, graphHandle, TRUE);
}
void MarioFont::DrawNumber(int x, int y, int number, int digitCount)
{
    int temp = number;
    int fontWidth = 32; // 1文字あたりの幅（お持ちのPNGのサイズに合わせて調整してください）

    // 右詰め（1の位から順に）で描画するためのループ
    for (int i = 0; i < digitCount; ++i)
    {
        int digit = temp % 10;
        temp /= 10;

        int drawX = x + (digitCount - 1 - i) * fontWidth;

        if (fontGraphHandles[digit] != -1)
        {
            DrawGraph(drawX, y, fontGraphHandles[digit], TRUE);
        }
    }
}

void MarioFont::Exit()
{
    for (int i = 0; i < 10; ++i)
    {
        if (fontGraphHandles[i] != -1)
        {
            DeleteGraph(fontGraphHandles[i]);
            fontGraphHandles[i] = -1;
        }
    }
    // Clean up the label asset
    if (timeLabelHandle != -1) {
        DeleteGraph(timeLabelHandle);
        timeLabelHandle = -1;
    }
}