#include "game.h"

#include <cmath>
#include <queue>
#include <vector>
#include <QtDebug>

#include "mainwindow.h"

using namespace std;

// 棋型
enum class ChessType: int { OTHER = 0,  // 0, 其他棋型不考虑
                            BLACK_FIVE,  // 10000000
                            WHITE_FIVE,  // -100000, 白赢
                            BLACK_LIVE_FOUR,  // 80000
                            WHITE_LIVE_FOUR,  // -50000，白活4
                            BLACK_RUSH_FOUR_A,
                            WHITE_RUSH_FOUR_A,
                            BLACK_RUSH_FOUR,  // 80000
                            WHITE_RUSH_FOUR,  // -400
                            BLACK_LIVE_THREE,  // 8000
                            WHITE_LIVE_THREE,  // -400
                            BLACK_SLEEP_THREE,  // 40
                            WHITE_SLEEP_THREE,  // -20
                            BLACK_LIVE_TWO,  // 40
                            WHITE_LIVE_TWO,  // -20
                            BLACK_SLEEP_TWO,  // 3
                            WHITE_SLEEP_TWO,  // -1
                            BLACK_LIVE_ONE,  // 3
                            WHITE_LIVE_ONE,  // -1
                            BLACK_LIVE_LONG,  // 黑长连  10000000
                            WHITE_LIVE_LONG   // 白长连 -100000
                          };  // 限定作用域的枚举类型

extern const int kGridNum;  // 声明mainwindow头文件中的变量

bool Game::isWin(int row, int col)
{
  for (int i = 0; i < 5; i++)
    {
      if (col - i >= 0 &&
          col - i + 4  < kGridNum &&
          chess_board_[row][col - i] == chess_board_[row][col - i + 1] &&
          chess_board_[row][col - i] == chess_board_[row][col - i + 2] &&
          chess_board_[row][col - i] == chess_board_[row][col - i + 3] &&
          chess_board_[row][col - i] == chess_board_[row][col - i + 4])
        return true;
    }
  for (int i = 0; i < 5; i++)
    {
      if (row - i >= 0 &&
          row - i + 4 < kGridNum &&
          chess_board_[row - i][col] == chess_board_[row - i + 1][col] &&
          chess_board_[row - i][col] == chess_board_[row - i + 2][col] &&
          chess_board_[row - i][col] == chess_board_[row - i + 3][col] &&
          chess_board_[row - i][col] == chess_board_[row - i + 4][col])
        return true;
    }
  // 正斜
  for (int i = 0; i < 5; i++)
    {
      if (row - i >= 0 &&  // 参数要尽量对齐
          col - i >= 0 &&
          row - i + 4 < kGridNum &&
          col - i + 4 < kGridNum &&
          chess_board_[row - i][col - i] == chess_board_[row - i + 1][col - i + 1] &&
          chess_board_[row - i][col - i] == chess_board_[row - i + 2][col - i + 2] &&
          chess_board_[row - i][col - i] == chess_board_[row - i + 3][col - i + 3] &&
          chess_board_[row - i][col - i] == chess_board_[row - i + 4][col - i + 4])
        return true;
    }
  // 反斜
  for (int i = 0; i < 5; i++)
    {
      if (row + i - 4 >= 0 &&
          col - i >= 0 &&
          row + i < kGridNum &&
          col - i + 4 < kGridNum &&
          chess_board_[row + i][col - i] == chess_board_[row + i - 1][col - i + 1] &&
          chess_board_[row + i][col - i] == chess_board_[row + i - 2][col - i + 2] &&
          chess_board_[row + i][col - i] == chess_board_[row + i - 3][col - i + 3] &&
          chess_board_[row + i][col - i] == chess_board_[row + i - 4][col - i + 4])
        return true;
    }
  return false;
}

bool Game::isDeadGame()
{
  for (int i = 0; i < kGridNum; ++i)
    for (int j = 0; j < kGridNum; ++j)
      {
        if (!(chess_board_[i][j] == 1 || chess_board_[i][j] == -1))
          {
            return false;
          }
      }
  return true;
}

void Game::updatePoint(int x, int y)
{
  chess_board_[x][y] = -2;  // -2为打点子
  // 同时不改变number数组的状态
}

void Game::updateMap (int x, int y)
{
  if (player_flag_) {
      chess_board_[x][y] = 1;
      prompt_text_ = "白方正在行棋...";
    } else {
      chess_board_[x][y] = -1;
      prompt_text_ = "黑方正在行棋...";
    }
  number[x][y] = ++num;
  player_flag_ = !player_flag_;  // 交换棋权
}

void Game::seekKillBlack(vector<pair<int, int>>& pointsList, int flag)  // 找到黑棋的连五，活四，冲四，活三的杀棋位置 TODO
{
  for (int i = flag - 1; i >= 0; --i) {
      int row = sort_heap[i][1];
      int col = sort_heap[i][2];
      chess_board_[row][col] = 1;
      calculateScore();  // 估值
      if (stat_[1] > 0) { // 产生黑连五
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[3] > 0) { // 产生黑活四
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[5] > 0) { // 产生黑冲四_A
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[7] > 0) {  // 产生黑冲四
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[9] > 0) {  // 产生黑活三
          pointsList.push_back(make_pair(row, col));
        }
      chess_board_[row][col] = 0;
    }
}

void Game::seekKillWhite(vector<pair<int, int> >& pointsList, int flag)
{
  for (int i = flag - 1; i >= 0; --i) {
      int row = sort_heap[i][1];
      int col = sort_heap[i][2];
      chess_board_[row][col] = -1;  // 模拟落子
      calculateScore();  // 估值
      if (stat_[2] > 0) { // 产生白连五
          pointsList.push_back(make_pair(row, col));
      } else if (stat_[4] > 0) { // 产生白活四
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[6] > 0) {  // 产生白冲四_A
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[8] > 0) {  // 产生白冲四
          pointsList.push_back(make_pair(row, col));
        } else if (stat_[10] > 0) {  // 产生白活三
          pointsList.push_back(make_pair(row, col));
        }
      chess_board_[row][col] = 0;  // 回溯
    }
}

