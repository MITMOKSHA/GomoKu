#include "game.h"

#include <cmath>
#include <queue>

#include "mainwindow.h"

using namespace std;

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
  // 左斜
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
  // 右斜
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

void Game::updateMap (int x, int y)
{
  if (player_flag_)
    chess_board_[x][y] = 1;
  else
    chess_board_[x][y] = -1;
  player_flag_ = !player_flag_;  // 交换棋权
}

void Game::actionByAI()  // ai下棋
{
  // 从评分中找出最大分数的位置
  vector<pair<int, int>> maxPoints;
  AlphaBeta(depth_, maxPoints);  // 极大极小搜索+阿尔法贝塔剪枝
  // 如果有多个相同估值的点， 随机落子
  srand((unsigned)time(0));
  int index = rand() % maxPoints.size();
  
  pair<int, int> pointPair = maxPoints.at(index);
  chess_x_ = pointPair.first; // 记录落子点
  chess_y_ = pointPair.second;
  updateMap(chess_x_, chess_y_);  // 在棋盘上记录落子值
}

void Game::maxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& heap, int& flag, vector<vector<int>>& sort_heap)
{
  for (int i = chess_y_ - 5; i <= chess_y_ + 5; ++i)
    {
      if (i >= 0 && i < kGridNum && chess_board_[chess_x_][i] == 0) {
        heap.push(vector<int>{calculateScore(chess_x_, i), chess_x_, i});  // 向大顶堆中添加元素
        ++flag;
      }
    }
  for (auto row = chess_x_ - 5; row <= chess_x_ + 5; row++)
    for (auto col = chess_y_ - 5; col <= chess_y_ + 5; col++)
      {
        if (row >= 0 && row < kGridNum &&   // 防止越界
              col >= 0 &&
              col < kGridNum &&
              chess_board_[row][col] == 0 &&
              row != chess_x_ &&
              calculateScore(row, col) < heap.top()[0]) {  // 若估值元素小于大顶堆元素
            heap.pop();  // 删除堆顶元素
            heap.push(vector<int>{calculateScore(row, col), row, col});  // 插入该值
          }
      }
  for (int i = 0; i < flag; ++i)
    {
      sort_heap.push_back(heap.top());
      heap.pop();
    }
}

void Game::minHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& heap, int& flag, vector<vector<int>>& sort_heap)
{
  for (int i = chess_y_ - 5; i <= chess_y_ + 5; ++i)
    {
      if (i >= 0 && i < kGridNum && chess_board_[chess_x_][i] == 0) {
        heap.push(vector<int>{calculateScore(chess_x_, i), chess_x_, i});  // 向小顶堆中添加元素
        ++flag;
      }
    }
  for (auto row = chess_x_ - 5; row <= chess_x_ + 5; row++)
    for (auto col = chess_y_ - 5; col <= chess_y_ + 5; col++)
      {
        if (row >= 0 && row < kGridNum &&   // 防止越界
              col >= 0 &&
              col < kGridNum &&
              chess_board_[row][col] == 0 &&
              row != chess_x_ &&
              calculateScore(row, col) > heap.top()[0]) {  // 若估值元素大于小顶堆元素
            heap.pop();  // 删除堆顶元素
            heap.push(vector<int>{calculateScore(row, col), row, col});  // 插入该值
          }
      }
  for (int i = 0; i < flag; ++i)
    {
      sort_heap.push_back(heap.top());
      heap.pop();
    }
}

