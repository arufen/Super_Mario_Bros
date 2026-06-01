//#pragma once
//#include "IBlock.h"
//
//class Pipe : public IBlock
//{
//public:
//    Pipe();
//
//    void Init(Float2 startPos, int tLeft, int tRight, int bLeft, int bRight, int graphHandle);
//    void Update() override;
//    void Render(Float2 cameraPos) override;
//
//    // Triggered when Mario hits the block from underneath
//    void Bump() override;
//
//    Float2 GetPos() const override { return pos; }
//    bool IsActive() const { return active; }
//
//private:
//    Float2 pos;
//    bool active;
//    bool isWarpPipe;
//    int total_blocks;
//
//    int texTL, texTR, texBL, texBR;// texture handle for 4 parts of pipe
//
//    Image image;
//};
