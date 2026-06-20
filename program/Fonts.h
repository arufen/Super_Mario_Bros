#pragma once

class MarioFont {
public:
	static MarioFont& GetInstance() {
		static MarioFont instance;
		return instance;
	}

	void Init(); // 画像の読み込みと分割
	void DrawNumber(int x, int y, int number, int digitCount); // 指定桁数で数字を描画
	void DrawTimeLabel(int x, int y);
	void DrawWorldLabel(int x, int y);
	void DrawWorldNumberLabel(int x, int y, int graphHandle);
	void DrawMarioLabel(int x, int y);
	void Exit(); // メモリ解放
	
private:
	int fontGraphHandles[10]; // 0~9のグラフィックハンドルを保持する配列
	int timeLabelHandle;
	int worldLabelHandle;
	int marioLabelHandle;
};