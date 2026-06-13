#pragma once
#include "HelpfulFunc.h"
#include "Camera.h"
#include "Collidable.h"

const int line1 = 17;
const int line2 = 25;
const int line3 = 26;
const int line4 = 27;

enum class BlockType {BRICK, COINBRICK, QUESTION, HARD, PIPE, UNDERWORLDPIPE, HARDPIPE, HIDDEN};

enum class WorldZone { OVERWORLD, UNDERWORLD};

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

    { 64, 8, BlockType::HIDDEN},
    // 2nd Block Cluster (Long structural ceiling row 75-90)
    { 78, 9, BlockType::QUESTION},
    { 77, 9 , BlockType::BRICK}, { 79, 9 , BlockType::BRICK}, { 80, 5 , BlockType::BRICK}, { 81, 5 , BlockType::BRICK}, 
    { 82, 5 , BlockType::BRICK}, { 83, 5 , BlockType::BRICK}, { 84, 5 , BlockType::BRICK}, { 85, 5 , BlockType::BRICK},
    { 86, 5 , BlockType::BRICK},{ 87, 5 , BlockType::BRICK},

    // 3rd Block Cluster (91-115)
    {94, 5, BlockType::QUESTION},
    { 91, 5 , BlockType::BRICK}, { 92, 5 , BlockType::BRICK}, { 93, 5 , BlockType::BRICK}, { 94, 9 , BlockType::COINBRICK},
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

const BlockSpawnData underworld1_1data[] =
{
    //WALL
  // Left wall blocks (HARD)
{ 56, 17, BlockType::HARD },
{ 56, 18, BlockType::HARD },
{ 56, 19, BlockType::HARD },
{ 56, 20, BlockType::HARD },
{ 56, 21, BlockType::HARD },
{ 56, 22, BlockType::HARD },
{ 56, 23, BlockType::HARD },
{ 56, 24, BlockType::HARD },
{ 56, 25, BlockType::HARD },
{ 56, 26, BlockType::HARD },
{ 56, 27, BlockType::HARD },

// Right pipe wall blocks (HARDPIPE)
{ 71, 17, BlockType::HARDPIPE },
{ 71, 18, BlockType::HARDPIPE },
{ 71, 19, BlockType::HARDPIPE },
{ 71, 20, BlockType::HARDPIPE },
{ 71, 21, BlockType::HARDPIPE },
{ 71, 22, BlockType::HARDPIPE },
{ 71, 23, BlockType::HARDPIPE },
{ 71, 24, BlockType::HARDPIPE },
{ 71, 25, BlockType::HARDPIPE },

  { 69, 26, BlockType::UNDERWORLDPIPE},

  { 60, line1 , BlockType::HARD },{ 61, line1 , BlockType::HARD },{ 62, line1 , BlockType::HARD },{ 63, line1, BlockType::HARD },{ 64, line1 , BlockType::HARD },{ 65, line1 , BlockType::HARD },{ 66, line1 , BlockType::HARD },
  { 60, line2 , BlockType::HARD },{ 61, line2 , BlockType::HARD },{ 62, line2 , BlockType::HARD },{ 63, line2, BlockType::HARD },{ 64, line2 , BlockType::HARD },{ 65, line2 , BlockType::HARD },{ 66, line2 , BlockType::HARD },
  { 60, line3 , BlockType::HARD },{ 61, line3 , BlockType::HARD },{ 62, line3 , BlockType::HARD },{ 63, line3, BlockType::HARD },{ 64, line3 , BlockType::HARD },{ 65, line3 , BlockType::HARD },{ 66, line3 , BlockType::HARD },
  { 60, line4 , BlockType::HARD },{ 61, line4 , BlockType::HARD },{ 62, line4 , BlockType::HARD },{ 63, line4, BlockType::HARD },{ 64, line4 , BlockType::HARD },{ 65, line4 , BlockType::HARD },{ 66, line4 , BlockType::HARD },
};

const BlockSpawnData world1_4data[] = 
{
    { 20, 9 , BlockType::BRICK},
    
};
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
