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

// 包含了五子棋游戏的基本过程操作
class Game
{
 public:
  Game(){
//    stat_ = vector<int>(11, 0);  // 初始化
  }
  Game(const Game&) = delete;                                                                                                                                                                         // not defined
  // Game& operator=(const Game&) = delete;                                                                                                                                                  // not defined
  friend class MainWindow;                                                                                                                                                                              // 将MainWindow类声明为友元使得其能访问Game类的私有成员
  bool isWin(int row, int col);                                                                                                                                                                            // 判断胜利
  bool isDeadGame();                                                                                                                                                                                        // 判断和棋
  void updateMap(int x, int y);                                                                                                                                                                          //  更新回合状态
  void actionByAI();                                                                                                                                                                                         // 机器执行下棋
  int calculateScore();                                                                                                                                                                                     // 估值函数
  int thread_calculateScore(int threadId);                                                                                                                                                     // 线程版本的估值函数
  void startGame(GameType);                                                                                                                                                                          // 开始游戏
  int AlphaBeta(int dep, int alpha, int beta, int threadIndex, bool isWin);                                                               // αβ剪枝
  int threadAlphaBeta(int dep, int threadIndex, pair<int, int>& maxPoints);                                                                                                   // 多线程AlphaBeta剪枝
  void maxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& , int&, int, vector<vector<int>>&);                                   // 大根堆
  void minHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& , int&, int, vector<vector<int>>&);                              // 小根堆
  void threadMaxHeap(priority_queue<vector<int>, vector<vector<int>>, less<vector<int>>>& , int&, int, int, vector<vector<int>>&);                  // 大顶堆
  void threadMinHeap(priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>>& , int&, int, int, vector<vector<int>>&);             // 小顶堆
  void judgeChessTypeEva(vector<vector<int>>&, vector<int>&);                                                                                                                      // 判断棋局估值
  bool judgeProhibit(int row, int col);                                                                                                                                                              // 判断禁手
  void updatePoint(int x, int y);                                                                                                                                                                        // 更新打点棋
  bool judgeWinType(int x, int y, int threadIndex);                                                                                                                                         // 判断必胜棋型并传给枚举变量(线程版本)
  bool judgeProhibitThread(int row, int col, int threadIndex);                                                                                                                        // 多线程情况下的判断禁手

 private:
  GameStatus running_status_;              // 运行状态
  GameType battle_type_;                     // 对战模式
  RuningStatus run_procedure_;           // 下棋状态
  vector<vector<int>> chess_board_;  // 0表示位置空，1表示黑棋，-1表示白棋
  vector<vector<vector<int>>> thread_chess_board_;  // 线程版的棋盘
  vector<vector<int>> number_;          // 给棋子计数
  vector<pair<int, int>> trace_;             // 跟踪已经下了的棋（方便进行悔棋）
  int num_ = 0;                                     // 计数数字
  bool player_flag_;                             // 下棋状态(己方为true，敌方为false)
//  vector<int> stat_;                             // 统计必杀棋型数
  int chess_x_ = -1;                             // 实际落子之后的坐标
  int chess_y_ = -1;
  int depth_ = 4;                                 // 搜索树深度
  int kill_depth_ = 8;
  int pointNum = 0;                           // 打点数
  QString prompt_text_;                 // 执行方的提示文本
  mutex m_mutex_;                          // 自旋锁
  int thread_num_ = 8;                     // 线程数
  int multi_ = 1;                               // 博弈树宽度 = multi * thread_num_
  bool color_;                                     // 判断哪边是电脑, 黑方是AI为true, 白方是AI为false
  int counts = 0;                                // 处理个数
  int alpha_;                                     // 最大下限
  int beta_;                                    // 最小上限、
};

#endif // GAME_H_