void Game::AlphaBeta(int dep, vector<pair<int, int>>& maxPoints)  // 极大极小值搜索
{
  int alpha = INT_MIN;
  int beta = INT_MAX;

  int bestvalue = 0;
  if (!player_flag_) {
      priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>> heap;  // 建立大顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      vector<vector<int>> sort_heap;
      maxHeap(heap, flag, sort_heap);  // 从大顶堆中筛选最有价值的点，存放在sort_heap中（启发式搜索）
      for (int i = 0; i < flag; ++i)
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          bestvalue =  minSearch(dep - 1, row, col, alpha);  // 极小值搜索(AI执行器找最小值最有利)返回当前层最小值
          if (bestvalue < beta)                                             // 若该点的极值最小，添加该点坐标
            {
              maxPoints.clear();
              beta = bestvalue;  // 极小值搜索(AI执行器找最小值最有利)返回当前层最小值
              maxPoints.push_back(make_pair(row, col));
            }
          else if (bestvalue == beta)  // 如果有多个最大的数存起来随机走子
            maxPoints.push_back(make_pair(row, col));
        }
    }
  else {
      // 电脑先手应添加开局库，此处未完善2020/1/17
      priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
      int flag = 0;  // 标记记录最佳估值的个数
      vector<vector<int>> sort_heap;
      minHeap(heap, flag, sort_heap);  // 从小顶堆中筛选最有价值的点，存放在sort_heap中（启发式搜索）
      for (int i = 0; i < flag; ++i)
        {
          int row = sort_heap[i][1];
          int col = sort_heap[i][2];
          bestvalue =  maxSearch(dep - 1, row, col, beta);  // 极大值搜索
          if (bestvalue > alpha)                                          // 若该点的极值最小，添加该点坐标
            {
              maxPoints.clear();
              alpha = bestvalue;  // 极小值搜索(AI执行器找最小值最有利)返回当前层最小值
              maxPoints.push_back(make_pair(row, col));
            }
          else if (bestvalue == alpha)  // 如果有多个最大的数存起来随机走子
            maxPoints.push_back(make_pair(row, col));
        }
    }
}

int Game::maxSearch(int dep, int x, int y, int beta)  // 当前层进行极大值搜索
{
  int alpha = INT_MIN;
  int bestvalue = 0;
  if (dep == 0 || isDeadGame())  // 递归终止的条件
    {
      return calculateScore(x, y);  // 估值
    }
  priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
  int flag = 0;  // 标记记录最佳估值的个数
  vector<vector<int>> sort_heap;
  minHeap(heap, flag, sort_heap);
  for (int i = 0; i < flag; ++i)
    {
      int row = sort_heap[i][1];
      int col = sort_heap[i][2];
        // 前提是这个坐标是空的
        if (chess_board_[row][col] == 0)
          {
            chess_board_[row][col] = 1;                           // 虚拟玩家棋盘走子
            bestvalue = minSearch(dep - 1, x, y, alpha);  // 极小值搜索
            chess_board_[row][col] = 0;                          // 回溯
            if (bestvalue > alpha)  // 更新alpha的值
              {
                alpha = bestvalue;
              }
            if (alpha >= beta)
              return alpha;  // 进行剪枝
          }
      }
  return alpha;  // 返回当前层的最大值
}

int Game::minSearch(int dep, int x, int y, int alpha)  // 当前层进行极小值搜索
{
  int beta = INT_MAX;
  int bestvalue = 0;
  if (dep == 0 || isDeadGame())
    {
      return calculateScore(x, y);
    }
  priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>> heap;  // 建立大顶堆
  int flag = 0;  // 标记记录最佳估值的个数
  vector<vector<int>> sort_heap;
  maxHeap(heap, flag, sort_heap);
  for (int i = 0; i < flag; ++i)
    {
      int row = sort_heap[i][1];
      int col = sort_heap[i][2];
        if (chess_board_[row][col] == 0)  // 对当前空坐标估值
          {
            chess_board_[row][col] = -1;  // 虚拟AI走子
            bestvalue = maxSearch(dep - 1, x, y, beta);
            chess_board_[row][col] = 0;  // 回溯
            if (bestvalue < beta)  // 更新beta的值
              {
                beta = bestvalue;
              }
            if (alpha >= beta)  // 进行剪枝
              return alpha;
          }
      }
  return beta;  // 返回当前层的最小值
}