// TODO总结一下，算杀首先由当前层贪心找到活三或者冲四才能进行深度搜索，深度搜索的目的是为了判断黑棋是否能胜利
bool Game::analyse_kill(int dep, vector<pair<int, int>>& maxPoints)  // 寻找杀棋TODO  目前为贪心0层
{
  if (dep == 0 || isDeadGame() || (dep != 8 && result_ != Result::R_DRAW))  // 递归终止的条件TODO，当前层出现必胜棋子
    {
      if (dep == 0) {  // 走一步对黑棋最好的位置，若黑棋还没赢则返回false
//          priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
//          int flag = 0;  // 标记记录最佳估值的个数
//          minHeap(heap, flag, 1);
//          int row = sort_heap[flag-1][1];
//          int col = sort_heap[flag-1][2];
//          chess_board_[row][col] = 1;
//          calculateScore();
//          chess_board_[row][col] = 0;
//          if (result_ == Result::R_BLACK)
//            return true;
//          else
            return false;
        } else if (result_ == Result::R_BLACK) {  // 找到黑棋杀棋
          return true;
        } else {
          return false;  // 黑棋输
        }
    } else if (dep % 2 ==  0) {  // max层
      bool bestvalue;
      vector<pair<int, int>> pointsList;
      priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      minHeap(heap, flag, 10);
      // 电脑有活三和冲四的时候玩家是必须防守的
      seekKillBlack(pointsList, flag);  // 搜索黑方的活三和冲四节点，只要有一个子节点能赢即可
      if (pointsList.empty()) {  // 判断是否有活三和冲四
          return false;
        }  // 对能产生杀棋点的位置进行搜索，对其进行深度估值
      for (auto i : pointsList)
        {
          int row = i.first;
          int col = i.second;
          chess_board_[row][col] = 1;                           // 虚拟走子
          bestvalue = analyse_kill(dep - 1, maxPoints);  // 判断黑子是否能胜利
          chess_board_[row][col] = 0;                          // 回溯
          // 只要有一个节点能赢即可
          if (bestvalue)  // 如果能必胜
            {
              if (dep == kill_depth_) {  // 仅限第一层的情况
                      maxPoints.push_back(make_pair(row, col));
                    }
                }
              return true;
            }
      // 未找到杀棋
      return false;
    }
  else {  // min层
      // 玩家防守的时候却不一定根据电脑的棋来走，而是选择走自己最好的棋，比如有可能是选择自己冲四
      bool bestvalue;
      priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>> heap;  // 建立大顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      maxHeap(heap, flag, 1);
      int row = sort_heap[flag - 1][1];
      int col = sort_heap[flag - 1][2];
      chess_board_[row][col] = -1;  // 虚拟走一步白棋的最好的位置
      bestvalue = analyse_kill(dep - 1, maxPoints);  // 如果黑棋能胜利
      chess_board_[row][col] = 0;  // 回溯
      if (bestvalue) {  // 只要有一个节点保持不败
          return true;
        }
      return false;
    }
#if 0
  if (dep == 0 || isDeadGame()) {  // 若抵达最深层，走一步对黑棋的最好位置，若黑棋还没赢则返回false
      priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      minHeap(heap, flag, 1);  // 已对选择的点估值过
      int row = sort_heap[0][1];
      int col = sort_heap[0][2];
      chess_board_[row][col] = 1;  // 走一步最佳估值点
      calculateScore();
      chess_board_[row][col] = 0;
      if (result_ == Result::R_BLACK) {  // 黑棋必胜
          return true;
        } else {
          return false;
        }
  } else if (dep % 2 == 0) {  // max 层，黑方决策

      priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      minHeap(heap, flag, 20);  // 一般来说，能冲四或者活三的必在评分20的点内
      seekKillBlack(pointsList, flag);  // 搜索黑方的活三和冲四节点，只要有一个子节点能赢即可
      if (!pointsList.empty()) {  // 判断是否有活三和冲四
          //  存在活三和冲四
          if (dep == 12) {   // 只在开始层记录坐标
              maxPoints.first = pointsList[0].first;  // 只取第一个坐标
              maxPoints.second = pointsList[0].second;
            }
          return true;  // 找到杀棋点
        }
      // 继续寻找杀棋点
      for (int i = flag - 1; i >= 0; --i)  // 当前层点估值由大到小遍历
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          chess_board_[row][col] = 1;
          bool isChecked = analyse_kill(dep - 1, maxPoints);  // 否则则继续搜索杀棋点
          chess_board_[row][col] = 0;
          if (isChecked) {
              if (dep == 12) {   // 只在开始层记录坐标
                  maxPoints.first = row;  // 只取第一个坐标
                  maxPoints.second = col;
                }
              return true;  // 找到杀棋点
            }
        }
      // TODO只要有一个保持不败，则返回该点，如果黑方没有杀棋，则搜索白方杀棋
    } else {  // min层，白方决策
      vector<pair<int, int>> pointsList;
      priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>> heap;  // 建立大顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      maxHeap(heap, flag, 20);  // 一般来说，能冲四或者活三的必在评分20的点内
      seekKillWhite(pointsList, flag);  // 搜索黑方的活三和冲四节点
      if (!pointsList.empty()) {
          if (dep == 11) {   // 只在开始层记录坐标
              maxPoints.first = pointsList[0].first;  // 只取第一个坐标
              maxPoints.second = pointsList[0].second;
            }
          return true;  // 找到白方的杀棋点
        }
      // 继续寻找杀棋点
      for (int i = flag - 1; i >= 0; --i)  // 当前层点估值由大到小遍历
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          chess_board_[row][col] = -1;
          bool isChecked = analyse_kill(dep - 1, maxPoints);  // 否则则继续搜索杀棋点
          chess_board_[row][col] = 0;
          if (isChecked) {
              if (dep == 11) {   // 只在开始层记录坐标
                  maxPoints.first = row;  // 只取第一个坐标
                  maxPoints.second = col;
                }
              return true;  // 找到杀棋点
            }
        }
    }
