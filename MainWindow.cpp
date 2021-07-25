#include "mainwindow.h"
#include "ui_mainwindow.h"

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
#include <QLabel>
#include <QAbstractButton>
#include <QInputDialog>

#define CHESS_ONE_SOUND ":/res/sound/chessone.wav"  // 声音文件
#define WIN_SOUND ":/res/sound/win.wav"
#define LOSE_SOUND ":/res/sound/lose.wav"

const int kAIDelay = 700; // AI下棋的思考时间

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
  ui->setupUi(this);  // 提示下棋方
  // 对容器进行初始化
  setWindowTitle("五子棋");
  setFixedSize(900, 900);  // 设置窗口固定大小
  grid_x_ = height() / 20;
  grid_y_ = width() / 20;
  start_x_ = 3 * grid_x_;
  start_y_ = 3 * grid_y_;

  // 设置菜单栏及其选项
  QMenu* menu = menuBar()->addMenu(tr("游戏模式"));
  QAction* actionPVP = new QAction("玩家对战", this);
  QAction* actionPVB = new QAction("人机对战", this);

  // 设置菜单栏及其选项
  QMenu* menu_func = menuBar()->addMenu(tr("功能"));
  QAction* save_chess_game = new QAction("保存棋局", this);
  QAction* repent = new QAction("悔棋", this);

  // 给菜单栏添加选项
  menu->addAction(actionPVP);
  menu->addAction(actionPVB);
  menu_func->addAction(save_chess_game);
  menu_func->addAction(repent);
  // 菜单栏的信号与槽
  connect(actionPVP, &QAction::triggered, this, &MainWindow::PVPinitGame);  // 人人对战
  connect(actionPVB, &QAction::triggered, this, &MainWindow::PVBinitGame);  // 人机对战
  connect(save_chess_game, &QAction::triggered, this, [this](){   // 保存当前棋局, 保存至当前电脑的桌面
      QPixmap pixMap_ = QWidget::grab();
      pixMap_.save("C:/Users/49013/Desktop/savedchessGame/chessGame.png", "PNG");
      qDebug() << "saved";
  });
  connect(repent, &QAction::triggered, this, &MainWindow::repentance);
  connect(actionPVB, &QAction::triggered, this, &MainWindow::printTeamNameDialog);  // 打印队伍名对话框
  connect(actionPVB, &QAction::triggered, this, &MainWindow::initiativeDialog);  // 开局选择先后手
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
  digit_ = 0;
  update();
}

void MainWindow::winDialog(const QString& str)
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("游戏结束");
  btnValue.setText(str + "胜利!");
  btnValue.setStandardButtons(QMessageBox::Ok);  // 设置按钮
  btnValue.setDefaultButton(QMessageBox::Ok);     // 设置默认按钮
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: btnValue.close(); break;
    default: close();  // 默认关闭
    }
}

void MainWindow::deadDialog()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("游戏结束");
  btnValue.setText("平局!");
  btnValue.setStandardButtons(QMessageBox::Ok);  // 设置按钮
  btnValue.setDefaultButton(QMessageBox::Ok);                                     // 设置默认按钮
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: btnValue.close(); break;
    default: close();  // 默认关闭
    }
}

