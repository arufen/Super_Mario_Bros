#pragma once

class GameScore
{
private:
    int score;          // スコア

public:
    void Init();
    void Update();
    void Render();

    // 外部（マリオクラスなど）から時間切れを判定したい時のための関数
    void AddScore(int amount);
    int GetScore() const { return score; }
    void SetScore(int newScore) { score = newScore; }
};

extern GameScore MainScore;