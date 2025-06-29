#include <iostream>
#include <framework.h>
#include <enum.h>

int hhh = 0;//零表示有干净盘子 1表示没有干净盘子

// 更新任务状态
void refresh_tasks()
{
    // 先处理烹饪任务
    if(chef.task_index == -1) {
        chef.task_index = 0;
        if(Plate_loc["Plate"].empty()) {
            chef.current_tasks = task_list[{"wait"}];
            hhh = 1;
        } else {
            hhh = 0;
            chef.current_tasks = task_list[Order[0].recipe];
        }
    }
    
    if(cleaner.task_index == -1) {
        cleaner.task_index = 0;
        if(Plate_loc["DirtyPlates"].empty()) {
            cleaner.current_tasks = task_list[{"wait_dirty_plates"}];
        } else {
            cleaner.current_tasks = task_list[{"wash"}];
        }
    }
}

// 获取烹饪目标位置
// ... [其他代码保持不变] ...

struct Point locate_cook_target()
{
    const std::string& target_obj = chef.current_tasks[chef.task_index].object; 
    Point target_pos = {-1, -1}; // 默认无效位置
    
    if(target_obj == "Pan" || target_obj == "Pot") {
        // 添加空向量检查
        if(!Item_loc[target_obj].empty()) {
            target_pos = Item_loc[target_obj][0];
        }
    }
    else if(target_obj == "Chop") {
        if(!Item_loc["Chop"].empty()) {
            target_pos = Item_loc["Chop"][0];
        }
    }
    else if(target_obj == "ServiceWindow") {
        if(!Item_loc["ServiceWindow"].empty()) {
            target_pos = Item_loc["ServiceWindow"][0];
        }
    }
    else if(target_obj == "temp_plate_pos") {
        target_pos = plate_temp_pos;  
        last_plate_location = plate_temp_pos;  
    }
    // 修改后的 Plate 定位逻辑
    else if(target_obj == "Plate") {
    // 优先使用上次记录的盘子位置
        if (last_plate_location.x != -1 && last_plate_location.y != -1) {
            target_pos = last_plate_location;
        } 
    // 其次检查实体列表中的干净盘子
        else {
            for(int i = 0; i < 25; i++) {
                if(Entity[i].containerKind == ContainerKind::Plate && 
                    Entity[i].entity.size() > 0) { // 干净的盘子
                    target_pos = {Entity[i].x, Entity[i].y};
                    last_plate_location = target_pos;
                    break;
                }
            }
        // 最后尝试使用 Plate_loc 中的位置
            //if(target_pos.x == -1 && !Plate_loc["Plate"].empty()) {
                //target_pos = Plate_loc["Plate"][0];
            //}
        }
    }
    else if(target_obj == "fish" || target_obj == "rice" || target_obj == "kelp") {
        if(!Item_loc[target_obj].empty()) {
            target_pos = Item_loc[target_obj][0];
        }
    }
    
    return target_pos;
}

// ... [其他代码保持不变] ...

// 获取烹饪任务
struct Task obtain_chef_task()  
{
    Point destination = locate_cook_target();
    chef.player_id = 0;  
    Players[0].dst = destination;
    return chef.current_tasks[chef.task_index];  // 直接返回
}

// 获取洗碗目标位置
struct Point locate_clean_target()  
{
    std::string target_obj = cleaner.current_tasks[cleaner.task_index].object;  
    return (target_obj == "PlateReturn") ? 
           Item_loc["PlateReturn"][0] : 
           Item_loc["Sink"][0];  
}

// 获取洗碗任务
struct Task obtain_cleaner_task()  
{
    Point destination = locate_clean_target();
    cleaner.player_id = 1;  
    Players[1].dst = destination;
    return cleaner.current_tasks[cleaner.task_index];  // 直接返回
}

// 处理任务
std::string execute_task(Task job, int worker_id)  
{
    std::pair<int, std::string> result;
    std::string command;  
    
    // 使用switch处理不同任务类型
    switch (job.action) {
        case Pick:
            result = Pick_object(Players[worker_id].dst, job.object);
            break;
        case Put:
            result = Put_object(Players[worker_id].dst, worker_id);
            break;
        case Interact:
            result = Interact_with_object(Players[worker_id].dst);
            break;
        case Move: {
            Point start = {Players[worker_id].x, Players[worker_id].y};
            result = navigatePath(start, Players[worker_id].dst, worker_id);
            break;
        }
        default:
            //assert(false);
            break;
    }
    
    command = result.second;
    
    // 处理任务完成状态
    if(result.first == 1) {
        Players[worker_id].status = free_;
        if(worker_id == 0) {
            chef.task_index++;
            if(chef.task_index == chef.current_tasks.size()) {
                chef.task_index = -1;
                chef.current_tasks.clear();//修改1
                last_plate_location = {-1, -1};
            }
        } 
        else {
            cleaner.task_index++;
            if(cleaner.task_index == cleaner.current_tasks.size()) {
                cleaner.task_index = -1;
                cleaner.current_tasks.clear();//修改1
                hhh = 0;
            }
        }
    }
    
    return command;
}

std::string player_action(int player_id){
    struct Task job;
    if (player_id == 0 && hhh == 0){
        job = obtain_chef_task();
    }
    else if (player_id == 0 && hhh == 1){
        return "Move ";
    }
    else if (player_id == 1 && hhh == 1){
        job = obtain_cleaner_task();
    }
    else if (player_id == 1 && hhh == 0){
        return "Move ";
    }
    else{
        return "Move ";
    }
    return execute_task(job, player_id);
}