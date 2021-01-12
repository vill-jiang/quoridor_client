#include "XlinePlayer.h"

#include <queue>

namespace QuoridorUtils {

// 棋盘数据结构维护函数, 负责初始化棋盘. 
void XlinePlayer::chessboardInit() {
    for (int i = 1; i <= QuoridorUtils::SIZE; i++) {
        for (int j = 1; j <= QuoridorUtils::SIZE; j++) {
            this->chessboard[Location(i, j)] = Doors();
            if (i == 1) {
                this->chessboard[Location(i, j)].left = false;
            } else if (i == 9) {
                this->chessboard[Location(i, j)].right = false;
            }
            if (j == 1) {
                this->chessboard[Location(i, j)].down = false;
            } else if (j == 9) {
                this->chessboard[Location(i, j)].up = false;
            }
        }
    }
}

// 棋盘数据结构维护函数, 负责添加和删除挡板. 
void XlinePlayer::addOrDelBlock(const BlockBar& block, const bool isDel) {
    if (block.isNan()) {
        return;
        return;
    }
    if (block.isH() && block.start.y > 0 && block.start.y < SIZE) {
        // eg: ((0,1), (2,1)) will block (1,1) u, (2,1) u (1,2) d, (2,2) d
        auto stopLoc = block.start.x > block.stop.x ? block.start : block.stop;
        this->chessboard[stopLoc].up = isDel;
        stopLoc.x--;
        this->chessboard[stopLoc].up = isDel;
        stopLoc = block.start.x > block.stop.x ? block.start : block.stop;
        stopLoc.y++;
        this->chessboard[stopLoc].down = isDel;
        stopLoc.x--;
        this->chessboard[stopLoc].down = isDel;
    } else if (block.isV() && block.start.x > 0 && block.start.x < SIZE) {
        // eg: ((2,1), (2,3)) will block (2,2) r, (2,3) r (3,2) l, (3,3) l
        auto stopLoc = block.start.y > block.stop.y ? block.start : block.stop;
        this->chessboard[stopLoc].right = isDel;
        stopLoc.y--;
        this->chessboard[stopLoc].right = isDel;
        stopLoc = block.start.x > block.stop.x ? block.start : block.stop;
        stopLoc.x++;
        this->chessboard[stopLoc].left = isDel;
        stopLoc.y--;
        this->chessboard[stopLoc].left = isDel;
    }
}

int XlinePlayer::minDistance(const Location& startLoc, const Location& anotherLoc, const Location& stopLoc) {
    // BFS 寻找最短路径, 只会寻找不经过其它等于 myTarget.y 的最短路径长度 
    std::vector<std::vector<int>> minDistanceMatrix(SIZE, std::vector<int>(SIZE, -1));
    minDistanceMatrix[startLoc.x - 1][startLoc.y - 1] = 0;
    for (int i = 0; i < SIZE; i++) {
        if (i != stopLoc.x - 1) {
            minDistanceMatrix[i][stopLoc.y - 1] = -2;
        }
    }
    auto returnMinDistance = INT32_MAX;
    std::queue<std::pair<Location, int>> nextLocations;
    nextLocations.emplace(startLoc, 0);  // 刚开始入队列第0步 
    while (!nextLocations.empty()) {
        const auto tmp = nextLocations.front();
        nextLocations.pop();
        if (tmp.first == stopLoc && (returnMinDistance == INT32_MAX || returnMinDistance == tmp.second)) {
            returnMinDistance = tmp.second;
            break;
        } else if (returnMinDistance == INT32_MAX) {
            if (this->chessboard[tmp.first].up && minDistanceMatrix[tmp.first.x - 1][tmp.first.y] == -1) {     // 可以向上走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y + 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].down && minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] == -1) {   // 可以向下走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y - 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].right && minDistanceMatrix[tmp.first.x][tmp.first.y - 1] == -1) {  // 可以向右走 
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x + 1, tmp.first.y), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].left && minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] == -1) {   // 可以向左走 
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x - 1, tmp.first.y), tmp.second + 1);
            }
        }
    }
    return returnMinDistance;
}

