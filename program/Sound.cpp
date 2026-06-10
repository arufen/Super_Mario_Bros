#include "Sound.h"

void SoundManager::Init()
{
	// BGMの読み込み
	bgmList["Stage1-1"] = LoadSoundMem("data/sound/bgm_stage1-1.mp3");
	bgmList["Stage1-1_Hurry"] = LoadSoundMem("data/sound/bgm_stage1-1_hurry.mp3");

	// SE（効果音）の読み込み
	seList["Jump_Small"] = LoadSoundMem("data/sound/mario_jump_small.wav");
	// seList["Coin"]  = LoadSoundMem("data/sound/se_coin.wav");
	// seList["Stomp"] = LoadSoundMem("data/sound/se_stomp.wav");
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

void SoundManager::StopBGM()
{
	if (currentBGM != -1 && CheckSoundMem(currentBGM))
	{
		StopSoundMem(currentBGM);
	}
}

void SoundManager::PlaySE(const std::string& name)
{
	// 指定されたSEが登録されていない場合は何もしない
	if (seList.find(name) == seList.end()) return;

	// SEは複数同時に鳴る可能性があるのでバックグラウンド再生
	PlaySoundMem(seList[name], DX_PLAYTYPE_BACK);
}

void SoundManager::Release()
{
	// 全てのサウンドメモリを解放する
	InitSoundMem();
	bgmList.clear();
	seList.clear();
	currentBGM = -1;
}