void MainWindow::initiativeDialog()  // 先手对话框
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Question);
  btnValue.setWindowTitle("选择");
  btnValue.setText("是否由计算机开局");
  btnValue.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
  btnValue.setButtonText(QMessageBox::No, QString("否")); // 设置按钮文本为中文
  btnValue.setButtonText(QMessageBox::Ok, QString("是"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::No: {
        game_->color_ = false;
        ui->label_black->setText("黑方用户:" + text);
        ui->label_white->setText("白方用户:计算机");
        initGame(game_->battle_type_); // 初始化棋盘
        pointDialog();  // 用户进行打点
      }
      break;
    case QMessageBox::Ok: {
        game_->color_ = true;
        ui->label_black->setText("黑方用户:计算机");
        ui->label_white->setText("白方用户:" + text);
        game_->chess_board_[7][7] = 1;
        game_->number_[7][7] = ++game_->num_;
        game_->chess_board_[7][6] = -1;
        game_->number_[7][6] = ++game_->num_;
        game_->chess_board_[9][5] = 1;
        game_->number_[9][5] = ++game_->num_;
        game_->trace_.push_back(make_pair(7, 7));
        game_->trace_.push_back(make_pair(7, 6));
        game_->trace_.push_back(make_pair(9, 5));
        game_->player_flag_ = !game_->player_flag_;
        game_->pointNum = 3;  // 针对疏星局选择的打点个数为3
        startPointAI();  // 提示AI打点个数的对话框
        exchangeDialogPC();  // 玩家选择是否交换的对话框
      }
      break;
    default: close();  // 默认
    }

}

void MainWindow::printTeamNameDialog()
{
  bool ok;
  text = QInputDialog::getText(this, tr("输入"),
                                       tr("请输入用户名:"), QLineEdit::Normal,
                                       nullptr, &ok);
  if (ok && !text.isEmpty()) {
      ui->label_black->setText("黑方用户:" + text);
      ui->label_white->setText("白方用户:计算机");
    }
    ui->label_vs->setText("VS");
}

void MainWindow::pointDialog()
{
  bool ok;
  int i = QInputDialog::getInt(this, tr("输入"),
                               tr("请输入打点数:"), 0, 0, 50, 1, &ok);
  if (ok) {
      game_->pointNum = i;  // 传递打点数
    }
}

void MainWindow::startPointPC()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("打点开始提示");
  btnValue.setText("开始打点！");
  btnValue.setStandardButtons(QMessageBox::Ok);
  btnValue.setButtonText(QMessageBox::Ok, QString("确定")); // 设置按钮文本为中文
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: btnValue.close();
      break;
    default: close();  // 默认
    }
}

void MainWindow::startPointAI()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("打点提示");
  // TODO;根据不同的开局给出不同的打点数
  btnValue.setText("计算机给出了打点数:" + QString::number(game_->pointNum));
  btnValue.setStandardButtons(QMessageBox::Ok);
  btnValue.setButtonText(QMessageBox::Ok, QString("确定")); // 设置按钮文本为中文
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: btnValue.close();
      break;
    default: close();  // 默认
    }
}

void MainWindow::endPointPC()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("打点结束提示");
  btnValue.setText("打点结束，下面由白方进行选择！");
  btnValue.setStandardButtons(QMessageBox::Ok);
  btnValue.setButtonText(QMessageBox::Ok, QString("确定"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: {
        btnValue.close();
        // 白棋方选择对白棋方有利的打点位置（即对黑棋不利的点，小的值），并帮黑棋下上
        int bestvalue = INT_MAX;
        pair<int, int> bestPoint;  // 记录该子
        for (auto i : record_) {
            game_->chess_board_[i.first][i.second] = 0;  // 清除绘制的打点子
          }
        for (auto i : record_) {
            game_->chess_board_[i.first][i.second] = 1;  // 虚拟走子估该点的值
            int cal = game_->calculateScore();
            if (cal < bestvalue) {  // 找到最小的值
                bestvalue = cal;
                bestPoint.first = i.first;
                bestPoint.second = i.second;
              }
            game_->chess_board_[i.first][i.second] = 0;  // 回溯
          }
        // 白子帮黑子执行，打点结束由AI进行选择
        game_->chess_board_[bestPoint.first][bestPoint.second] = 1;
        game_->player_flag_ = !game_->player_flag_;  // 交换执行权
        game_->number_[bestPoint.first][bestPoint.second] = ++game_->num_;
        record_.clear();
        vector<pair<int, int>>().swap(record_);
      }
      break;
    default: close();  // 默认
    }
}

