#include "MyPlayer.h"

#include <iostream>
#include <cstdlib>

// �������ʵ�ֿ�ʼ
//using namespace QuoridorUtils;
//int targetY = 0;
//std::vector<QuoridorUtils::BlockBar> blocks;
//QuoridorUtils::Location randomWalk(const QuoridorUtils::Location& myLoc, const QuoridorUtils::Location& enemyLoc) {
//    bool directions[4];
//    directions[0] = myLoc.y < QuoridorUtils::SIZE;           // �Ƿ���������� 
//    directions[1] = myLoc.y > 1;                             // �Ƿ���������� 
//    directions[2] = myLoc.x > 1;                             // �Ƿ���������� 
//    directions[3] = myLoc.x < QuoridorUtils::SIZE;           // �Ƿ���������� 
//    for (auto block : blocks) {                        // ���������б�, �ҵ����赲�ķ��� 
//        if (block.isH()) {                                   // ˮƽ���򵲰�, start �� stop �� y ��� 
//            if (block.start.x == myLoc.x - 1 ||
//                block.start.x == myLoc.x - 2) {              // ���ܵ�·�ĵ��� 
//                if (block.start.y == myLoc.y) {
//                    directions[0] = false;
//                } else if (block.start.y == myLoc.y - 1) {
//                    directions[1] = false;
//                }
//            }
//        }
//        if (block.isV()) {                                   // ��ֱ���򵲰�, start �� stop �� x ��� 
//            if (block.start.y == myLoc.y - 1 ||
//                block.start.y == myLoc.y - 2) {              // ���ܵ�·�ĵ��� 
//                if (block.start.x == myLoc.x) {
//                    directions[3] = false;
//                } else if (block.start.x == myLoc.x - 1) {
//                    directions[2] = false;
//                }
//            }
//        }
//    }
//    int optionsCount = directions[0] + directions[1] + 
//                       directions[2] + directions[3];        // �����ܶ�������, �ض����ڵ���1 
//    int tmp = rand() % optionsCount;                         // ���һ�� [0, optionsCount - 1] ������
//    int directionIndex;                                      // �ҳ����������±� 
//    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
//        if (directions[directionIndex]) {
//            if (tmp <= 0) {
//                break;
//            }
//            tmp--;
//        }
//    }
//    QuoridorUtils::Location nextLoc = myLoc;
//    switch (directionIndex) {
//    case 0:   // ������
//        nextLoc.y++;
//        break;
//    case 1:   // ������
//        nextLoc.y--;
//        break;
//    case 2:   // ������
//        nextLoc.x--;
//        break;
//    case 3:   // ������
//        nextLoc.x++;
//        break;
//    default: ;
//    }
//    if (nextLoc.distance(enemyLoc) == 0) {                   // ��һ���ǵо�λ����Ӧ�����о����� 
//        nextLoc.x = -1;                                      // �˴�����ʵ��, Ĭ����һ���ղ��� 
//        nextLoc.y = -1;
//    }
//    return nextLoc;
//}
// �������ʵ�ֽ���

namespace QuoridorUtils {

/*
 * Step MyPlayer::nextStep(const ChessboardChange& newChange)
 *
 * newChange Ϊ�޸ĵ����, ���ʽΪ
 * (GameStatus status, Location enemyLoc, Location myLoc, BlockBar newEnemyBlockBar)
 * ������������͵����������ͼ, ������û������µĵ���ʱ,
 * newChange.newEnemyBlockBar.isNan()Ϊtrue
 * 
 * ��Ҫ���ص�����StepΪ
 * (Location myNewLoc, BlockBar myNewBlockBar)
 * ����ѡ�����ƶ�����һ����ǰ�������ĸ����򣩻��߷�һ������:
 * ���ƶ�����ʱ, step.myNewBlockBar.isNan()Ϊtrue; 
 * �����õ���ʱ, step.myNewLocΪnewChange�е�myLoc
 *
 * ���д���(����λ�ô���, ����������, �ƶ�λ�ô���, ͬʱ�ƶ������õ���, ��ʱ��)���ɷ��������,
 * ������Ҫ������ʵ�ִ�����⹦��. 
 *
 *  ������������:                 ��������ṹ���£�
 *  ��-��-��-��-��-��-��-��-��-��         ��-��-��-��-��-��-��-��-��-��9
 *  ��-��-��-��-��-��-��-��-��-�ȨI9       ��-��-��-��-��-��-��-��-��-��8
 *  ��-��-��-��-��-��-��-��-��-�ȨI8       ��-��-��-��-��-��-��-��-��-��7
 *  ��-��-��-��-��-��-��-��-��-�ȨI7       ��-��-��-��-��-��-��-��-��-��6
 *  ��-��-��-��-��-��-��-��-��-�ȨI6       ��-��-��-��-��-��-��-��-��-��5
 *  ��-��-��-��-��-��-��-��-��-�ȨI5       ��-��-��-��-��-��-��-��-��-��4
 *  ��-��-��-��-��-��-��-��-��-�ȨI4       ��-��-��-��-��-��-��-��-��-��3
 *  ��-��-��-��-��-��-��-��-��-�ȨI3       ��-��-��-��-��-��-��-��-��-��2
 *  ��-��-��-��-��-��-��-��-��-�ȨI2       ��-��-��-��-��-��-��-��-��-��1
 *  ��-��-��-��-��-��-��-��-��-���I1 y     ��-��-��-��-��-��-��-��-��-��0 y
 * x 1 2 3 4 5 6 7 8 9        x 0 1 2 3 4 5 6 7 8 9
 *
 */

// �������ʵ�ֿ�ʼ
Location MyPlayer::randomWalk(const Location& myLoc, const Location& enemyLoc) {
    bool directions[4];
    directions[0] = myLoc.y < SIZE;                          // �Ƿ���������� 
    directions[1] = myLoc.y > 1;                             // �Ƿ���������� 
    directions[2] = myLoc.x > 1;                             // �Ƿ���������� 
    directions[3] = myLoc.x < SIZE;                          // �Ƿ���������� 
    for (auto block : this->blocks) {                        // ���������б�, �ҵ����赲�ķ��� 
        if (block.isH()) {                                   // ˮƽ���򵲰�, start �� stop �� y ��� 
            if (block.start.x == myLoc.x - 1 ||
                block.start.x == myLoc.x - 2) {              // ���ܵ�·�ĵ��� 
                if (block.start.y == myLoc.y) {
                    directions[0] = false;
                } else if (block.start.y == myLoc.y - 1) {
                    directions[1] = false;
                }
            }
        }
        if (block.isV()) {                                   // ��ֱ���򵲰�, start �� stop �� x ��� 
            if (block.start.y == myLoc.y - 1 ||
                block.start.y == myLoc.y - 2) {              // ���ܵ�·�ĵ��� 
                if (block.start.x == myLoc.x - 1) {
                    directions[2] = false;
                } else if (block.start.x == myLoc.x) {
                    directions[3] = false;
                }
            }
        }
    }
    int optionsCount = int(directions[0]) + int(directions[1]) + 
                       int(directions[2]) + int(directions[3]);    // �����в��ܶ�������, �ض����ڵ���1
    int tmp = rand() % optionsCount;                               // ���һ�� [0, optionsCount - 1] ������
    int directionIndex;                                            // �ҳ����������±� 
    for (directionIndex = 0; directionIndex < 4; directionIndex++) {
        if (directions[directionIndex]) {
            if (tmp <= 0) {
                break;
            }
            tmp--;
        }
    }
    Location nextLoc = myLoc;
    switch (directionIndex) {
    case 0:   // ������
        nextLoc.y++;
        break;
    case 1:   // ������
        nextLoc.y--;
        break;
    case 2:   // ������
        nextLoc.x--;
        break;
    case 3:   // ������
        nextLoc.x++;
        break;
    default: ;
    }
    if (nextLoc.distance(enemyLoc) == 0) {                   // ��һ���ǵо�λ����Ӧ�����о����� 
        nextLoc.x = -1;                                      // �˴�����ʵ��, Ĭ����һ���ղ��� 
        nextLoc.y = -1;
    }
    return nextLoc;
}

Step MyPlayer::nextStep(const ChessboardChange& newChange) {
    // ��֪: ��ʾ���������Ϊ API ʹ���Լ���Ϸ�����˽ⷶ��, ������֤������ȷ��, �����ճ��հ� 
    if (this->targetY == 0) {                                // �жϲ���¼�Լ���Ŀ�� 
        const auto d = std::chrono::system_clock::now().time_since_epoch();
        const auto nanosecond = int((std::chrono::nanoseconds(d).count() / 100) % int64_t(RAND_MAX));
        srand(nanosecond);
        if (newChange.myLoc == PLAYER0_LOC) {                // ������ʼ����Ϊ (5,1) ��Ŀ��Ϊ (x,9) 
            this->targetY = PLAYER1_LOC.y;
        } else {                                             // ������ʼ����Ϊ (5,9) ��Ŀ��Ϊ (x,1) 
            this->targetY = PLAYER0_LOC.y;
        }
    }
    std::cout << newChange;                                  // ������յ������ݵ�����̨��ʾ 
    if (!newChange.newEnemyBlockBar.isNan()) {               // �Է������˵��� 
        BlockBar tmp = newChange.newEnemyBlockBar;
        tmp.normalization();                                 // �淶Ϊ start ����С�� stop �������ʽ 
        this->blocks.push_back(tmp);                         // �洢�淶���� 
    }
    Step step;                                               // ��ʼ�� step Ĭ�ϲ��ƶ����Ű� 
    step.myNewLoc = randomWalk(newChange.myLoc, newChange.enemyLoc);
    std::cout << " -> " << step << std::endl;                // ����ҵľ��ߵ�����̨��ʾ 
    // ������� 
    // std::cout << " ->  ";
    // std::cin >> step.myNewLoc.x >> step.myNewLoc.y
    //          >> step.myNewBlockBar.start.x >> step.myNewBlockBar.start.y
    //          >> step.myNewBlockBar.stop.x >> step.myNewBlockBar.stop.y;
    return step;
}

void MyPlayer::restart() {
    this->blocks.clear();
    this->targetY = 0;
}
// �������ʵ�ֽ��� 

// �������ʵ�ֿ�ʼ 
//Step MyPlayer::nextStep(const ChessboardChange& newChange) {
//    // ��֪: ��ʾ���������Ϊ API ʹ���Լ���Ϸ�����˽ⷶ��, ������֤������ȷ��, �����ճ��հ� 
//    if (targetY == 0) {                                // �жϲ���¼�Լ���Ŀ��
//        //const auto d = std::chrono::system_clock::now().time_since_epoch();
//        //const int nanosecond = (std::chrono::nanoseconds(d).count() / 100) % int64_t(INT32_MAX);
//        //srand(nanosecond);
//        if (newChange.myLoc == PLAYER0_LOC) {                // ������ʼ����Ϊ (5,1) ��Ŀ��Ϊ (x,9)
//            targetY = PLAYER1_LOC.y;
//        } else {                                             // ������ʼ����Ϊ (5,9) ��Ŀ��Ϊ (x,1)
//            targetY = PLAYER0_LOC.y;
//        }
//    }
//    std::cout << newChange << std::endl;                     // ������յ������ݵ�����̨��ʾ  
//    if (!newChange.newEnemyBlockBar.isNan()) {               // �Է������˵��� 
//        BlockBar tmp = newChange.newEnemyBlockBar;
//        tmp.normalization();                                 // �淶Ϊ start ����С�� stop �������ʽ 
//        blocks.push_back(tmp);                         // �洢���� 
//    }
//    Step step;                                               // ��ʼ�� step Ĭ�ϲ��ƶ����Ű� 
//    step.myNewLoc = randomWalk(newChange.myLoc, newChange.enemyLoc);
//    std::cout << step << std::endl;                          // ����ҵľ��ߵ�����̨��ʾ 
//    return step;
//}
//
//void MyPlayer::restart() {
//    blocks.clear();
//    targetY = 0;
//}
// �������ʵ�ֽ���
}