// 估值函数(对棋盘进行估值)
int Game::calculateScore(int row, int col)  // 传入估值点
{
  int black = 0;
  int white = 0;
  
  // 统计玩家或者电脑连成的子
  int blackNum = 0;  // 黑子连成子的个数
  int whiteNum = 0; // 白子连成子的个数
  int emptyNum = 0; // 各方向空白位的个数
  
  // 遍历周围八个方向（考虑五元组中棋子的状态）
  for (int y = -1; y <= 1; y++)
    for (int x = -1; x <= 1; x++)
      {
        // 重置
        blackNum = 0;
        whiteNum = 0;
        emptyNum = 0;
        
        // 原坐标不算
        if (!(y == 0 && x == 0))
          {
            // 每个方向延伸4个子
            // 分别分析每个五元组
            // 对黑子评分
            for (int i = 1; i <= 4; i++)
              {
                if (row + i * y >= 0 && row + i * y < kGridNum &&
                    col + i * x >= 0 && col + i * x < kGridNum &&
                    chess_board_[row + i * y][col + i * x] == 1) // 黑子
                  {
                    blackNum++;
                  }
                else if (row + i * y >= 0 && row + i * y < kGridNum &&
                         col + i * x >= 0 && col + i * x < kGridNum &&
                         chess_board_[row + i * y][col + i * x] == 0) // 空白位
                  {
                    emptyNum++;
                    break;
                  }
                else            // 出边界
                  break;
              }

            for (int i = 1; i <= 4; i++)
              {
                if (row - i * y >= 0 && row - i * y < kGridNum &&
                    col - i * x >= 0 && col - i * x < kGridNum &&
                    chess_board_[row - i * y][col - i * x] == 1) // 黑子
                  {
                    blackNum++;
                  }
                else if (row - i * y >= 0 && row - i * y < kGridNum &&
                         col - i * x >= 0 && col - i * x < kGridNum &&
                         chess_board_[row - i * y][col - i * x] == 0) // 空白位
                  {
                    emptyNum++;
                    break;
                  }
                else            // 出边界
                  break;
              }

            if (blackNum == 1)                      // 杀二
              black += 10;
            else if (blackNum == 2)                 // 杀三
              {
                if (emptyNum == 1)
                  black += 30;
                else if (emptyNum == 2)
                  black += 40;
              }
            else if (blackNum == 3)                 // 杀四
              {
                // 量变空位不一样，优先级不一样
                if (emptyNum == 1)
                  black += 60;
                else if (emptyNum == 2)
                  black += 110;
              }
            else if (blackNum == 4)                 // 杀五
              black += 10100;
            
            // 进行一次清空
            emptyNum = 0;
            
            // 对白子评分
            for (int i = 1; i <= 4; i++)
              {
                if (row + i * y >= 0 && row + i * y < kGridNum &&
                    col + i * x >= 0 && col + i * x < kGridNum &&
                    chess_board_[row + i * y][col + i * x] == -1) // 白子
                  {
                    whiteNum++;
                  }
                else if (row + i * y >= 0 && row + i * y < kGridNum &&
                         col + i * x >= 0 && col + i * x < kGridNum &&
                         chess_board_[row +i * y][col + i * x] == 0) // 空白位
                  {
                    emptyNum++;
                    break;
                  }
                else            // 出边界
                  break;
              }

            for (int i = 1; i <= 4; i++)
              {
                if (row - i * y >= 0 && row - i * y < kGridNum &&
                    col - i * x >= 0 && col - i * x < kGridNum &&
                    chess_board_[row - i * y][col - i * x] == -1) // 白子
                  {
                    whiteNum++;
                  }
                else if (row - i * y >= 0 && row - i * y < kGridNum &&
                         col - i * x >= 0 && col - i * x < kGridNum &&
                         chess_board_[row - i * y][col - i * x] == 0) // 空白位
                  {
                    emptyNum++;
                    break;
                  }
                else            // 出边界
                  break;
              }

            if (whiteNum == 0)                        // 普通下子
              white += 5;
            else if (whiteNum == 1)                 // 活二
              white += 10;
            else if (whiteNum == 2)
              {
                if (emptyNum == 1)                //
                  white += 25;
                else if (emptyNum == 2)
                  white += 50;                        // 活三（再走一步就可以形成活四）
              }
            else if (whiteNum == 3)
              {
                if (emptyNum == 1)                // 冲四（有一个威胁的四）
                  white += 55;
                else if (emptyNum == 2)
                  white += 100;                     // 活四（有两个威胁的四）
              }
            else if (whiteNum >= 4)
              white += 10000;                     // 活五
          }
      }
  if (player_flag_) {
      return  black - white;
    } else {
      return white - black;
    }
}

void Game::startGame(GameType t)
{
  battle_type_ = t;  // 将传递的GameType赋给枚举类型对象，作为当前运行模式
  // 初始棋盘
  chess_board_.clear();  // 清除容器中的所有元素
  for (int i = 0; i <= kGridNum; i++)
    {
      std::vector<int> lineBoard;
      for (int j = 0; j < kGridNum; j++)
        lineBoard.push_back(0);
      chess_board_.push_back(lineBoard);
    }
  
  // 己方下为true,对方下位false
  player_flag_ = true;  // 下棋状态
  run_procedure_ = DONE;  // 走子状态
}
