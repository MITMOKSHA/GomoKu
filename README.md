# 五子棋程序

## 一、程序详情
1. 该程序遵循Google C++代码规范
2. 优化
    1. αβ剪枝
    2. 极大极小算法
    3. 启发式搜索
    4. 多线程
3. 实现功能
    1. 开局选择(添加了开局库)
    2. 三手交换
    3. 五手N打
    4. 悔棋
    5. 保存棋局
    6. 保存棋谱
    7. 计时
    8. 禁手
    9. Pass

## 二、玩法说明
1. 该游戏分为PVB(人机模式)与PVP(玩家模式)两种模式，可在程序左上方Mode菜单栏中进行选择;
2. PVB模式: 开局规则参照国际标准开局, 首先选择先后手, 先手玩家必须先走三个子, 即2个黑子和1个白子, 且第一步棋必须走在天元处"棋盘正中心")。后手玩家选择N个打点数(即到第四步走子结束之后, 也就是第五步, 黑方需要根据后手玩家选择的打点数进行打点，即选择N个位置让白方来选择其中一个位置落子，即帮黑方走一步)。接下来由白方根据当前局势, 选择与黑方进行角色互换。第五步棋开始打点，随后进入正常行棋状态。注意到禁手功能(分为三三禁手, 四四禁手, 长连禁手)。上述规则都是限制黑棋的优势。

## 三、程序演示
### 程序开始界面
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/界面.png" width="450" height="450" />

### 开局选择
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/选择开局.png" width="450" height="450" />

### 输入打点数
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/输入打点数.png" width="450" height="450" />

### 三手可交换
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/三手可交换.png" width="450" height="450" />

### 打点
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/打点.png" width="450" height="450" />

### 禁手判断
.<img src="https://github.com/MITMOKSHA/Gobang/blob/develop/example/禁手判断.png" width="450" height="450" />
