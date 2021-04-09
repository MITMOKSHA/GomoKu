#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <vector>

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QSound>  // 需要在pro文件添加QT += multimedia
#include <QMenu>
#include <QAction>
#include <QRandomGenerator>  // 随机数引擎
#include <QTimer>
#include <QBrush>
#include <QFont>
#include <QAbstractButton>

#define CHESS_ONE_SOUND ":/res/sound/chessone.wav"  // 声音文件
#define WIN_SOUND ":/res/sound/win.wav"
#define LOSE_SOUND ":/res/sound/lose.wav"

const int kAIDelay = 700; // AI下棋的思考时间

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  // 对容器进行初始化
  setWindowTitle("五子棋");
  setFixedSize(1000, 1000);  // 设置窗口固定大小
  grid_x_ = width() / 20;
  grid_y_ = height() / 20;
  start_x_ = 3 * grid_x_;
  start_y_ = 3 * grid_y_;

  // 设置菜单栏及其选项
  QMenu* menu = menuBar()->addMenu(tr("游戏模式"));
  QAction* actionPVP = new QAction("玩家对战", this);
  QAction* actionPVB = new QAction("人机对战", this);

  // 给菜单栏添加选项
  menu->addAction(actionPVP);
  menu->addAction(actionPVB);

  // 菜单栏的信号与槽
  connect(actionPVP, &QAction::triggered, this, &MainWindow::PVPinitGame);
  connect(actionPVB, &QAction::triggered, this, &MainWindow::PVBinitGame);
  connect(actionPVB, &QAction::triggered, this, &MainWindow::initiativeDialog);
  initGame(PERSON);  // 默认为PVP开局
}

MainWindow::~MainWindow()
{
  if (game_)  // 处理game动态开辟的指针
    {
      delete game_;
      game_ = nullptr;
    }
  delete ui;
}

void MainWindow::initGame(GameType t)
{
  game_ = new Game;  // 给Game指针动态开辟空间
  if (t == PERSON)
    PVPinitGame();
  else if (t == ROBOT)
    PVBinitGame();
}

void MainWindow::PVPinitGame()
{
  game_->startGame(PERSON);
  game_->running_status_ = PLAYING;
  update();
}

void MainWindow::PVBinitGame()
{
  game_->startGame(ROBOT);
  game_->running_status_ = PLAYING;
  update();
}

void MainWindow::winDialog(const QString& str)
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("游戏结束");
  btnValue.setText(str + "胜利!");
  btnValue.setInformativeText("你还想再来一局吗?");
  btnValue.setStandardButtons(QMessageBox::Ok | QMessageBox::No);  // 设置按钮
  btnValue.setDefaultButton(QMessageBox::Ok);                                     // 设置默认按钮
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: initGame(game_->battle_type_); break;
    case QMessageBox::No: close(); break;
    default: close();  // 默认关闭
    }
}

void MainWindow::deadDialog()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("游戏结束");
  btnValue.setText("平局!");
  btnValue.setInformativeText("你还想再来一局吗?");
  btnValue.setStandardButtons(QMessageBox::Ok | QMessageBox::No);  // 设置按钮
  btnValue.setDefaultButton(QMessageBox::Ok);                                     // 设置默认按钮
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: initGame(game_->battle_type_); break;
    case QMessageBox::No: close(); break;
    default: close();  // 默认关闭
    }
}

void MainWindow::initiativeDialog()  // 先手对话框
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Question);
  btnValue.setWindowTitle("选择");
  btnValue.setText("由谁先手");
  btnValue.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
  btnValue.setButtonText(QMessageBox::Ok, QString("玩家先手")); // 设置按钮文本为中文
  btnValue.setButtonText(QMessageBox::No, QString("电脑先手"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: initGame(game_->battle_type_);  // 初始化棋盘
      break;
    case QMessageBox::No: game_->actionByAI();  // 电脑走子
      break;
    default: close();  // 默认关闭
    }

}