void MainWindow::endPointAI()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("打点结束提示");
  btnValue.setText("打点结束，下面由白方进行选择！");
  btnValue.setStandardButtons(QMessageBox::Ok);
  btnValue.setButtonText(QMessageBox::Ok, QString("确定"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: {
          // 打点结束由玩家进行选择
        btnValue.close();
      }
      break;
    default: close();  // 默认
    }
}

bool MainWindow::exchange()  // 根据开局库判断是否需要交换
{
  if (game_->pointNum >= 7) {  // 打点数大于等于7选择不交换
      return false;
    }
  if (game_->chess_board_[7][7] == 1 && game_->chess_board_[7][6] == -1) {  // 直指开局
      if (game_->chess_board_[7][5] == 1 && game_->pointNum <= 3 )  // 寒星局
        return true;
      else if (game_->chess_board_[8][5] == 1 && game_->pointNum <= 5)  // 溪月局
        return true;
      else if (game_->chess_board_[9][5] == 1)  // 疏星局
        return false;
      else if (game_->chess_board_[8][6] == 1 && game_->pointNum <= 5)  // 花月局
        return true;
      else if (game_->chess_board_[9][6] == 1 && game_->pointNum <= 5)  // 残月局
        return true;
      else if (game_->chess_board_[8][7] == 1 && game_->pointNum <= 3)  // 雨月局
        return true;
      else if (game_->chess_board_[9][7] == 1 && game_->pointNum <= 4)  // 金星局
        return true;
      else if (game_->chess_board_[7][8] == 1 && game_->pointNum <= 3)  // 松月局
        return true;
      else if (game_->chess_board_[8][8] == 1 && game_->pointNum <= 1)  // 丘月局
        return true;
      else if (game_->chess_board_[9][8] == 1 && game_->pointNum <= 2)  // 新月局
        return true;
      else if (game_->chess_board_[7][9] == 1 && game_->pointNum <= 1)  // 瑞星局
        return true;
      else if (game_->chess_board_[8][9] == 1 && game_->pointNum <= 2)  // 山月局
        return true;
      else if (game_->chess_board_[9][9] == 1) // 游星局
          return false;
    } else if (game_->chess_board_[7][7] == 1 && game_->chess_board_[8][6] == -1) {  // 斜指开局
      if (game_->chess_board_[9][5] == 1)  // 长星局
        return false;
      else if (game_->chess_board_[9][6] == 1 && game_->pointNum <= 5)  // 峡月局
        return true;
      else if (game_->chess_board_[9][7] == 1 && game_->pointNum <= 3)  // 恒星局
        return true;
      else if (game_->chess_board_[9][8] == 1 && game_->pointNum <= 5)  // 水月局
        return true;
      else if (game_->chess_board_[9][9] == 1)  // 流星局
        return false;
      else if (game_->chess_board_[8][7] == 1 && game_->pointNum <= 3)  // 云月局
        return true;
      else if (game_->chess_board_[8][8] == 1 && game_->pointNum <= 6)  // 蒲月局
        return true;
      else if (game_->chess_board_[8][9] == 1 && game_->pointNum <= 2)  // 岚月局
        return true;
      else if (game_->chess_board_[7][8] == 1 && game_->pointNum <= 3)  // 银月局
        return true;
      else if (game_->chess_board_[7][9] == 1 && game_->pointNum <= 3)  // 明星局
        return true;
      else if (game_->chess_board_[6][8] == 1 && game_->pointNum <= 1)  // 斜月局
        return true;
      else if (game_->chess_board_[6][9] == 1 && game_->pointNum <= 2)  // 名月局
        return true;
      else if (game_->chess_board_[5][9] == 1)
          return false;
    }
  return false;
}

