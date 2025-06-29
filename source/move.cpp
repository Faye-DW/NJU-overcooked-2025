#include <cmath>
#include <enum.h>
#include <framework.h>
#include <utility>
#include <queue>
#include <vector>

constexpr double POSITION_TOLERANCE = 0.35;
constexpr double ARRIVAL_DISTANCE = 0.2;
constexpr double FINE_ADJUST_THRESHOLD = 0.1;

Point adjustPlayerPosition(Point position, int playerID) {
    Point gridCenter = {floor(position.x) + 0.5, floor(position.y) + 0.5};
    double xDiff = fabs(gridCenter.x - position.x);
    double yDiff = fabs(gridCenter.y - position.y);
    
    if (xDiff < POSITION_TOLERANCE && yDiff < POSITION_TOLERANCE) {
        Players[playerID].src = gridCenter;
        Players[playerID].adjust = false;
    } else {
        Players[playerID].adjust = true;
    }
    return Players[playerID].src;
}

Point calculateExitPoint(Point target) {
    int gridX = (int)(target.x);
    int gridY = (int)(target.y);
    
    if (gridX == 0) return {1.5, target.y + 0.5};
    if (gridX == width - 1) return {target.x - 0.5, target.y + 0.5};
    if (gridY == 0) return {target.x + 0.5, 1.5};
    if (gridY == height - 1) return {target.x + 0.5, target.y - 0.5};
    
    // Should never reach here for valid exit points
    return target;
}

bool isApproaching(Point current, Point target, int playerID) {
    return current.x == target.x && 
           current.y == target.y && 
           Players[playerID].X_Velocity == 0 && 
           Players[playerID].Y_Velocity == 0;
}

bool hasReached(Point current, Point target, int playerID) {
    double dx = fabs(current.x - target.x);
    double dy = fabs(current.y - target.y);
    double dist = hypot(dx, dy);
    return dist < ARRIVAL_DISTANCE && 
           Players[playerID].X_Velocity == 0 && 
           Players[playerID].Y_Velocity == 0;
}

std::string getFineAdjustment(Point current, Point target, int playerID) {
    double dx = fabs(current.x - target.x);
    double dy = fabs(current.y - target.y);
    double dist = hypot(dx, dy);
    
    if (dist < FINE_ADJUST_THRESHOLD) {
        return " ";
    }
    
    std::string adjustment;
    if (current.x > target.x) adjustment += "L";
    if (current.x < target.x) adjustment += "R";
    if (current.y > target.y) adjustment += "U";
    if (current.y < target.y) adjustment += "D";
    return adjustment;
}

int updateDirection(int currentDir, int newDir, int playerID) {
    bool isStopped = Players[playerID].X_Velocity == 0 && 
                     Players[playerID].Y_Velocity == 0;
                     
    if (isStopped && !Players[playerID].adjust) {
        Players[playerID].cur_direction = newDir;
        return newDir;
    }
    
    if (Players[playerID].adjust) {
        return currentDir;
    }
    
    return -1;
}

std::pair<int, std::string> navigatePath(Point currentPos, Point targetPos, int playerID) {

    if (targetPos.x < 0 || targetPos.x >= width || targetPos.y < 0 || targetPos.y >= height) {
        return {0, "Move "}; // 无效目标，不移动
    }
    Point exitPoint = calculateExitPoint(targetPos);
    Point adjustedPos = adjustPlayerPosition(currentPos, playerID);
    
    // Handle arrival at destination
    if (isApproaching(adjustedPos, exitPoint, playerID)) {
        Players[playerID].cur_direction = -1;
        if (hasReached(currentPos, exitPoint, playerID)) {
            return {1, "Move "};
        }
        return {0, "Move " + getFineAdjustment(currentPos, exitPoint, playerID)};
    }
    
    // Calculate new movement direction
    int newDirection = search_path_direction(playerID, adjustedPos, exitPoint);
    
    // Handle direction changes
    if (Players[playerID].cur_direction == newDirection) {
        if (newDirection < 0) {
            return {0, "Move "};
        }
        return {0, "Move " + directions[newDirection]};
    }
    
    int finalDirection = updateDirection(
        Players[playerID].cur_direction, newDirection, playerID);
    
    if (finalDirection < 0) {
        return {0, "Move "};
    }
    return {0, "Move " + directions[finalDirection]};
}