#endif
}

bool Game::judgeProhibit(vector<vector<int>>& state)  // TODO判断禁手
{
  if ((state[0][9] > 0 &&  state[1][9] > 0) || (state[0][9] > 0 &&  state[2][9] > 0) || (state[0][9] > 0 &&  state[3][9] > 0) ||
      (state[1][9] > 0 &&  state[2][9] > 0) || (state[1][9] > 0 &&  state[3][9] > 0) ||
      (state[2][9] > 0 &&  state[3][9] > 0)) {
      prohibit_ = Prohibit::THREE_PRO;  // 三三禁手（只能是活三）
    }
  if ((state[0][9] > 0 &&  state[1][9] > 0) || (state[0][9] > 0 &&  state[2][9] > 0) || (state[0][9] > 0 &&  state[3][9] > 0) ||
      (state[1][9] > 0 &&  state[2][9] > 0) || (state[1][9] > 0 &&  state[3][9] > 0) ||
      (state[2][9] > 0 &&  state[3][9] > 0)) {
      prohibit_ = Prohibit::FOUR_PRO;  // 四四禁手（活四，冲四都算）
    }
}

void Game::actionByAI()  // ai下棋
{
  // 从评分中找出最大分数的位置
  int alpha = INT_MIN;  // 最大下限
  int beta = INT_MAX;  // 最小上限
  pair<int, int> maxPoints;
  // TODO:算杀模块
  if (player_flag_) {   // TODO
      // AI走黑子
//      if (!analyse_kill(kill_depth_, maxPoints)) {
        AlphaBeta(depth_, maxPoints, alpha, beta);
      } else {
      // AI走白子
//      if (!analyse_kill(7, maxPoints))
        AlphaBeta(depth_ - 1, maxPoints, alpha, beta);  // 极大极小搜索+阿尔法贝塔剪枝
    }
  chess_x_ = maxPoints.first; // 记录落子点
  chess_y_ = maxPoints.second;
  updateMap(chess_x_, chess_y_);  // 在棋盘上记录落子值
}

void Game::maxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& heap, int& flag, int max_flag)
{
  // 防止对同一个位置重复估值，只针对本次，结束时销毁
  vector<vector<bool>> marked(kGridNum, vector<bool>(kGridNum, false));  // 添加标记（初始化都为未标记）
  int val = 0;
  int extension = 1;  // 延长的深度
  // 对非空点的八个方向延申extension个深度
  for (int row = 0; row < kGridNum; ++row)
    for (int col = 0; col < kGridNum; ++col) {
        if (chess_board_[row][col] != 0) { // 若为非空点
            for (int i =  row - extension; i <= row + extension; ++i)  // 平行
              {
                if (i >= 0 && i < kGridNum &&  // 判断是否越界
                    chess_board_[i][col] == 0 &&  // 空位(才能进行回溯生成走法)
                    marked[i][col] == false // 且没有被估值过
                    ) {
                    chess_board_[i][col] = -1;
                    val = calculateScore();
                    if (flag < max_flag) {
                        heap.push(vector<int>{val, i, col});  // 向大顶堆中添加元素
                        marked[i][col] = true;
                        ++flag;  // 记录走法生成器中的点的个数
                      } else {
                        if (val < heap.top()[0]) {
                            heap.pop();
                            heap.push(vector<int>{val, i, col});
                            marked[i][col] = true;
                          }
                      }
                    chess_board_[i][col] = 0;
                  }
              }
            for (int i = col - extension; i <= col + extension; ++i) {  // 垂直
                if (i >= 0 && i < kGridNum &&
                    chess_board_[row][i] == 0 &&
                    marked[row][i] == false // 没有被估值过
                    ) {
                    chess_board_[row][i] = -1;
                    val = calculateScore();
                    if (flag < max_flag) {  // 选10个
                        heap.push(vector<int>{val, row, i});
                         marked[row][i] = true;  // 标记
                        ++flag;
                      } else {
                        if (val < heap.top()[0]) {  // 若估值元素小于大顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, row, i});  // 插入该值
                             marked[row][i] = true;  // 标记
                          }
                      }
                    chess_board_[row][i] = 0;  // 回溯
                  }
              }
            for (int i = row - extension, j = col - extension; i <= row + extension && j <= col + extension; ++i, ++j) {  // 正斜
                if (i >= 0 && i < kGridNum &&
                    j >= 0 && j < kGridNum &&
                    chess_board_[i][j] == 0 &&
                    marked[i][j] == false // 没有被估值过
                    ) {
                    chess_board_[i][j] = -1;
                    val = calculateScore();
                    if (flag < max_flag) {
                        heap.push(vector<int>{val, i, j});
                        marked[i][j] = true;  // 标记
                        ++flag;
                      } else {
                        if (val < heap.top()[0]) {  // 若估值元素小于大顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, i, j});  // 插入该值
                            marked[i][j] = true;  // 标记
                          }
                      }
                    chess_board_[i][j] = 0;  // 回溯
                  }
              }
            for (int i = row - extension, j = col + extension; i <= row + extension && j >= col - extension; i++, j--) {  // 反斜
                if (i >= 0 && i < kGridNum &&
                    j >= 0 && j < kGridNum &&
                    chess_board_[i][j] == 0 &&
                    marked[i][j] == false // 没有被估值过
                    ) {
                    chess_board_[i][j] = -1;
                    val = calculateScore();
                    if (flag < max_flag) {
                        heap.push(vector<int>{val, i, j});
                        marked[i][j] = true;  // 标记
                        ++flag;
                      } else {
                        if (val < heap.top()[0]) {  // 若估值元素小于大顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, i, j});  // 插入该值
                            marked[i][j] = true;  // 标记
                          }
                      }
                    chess_board_[i][j] = 0;  // 回溯
                  }
              }
          }
      }
  sort_heap.clear();
  vector<vector<int>>().swap(sort_heap);  // 清空sort_heap的内存
  for (int i = 0; i < flag; ++i)  // 此时顺序为从大到小
    {
      sort_heap.push_back(heap.top());  // 放入数组中
      heap.pop();
    }
}

