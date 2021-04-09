#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "game.h"
#include <vector>
#include <QMainWindow>


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
 private:
  Ui::MainWindow *ui;
  Game* game_; // 聚合类(game根据棋盘实现)
  int grid_x_;      // 棋盘格长宽
  int grid_y_;
  int start_x_; // 起始点坐标
  int start_y_;
};
#endif // MIANWINDOW_H_
