#pragma once
#include <DxLib.h>
#include <string>
#include <unordered_map>

class SoundManager
{
private:
	// サウンドメモリのハンドルを文字列キーで保存するマップ
	std::unordered_map<std::string, int> bgmList;
	std::unordered_map<std::string, int> seList;

	// 現在再生中のBGMのハンドル
	int currentBGM;

	// シングルトンのためのプライベートコンストラクタ
	SoundManager() : currentBGM(-1) {}
	~SoundManager() {}

public:
	// インスタンスの取得
	static SoundManager& GetInstance()
	{
		static SoundManager instance;
		return instance;
	}

	// コピーを禁止する
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	// 機能
	void Init();                               // サウンドファイルの読み込み
	void PlayBGM(const std::string& name);     // BGMの再生
	void StopBGM();                            // BGMの停止
	void PlaySE(const std::string& name);      // 効果音(SE)の再生
	void Release();                            // サウンドメモリの解放
};