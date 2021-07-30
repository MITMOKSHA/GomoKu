#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "game.h"
#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QTime>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

const int kLineNum = 14;  // 棋盘线长度
const int kGridNum = 15;

class MainWindow : public QMainWindow
{
  Q_OBJECT

 protected:
  void paintEvent(QPaintEvent*);  // 绘图事件
  void mousePressEvent(QMouseEvent* event);  // 鼠标按下事件

 public:
  MainWindow(QWidget *parent = nullptr);
  MainWindow(const MainWindow&) = delete;  // not defined
  MainWindow& operator=(const MainWindow&) = delete;  // not defined
  ~MainWindow();
  void initGame(GameType);            // 默认对战初始化
  void PVPinitGame();                     // 玩家对战初始化
  void PVBinitGame();                     // 人机对战初始化
  void chessOneByPerson();            // 人下棋
  void winDialog(const QString&);  // 胜利对话框
  void deadDialog();                       // 平局对话框
  void initiativeDialog();                // 先行棋对话框
  void pointDialog();                      // 打点数对话框
  void printTeamNameDialog();    // 打印队伍名对话框
  void startPointPC();                         // 开始打点提示对话框（玩家先手）
  void startPointAI();                          // 开始打点提示对话框（AI先手）
  void endPointPC();                           // 玩家打点结束提示对话框
  void endPointAI();                        // 打点结束提示对话框
  bool exchange();                         // 针对开局库决定是否换手
  void exchangeDialogPC();              // 玩家决定是否换手对话框
  void exchangeDialogAI();             //  AI提示是否换手对话框
  void repentance();                        //  悔棋操作函数
  void prohibitHandDialog();            // 禁手对话框提示

 private:
  Ui::MainWindow *ui;
  Game* game_;                          // 聚合类(game根据棋盘实现)
  int grid_x_;                             // 棋盘格长宽
  int grid_y_;
  int start_x_;                           // 起始点坐标
  int start_y_;
  int digit_ = 0;                          // 计数
  QString text;                          // 输入的用户名称
  bool pointing_= false;              // 用户是否正在打点
  bool pointing_ai_ = false;         // AI是否正在打点
  vector<pair<int, int>> record_;  // 记录用户打点子的坐标
  vector<pair<int, int>> ai_record_;          // 记录AI打点子的坐标
  QTimer *black_timer_;                    // 比赛计时, 定时器
  QTimer *white_timer_;
  QTime black_show_time_;               // 计时显示
  QTime white_show_time_;
};
#endif // MIANWINDOW_H_
