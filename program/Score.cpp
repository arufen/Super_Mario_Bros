#include <DxLib.h>
#include "Score.h"
#include "Fonts.h"

// Instantiate the global score manager
GameScore MainScore;

void GameScore::Init()
{
    score = 0; // Start at 000000
}

void GameScore::AddScore(int amount)
{
    score += amount;

    // Prevent the score from overflowing past 6 digits max (999,999)
    if (score > 999999)
    {
        score = 999999;
    }
}

void GameScore::Render()
{
    // Draw the score on the upper left side of the screen
    // X=80 matches the alignment of Mario's name text layer, Y=52 aligns with the Timer numbers height
    MarioFont::GetInstance().DrawNumber(80, 52, score, 6);
}