void MainWindow::exchangeDialogPC()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("三手可交换");
  btnValue.setText("是否交换执棋？");
  btnValue.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
  btnValue.setButtonText(QMessageBox::No, QString("否")); // 设置按钮文本为中文
  btnValue.setButtonText(QMessageBox::Ok, QString("是"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: {  // 交换执行棋
        game_->actionByAI();  // AI走一步后开始打点
        startPointPC();            // 开始打点
        pointing_ = true;  // 正在打点
        game_->color_ = !game_->color_;      // 交换执行棋
        ui->label_black->setText("黑方用户:" + text);
        ui->label_white->setText("白方用户:计算机");
      }
      break;
    case QMessageBox::No: {  // 不交换则关闭对话框
        btnValue.close();
      }
      break;
    default: close();
    }
}

void MainWindow::exchangeDialogAI()
{
  QMessageBox btnValue;
  btnValue.setIcon(QMessageBox::Information);
  btnValue.setWindowTitle("三手可交换");
  QString str;
  if (exchange()) {  // 可交换
      str = "交换！";
      game_->run_procedure_ = DONE;  // 等同于先让玩家走子
      game_->color_ = !game_->color_;      // 交换执行棋
      ui->label_black->setText("黑方用户:计算机");
      ui->label_white->setText("白方用户:" + text);
    } else {  // 不可交换
      str = "不交换！";
    }
  btnValue.setText("计算机选择了" + str);
  btnValue.setStandardButtons(QMessageBox::Ok);
  btnValue.setButtonText(QMessageBox::Ok, QString("是"));
  int ret = btnValue.exec();
  switch (ret) {
    case QMessageBox::Ok: {
        btnValue.close();
      }
      break;
    default: break;
  }
}


/*
 因为用户走子之后，未单独分配出线程进行悔棋，因此只能等到AI走子之后才进行悔棋。
此时悔棋则悔两步，因此playflag的状态，以及枚举run_procedure不需要改变，只需要回退两步，对计数num-2，并对打印整个棋盘计数的number_数组清零，最后再对棋盘进行一次更新
 */
