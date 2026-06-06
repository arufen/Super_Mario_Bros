#pragma once
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Collidable.h"

enum class BlockType {BRICK, QUESTION, HARD, PIPE};
enum class WorldZone {OVERWORLD1_1, UNDERWORLD1_1, OVERWORLD1_4};

struct BlockSpawnData {
    int gridX;
    int gridY;
    BlockType type;

    int pipeHeight = 0;
    bool isWarpPipe = false;
};

const BlockSpawnData world1_1data[] = {

    // 1st Block Cluster (Overworld 1-25)
    {16, 9, BlockType::QUESTION},{21, 9, BlockType::QUESTION},{22, 5, BlockType::QUESTION},{23, 9, BlockType::QUESTION},
    { 20, 9 , BlockType::BRICK}, { 22, 9 , BlockType::BRICK}, { 24, 9 , BlockType::BRICK},
    { 28, 11, BlockType::PIPE, 2, false},
    { 38, 10, BlockType::PIPE, 3, false},
    { 46, 9, BlockType::PIPE, 4, false},
    { 57, 9, BlockType::PIPE, 4, true},
    // 2nd Block Cluster (Long structural ceiling row 75-90)
    { 78, 9, BlockType::QUESTION},
    { 77, 9 , BlockType::BRICK}, { 79, 9 , BlockType::BRICK}, { 80, 5 , BlockType::BRICK}, { 81, 5 , BlockType::BRICK}, 
    { 82, 5 , BlockType::BRICK}, { 83, 5 , BlockType::BRICK}, { 84, 5 , BlockType::BRICK}, { 85, 5 , BlockType::BRICK},
    { 86, 5 , BlockType::BRICK},{ 87, 5 , BlockType::BRICK},

    // 3rd Block Cluster (91-115)
    {94, 5, BlockType::QUESTION},
    { 91, 5 , BlockType::BRICK}, { 92, 5 , BlockType::BRICK}, { 93, 5 , BlockType::BRICK}, { 94, 9 , BlockType::BRICK},
    { 100, 9 , BlockType::BRICK},{ 101, 9 , BlockType::BRICK},
    {106, 9, BlockType::QUESTION},{109, 5, BlockType::QUESTION},{109, 9, BlockType::QUESTION},{112, 9, BlockType::QUESTION},
    // 4th Block Cluster (Double row block bridge spanning across columns 118-132)

    { 118, 9 , BlockType::BRICK},
    { 121, 5 , BlockType::BRICK}, { 122, 5 , BlockType::BRICK}, { 123, 5 , BlockType::BRICK},
    { 128, 5 , BlockType::BRICK}, { 129, 9 , BlockType::BRICK}, { 130, 9 , BlockType::BRICK}, { 131, 5 , BlockType::BRICK},
    { 129, 5 , BlockType::QUESTION}, { 130, 5 , BlockType::QUESTION},

    // 5th Block Cluster (Final structures right before the pyramid walls)
    { 163, 11, BlockType::PIPE, 2, false},
    { 179, 11, BlockType::PIPE, 2, false},
    { 170, 9 , BlockType::QUESTION},
    { 168, 9 , BlockType::BRICK}, { 169, 9 , BlockType::BRICK}, { 171, 9 , BlockType::BRICK}
};
//const BlockSpawnData underworld1_1data[] =
//{
//
//};
class IBlock
{
public:
    virtual ~IBlock() {}
    virtual void Update() = 0;
    virtual void Render(Camera camera) = 0;
    

    virtual Float2 GetPos() const = 0;
    virtual bool IsActive() const = 0;
    virtual Image GetSprite() const = 0;
};
