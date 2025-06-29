#include <framework.h>
#include <enum.h>

std::string Direction(Point dst){
    if((int)dst.x == 0) return "L";
    else if((int)dst.x == width - 1) return "R";
    else if((int)dst.y == 0) return "U";
    else if ((int)dst.y == height - 1)return "D";
    else return "";
}

std::pair<int, std::string> Interact_with_object(Point dst){
    std::string interact_direction = Direction(dst);
    int i;
    for(i=0; i < 25; i++){
        if(dst.x == Entity[i].x && dst.y == Entity[i].y){
            break;
        }
    }
    int flag;
    flag = Entity[i].currentFrame == Entity[i].totalFrame - 1 ? 1 : 0;
    return {flag, "Interact " + interact_direction};
}

std::pair<int, std::string> Put_object(Point dst, int player_id)
{
    std::string put_or_pick_direction = Direction(dst);
    return {1, "PutOrPick " + put_or_pick_direction};
}

std::pair<int, std::string> Pick_object(Point dst, std::string object)
{
    std::string put_or_pick_direction = Direction(dst);
    int i;
    if(object == "Pan" || object == "Pot"){//需要等待食物烹饪完成再拿
        for(i = 0; i < entityCount; i++){
            if(Entity[i].x == dst.x && Entity[i].y == dst.y){
                break;
            }
        }
    }
    int flag;
    flag = Entity[i].currentFrame >= Entity[i].totalFrame ? 1 : 0;
    std::string action = Entity[i].currentFrame >= Entity[i].totalFrame ? "PutOrPick " + put_or_pick_direction : "Move ";
    return {flag, action};
}