int XlinePlayer::minDistancePath(const Location& startLoc, const Location& anotherLoc, const Location& stopLoc,
                                 std::queue<std::pair<Location, int>>& path) {
    while (!path.empty()) {
        path.pop();
    }
    if (startLoc == stopLoc) {
        return 0;
    }
    // BFS 寻找最短路径 
    std::vector<std::vector<int>> minDistanceMatrix(SIZE, std::vector<int>(SIZE, -1));
    minDistanceMatrix[startLoc.x - 1][startLoc.y - 1] = 0;
    //minDistanceMatrix[enemyLoc.x][enemyLoc.y] = -2;
    auto returnMinDistance = INT32_MAX;
    std::queue<std::pair<Location, int>> nextLocations;
    nextLocations.emplace(startLoc, 0);  // 刚开始入队列第0步 
    std::queue<std::pair<Location, int>> tmpPath;
    while (!nextLocations.empty()) {
        const auto tmp = nextLocations.front();
        nextLocations.pop();
        if (tmp.first == stopLoc && (returnMinDistance == INT32_MAX || returnMinDistance == tmp.second)) {
            returnMinDistance = tmp.second;
            tmpPath.emplace(tmp);
            path.emplace(tmp);
            continue;
        } else if (returnMinDistance == INT32_MAX) {
            if (this->chessboard[tmp.first].up && minDistanceMatrix[tmp.first.x - 1][tmp.first.y] == -1) {     // 可以向上走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y + 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].down && minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] == -1) {   // 可以向下走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y - 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].right && minDistanceMatrix[tmp.first.x][tmp.first.y - 1] == -1) {  // 可以向右走 
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x + 1, tmp.first.y), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].left && minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] == -1) {   // 可以向左走 
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x - 1, tmp.first.y), tmp.second + 1);
            }
        }
    }
    if (returnMinDistance != INT32_MAX) {  // 可达就往回找 
        while (!tmpPath.empty()) {
            const auto tmp = tmpPath.front();
            tmpPath.pop();
            if (tmp.second == 1) {
                continue;
            }
            if (this->chessboard[tmp.first].up &&
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] == tmp.second - 1) {  // 可以向上回溯 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] = -1;                 // 防止重新入队 
                path.emplace(Location(tmp.first.x, tmp.first.y + 1), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].down &&
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] == tmp.second - 1) {  // 可以向下回溯 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] = -1;
                path.emplace(Location(tmp.first.x, tmp.first.y - 1), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].right &&
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] == tmp.second - 1) {  // 可以向右回溯 
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] = -1;
                path.emplace(Location(tmp.first.x + 1, tmp.first.y), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].left &&
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] == tmp.second - 1) {  // 可以向左回溯 
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] = -1;
                path.emplace(Location(tmp.first.x - 1, tmp.first.y), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
        }
    }
    return returnMinDistance;
}

int XlinePlayer::minDistancePath(const Location& startLoc, const Location& anotherLoc, const int targetY,
                                 std::queue<std::pair<Location, int>>& path) {
    while (!path.empty()) {
        path.pop();
    }
    if (startLoc.y == targetY) {
        return 0;
    }
    // BFS 寻找最短路径 
    std::vector<std::vector<int>> minDistanceMatrix(SIZE, std::vector<int>(SIZE, -1));
    minDistanceMatrix[startLoc.x - 1][startLoc.y - 1] = 0;
    //minDistanceMatrix[enemyLoc.x][enemyLoc.y] = -2;
    auto returnMinDistance = INT32_MAX;
    std::queue<std::pair<Location, int>> nextLocations;
    nextLocations.emplace(startLoc, 0);  // 刚开始入队列第0步 
    std::queue<std::pair<Location, int>> tmpPath;
    while (!nextLocations.empty()) {
        const auto tmp = nextLocations.front();
        nextLocations.pop();
        if (tmp.first.y == targetY && (returnMinDistance == INT32_MAX || returnMinDistance == tmp.second)) {
            returnMinDistance = tmp.second;
            tmpPath.emplace(tmp);
            path.emplace(tmp);
            continue;
        } else if (returnMinDistance == INT32_MAX) {
            if (this->chessboard[tmp.first].up && minDistanceMatrix[tmp.first.x - 1][tmp.first.y] == -1) {     // 可以向上走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y + 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].down && minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] == -1) {   // 可以向下走 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x, tmp.first.y - 1), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].right && minDistanceMatrix[tmp.first.x][tmp.first.y - 1] == -1) {  // 可以向右走 
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x + 1, tmp.first.y), tmp.second + 1);
            }
            if (this->chessboard[tmp.first].left && minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] == -1) {   // 可以向左走 
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] = tmp.second + 1;
                nextLocations.emplace(Location(tmp.first.x - 1, tmp.first.y), tmp.second + 1);
            }
        }
    }
    if (returnMinDistance != INT32_MAX) {  // 可达就往回找 
        while (!tmpPath.empty()) {
            const auto tmp = tmpPath.front();
            tmpPath.pop();
            if (tmp.second == 1) {
                continue;
            }
            if (this->chessboard[tmp.first].up &&
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] == tmp.second - 1) {  // 可以向上回溯 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y] = -1;                 // 防止重新入队 
                path.emplace(Location(tmp.first.x, tmp.first.y + 1), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].down &&
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] == tmp.second - 1) {  // 可以向下回溯 
                minDistanceMatrix[tmp.first.x - 1][tmp.first.y - 2] = -1;
                path.emplace(Location(tmp.first.x, tmp.first.y - 1), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].right &&
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] == tmp.second - 1) {  // 可以向右回溯 
                minDistanceMatrix[tmp.first.x][tmp.first.y - 1] = -1;
                path.emplace(Location(tmp.first.x + 1, tmp.first.y), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
            if (this->chessboard[tmp.first].left &&
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] == tmp.second - 1) {  // 可以向左回溯 
                minDistanceMatrix[tmp.first.x - 2][tmp.first.y - 1] = -1;
                path.emplace(Location(tmp.first.x - 1, tmp.first.y), tmp.second - 1);
                tmpPath.emplace(path.back());
            }
        }
    }
    return returnMinDistance;
}

}
