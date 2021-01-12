#pragma once
#include <queue>
#include <unordered_map>

#include "Player.h"

namespace QuoridorUtils {
class XlinePlayer : public Player {
protected:
    Location myInitLoc;
    int remainBlocks = BLOCK_SUM;
    int remainChance = 3;
    std::unordered_map<Location, Doors, LocationHash> chessboard;
    void chessboardInit();
    // ת�����嵽 chessboard, isDel ������ӻ�ɾ��, Ĭ��Ϊ��ӵ���  
    void addOrDelBlock(const BlockBar& block, const bool isDel = false);
    // �������������Ŀ��yֵ, ���� myLoc ����ָ�� y ֵ����С������ɴ�·��, ���ɴ��򷵻� INT32_MAX 
    // ע��: ��ʱδʹ�� enemyLoc 
    // ���� path ���ȱ����, �ٰ����� myLoc ������������λ������� pair, ��С����Ϊ1 
    int minDistance(const Location& startLoc, const Location& anotherLoc, const Location& stopLoc);
    int minDistancePath(const Location& startLoc, const Location& anotherLoc,
                        const Location& stopLoc, std::queue<std::pair<Location, int>>& path);
    int minDistancePath(const Location& startLoc, const Location& anotherLoc,
                        const int targetY, std::queue<std::pair<Location, int>>& path);
public:
    XlinePlayer(const std::string& key) : Player(key) { };
};
}
