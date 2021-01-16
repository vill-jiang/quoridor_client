# Quoridor 客户端 

Quoridor(步步为营) 游戏客户端，快用自己的算法打败 Baseline 吧！

请结合[Quoridor服务端](https://github.com/JY0284/quoridor_server)使用。

## 客户端功能简介

1. 客户端提供到 Quoridor 服务端的通信封装，便于实现自己的下棋算法；
2. 客户端已经实现 4 种简单的下棋算法作为对战机器人，可将之作为自己的算法的对手；
3. 客户端提供多线程并行执行多个相同的机器人，方便实现自己算法的快速对战；
4. 提供客户端日志记录，但由于客户端不判断自己的决策是否违规，所以在某些特殊情况下客户端日志与服务段日志可能不一致；
5. 客户端独立提供日志复盘记录，通过命令行界面可以看到自己与对手一步步激烈的对战。

## 编译须知

### windows 编译

本机编译环境：
```shell
> g++ --version
g++.exe (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project) 8.1.0
Copyright (C) 2018 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

```shell
> make -f .\Makefile_win
```

将会获得如下文件：
```shell
> ls bin
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----        2020/5/7  上午 10:51            187 config.json
-a----       2020/4/14  下午 04:30            919 log_xxxxxxxx_xxxxxx.csv
-a----       2020/3/29  下午 12:43           1323 panel
-a----       2021/1/12  下午 09:52        3632256 QuoridorClient.exe
-a----       2021/1/12  下午 09:52        3484465 QuoridorUI.exe
-a----        2021/1/6  下午 02:46            320 uiconfig.json
```

两个 .exe 文件即为客户端和日志复盘器。

### linux 编译 & osx 编译

仅在 linux 下测试，**注意**：更低版本的gcc/g++(例如 4.8.5)编译会因regex不兼容而出错。

```shell
$ g++ --version
g++ (GCC) 7.3.1 20180303 (Red Hat 7.3.1-5)
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

选择对应的 Makefile 即可。

## 启动客户端和日志复盘器示例

### 客户端启动参数
```shell
> cd bin
> .\QuoridorClient.exe --help
Quoridor game client. Author: Jiang HuiYu.
Usage:
  QuoridorClient [OPTION...]

  -c, --config arg     Config file
  -h, --host arg       Host ip (default: 127.0.0.1)
  -p, --port arg       Port number (default: 19330)
  -t, --threads arg    Threads count, 0 or 1 for close multiple threads
  -m, --multiPorts     Enable thread multiple port
  -k, --key arg        Player key, not for robot (default: 19120000)
  -l, --log            Enable save log
  -r, --replay         Enable game replay
  -C, --replayCmd arg  Game replay Command (default: ./QuoridorUI %file%)
  -R, --robot arg      Choose robot [0.MyPlayer, 1.Defensive, 2.Run,
                       3.DefensiveRun, 4.Baseline] (default: 0)
      --help           Print usage
```

一般情况下设置好服务端的ip地址和端口，加上自己的学号即可与服务端建立通信，服务端会在有2位玩家就绪时自动开始游戏，以下是 Baseline 与 DefensiveRun 的对战输出：

```shell
> .\QuoridorClient.exe -h 127.0.0.1 -p 19330 -R 4
Build robot: Baseline
connect to 127.0.0.1:19330
Waiting for start.
game status: Ok; my loc: (5, 1); enemy loc: (5, 9)  ->  move to: (5, 2)
game status: Ok; my loc: (5, 2); enemy loc: (5, 9); enemy new BlockBar: [(3, 2); (5, 2)]  ->  move to: (6, 2)
game status: Ok; my loc: (6, 2); enemy loc: (5, 9); enemy new BlockBar: [(5, 2); (7, 2)]  ->  move to: (7, 2)
game status: Ok; my loc: (7, 2); enemy loc: (5, 9); enemy new BlockBar: [(7, 0); (7, 2)]  ->  add BlockBar: [(1, 1); (3, 1)]
......
game status: Ok; my loc: (5, 8); enemy loc: (1, 2)  ->  move to: (5, 9)
Game result: Win
Clean your player.
All done.
result: victory(1), defeated(0).
```

DefensiveRun 机器人的输出：

```shell
> .\QuoridorClient.exe -h 127.0.0.1 -p 19330 -R 3
Build robot: DefensiveRun
connect to 127.0.0.1:19330
Waiting for start.
game status: Ok; my loc: (5, 9); enemy loc: (5, 2)  ->  add BlockBar: [(3, 2); (5, 2)]
game status: Ok; my loc: (5, 9); enemy loc: (6, 2)  ->  add BlockBar: [(5, 2); (7, 2)]
game status: Ok; my loc: (5, 9); enemy loc: (7, 2)  ->  add BlockBar: [(7, 0); (7, 2)]
game status: Ok; my loc: (5, 9); enemy loc: (7, 2); enemy new BlockBar: [(1, 1); (3, 1)]  ->  add BlockBar: [(6, 1); (6, 3)]
......
game status: Ok; my loc: (2, 2); enemy loc: (5, 8)  ->  move to: (1, 2)
Game result: Lost
Clean your player.
All done.
result: victory(0), defeated(1).
```

此时，在服务端目录下生成一个日志文件 ```2021-01-12-22-02-32\round_1\0_193304DEFENSIVERUN_VS_193304BASELINE.csv```。

### 日志复盘器启动参数
```shell
> cd bin
> .\QuoridorUI.exe --help
Quoridor game user interface. Author: Jiang HuiYu.
Usage:
  QuoridorUI [OPTION...] positional parameters

  -c, --config arg  Config file name (default: uiconfig.json)
  -l, --log arg     Log file name
  -h, --help        Print usage
```

我们来用日志复盘器打开上面的日志文件(已经将```0_193304DEFENSIVERUN_VS_193304BASELINE.csv```拷贝到了```bin```目录下)：
```shell
> .\QuoridorUI.exe -l .\0_193304DEFENSIVERUN_VS_193304BASELINE.csv
<`````````````````````````````````````````````````>
<                     Quoridor                    >
<      0   1   2   3   4   5   6   7   8   9      >
<    9 *---*---*---*---*---*---*---*---*---*      >
<      |                 V                 | 9    >
<    8 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 8    >
<    7 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 7    >
<    6 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 6    >
<    5 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 5    >
<    4 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 4    >
<    3 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 3    >
<    2 *   *   *   *   *   *   *   *   *   *      >
<      |                                   | 2    >
<    1 *   *   *   *   *   *   *   *   *   *      >
<      |                 A                 | 1    >
<    0 *---*---*---*---*---*---*---*---*---*      >
<        1   2   3   4   5   6   7   8   9        >
<                                                 >
<.................................................>
start game!
193304BASELINE: (5, 1); 193304DEFENSIVERUN: (5, 9)
choose quit(q), prev(p) or next(n, \n):
```

视频展示如下:

[![asciicast](https://asciinema.org/a/384440.svg)](https://asciinema.org/a/384440)

**注意**：`Quoridor\QuoridorUI\uiconfig.json` 必须与 `Quoridor\QuoridorUI\panel` 相互配合，无特殊要求**请勿修改**这两个文件，详细信息见源代码。

## 自己实现机器人?

请参看源文件 ```QuoridorClient/MyPlayer.cpp``` 和 ```QuoridorClient/MyPlayer.h```及其关联的底层数据结构定义文件 ```QuoridorUtils/QuoridorUtils.cpp``` 和 ```QuoridorUtils/QuoridorUtils.h```。

## 依赖库

已放在 ```libs``` 文件夹下。

[asio](https://think-async.com/Asio/)

[cxxopts](https://github.com/jarro2783/cxxopts)

[nlohmann-json](https://github.com/nlohmann/json)

[fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser)