void MainWindow::repentance()
{
        auto sit = game_->trace_.back();                             // 最近一次更新的棋子坐标
         game_->chess_board_[sit.first][sit.second] = 0;    // 对AI子进行一次清空
         game_->number_[sit.first][sit.second] = 0;            // 对计数数组中对应该坐标的元素清空为0
         game_->trace_.pop_back();                                    // 容器末尾进行弹出操作
         auto sit_prev = game_->trace_.back();
         game_->chess_board_[sit_prev.first][sit_prev.second] = 0;    // 并同时清空上次用户的走子（即悔棋）
         game_->number_[sit_prev.first][sit_prev.second] = 0;
         game_->num_ -= 2;                                                                   // 回退两次计数
         update();                                                                                  // 最后再对棋盘进行一次更新
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
  painter.drawPixmap(0, 0, 1000, 1000, QPixmap("D:/Qt project/Gobang/棋盘背景.png"));

  // 绘制棋盘
  for (int i = 0; i < kGridNum; ++i)
    {
      painter.drawLine(start_x_, start_y_ + grid_y_ * i, start_x_ + kLineNum * grid_x_, start_y_ + grid_y_ * i);  // 横线
      painter.drawLine(start_x_ + grid_x_ * i, start_y_, start_x_ + grid_x_ * i, start_y_ + kLineNum * grid_y_);  // 竖线
    }

  // 绘制天元点以及5X5线
  for (int i = 3; i < kGridNum; i += 4) {
      for (int j = 3; j < kGridNum; j += 4) {
          brush.setColor(Qt::black);
          brush.setStyle(Qt::SolidPattern);
          painter.setBrush(brush);
          painter.drawEllipse(QPointF(start_x_ + grid_x_ * i, start_y_ + grid_y_ * j), 6, 6);
        }
    }

  // 绘制字符
  QFont font ("Microsoft YaHei", 17, 80, false);
  font.setCapitalization(QFont::QFont::AllUppercase);  // 设置为大写
  font.setLetterSpacing(QFont::AbsoluteSpacing, grid_x_ - font.pointSize() - pen.width() / 2 - 4);  // 设置字符间的间距
  painter.setFont(font);         // 使用字体
  painter.setPen(Qt::black);  // 设置画笔颜色
  painter.drawText(start_x_ - font.pointSize() / 2, start_y_ + kGridNum * grid_y_ + 5, tr("abcdefghijklmno"));  // 绘制文本

  font.setLetterSpacing(QFont::AbsoluteSpacing, 0);  // 重新设置字符间的间距
  painter.setFont(font);
  for (int i = kGridNum; i > 0; --i)
    {
      painter.drawText(start_x_ - grid_x_ - font.pointSize() / 2 - 5, start_y_ + font.pointSize() / 2 + (15 - i) * grid_y_, QString::number(i));  // QString::number()将数字转化为字符串
    }

   // 绘制棋
  QFont chessFont("Microsoft YaHei", 17, 100, false);
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
            // 棋上的数字
            painter.setFont(chessFont);         // 使用字体
            painter.setPen(Qt::red);  // 设置画笔颜色
            painter.drawText(start_x_+ i * grid_x_ - grid_x_ * (3.5 / 12.0), start_y_ + j * grid_y_ + grid_y_ * (3.0 / 12.0)
                             , QString::number(game_->number_[i][j]));
            painter.setPen(Qt::black);  // 设置画笔颜色

          } else if (game_->chess_board_[i][j] == -1) {
            brush.setColor(Qt::white);                                                                       // 设置画刷颜色
            brush.setStyle(Qt::SolidPattern);                                                             // 填充风格
            painter.setBrush(brush);                                                                        // 将画笔交给画家
            painter.drawEllipse(QPointF(start_x_+ i * grid_x_ , start_y_ + j * grid_y_)  // 圆心(浮点精度)
                                , grid_x_ * (5.0 / 12.0), grid_y_ * (5.0 / 12.0));  // 半径
            // 棋上的数字
            painter.setFont(chessFont);         // 使用字体
            painter.setPen(Qt::red);  // 设置画笔颜色
            painter.drawText(start_x_+ i * grid_x_ - grid_x_ * (3.5 / 12.0), start_y_ + j * grid_y_ + grid_y_ * (3.0 / 12.0)
                             , QString::number(game_->number_[i][j]));
            painter.setPen(Qt::black);  // 设置画笔颜色

          } else if (game_->chess_board_[i][j] == -2) {  // 绘制打点子
            brush.setColor(Qt::black);                                                                      // 设置画刷颜色
            brush.setStyle(Qt::SolidPattern);                                                            // 一定要设置填充风格
            painter.setBrush(brush);                                                                        // 将画笔交给画家
            painter.drawEllipse(QPointF(start_x_+ i * grid_x_, start_y_ + j * grid_y_)  // 圆心(浮点精度)
                                , grid_x_ * (5.0 / 12.0), grid_y_ * (5.0 / 12.0));  // 半径
            brush.setColor(Qt::red);                                                                      // 设置画刷颜色
            brush.setStyle(Qt::SolidPattern);                                                            // 一定要设置填充风格
            painter.setBrush(brush);                                                                        // 将画笔交给画家
            // 绘制矩形
            painter.drawRect(start_x_+ i * grid_x_ - grid_x_ * (3 / 12.0), start_y_ + j * grid_y_- grid_y_ * (1 / 12.0), 25, 10);
          }
      }
  // 提示执行棋文本
  QFont promptFont("Microsoft YaHei", 15, 50, false);
  ui->label->setFont(promptFont);
  ui->label->setText(game_->prompt_text_);
  // 判断胜负
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
    if  (game_->run_procedure_ == DONE) {  // 且没有在打点
          chessOneByPerson();
          if (game_->number_[game_->chess_x_][game_->chess_y_] == 3) {  // AI选择是否交换的对话框
              exchangeDialogAI();
            }
          if (game_->battle_type_ == ROBOT && game_->number_[game_->chess_x_][game_->chess_y_] == 4) {
              startPointPC();  // 提示开始打点（机器自动打点）
              // 打出当前（黑子）前几个最优势的棋，然后用户（白棋）选择电脑（黑棋）下哪一步
              int flag = 0;
              priority_queue<vector<int>, vector<vector<int>>, greater<vector<int>>> heap;  // 建立小顶堆
              vector<vector<int>> sort_heap;      // 存储排序后估值较好的点
              game_->minHeap(heap, flag, game_->pointNum, sort_heap);
              for (int i = 0; i < game_->pointNum; ++i) {
                  int row = sort_heap[i][1];
                  int col = sort_heap[i][2];
                  game_->chess_board_[row][col] = -2;
                  ai_record_.push_back(make_pair(row, col));    //  记录AI打点的坐标
                }
              game_->run_procedure_ = DONE;  // 玩家执行棋
              pointing_ai_ = true;
              endPointAI();  // 打点结束提示
            }
      }
    // 如果是人机模式，需要调用AI下棋
    if (game_->battle_type_ == ROBOT  && game_->run_procedure_ == CHESSING
        && !game_->isDeadGame() &&  // 解决棋走完还下的问题
        !game_->isWin(game_->chess_x_, game_->chess_y_) &&
        game_->number_[game_->chess_x_][game_->chess_y_] >= 3 &&  // 指定开局黑方走三步
        !pointing_)  // 打点时AI不走子
      {
        // 用定时器做一个延迟
        QTimer::singleShot(kAIDelay, this,
                           [this]() {
              game_->actionByAI();  // AI走子
              QSound::play(CHESS_ONE_SOUND);
              update();  // 更新棋盘
              if (game_->number_[game_->chess_x_][game_->chess_y_] == 4) {  // 针对第四步打点
                  startPointPC();
                  pointing_ = true;  // 正在打点
                }
          });
      }
  game_->run_procedure_ = DONE;
}

