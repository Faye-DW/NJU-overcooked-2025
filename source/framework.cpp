#include <enum.h>
#include <framework.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

/* 按照读入顺序定义 */
int width, height;
char Map[20 + 5][20 + 5];
int IngredientCount;
struct Ingredient Ingredient[20 + 5];
int recipeCount;
struct Recipe Recipe[20 + 5];
int totalTime, randomizeSeed, totalOrderCount;
struct Order totalOrder[20 + 5];
int orderCount;
struct Order Order[20 + 5];
int k;
struct Player Players[2 + 5];
int entityCount;
struct Entity Entity[20 + 5];
int remainFrame, Fund;
std::map<std::string, std::vector<struct Point>> Item_loc;//食材、煎锅、煮锅以及不同方块的位置
std::map<std::string, std::vector<struct Point>> Plate_loc;//盘子和脏盘子的位置
std::map<std::vector<std::string>, std::vector<struct Task>> task_list;//任务列表
Point last_plate_location;//上一次与玩家交互的盘子的位置
Point plate_temp_pos;//盘子的临时位置
Work chef;
Work cleaner;

void init_read()
{
    std::string s;
    std::stringstream ss;
    int frame;

    /* 读取初始地图信息 */
    std::getline(std::cin, s, '\0');
    ss << s;

    /* 若按照该读入，访问坐标(x, y)等价于访问Map[y][x],你可按照自己的习惯进行修改 */
    ss >> width >> height;
    std::cerr << "Map size: " << width << "x" << height << std::endl;
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            ss >> Map[i][j];

    /* 读入原料箱：位置、名字、以及采购单价 */
    ss >> IngredientCount;
    for (int i = 0; i < IngredientCount; i++)
    {
        ss >> s;
        assert(s == "IngredientBox");
        ss >> Ingredient[i].x >> Ingredient[i].y >> Ingredient[i].name >> Ingredient[i].price;
        struct Point a;
        a.x = Ingredient[i].x;
        a.y = Ingredient[i].y;
        Item_loc[Ingredient[i].name].push_back(a);//记录食材的位置
    }

    /* 读入配方：加工时间、加工前的字符串表示、加工容器、加工后的字符串表示 */
    ss >> recipeCount;
    for (int i = 0; i < recipeCount; i++)
    {
        ss >> Recipe[i].time >> Recipe[i].nameBefore >> Recipe[i].kind >> Recipe[i].nameAfter;
    }

    /* 读入总帧数、当前采用的随机种子、一共可能出现的订单数量 */
    ss >> totalTime >> randomizeSeed >> totalOrderCount;

    /* 读入订单的有效帧数、价格、权重、订单组成 */
    for (int i = 0; i < totalOrderCount; i++)
    {
        ss >> totalOrder[i].validFrame >> totalOrder[i].price >> totalOrder[i].frequency;
        getline(ss, s);
        std::stringstream tmp(s);
        while (tmp >> s)
            totalOrder[i].recipe.push_back(s);
    }

    /* 读入玩家信息：初始坐标 */
    ss >> k;
    assert(k == 2);
    for (int i = 0; i < k; i++)
    {
        ss >> Players[i].x >> Players[i].y;
        Players[i].containerKind = ContainerKind::None;
        Players[i].entity.clear();
    }

    /* 读入实体信息：坐标、实体组成 */
    ss >> entityCount;
    for (int i = 0; i < entityCount; i++)
    {
        ss >> Entity[i].x >> Entity[i].y >> s;
        Entity[i].entity.push_back(s);
    }
}

