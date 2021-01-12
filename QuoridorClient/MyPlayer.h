#pragma once
#include <chrono>
#include <vector>
#include "Player.h"

namespace QuoridorUtils {
class MyPlayer final : public Player{
private:
    std::vector<BlockBar> blocks;                                           // ʵ������Ҫ, ��ɾ 
    int targetY = 0;                                                        // ʵ������Ҫ, ��ɾ 
    Location randomWalk(const Location& myLoc, const Location& enemyLoc);   // ʵ������Ҫ, ��ɾ 
public:
    MyPlayer(const std::string& key) : Player(key) { };           // �������, �����޸� 
    Step nextStep(const ChessboardChange& newChange) override;    // ��������ʵ�� 
    void restart() override;                                      // ��������ʵ�� 
};
}

// �������
//namespace QuoridorUtils {
//class MyPlayer final : public Player {
//public:
//    MyPlayer(const std::string& key) : Player(key) { };          // ������� 
//    Step nextStep(const ChessboardChange& newChange) override;   // ��������ʵ�� 
//    void restart() override;                                     // ��������ʵ�� 
//};
//}

/*
 *
 * ������������:                 ��������ṹ���£�
 * ��-��-��-��-��-��-��-��-��-��         ��-��-��-��-��-��-��-��-��-��9
 * ��-��-��-��-��-��-��-��-��-�ȨI9       ��-��-��-��-��-��-��-��-��-��8
 * ��-��-��-��-��-��-��-��-��-�ȨI8       ��-��-��-��-��-��-��-��-��-��7
 * ��-��-��-��-��-��-��-��-��-�ȨI7       ��-��-��-��-��-��-��-��-��-��6
 * ��-��-��-��-��-��-��-��-��-�ȨI6       ��-��-��-��-��-��-��-��-��-��5
 * ��-��-��-��-��-��-��-��-��-�ȨI5       ��-��-��-��-��-��-��-��-��-��4
 * ��-��-��-��-��-��-��-��-��-�ȨI4       ��-��-��-��-��-��-��-��-��-��3
 * ��-��-��-��-��-��-��-��-��-�ȨI3       ��-��-��-��-��-��-��-��-��-��2
 * ��-��-��-��-��-��-��-��-��-�ȨI2       ��-��-��-��-��-��-��-��-��-��1
 * ��-��-��-��-��-��-��-��-��-���I1 y     ��-��-��-��-��-��-��-��-��-��0 y
 *x 1 2 3 4 5 6 7 8 9         x0 1 2 3 4 5 6 7 8 9
 *
 *
 */