void Game::minHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& heap, int& flag, int max_flag)
{
  // 防止对同一个位置重复估值，只针对本次，结束时销毁
  vector<vector<bool>> marked(kGridNum, vector<bool>(kGridNum, false));  // 添加标记（初始化都为未标记）
  int val = 0;  // 估值
  int extension = 1;  // 延长的深度TODO
  // 对非空点的八个方向延申extension个深度
  for (int row = 0; row < kGridNum; ++row)
    for (int col = 0; col < kGridNum; ++col) {
        if (chess_board_[row][col] != 0) { // 若为非空点
            for (int i =  row - extension; i <= row + extension; ++i)  // 平行
              {
                if (i >= 0 && i < kGridNum &&
                    chess_board_[i][col] == 0 &&  // 空位(才能进行回溯生成走法)
                    marked[i][col] == false // 且没有被估值过
                    ) {
                    chess_board_[i][col] = 1;
                    val = calculateScore();  // 存储估值
                    if (flag < max_flag) {  // 维护max_flag走法
                        heap.push(vector<int>{val, i, col});  // 向小顶堆中添加元素
                        marked[i][col] = true;  // 标记
                        ++flag;  // 记录走法生成器中的点的个数
                      } else {
                        if (val > heap.top()[0]) {
                            heap.pop();
                            heap.push(vector<int>{val, i, col});
                            marked[i][col] = true;  // 标记
                          }
                      }
                    chess_board_[i][col] = 0;
                  }
              }
            for (int i = col - extension; i <= col + extension; ++i) {  // 垂直
                if (i >= 0 && i < kGridNum &&
                    chess_board_[row][i] == 0 &&
                    marked[row][i] == false // 没有被估值过
                    ) {
                    chess_board_[row][i] = 1;
                    val = calculateScore();
                    if (flag < max_flag) {  // 选max_flag个
                        heap.push(vector<int>{val, row, i});
                        marked[row][i] = true;  // 标记
                        ++flag;
                      } else {
                        if (val > heap.top()[0]) {  // 若估值元素大于小顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, row, i});  // 插入该值
                            marked[row][i] = true;  // 标记
                          }
                      }
                    chess_board_[row][i] = 0;  // 回溯
                  }
              }
            for (int i = row - extension, j = col - extension; i <= row + extension && j <= col + extension; ++i, ++j) {  // 正斜
                if (i >= 0 && i < kGridNum &&
                    j >= 0 && j < kGridNum &&
                    chess_board_[i][j] == 0 &&
                    marked[i][j] == false // 没有被估值过
                    ) {
                    chess_board_[i][j] = 1;
                    val = calculateScore();
                    if (flag < max_flag) {
                        heap.push(vector<int>{val, i, j});
                        marked[i][j] = true;  // 标记
                        ++flag;
                      } else {
                        if (val > heap.top()[0]) {  // 若估值元素大于小顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, i, j});  // 插入该值
                            marked[i][j] = true;  // 标记
                          }
                      }
                    chess_board_[i][j] = 0;  // 回溯
                  }
              }
            for (int i = row - extension, j = col + extension; i <= row + extension && j >= col - extension; i++, j--) {  // 反斜
                if (i >= 0 && i < kGridNum &&
                    j >= 0 && j < kGridNum &&
                    chess_board_[i][j] == 0 &&
                    marked[i][j] == false // 没有被估值过
                    ) {
                    chess_board_[i][j] = 1;
                    val = calculateScore();
                    if (flag < max_flag) {
                        heap.push(vector<int>{val, i, j});
                        marked[i][j] = true;  // 标记
                        ++flag;
                      } else {
                        if (val > heap.top()[0]) {  // 若估值元素小于大顶堆元素
                            heap.pop();  // 删除堆顶元素
                            heap.push(vector<int>{val, i, j});  // 插入该值
                            marked[i][j] = true;  // 标记
                          }
                      }
                    chess_board_[i][j] = 0;  // 回溯
                  }
              }
          }
      }
  marked.clear();
  vector<vector<bool>>().swap(marked);
  sort_heap.clear();
  vector<vector<int>>().swap(sort_heap);  // 清空sort_heap的内存
  for (int i = 0; i < flag; ++i)  // 此时顺序为从小到大（需要调整）
    {
      sort_heap.push_back(heap.top());
      heap.pop();
    }
}