void MainWindow::chessOneByPerson()
{
  if (game_->chess_x_ != -1 && game_->chess_y_ != -1 &&
      (game_->chess_board_[game_->chess_x_][game_->chess_y_] == 0 ||
       game_->chess_board_[game_->chess_x_][game_->chess_y_] == -2))  // 保证在AI打点时，玩家能选择打点子
    {
       // 落子一定要放在鼠标事件中
      if (!pointing_) {
        game_->updateMap(game_->chess_x_, game_->chess_y_);
        game_->run_procedure_ = CHESSING;
        QSound::play(CHESS_ONE_SOUND);  // 发出下棋的声音
        if (pointing_ai_) {  // 对所AI绘制的打点子进行清空, 并走子
            for (auto i : ai_record_) {
                int row = i.first;
                int col = i.second;
                if (row == game_->chess_x_ && col == game_->chess_y_) {  // 选择下的棋就不必回溯
                    continue;
                  }
                game_->chess_board_[row][col] = 0;  // 回溯
              }
            // 清空AI打点子结束后，清空容器
            ai_record_.clear();
            vector<pair<int, int>>().swap(ai_record_);
            pointing_ai_ = false;
            game_->run_procedure_ = DONE;
          }
      } else {  // 打点
        game_->updatePoint(game_->chess_x_, game_->chess_y_);
        // 不改变run_procedure
        record_.push_back(make_pair(game_->chess_x_, game_->chess_y_));  // 将所打的点存入record_中
        if (++digit_ >= game_->pointNum) {  // 选择足够的打点数
            pointing_ = !pointing_;  // 结束打点
            endPointPC();
            game_->actionByAI();  // 轮到电脑走子
          }
     }
    }
}
