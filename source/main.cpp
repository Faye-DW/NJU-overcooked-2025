#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <string>
#include <framework.h>
#include <enum.h>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cerr.tie(nullptr);
    std::cerr << std::nounitbuf;
    std::string s;
    std::stringstream ss;
    int frame;

    init_read();
    init();

    int totalFrame = 14400;
    for (int i = 0; i < totalFrame; i++)
    {
        bool skip = frame_read(i);
        if (skip) continue;
        update_plate();
        refresh_tasks();
        std::cout << "Frame " << i << "\n";
        //std::pair<std::string, std::string> ans = player_action();
        std::string action = player_action(0) + "\n" + player_action(1) + "\n";
        std::cout << action;
        std::cout.flush();
    }
}