int Game::AlphaBeta(int dep, pair<int, int>& maxPoints, int alpha, int beta)  // 极大极小值搜索
{
  // alpha 为最大下界，beta为最小上界
  if (dep % 2 ==  0) {  // max层
      int bestvalue = 0;
      if (dep == 0 || isDeadGame() || (dep != depth_ && result_ != Result::R_DRAW))  // 递归终止的条件，当前层出现必胜棋
        {
          return calculateScore();  // 估值
        }
      priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      minHeap(heap, flag, 10);
      for (int i = flag - 1; i >= 0; --i)  // 当前层点估值由大到小遍历，提高剪枝效率
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          chess_board_[row][col] = 1;                           // 虚拟走子
          bestvalue = AlphaBeta(dep - 1, maxPoints, alpha, beta);  // 返回极大值
          chess_board_[row][col] = 0;                          // 回溯
          if (alpha < bestvalue)  // 更新alpha的值
            {
              alpha = bestvalue;  // max层更新自己的下界
              if (depth_ - 1 == dep) {  // 仅限第一层的情况
                  maxPoints.first = row;
                  maxPoints.second = col;
                }
              if (alpha >= beta)
                break;
            }
        }
      return alpha;
    }
  else {  // min层
      int bestvalue = 0;
      if (dep == 0 || isDeadGame() || (dep != depth_ - 1 && result_ != Result::R_DRAW))  // 必杀棋直接返回结果
        {
          return calculateScore();
        }
      priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>> heap;  // 建立大顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      maxHeap(heap, flag, 10);
      for (int i = flag - 1; i >= 0; --i)  // 当前层点估值由小到大遍历，提高剪枝效率
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          chess_board_[row][col] = -1;  // 虚拟AI走子
          bestvalue = AlphaBeta(dep - 1, maxPoints, alpha, beta);  // 返回极小值
          chess_board_[row][col] = 0;  // 回溯
          if (beta > bestvalue)  // 更新beta的值
            {
              beta = bestvalue;  // min层更新自己的上界
              if (depth_ - 1 == dep) {  // 仅限第一层的情况
                  maxPoints.first = row;
                  maxPoints.second = col;
                }
              if (alpha >= beta)  // 进行剪枝
                break;
            }
        }
      return beta;
    }
}
  // 判断棋局估值
