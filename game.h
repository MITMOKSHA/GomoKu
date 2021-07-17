#ifndef GAME_H_
#define GAME_H_

#include <queue>
#include <vector>
#include <mutex>
#include <QString>

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

enum RuningStatus {  // 防止抢棋
    DONE,        // 下棋结束
    CHESSING  // 走子
};

enum class Result: int { R_DRAW,  // 正常行棋
                         R_WHITE,
                         R_BLACK,
};

enum class Prohibit: int {THREE_PRO,  // 三三禁手
                          FOUR_PRO,  // 四四禁手
                          FIVE_PRO ,  // 长连禁手

};

// 包含了五子棋游戏的基本过程操作
class Game
{
 public:
  Game(){
    stat_ = vector<int>(11, 0);  // 初始化
  }
  Game(const Game&) = delete;  // not defined
  // Game& operator=(const Game&) = delete;  // not defined
  friend class MainWindow;               // 将MainWindow类声明为友元使得其能访问Game类的私有成员
  bool isWin(int row, int col);             // 判断胜利
  bool isDeadGame();                        // 判断和棋
  void updateMap(int x, int y);           //  更新回合状态
  void actionByAI();                           // 机器执行下棋
  int calculateScore();                        // 估值函数
  int thread_calculateScore(int threadId);          // 线程版本的估值函数
  void startGame(GameType);           // 开始游戏
  int AlphaBeta(int dep, int alpha, int beta, pair<int, int>& maxPoints);                                                                                           // αβ剪枝
  int threadAlphaBeta(int dep, int threadIndex, vector<pair<int, int>>& maxPoints, int alpha, int beta);                                                   // 多线程AlphaBeta剪枝
  void threadDistribute();                                                                                                                                                             // 线程分配
  void maxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& , int&, int, vector<vector<int>>&);       // 大根堆
  void minHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& , int&, int, vector<vector<int>>&);  // 小根堆
  void threadMaxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& , int&, int, int);       // 大顶堆
  void threadMinHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& , int&, int, int);  // 小顶堆
  void judgeChessTypeEva(vector<vector<int>>&, vector<int>&);                                                                                                  // 判断棋局估值
  void seekKillBlack(vector<pair<int, int>>&, int, vector<vector<int>>&);       // 寻找黑杀棋
  void seekKillWhite(vector<pair<int, int>>&, int, vector<vector<int>>&);       // 寻找白杀棋
  bool analyse_kill(int dep, pair<int, int>&);     // 算杀
  bool judgeProhibit(vector<vector<int>>&);  // TODO判断禁手
  void updatePoint(int x, int y);                       // 更新打点棋

 private:
  GameStatus running_status_;              // 运行状态
  GameType battle_type_;                     // 对战模式
  RuningStatus run_procedure_;           // 下棋状态
  vector<vector<int>> chess_board_;  // 0表示位置空，1表示黑棋，-1表示白棋
  vector<vector<vector<int>>> thread_chess_board_;  // 线程版的棋盘
  vector<vector<int>> number_;          // 给棋子计数
  vector<vector<vector<int>>> thread_sort_heap;
  vector<pair<int, int>> trace_;             // 跟踪已经下了的棋（方便进行悔棋）
  int num_ = 0;                                     // 计数数字
  bool player_flag_;                             // 下棋状态(己方为true，敌方为false)
  vector<int> stat_;                             // 统计必杀棋型数
  int chess_x_ = -1;                             // 实际落子之后的坐标
  int chess_y_ = -1;
  int depth_ = 2;                                 // 搜索树深度
  int kill_depth_ = 8;
  Result result_;                                 // 判断是否存在必胜棋
  Prohibit prohibit_;                            // 判断禁手
  int pointNum = 0;                           // 打点数
  QString prompt_text_;                // 执行方的提示文本
  mutex m_mutex_;                         // 自旋锁
  int thread_num_ = 5;                     // 线程数
  int multi_ = 2;                               // 博弈树宽度 = multi * thread_num_
  bool color_;                                     // 判断哪边是电脑, 黑方是AI为true, 白方是AI为false
  bool first_step_ = true;                          // 标记第一层，方便记录将要下的节点位置
};

#endif // GAME_H_
