#include "Sound.h"

void SoundManager::Init()
{
	// BGMの読み込み
	bgmList["Stage1-1"] = LoadSoundMem("data/sound/bgm_stage1-1.mp3");
	bgmList["Stage1-1_Hurry"] = LoadSoundMem("data/sound/bgm_stage1-1_hurry.mp3");

	// 地下BGMの読み込み
	bgmList["Stage1-1_Underground"] = LoadSoundMem("data/sound/bgm_stage1-1_underground.mp3");
	bgmList["Stage1-1_Underground_Hurry"] = LoadSoundMem("data/sound/bgm_stage1-1_underground_hurry.mp3");

	// 死亡BGMの読み込み
	bgmList["Mario_Dead"] = LoadSoundMem("data/sound/bgm_mario_dead.mp3");

	// 急ぎBGMの読み込み
	bgmList["HurryAction"] = LoadSoundMem("data/sound/bgm_hurryaction.mp3");

	// SE（効果音）の読み込み
	seList["Jump_Small"] = LoadSoundMem("data/sound/small_mario_jump.wav");
	seList["Coin"]  = LoadSoundMem("data/sound/se_coin.wav");
	seList["Warp"] = LoadSoundMem("data/sound/se_warp.wav");
	seList["Bump"] = LoadSoundMem("data/sound/se_brick_block_bump.wav");
	seList["Break"] = LoadSoundMem("data/sound/se_brick_block_break.wav");
	seList["Kick"] = LoadSoundMem("data/sound/se_kick.wav");
	seList["Stomp"] = LoadSoundMem("data/sound/se_stomp.wav");
}

void SoundManager::PlayBGM(const std::string& name)
{
	// 指定されたBGMが登録されていない場合は何もしない
	if (bgmList.find(name) == bgmList.end()) return;

	int nextBGM = bgmList[name];

	// すでに同じBGMが流れている場合は最初から再生し直さない
	if (currentBGM == nextBGM && CheckSoundMem(currentBGM)) return;

	// 別のBGMが流れているなら止める
	if (currentBGM != -1 && CheckSoundMem(currentBGM))
	{
		StopSoundMem(currentBGM);
	}

	currentBGM = nextBGM;
	PlaySoundMem(currentBGM, DX_PLAYTYPE_LOOP); // ループ再生
}

void SoundManager::PlayBGMOnce(const std::string& name)
{
	if (bgmList.find(name) == bgmList.end()) return;

	int nextBGM = bgmList[name];

	if (currentBGM == nextBGM && CheckSoundMem(currentBGM)) return;

	if (currentBGM != -1 && CheckSoundMem(currentBGM))
	{
		StopSoundMem(currentBGM);
	}

	currentBGM = nextBGM;
	PlaySoundMem(currentBGM, DX_PLAYTYPE_BACK); // 1回だけ再生
}

void SoundManager::StopBGM()
{
	if (currentBGM != -1 && CheckSoundMem(currentBGM))
	{
		StopSoundMem(currentBGM);
	}
}

void SoundManager::PlayDeadBGM()
{
	// 登録されているかチェック
	if (bgmList.find("Mario_Dead") == bgmList.end()) return;

	// 現在流れているステージBGMを即座に停止
	StopBGM();

	// 死亡BGMのハンドルを現在の再生中BGM（currentBGM）として上書き記憶する
	currentBGM = bgmList["Mario_Dead"];

	// 1回だけ再生（DX_PLAYTYPE_BACK）で鳴らす
	PlaySoundMem(currentBGM, DX_PLAYTYPE_BACK);
}

void SoundManager::PlaySE(const std::string& name)
{
	// 指定されたSEが登録されていない場合は何もしない
	if (seList.find(name) == seList.end()) return;

	// SEは複数同時に鳴る可能性があるのでバックグラウンド再生
	PlaySoundMem(seList[name], DX_PLAYTYPE_BACK);
}

bool SoundManager::IsPlayingBGM(const std::string& name)
{
	if (bgmList.find(name) == bgmList.end()) return false;
	return currentBGM == bgmList[name] && CheckSoundMem(currentBGM) == 1;
}

bool SoundManager::IsPlayingSE(const std::string& name)
{
	if (seList.find(name) == seList.end()) return false;
	return CheckSoundMem(seList[name]) == 1;
}

void SoundManager::Release()
{
	// 全てのサウンドメモリを解放する
	InitSoundMem();
	bgmList.clear();
	seList.clear();
	currentBGM = -1;
}