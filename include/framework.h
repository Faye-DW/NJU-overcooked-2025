#ifndef FRAMEWORK
#define FRAMEWORK

#include <string>
#include <vector>
#include <map>
#include <enum.h>

const double INF = 1e9;

typedef struct Point
{
    double x, y;
}Point;

typedef struct Node
{
    Point pos;
    Point parent;
    double G, H, F;
}Node;

typedef struct Task
{
    int action;
    std::string object;
    Task() : action(Move), object("") {}
    Task(int _action, std::string _object) : action(_action), object(_object) {}
}Task;

struct Ingredient
{
    int x, y, price;
    std::string name;
};

struct Recipe
{
    int time;
    std::string nameBefore, nameAfter;
    std::string kind;
};

struct Order
{
    int validFrame;
    int price;
    int frequency;
    std::vector<std::string> recipe;
};

struct Player
{
    double x, y;
    Point src;
    Point dst;
    double X_Velocity;
    double Y_Velocity;
    int cur_direction;
    int status;
    bool adjust;
    int live;
    ContainerKind containerKind;
    std::vector<std::string> entity;
};

struct Entity
{
    double x, y;
    ContainerKind containerKind;
    std::vector<std::string> entity;
    int currentFrame, totalFrame;
    int sum;
};

typedef struct Work
{
    int player_id;
    std::vector<Task> current_tasks;
    int task_index;
}Work;

void init_read();//初始化时的读入

void init();//预处理

bool frame_read(int nowFrame);//每一帧的读入；返回：是否跳帧

void refresh_tasks();//更新动态盘子的位置

void get_plate_temp_loc();//获取盘子的临时存放点 要在离送菜窗口近一点的操作台上

std::string player_action(int player_id);//玩家的动作命令
//std::pair<std::string, std::string> player_action();

int search_path_direction(int id, Point start, Point end);//根据起点和终点通过A*算法寻得方向

std::pair<int, std::string> Interact_with_object(Point dst);//交互函数

std::pair<int, std::string> Put_object(Point dst, int player_id);//拿去物品和放下物品的操作函数

std::pair<int, std::string> Pick_object(Point dst, std::string object);

std::pair<int, std::string> navigatePath(Point src, Point dst, int player_id);//沿路线运动函数

std::string Direction(Point dst);//获取交互的方向

struct Point getGridCenter(double x, double y);

double ABS(double a, double b);//直线距离函数

double distance(double x1, double y1, double x2, double y2);

double manhattanDist(const Point& a, const Point& b);

void update_plate();

//声明跨文件作用域变量
extern int width;
extern int height;
extern char Map[20 + 5][20 + 5];
extern struct Player Players[2 + 5];
extern int entityCount;
extern struct Entity Entity[20 + 5];
extern struct Order Order[20 + 5];
extern std::string directions[8];
extern Point last_plate_location;//上一次与玩家交互的盘子的位置
extern Point plate_temp_pos;//临时放置盘子去做别的菜的位置 一般在上菜窗口的旁边
extern int nxt[8][2];
extern Work chef;
extern Work cleaner;
extern std::map<std::string, std::vector<Point>> Item_loc;//食材、煎锅、煮锅以及不同方块的位置
extern std::map<std::string, std::vector<Point>> Plate_loc;//盘子和脏盘子的位置
extern std::map<std::vector<std::string>, std::vector<Task>> task_list;//任务列表
extern int hhh;

#endif