bool frame_read(int nowFrame)
{
    std::string s;
    std::stringstream ss;
    int frame;
    std::getline(std::cin, s, '\0');
    ss.str(s);
    /*
      如果输入流中还有数据，说明游戏已经在请求下一帧了
      这时候我们应该跳过当前帧，以便能够及时响应游戏。
    */
    if (std::cin.rdbuf()->in_avail() > 0)
    {
        std::cerr << "Warning: skipping frame " << nowFrame
             << " to catch up with the game" << std::endl;
        return true;
    }
    ss >> s;
    assert(s == "Frame");
    int currentFrame;
    ss >> currentFrame;
    assert(currentFrame == nowFrame);
    ss >> remainFrame >> Fund;
    /* 读入当前的订单剩余帧数、价格、以及配方 */
    ss >> orderCount;
    for (int i = 0; i < orderCount; i++)
    {
        ss >> Order[i].validFrame >> Order[i].price;
        Order[i].recipe.clear();
        getline(ss, s);
        std::stringstream tmp(s);
        while (tmp >> s)
        {
            Order[i].recipe.push_back(s);
        }
    }
    ss >> k;
    assert(k == 2);
    /* 读入玩家坐标、x方向速度、y方向速度、剩余复活时间 */
    for (int i = 0; i < k; i++)
    {
        ss >> Players[i].x >> Players[i].y >> Players[i].X_Velocity >> Players[i].Y_Velocity >> Players[i].live;
        getline(ss, s);
        std::stringstream tmp(s);
        Players[i].containerKind = ContainerKind::None;
        Players[i].entity.clear();
        while (tmp >> s)
        {
            /*
                若若该玩家手里有东西，则接下来一个分号，分号后一个空格，空格后为一个实体。
                以下是可能的输入（省略前面的输入）：
                 ;  : fish
                 ; @  : fish
                 ; @ Plate : fish
                 ; Plate
                 ; DirtyPlates 1
                ...
            */

            /* 若你不需要处理这些，可直接忽略 */
            if (s == ";" || s == ":" || s == "@" || s == "*")
                continue;

            /* 
                Todo: 其他容器
            */
            if (s == "Plate")
                Players[i].containerKind = ContainerKind::Plate;
            else if (s == "DirtyPlates")
                Players[i].containerKind = ContainerKind::DirtyPlates;
            else
                Players[i].entity.push_back(s);
        }
    }

    ss >> entityCount;
    /* 读入实体坐标 */
    for (int i = 0; i < entityCount; i++)
    {
        ss >> Entity[i].x >> Entity[i].y;
        getline(ss, s);
        std::stringstream tmp(s);
        Entity[i].containerKind = ContainerKind::None;
        Entity[i].entity.clear();
        Entity[i].currentFrame = Entity[i].totalFrame = 0;
        Entity[i].sum = 1;
        while (tmp >> s)
        {
            /*
                读入一个实体，例子：
                DirtyPlates 2
                fish
                DirtyPlates 1 ; 15 / 180

            */

            /* 若你不需要处理这些，可直接忽略 */
            if (s == ":" || s == "@" || s == "*")
                continue;
            if (s == ";")
            {
                tmp >> Entity[i].currentFrame >> s >> Entity[i].totalFrame;
                assert(s == "/");
                break;
            }
            
            /* 
                Todo: 其他容器
            */
            if (s == "Plate"){
                Entity[i].containerKind = ContainerKind::Plate;
            }
            else if (s == "DirtyPlates")
            {
                Entity[i].containerKind = ContainerKind::DirtyPlates;
                tmp >> Entity[i].sum;
            }
            else if (s == "Pan"){
                Entity[i].containerKind = ContainerKind::Pan;
            }
            else if (s == "Pot"){
                Entity[i].containerKind = ContainerKind::Pot;
            }
            Entity[i].entity.push_back(s);
        }
    }
    return false;
}

void update_plate()//每一帧更新盘子和脏盘子的位置
{
    Plate_loc.clear();
    for(int i = 0; i < entityCount; i++){
        if(Entity[i].containerKind == ContainerKind::Plate){
            Plate_loc["Plate"].push_back({Entity[i].x, Entity[i].y});
        }
        else if(Entity[i].containerKind == ContainerKind::DirtyPlates){
            Plate_loc["DirtyPlates"].push_back({Entity[i].x, Entity[i].y});
        }
    }
    if(Players[0].entity.size() > 0) Players[0].status = 1;
    if(Players[1].entity.size() > 0) Players[1].status = 1;
}

