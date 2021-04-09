#ifndef GAME_H_
#define GAME_H_

#include <queue>
#include <vector>

using namespace std;

enum GameType {
    ROBOT,  // AI
    PERSON  // 玩家
};

enum GameStatus {
    PLAYING,  // 运行
    WIN,        // 胜利
    DIED        // 死棋
};

enum RuningStatus {
    DONE,        // 下棋结束
    CHESSING  // 走子
};

// 包含了五子棋游戏的基本过程操作
class Game
{
 public:
  Game() = default;
  Game(const Game&) = delete;  // not defined
  Game& operator=(const Game&) = delete;  // not defined
  friend class MainWindow;               // 将MainWindow类声明为友元使得其能访问Game类的私有成员
  bool isWin(int row, int col);             // 判断胜利
  bool isDeadGame();                        // 判断和棋
  void updateMap(int x, int y);           //  更新回合状态
  void actionByAI();                           // 机器执行下棋
  int calculateScore(int row, int col);  // 估值函数
  void startGame(GameType);           // 开始游戏
  void AlphaBeta(int dep, vector<pair<int, int>>& maxPoints);  // αβ剪枝
  int maxSearch(int depth, int row, int col, int beta);                  // 极大值搜索
  int minSearch(int depth, int row, int col, int alpha);                  // 极小值搜索
  void maxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& heap, int& flag, vector<vector<int>>& sort_heap);  // 大顶堆
  void minHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& heap, int& flag, vector<vector<int>>& sort_heap);  // 小顶堆

 private:
  GameStatus running_status_;              // 运行状态
  GameType battle_type_;                     // 对战模式
  RuningStatus run_procedure_;           // 下棋状态
  vector<vector<int>> chess_board_;  // 0表示位置空，1表示黑棋，-1表示白棋
  int digit_ = 1;  // 下棋个数的计数
  bool player_flag_;                             // 下棋状态(己方为true，敌方为false)
  int chess_x_ = -1;                             // 棋盘矩阵坐标
  int chess_y_ = -1;
  int depth_ = 4;  // 搜索树深度
};

#endif // GAME_H_