void MainWindow::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(this);
  painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
  QPen pen;
  pen.setWidth(3);  // 调整粗细
  painter.setPen(pen);
  QBrush brush;
  painter.drawPixmap(0, 0, 1000, 1000, QPixmap("../棋盘背景.png"));
  // 绘制棋盘
  for (int i = 0; i < kGridNum; ++i)
    {
      painter.drawLine(start_x_, start_y_ + grid_y_ * i, start_x_ + kLineNum * grid_x_, start_y_ + grid_y_ * i);  // 横线
      painter.drawLine(start_x_ + grid_x_ * i, start_y_, start_x_ + grid_x_ * i, start_y_ + kLineNum * grid_y_);  // 竖线
    }

  // 绘制字符
  QFont font ("Helvetica", 17, 80, false);
  font.setCapitalization(QFont::QFont::AllUppercase);  // 设置为大写
  font.setLetterSpacing(QFont::AbsoluteSpacing, grid_x_ - font.pointSize() - pen.width() / 2);  // 设置字符间的间距
  painter.setFont(font);         // 使用字体
  painter.setPen(Qt::black);  // 设置画笔颜色
  painter.drawText(start_x_ - font.pointSize() / 2, start_y_ + kGridNum * grid_y_ + 5, tr("abcdefghijklmno"));  // 绘制文本

  font.setLetterSpacing(QFont::AbsoluteSpacing, 0);  // 重新设置字符间的间距
  painter.setFont(font);
  for (int i = kGridNum; i > 0; --i)
    {
      painter.drawText(start_x_ - grid_x_ - font.pointSize() / 2 - 5, start_y_ + font.pointSize() / 2 + (15 - i) * grid_y_, QString::number(i));  // QString::number()将数字转化为字符串
    }

  QFont font1("Helvetica", 17, 100, false);
  QFont font2("Helvetica", 17, 100, false);
  // 绘制棋
  for (int i = 0; i < kGridNum; ++i)
    for (int j = 0; j < kGridNum; ++j)
      {
        if (game_->chess_board_[i][j] == 1)
          {
            brush.setColor(Qt::black);                                                                      // 设置画刷颜色
            brush.setStyle(Qt::SolidPattern);                                                            // 一定要设置填充风格
            painter.setBrush(brush);                                                                        // 将画笔交给画家
            painter.drawEllipse(QPointF(start_x_+ i * grid_x_, start_y_ + j * grid_y_)  // 圆心(浮点精度)
                                , grid_x_ * (5.0 / 12.0), grid_y_ * (5.0 / 12.0));  // 半径
            painter.setFont(font1);
            painter.setPen(Qt::red);
            painter.drawText(start_x_ + i * grid_x_ - font.pointSize() / 2, start_y_ + j * grid_y_ + font.pointSize() / 2, QString::number(game_->digit_));
          }
        else if (game_->chess_board_[i][j] == -1)
          {
            brush.setColor(Qt::white);                                                                       // 设置画刷颜色
            brush.setStyle(Qt::SolidPattern);                                                             // 填充风格
            painter.setBrush(brush);                                                                        // 将画笔交给画家
            painter.drawEllipse(QPointF(start_x_+ i * grid_x_, start_y_ + j * grid_y_)  // 圆心(浮点精度)
                                , grid_x_ * (5.0 / 12.0), grid_y_ * (5.0 / 12.0));  // 半径
            painter.setFont(font2);
            // 打印棋子的计数
            painter.setPen(Qt::red);
            painter.drawText(start_x_ + i * grid_x_ - font.pointSize() / 2, start_y_ + j * grid_y_ + font.pointSize() / 2, QString::number(game_->digit_));
          }
      }

  if (game_->chess_x_ >= 0 && game_->chess_x_ < kGridNum &&
      game_->chess_y_ >= 0 && game_->chess_y_ < kGridNum &&
      (game_->chess_board_[game_->chess_x_][game_->chess_y_] == 1 ||
       game_->chess_board_[game_->chess_x_][game_->chess_y_] == -1))
    {
      if (game_->isWin(game_->chess_x_, game_->chess_y_) && game_->running_status_ == PLAYING)
        {
          game_->running_status_ = WIN;
          QSound::play(WIN_SOUND);  // 发出胜利的声音
          QString str;
          // 判断什么颜色的棋获胜
          if (game_->chess_board_[game_->chess_x_][game_->chess_y_] == 1)
            str = "黑棋";
          else if (game_->chess_board_[game_->chess_x_][game_->chess_y_] == -1)
            str = "白棋";
          // 对话框(提示哪一方赢了)
          winDialog(str);
        }
    }
  // 判断死局
  if (game_->isDeadGame() && game_->running_status_ == PLAYING)
    {
      game_->running_status_ = DIED;
      QSound::play(LOSE_SOUND);
      // 对话框提示死局
      deadDialog();
    }
  painter.end();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
  // 获取鼠标点击的坐标
  int x = event->x();
  int y = event->y();

  // 若点击范围棋盘可执行范围内
  if (x >= start_x_ - grid_x_ / 2 && x <= start_x_ - grid_x_ / 2 + kGridNum * grid_x_ &&
      y >= start_y_ - grid_y_ / 2 && y <= start_y_ - grid_y_ / 2 + kGridNum * grid_y_)
    {
      game_->chess_x_ = (x - (start_x_ - grid_x_ / 2)) / grid_x_;
      game_->chess_y_ = (y - (start_y_ - grid_y_ / 2)) / grid_y_;
      update();
    }
  // 人下棋，并且不能抢机器的棋
  if  (game_->run_procedure_ == DONE) {
      chessOneByPerson();
    }
  // 如果是人机模式，需要调用AI下棋
  if (game_->battle_type_ == ROBOT  && game_->run_procedure_ == CHESSING
      && !game_->isDeadGame() &&  // 解决棋走完还下的问题
      !game_->isWin(game_->chess_x_, game_->chess_y_))
    {
      // 用定时器做一个延迟
      QTimer::singleShot(kAIDelay, this,
                         [this]() {
          game_->actionByAI();  // AI走子
          QSound::play(CHESS_ONE_SOUND);
          update();  // 更新棋盘
        });
    }
  game_->run_procedure_ = DONE;
}

void MainWindow::chessOneByPerson()
{
  if (game_->chess_x_ != -1 && game_->chess_y_ != -1 && game_->chess_board_[game_->chess_x_][game_->chess_y_] == 0)  // 落子一定要放在鼠标事件中
    {
      game_->updateMap(game_->chess_x_, game_->chess_y_);
      QSound::play(CHESS_ONE_SOUND);  // 发出下棋的声音
      game_->run_procedure_ = CHESSING;
    }
}