void Game::judgeChessTypeEva(vector<vector<int>>& continue_element, vector<int>& state) {
  // 从估值最大的开始筛选
  ChessType type;  // 棋型枚举对象
  int ini = 1, oth = 2;  // 当前估值着重子和影响其估值的子（此时着重子是黑子，影响估值的子是白子）
  for (int i = 0; i < 2; ++i) {
      // 进行两次估值
      if (continue_element[0][ini] == 6) {
          // 长连
          if (i == 0) {
              type = ChessType::BLACK_LIVE_LONG;  // 长连
            } else {
              type = ChessType::WHITE_LIVE_LONG;
            }
        }
      if (continue_element[0][ini] >= 5 ||  // AAAAA? || AAAAAB || AAAAAA
          (continue_element[1][ini] == 5)) {  // ?AAAAA || BAAAAA
          // 成五
          if (i == 0) {
              type = ChessType::BLACK_FIVE;
            } else {
              type = ChessType::WHITE_FIVE;
            }
        } else if (continue_element[0][0] == 1&& continue_element[1][ini] == 4 && continue_element[2][0] == 1) {  //  ?AAAA?
          // 活四
          if (i == 0) {
              type = ChessType::BLACK_LIVE_FOUR;
            } else {
              type = ChessType::WHITE_LIVE_FOUR;
            }
          // AAAA?B || ?AAAAB ====== B?AAAA || BAAAA?
          // AAAA??||              ======   ??AAAA  ||
          // AAAA?A||            ====== A?AAAA || AAAAA?
          // TODO:区分冲四的类型
        } else if ((continue_element[0][ini] == 4 && continue_element[1][0] >= 1) || // AAAA?B || AAAA?? || AAAA?A
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 4) ||   // B?AAAA || A?AAAA
                   (continue_element[0][0] == 2 && continue_element[2][ini] == 4) ||  // ??AAAA
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 4) || // ?AAAAB
                   (continue_element[0][oth] == 1 && continue_element[1][ini] == 4 && continue_element[2][0] == 1)) {  // BAAAA?
          // 冲四_A
          if (i == 0) {
              type = ChessType::BLACK_RUSH_FOUR_A;
            } else {
              type = ChessType::WHITE_RUSH_FOUR_A;
            }
        } else if ((continue_element[1][ini] == 3 && continue_element[2][0] == 1 && continue_element[3][ini] == 1) ||   // BAAA?A || ?AAA?A
                   (continue_element[0][ini] == 3 && continue_element[1][0] == 1 && continue_element[2][ini] == 1) ||  // AAA?AB || AAA?AA || AAA?A?
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 3) ||  // BA?AAA || ?A?AAA
                   (continue_element[0][ini] == 2 && continue_element[1][0] == 1 && continue_element[2][ini] == 3) ||  // AA?AAA
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 1 && continue_element[2][ini] == 3) ||  // A?AAAB || A?AAA?
                   (continue_element[1][ini] == 2 && continue_element[2][0] == 1 && continue_element[3][ini] == 2) ||  // BAA?AA || ?AA?AA
                   (continue_element[0][ini] == 2 && continue_element[1][0] == 1 && continue_element[2][ini] == 2)  // AA?AAB || AA?AA?
                   ) {
          // 冲四
          if (i == 0) {
              type = ChessType::BLACK_RUSH_FOUR;
            } else {
              type = ChessType::WHITE_RUSH_FOUR;
            }
        } else if ((continue_element[0][0] == 2 && continue_element[1][ini] == 3 && continue_element[2][0] == 1) ||  // ??AAA?
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 2 && continue_element[4][0] == 1) || // ?A?AA?
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 2 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] == 1) ||  // ?AA?A?
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 3 && continue_element[2][0] == 2)  // ?AAA??
                   ) {
          // 活三（能形成活四的三）
          if (i == 0) {
              type = ChessType::BLACK_LIVE_THREE;
            } else {
              type = ChessType::WHITE_LIVE_THREE;
            }
          // AAA??B || ?AAA?B || ??AAAB || AA?A?B || AA??AB || A?AA?B || A??AAB || A?A?AB ====== B??AAA || B?AAA? || BAAA?? || B?A?AA || BA??AA || B?AA?A || BAA??A || BA?A?A
          // AAA??? ||              ||              || AA?A?? || AA??A?  ||  A?AA?? || A??AA? || A?A?A? ======  ???AAA ||             ||                || ??A?AA || ?A??AA  ||  ??AA?A  || ?AA??A   || ?A?A?A
          // AAA??A ||            ||             || AA?A?A ||AA??AA  ||               || A??AAA ||A?A?AA ======             ||             ||                ||               ||              ||  A?AA?A  ||               ||
        } else if ((continue_element[0][ini] == 3 && continue_element[1][0] >= 2 ) ||  // AAA??A || AAA??B || AAA???
                   (continue_element[0][oth] == 1 && continue_element[1][ini] == 3 && continue_element[2][0] == 2) ||  // BAAA??
                   (continue_element[1][0] == 2 && continue_element[2][ini] == 3) ||  // A??AAA || B??AAA
                   (continue_element[0][0] >= 2 && continue_element[1][ini] == 3) ||  // ???AAA || ??AAAB
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 2 && continue_element[2][ini] == 2) ||   // A??AAB || A??AA?
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 2 && continue_element[3][ini] == 1) ||  // BAA??A || ?AA??A
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 3 && continue_element[3][0] == 1) ||  // B?AAA?
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 3 && continue_element[2][0] >= 1) ||  // ?AAA?B
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 2 && continue_element[3][ini] == 2) ||  // BA??AA || ?A??AA
                   (continue_element[0][ini] == 2 && continue_element[1][0] == 2 && continue_element[2][ini] >= 1) ||  // AA??AB || AA??A? || AA??AA
                   (continue_element[0][ini] == 2 && continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] >= 1) ||  // AA?A?? || AA?A?A || AA?A?B
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] == 1 && continue_element[4][ini] == 2) ||  // B?A?AA
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 2) ||  // ??A?AA
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 2 && continue_element[2][0] == 1 && continue_element[3][ini] == 1) ||  // ??AA?A
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 1 && continue_element[2][ini] == 2 && continue_element[3][0] >= 1) ||  // A?AA??  || A?AA?A  || A?AA?B
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 2 && continue_element[3][0] == 1 && continue_element[4][ini] == 1) ||  // B?AA?A
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] == 1 && continue_element[4][ini] >= 1) ||  // A?A?A? || A?A?AA || A?A?AB
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] == 1 && continue_element[5][ini] == 1)  // ?A?A?A || BA?A?A
                   ) {
          // 眠三(可以连成冲四）
          if (i == 0) {
              type = ChessType::BLACK_SLEEP_THREE;
            } else {
              type = ChessType::WHITE_SLEEP_THREE;
            }
        } else if ((continue_element[0][0] == 2 && continue_element[1][ini] == 2 && continue_element[2][0] == 2) ||  // ??AA??
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 2 && continue_element[2][0] == 3) ||  // ?AA???
                   (continue_element[0][0] == 3 && continue_element[1][ini] == 2 && continue_element[2][0] == 1) || // ???AA?
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] == 2) || // ?A?A??
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] == 1) // ??A?A?
                   ) {
          // 活二（能形成活三的二）
          if (i == 0) {
              type = ChessType::BLACK_LIVE_TWO;
            } else {
              type = ChessType::WHITE_LIVE_TWO;
            }
          // AA???B || ?AA??B || ??AA?B || ???AAB || A?A??B || A??A?B || A???AB ||             || ?A?A?B || ?A??AB || ??A?AB
          // AA???? ||              ||              ||              || A?A???  || A??A?? ||  A???A?  ||             ||            || ?A??A?  ||
          // AA???A||              ||             ||             || A?A??A || A??A?A || A???AA ||             ||              ||              ||

          // B???AA || B??AA?||B?AA??||BAA???|| B??A?A || B?A??A|| BA???A ||               || B?A?A? || BA??A? ||  BA?A??
          //????AA ||               ||             ||            || ???A?A || ??A??A || ?A???A ||            ||               ||                ||
          //            ||               ||              ||            ||                ||                ||            ||               ||            ||                 ||
        } else if ((continue_element[0][ini] == 2 && continue_element[1][0] >= 3) ||  // AA???B || AA???? || AA???A
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 2 && continue_element[2][0] == 2) ||  // ?AA??B
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 2 && continue_element[2][0] == 1) ||  // ??AA?B
                   (continue_element[0][0] == 3 && continue_element[1][ini] == 2) ||  // ???AAB
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] >= 2) ||  // A?A??B || A?A??? || A?A??A
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 2 && continue_element[2][ini] == 1 && continue_element[3][0] >= 1) ||  // A??A?B || A??A?? || A??A?A
                   (continue_element[0][ini] == 1 && continue_element[1][0] == 3 && continue_element[2][ini] >= 1) ||  // A???AB || A???A? || A???AA
                   (continue_element[0][oth] == 1 && continue_element[1][0] == 3 && continue_element[2][ini] == 2) ||  // B???AA
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] >= 1) ||  // ?A?A?B
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 1 && continue_element[2][0] == 2 && continue_element[3][ini] == 1) ||   // ?A??AB || ?A??A?
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1) ||   // ??A?AB
                   (continue_element[0][0] == 4 && continue_element[1][ini] == 2) ||  // ????AA
                   (continue_element[1][0] == 2 && continue_element[2][ini] == 2) ||  // B??AA?
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 2 && continue_element[3][0] == 2) ||  // B?AA??
                   (continue_element[1][ini] == 2 && continue_element[2][0] == 3) ||  // BAA???
                   (continue_element[1][0] == 2 && continue_element[2][ini] == 1 && continue_element[3][0] == 1 && continue_element[4][ini] == 1) ||  // B??A?A
                   (continue_element[0][0] == 3 && continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1) ||  // ???A?A
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] == 2 && continue_element[4][ini] == 1) ||  // B?A??A
                   (continue_element[0][0] == 2 && continue_element[1][ini] == 1 && continue_element[2][0] == 2 && continue_element[3][ini] == 1) ||  // ??A??A
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 3 && continue_element[3][ini] == 1) ||  // BA???A || ?A???A
                   (continue_element[1][0] == 1 && continue_element[2][ini] == 1 && continue_element[3][0] == 1 && continue_element[4][ini] == 1 && continue_element[5][0] == 1) ||  // B?A?A?
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 2 && continue_element[3][ini] == 1 && continue_element[4][0] == 1) ||  // BA??A?
                   (continue_element[1][ini] == 1 && continue_element[2][0] == 1 && continue_element[3][ini] == 1 && continue_element[4][0] == 2)  // BA?A??
                   ) {
          // 眠二（能形成眠三的二）
          if (i == 0) {
              type = ChessType::BLACK_SLEEP_TWO;
            } else {
              type = ChessType::WHITE_SLEEP_TWO;
            }
        } else if ((continue_element[0][0] == 2 && continue_element[1][ini] == 1 && continue_element[2][0] == 3) ||  // ??A???
                   (continue_element[0][0] == 1 && continue_element[1][ini] == 1 && continue_element[2][0] == 4) ||  // ?A????
                   (continue_element[0][0] == 3 && continue_element[1][ini] == 1 && continue_element[2][0] == 2) ||  // ???A??
                   (continue_element[0][0] == 4 && continue_element[1][ini] == 1 && continue_element[2][0] == 1)) {  // ????A?
          // 活一（能形成眠二的一）
          if (i == 0) {
              type = ChessType::BLACK_LIVE_ONE;
            } else {
              type = ChessType::WHITE_LIVE_ONE;
            }
        } else {
          type = ChessType::OTHER;
        }
      // 统计出现的棋形存储到state数组里
      state[(int)type]++;
      ini = 2; oth = 1;  // 交换（第一次循环结束）
    }
}