void get_task_list()//根据订单种类生成任务列表
{
    for(int i = 0; i < totalOrderCount; i++){
        std::sort(totalOrder[i].recipe.begin(), totalOrder[i].recipe.end());
        for(auto s : totalOrder[i].recipe){
            if(s == "fish" || s == "rice" || s == "kelp"){
                task_list[totalOrder[i].recipe].push_back(Task(Move, s));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, s));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "temp_plate_pos"));
            }
            else if(s == "c_fish"){
                task_list[totalOrder[i].recipe].push_back(Task(Move, "fish"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "fish"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Interact, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "temp_plate_pos"));
            }
            else if(s == "c_kelp"){
                task_list[totalOrder[i].recipe].push_back(Task(Move, "kelp"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "kelp"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Interact, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "temp_plate_pos"));
            }
            else if(s == "s_rice"){
                task_list[totalOrder[i].recipe].push_back(Task(Move, "rice"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "rice"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Pot"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Pot"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Pot"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Pot"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "temp_plate_pos"));

            }
            else if(s == "s_fish"){
                task_list[totalOrder[i].recipe].push_back(Task(Move, "fish"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "fish"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Interact, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Chop"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Pan"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "Pan"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Plate"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "Pan"));
                task_list[totalOrder[i].recipe].push_back(Task(Pick, "Pan"));
                task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
                task_list[totalOrder[i].recipe].push_back(Task(Put, "temp_plate_pos"));
            }
            else{
                //assert(0);
            }
        }
        task_list[totalOrder[i].recipe].push_back(Task(Move, "temp_plate_pos"));
        task_list[totalOrder[i].recipe].push_back(Task(Pick, "temp_plate_pos"));
        task_list[totalOrder[i].recipe].push_back(Task(Move, "ServiceWindow"));
        task_list[totalOrder[i].recipe].push_back(Task(Put, "ServiceWindow"));
    }
    //洗盘子任务
    task_list[{"wash"}].push_back(Task(Move, "PlateReturn"));
    task_list[{"wash"}].push_back(Task(Pick, "PlateReturn"));
    task_list[{"wash"}].push_back(Task(Move, "Sink"));
    task_list[{"wash"}].push_back(Task(Put, "Sink"));
    task_list[{"wash"}].push_back(Task(Interact, "Sink"));
    //没有脏盘子时，返回脏盘子返回处
    task_list[{"wait_dirty_plates"}].push_back(Task(Move, "PlateReturn"));
    //没有可用的盘子时，停止等待
    task_list[{"wait"}].push_back(Task(Move, "ServiceWindow"));
}

void get_plate_temp_loc(){
    double tempx = -2.0, tempy = -2.0;
    double service_x = Item_loc["ServiceWindow"][0].x;
    double service_y = Item_loc["ServiceWindow"][0].y;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if((j == 0 && i != 0 && i != height - 1) || (j == width - 1 && i != 0 && i != height - 1) || (i == 0 && j != 0 && j != width - 1) || (i == height - 1 && j != 0 && j != width - 1)){//边缘且不是角落
                if(Map[i][j] == '*'){
                    if(distance((double)j, (double)i, service_x, service_y) <= distance(tempx, tempy, service_x, service_y)){
                        int player_temp_x = j, player_temp_y = i;
                        if(j == 0) player_temp_x = 1;
                        else if(j == width - 1) player_temp_x = j-1;
                        else if(i == 0) player_temp_y = i+1;
                        else if(i == height - 1) player_temp_y = i-1;
                        if(Map[player_temp_y][player_temp_x] != '.') continue;
                        tempx = j;
                        tempy = i;
                    }
                }
            }
        }
    }
    plate_temp_pos = {tempx, tempy};
}

void init()
{
    //获取不同方块的位置
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            struct Point a;
            a.x = j;
            a.y = i;
            switch (Map[i][j])
            {
                case 'c'://切菜板
                    Item_loc["Chop"].push_back(a);
                    break;
                case 'k'://洗水池
                    Item_loc["Sink"].push_back(a);
                    break;
                case 'r'://盘子洗净后会出现的位置
                    Item_loc["PlateRack"].push_back(a);
                    break;
                case 'p'://客人用餐一定时间后返还的脏盘子的位置
                    Item_loc["PlateReturn"].push_back(a);
                    break;
                case '$'://服务窗口
                    Item_loc["ServiceWindow"].push_back(a);
                    break;
                default:
                    break;
            }
        }
    }
    //获取煎锅和煮锅的位置
    for(int i = 0; i < entityCount; i++){
        if(Entity[i].entity[0] == "Pan" || Entity[i].entity[0] == "Pot"){
            struct Point a;
            a.x = Entity[i].x;
            a.y = Entity[i].y;
            Item_loc[Entity[i].entity[0]].push_back(a);
        }
    }
    //获取任务列表
    get_task_list();

    //初始化 上次放置盘子的地方
    last_plate_location.x = -1;
    last_plate_location.y = -1;
    //找到与洗碗池交互的位置
    int washing_interact_x = (int)Item_loc["Sink"][0].x;
    int washing_interact_y = (int)Item_loc["Sink"][0].y;

    //获得临时存放盘子的位置
    get_plate_temp_loc();

    //玩家属性初始化
    Players[0].src.x = Players[0].x;
    Players[0].src.y = Players[0].y;
    Players[1].src.x = Players[1].x;
    Players[1].src.y = Players[1].y;
    //cooking.task_index = washing.task_index = -1;
    chef.task_index = cleaner.task_index = -1;
    Players[0].status = Players[1].status = 0;
    Players[0].adjust = Players[1].adjust = false;
}

