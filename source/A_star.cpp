#include <utility>
#include <queue>
#include <vector>
#include <cmath>
#include <framework.h>

struct Point Center(double pos_x, double pos_y)
{
    struct Point center;
    int x_floor = (int)pos_x;
    int y_floor = (int)pos_y;
    double x_round = x_floor + 0.5;
    double y_round = y_floor + 0.5;
    center.x = x_round;
    center.y = y_round;
    return center;
}

double ABS(double a, double b){
    return a<b?b-a:a-b;
}

double distance(double x1, double y1, double x2, double y2)
{
    return ABS(x1, x2) + ABS(y1, y2);
}

std::string directions[8] = { "L", "LU", "U", "RU", "R", "RD", "D", "LD" };

struct PathNode {
    Point pos;
    double totalCost;
    double pathCost;
    double heuristic;
    Point parent;
};

PathNode nodeGrid[25][25];
int visited[25][25];

const int dirDelta[8][2] = {{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};

#define SQRT2 1.41421356237309504

Point getGridCenter(double x, double y) {
    return {floor(x) + 0.5, floor(y) + 0.5};
}

inline double manhattanDist(const Point& a, const Point& b) {
    return fabs(a.x - b.x) + fabs(a.y - b.y);
}

bool isMoveBlocked(int playerID, int dir, int cx, int cy, int nx, int ny) {
    if(nx < 0 || nx >= width || ny < 0 || ny >= height) 
        return true;
    
    if(Map[ny][nx] != '.' || Map[ny][cx] != '.' || Map[cy][nx] != '.') 
        return true;
    
    const Player& opponent = Players[playerID ^ 1];
    const Point oppPos = {floor(opponent.x), floor(opponent.y)};
    
    if((nx == oppPos.x && ny == oppPos.y) ||
       (cx == oppPos.x && ny == oppPos.y) ||
       (nx == oppPos.x && cy == oppPos.y)) 
        return true;
    
    if(opponent.cur_direction >= 0) {
        const int* delta = dirDelta[opponent.cur_direction];
        const Point oppNext = {oppPos.x + delta[0], oppPos.y + delta[1]};
        
        if((nx == oppNext.x && ny == oppNext.y) ||
           (cx == oppNext.x && ny == oppNext.y) ||
           (nx == oppNext.x && cy == oppNext.y))
            return true;
    }
    
    return false;
}

int search_path_direction(int playerID, Point start, Point target) {
    for(int x = 0; x < 25; ++x) {
        for(int y = 0; y < 25; ++y) {
            nodeGrid[x][y] = {{x+0.5, y+0.5}, INF, INF, INF, {-1,-1}};
            visited[x][y] = 0;
        }
    }

    Point startCenter = getGridCenter(start.x, start.y);
    Point targetCenter = getGridCenter(target.x, target.y);
    Point startGrid = {floor(startCenter.x), floor(startCenter.y)};
    Point targetGrid = {floor(targetCenter.x), floor(targetCenter.y)};

    auto nodeCompare = [](const PathNode& a, const PathNode& b) {
        return a.totalCost > b.totalCost;
    };

    std::priority_queue<PathNode, std::vector<PathNode>, decltype(nodeCompare)> 
        frontier(nodeCompare);

    nodeGrid[(int)startGrid.x][(int)startGrid.y] = {
        startCenter, 0.0, 0.0, manhattanDist(startCenter, targetCenter), startCenter
    };
    frontier.push(nodeGrid[(int)startGrid.x][(int)startGrid.y]);

    while(!frontier.empty()) {
        PathNode current = frontier.top();
        frontier.pop();
        
        int cx = (int)current.pos.x;
        int cy = (int)current.pos.y;
        
        if(visited[cx][cy]) continue;
        visited[cx][cy] = 1;
        
        if(cx == (int)targetGrid.x && cy == (int)targetGrid.y) {
            PathNode temp = current;

            //
            int step = 0;
            while(!(temp.parent.x == startCenter.x && temp.parent.y == startCenter.y)) {
                temp = nodeGrid[(int)temp.parent.x][(int)temp.parent.y];
            }
            
            for(int d = 0; d < 8; ++d) {
                if(startGrid.x + dirDelta[d][0] == (int)temp.pos.x && 
                   startGrid.y + dirDelta[d][1] == (int)temp.pos.y) {
                    return d;
                }
            }
            return -1;
        }

        for(int d = 0; d < 8; ++d) {
            int nx = cx + dirDelta[d][0];
            int ny = cy + dirDelta[d][1];
            
            if(nx >= 0 && nx < width && ny >= 0 && ny < height && 
               !visited[nx][ny] && !isMoveBlocked(playerID, d, cx, cy, nx, ny)) {
                
                double moveCost = (d % 2) ? SQRT2 : 1.0;
                double newPathCost = current.pathCost + moveCost;
                
                if(newPathCost < nodeGrid[nx][ny].pathCost) {
                    nodeGrid[nx][ny].pathCost = newPathCost;
                    nodeGrid[nx][ny].heuristic = manhattanDist(
                        {nx + 0.5, ny + 0.5}, targetCenter);
                    nodeGrid[nx][ny].totalCost = newPathCost + nodeGrid[nx][ny].heuristic;
                    nodeGrid[nx][ny].parent = {cx + 0.5, cy + 0.5};
                    frontier.push(nodeGrid[nx][ny]);
                }
            }
        }
    }
    return -1;
}