// 估值函数(采用全局估值)
int Game::calculateScore()
{ 
  vector<int> weight = { 0,10000000,-1000000,110000,-50000, 110000, -1600, 100000, -400, 8000, -400, 50, -20, 50, -20, 3, -1, 3,-1, 10000000, -1000000 };  // 权重
  vector<vector<int>>state(4, vector<int>(21, 0));  //统计4个方向上每种棋型的个数
  // 滑动窗口(每次处理一个六元组)
  // 对全局估值
  // 水平方向
  for (int col = 0; col < kGridNum; ++col)
    for (int row = 0; row < 10; ++row)
       {
          vector<int> slide(6, 0);  // 存储当前滑动窗口的元素（在每次循环结束后会自动重新初始化）
          // 棋值根据数组顺序分别对应0空位，1黑，2白
          // 棋型辨识数组
          vector<vector<int>> continue_element(6, vector<int>(3, 0));  // continue_element[出现连续顺序][棋的类型值] =该类型的个数
          for (int i = 0; i < 6; ++i)
          slide[i] = chess_board_[row + i][col];
          int index = 0;  // 记录每组连续的元素
          // 从左到右统计滑动窗口中的连续数目
          for (int curr = 0; curr < (int)slide.size(); ++curr) {
              int start = curr;  // start指针记录起始位置
              // 处理连续子
              while (curr + 1 < (int)slide.size() && slide[curr] == slide[curr+1]) {  // 若当前元素与下一个元素相等（出现连续）
                  curr++;
                }
              int continue_count = curr - start + 1;   // 当前元素的连续个数
              // 判断棋型
              if (slide[curr] == 0) {  // 空位
                  continue_element[index][0] = continue_count;
                } else if (slide[curr] == 1) {  // 黑棋
                  continue_element[index][1] = continue_count;
                } else if (slide[curr] == -1) {  // 白棋
                  continue_element[index][2] = continue_count;
                }
              index++;
            }
          // 判断棋局估值
          judgeChessTypeEva(continue_element, state[0]);  // 传递的是引用
          slide.clear();
          continue_element.clear();
          vector<int>().swap(slide);  // 清空滑动窗口的内存
          vector<vector<int>>().swap(continue_element);  // 清空容器并最小化它的容量
        }

  // 垂直方向
  for (int row = 0; row < kGridNum; ++row)
    for (int col = 0; col < 10; ++col)
       {
          vector<int> slide(6);  // 存储当前滑动窗口的元素（在每次循环结束后会自动重新初始化）
          // 棋值根据数组顺序分别对应0,1,2 -----空位,黑,白
          vector<vector<int>> continue_element(6, vector<int>(3, 0));  // continue_element[出现连续顺序][棋的类型值] =该类型的个数  （在每次循环结束后会自动重新初始化）
          for (int i = 0; i < 6; ++i)
          slide[i] = chess_board_[row][col + i];
          int index = 0;  // 记录每组连续的元素
          // 从左到右统计滑动窗口中的连续数目
          for (int curr = 0; curr < (int)slide.size(); ++curr) {
              int start = curr;  // start指针记录起始位置
              // 处理连续子
              while (curr + 1 < (int)slide.size() && slide[curr] == slide[curr+1]) {  // 若当前元素与下一个元素相等（出现连续）
                  curr++;
                }
              int continue_count = curr - start + 1;   // 当前元素的连续个数
              // 判断棋型
              if (slide[curr] == 0) {  // 空位
                  continue_element[index][0] = continue_count;
                } else if (slide[curr] == 1) {  // 黑棋
                  continue_element[index][1] = continue_count;
                } else if (slide[curr] == -1) {  // 白棋
                  continue_element[index][2] =continue_count;
                }
              index++;
            }

          // 判断棋局估值
          judgeChessTypeEva(continue_element, state[1]);  // 传递的是引用
          slide.clear();
          continue_element.clear();
          vector<int>().swap(slide);  // 清空滑动窗口的内存
          vector<vector<int>>().swap(continue_element);  // 清空容器并最小化它的容量
        }

  // 正斜
  for (int row = 0; row  < 10; ++row)
    for (int col = 0; col < 10; ++col)
       {
          vector<int> slide(6, 0);  // 存储当前滑动窗口的元素（在每次循环结束后会自动重新初始化）
          // 棋值根据数组顺序分别对应0,1,2 -----空位,黑,白
          vector<vector<int>> continue_element(6, vector<int>(3, 0));  // continue_element[出现连续顺序][棋的类型值] =该类型的个数  （在每次循环结束后会自动重新初始化）
          for (int i = 0; i < 6; ++i)
          slide[i] = chess_board_[row + i][col + i];
          int index = 0;  // 记录每组连续的元素
          // 从左到右统计滑动窗口中的连续数目
          for (int curr = 0; curr < (int)slide.size(); ++curr) {
              int start = curr;  // start指针记录起始位置
              // 处理连续子
              while (curr + 1 < (int)slide.size() && slide[curr] == slide[curr+1]) {  // 若当前元素与下一个元素相等（出现连续）
                  curr++;
                }
              int continue_count = curr - start + 1;   // 当前元素的连续个数
              // 判断棋型
              if (slide[curr] == 0) {  // 空位
                  continue_element[index][0] = continue_count;
                } else if (slide[curr] == 1) {  // 黑棋
                  continue_element[index][1] = continue_count;
                } else if (slide[curr] == -1) {  // 白棋
                  continue_element[index][2] =continue_count;
                }
              index++;
            }
          // 判断棋局估值
          judgeChessTypeEva(continue_element, state[2]);  // 传递的是引用
          slide.clear();
          continue_element.clear();
          vector<int>().swap(slide);  // 清空滑动窗口的内存
          vector<vector<int>>().swap(continue_element);  // 清空容器并最小化它的容量
        }

  // 反斜
  for (int row = 0; row < 10; ++row)
    for (int col = kGridNum - 1; col > 4; --col)
       {
          vector<int> slide(6, 0);  // 存储当前滑动窗口的元素（在每次循环结束后会自动重新初始化）
          // 棋值根据数组顺序分别对应0,1,2 -----空位,黑,白
          vector<vector<int>> continue_element(6, vector<int>(3, 0));  // continue_element[出现连续顺序][棋的类型值] =该类型的个数  （在每次循环结束后会自动重新初始化）
          for (int i = 0; i < 6; ++i)
          slide[i] = chess_board_[row + i][col - i];
          int index = 0;  // 记录每组连续的元素
          // 从左到右统计滑动窗口中的连续数目
          for (int curr = 0; curr < (int)slide.size(); ++curr) {
              int start = curr;  // start指针记录起始位置
              // 处理连续子
              while (curr + 1 < (int)slide.size() && slide[curr] == slide[curr+1]) {  // 若当前元素与下一个元素相等（出现连续）
                  curr++;
                }
              int continue_count = curr - start + 1;   // 当前元素的连续个数
              // 判断棋型
              if (slide[curr] == 0) {  // 空位
                  continue_element[index][0] = continue_count;
                } else if (slide[curr] == 1) {  // 黑棋
                  continue_element[index][1] = continue_count;
                } else if (slide[curr] == -1) {  // 白棋
                  continue_element[index][2] =continue_count;
                }
              index++;
            }
          // 判断棋局估值
          judgeChessTypeEva(continue_element, state[3]);  // 传递的是引用
          slide.clear();
          continue_element.clear();
          vector<int>().swap(slide);  // 清空滑动窗口的内存
          vector<vector<int>>().swap(continue_element);  // 清空容器并最小化它的容量
        }
  int score = 0;  // 统计当前位置的估值
  for (int i = 1; i < 21; ++i) {
      int count = state[0][i] + state[1][i] + state[2][i] + state[3][i];//统计所有方向上部分棋型的个数
      score += count * weight[i];  // 初步计分
      // 统计算杀棋
      if(i == 1 || i == 19) stat_[1] = count;  // 连五，以及长连都算连5
      else if  (i == 3) stat_[3] = count;  // 活四
      else if  (i == 5) stat_[5] = count;  // 冲四_A
      else if (i == 7) stat_[7] = count;  // 冲四
      else if (i == 9) stat_[9] = count; // 活三
    }
  result_ = Result::R_DRAW;  // 正常行棋
  // 判断禁手，只针对黑棋
  // judgeProhibit(state);
  if (stat_[1] > 0) result_ = Result::R_BLACK;
  else if (stat_[2] > 0) result_ = Result::R_WHITE;
  return score;
}

void Game::startGame(GameType t)
{
  battle_type_ = t;  // 将传递的GameType赋给枚举类型对象，作为当前运行模式
  // 初始棋盘
  chess_board_.clear();
  vector<vector<int>>().swap(chess_board_);  // 清空棋盘，防止clear()之后内存的泄露
  number.clear();
  vector<vector<int>>().swap(number);  // 清空number数组，防止clear()之后内存的泄露
  num = 0;  // 重新初始化
  pointNum = 0;
  for (int i = 0; i <= kGridNum; i++)
    {
      vector<int> lineBoard;
      for (int j = 0; j < kGridNum; j++)
        lineBoard.push_back(0);
      chess_board_.push_back(lineBoard);
      number.push_back(lineBoard);
    }
  // 己方下为true,对方下位false
  player_flag_ = true;  // 下棋状态
  run_procedure_ = DONE;  // 走子状态
}
