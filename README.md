# 阿Sue砸陶罐：150关进阶版 v1.1

在v1.0的100关进阶版上新增101～150关，前100份关卡JSON保持原样。第1关仍是更早v0.8第50关的完整配置：60个任务列、20000分、15列、组合奖励、种子1984与原操作时长。

新增101～150继续交替安排随机、竖向配对和交错排列，轮换颜色权重，结合任务长度、周期进列和目标分。新关任务118～144列、目标34000～69500；第150关142个任务列、目标40500分。不同排列的目标分别校准，数值不会逐关机械增加，场地容量始终15列。

原匀速推出150ms、初始一列立即显示、空白点击天降补罐、木槌、重力、计分和暂停保持。旧解锁、完成记录、最高分、音量、全屏与分辨率继续沿用，完成100的旧存档可继续101。

Windows 10/11 x64离线游戏，原生C++/Win32，无需安装Python、Node.js或引擎。EXE约127KB。150关均有带思考间隔的通关路径，2700次策略模拟、三页选关、20／50／100／150重试及存档检查通过。Windows实机运行、手感、FPS和长期进程内存仍需实测。

## 1. 如何运行游戏

1. 下载ZIP，右键“全部解压”，建议解压到新文件夹，避免启动旧版。
2. 双击文件夹中的ASuePotGame.exe，窗口标题应有“150关进阶版 v1.1”。
3. 保持EXE、assets、levels的相对位置，不要只单独移动EXE。
4. 左键点击可消除的相邻同色组合发槌；点孤立陶罐或场景空白处随机落罐。NEXT或空格提前进列；ESC暂停。
5. 即使场上有其他可消除组合，点击空白或不可消除罐也会补罐。菜单、NEXT、暂停按钮执行各自功能，顶部信息栏不触发补罐。罐在移动／下落时暂不接受攻击或NEXT，避免重复输入破坏动画。

关卡选择分三页：01～50、51～100、101～150，底部“上一页／下一页”切换。中间页同时显示上一页与下一页，三位数编号居中；灰色锁定关不可进入。新玩家从01顺序解锁，完成100解锁101、149解锁150；完成150显示全部完成，不进入未制作的151。

旧存档按最高已解锁编号继续，无需删除。旧50／80／100关通关存档可分别继续51／81／101，历史最高分、完成和设置保留，本次是否达标只看本局分数。想从头体验进阶版，可在选择关卡进入01。

levels/campaign.json的level_count=150限定本版关数，目录中残留的151以上数据不会混入。建议全部解压到新文件夹，启动标题含v1.1的EXE。

存档位于 `%APPDATA%\ASuePotGame\save.json`。在资源管理器地址栏输入 `%APPDATA%\ASuePotGame` 即可找到。游戏目录移动后进度仍在。失败选择“重新挑战本关”保持当前关卡ID；已解锁20时失败、退出重开后仍能选择20。每关完成后立即保存解锁、完成状态和最高分。音量、全屏、分辨率也保存；写临时文件、刷新后替换，保留有效备份.bak。

全部150关显示目标和“还差多少分”，需要完成所有任务列并达到目标才过关；达分不会提前结束，左侧溢出仍会失败。最后一列后有3秒结算窗口，正在进行的动作会先完成；暂停冻结计时。

全部150关使用大组合奖励：2罐20分、3罐60分、4罐120分，优先合并相邻同色陶罐，悬停可预览得分。已有组合时补罐颜色随机，全场无组合时尽可能配对救场；点不可消除陶罐和空白仍可补罐。自动进列保留动画占用的超时时间，连续补罐不能反复拖慢整体节奏。

## 2. 如何修改一个关卡

用记事本打开levels/level_01.json至level_150.json，修改数字后保存为UTF-8，重启游戏。数值写整数，开关使用0／1，保持引号、逗号和大括号完整。文件编号必须连续，遇到缺号停止读取；关卡ID以文件编号为准。

## 3. 如何修改陶罐数量

- rows：最高堆叠行数，1～10。
- wave_min_rows、wave_max_rows：新列陶罐数的随机范围，不能超过rows。固定满列时两者都等于rows。
- wave_count：本关需要进入的列数。
- columns：场地容纳的列数，2～15；本版150关统一15列。不要与wave_count的任务列数混淆。
- rain_min_count、rain_max_count：补罐数量范围，受可用空间限制。

例如rows=10，wave_min_rows=8，wave_max_rows=10，每个新列随机8～10个。补罐另算，不计入进列任务。

## 4. 如何修改游戏速度与难度

| 参数 | 含义 |
|---|---|
| next_wave_delay_ms | 自动进列间隔；4000表示4秒，比7000快 |
| pot_move_duration_ms | 匀速横向推出／移动时长，全部关卡150ms |
| attack_delay_ms | 点击至命中的时间 |
| attack_duration_ms | 木槌完整动作时间，必须大于命中时间 |
| pot_fall_duration_ms | 消除后的下落时间 |
| rain_duration_ms | 从天上补罐的下落时间 |
| score_popup_duration_ms | 得分文字持续时间 |
| level_end_delay_ms | 最后一列后结算等待，正在进行的动作会先完成 |
| target_score | 本关最低分，0关闭；任务列完成后按此目标结算 |
| burst_every_columns | 每隔几列缩短下一次进列间隔，0关闭 |
| burst_interval_ms | 上述周期进列所用间隔 |
| pattern_mode | 0随机、1竖向成对、2尽量避免竖向同色 |
| vertical_repeat_percent | 随机排列中重复上一罐颜色的百分比，0～100 |
| weight_blue／white／beige／brown | 四种颜色的相对出现权重；全部0自动恢复均分 |
| rain_enabled | 1启用补罐，0关闭 |
| rain_trigger_mode | 1点击未命中可消除组时落罐（全部150关）；0只有全场无组时落罐 |
| rain_match_mode | 0总是尽可能配对；1仅全场无组合时配对救场；2全部随机。本版全部150关用1 |
| keep_wave_overflow | 1保留自动进列的动画超时时间，连续补罐不能拖慢整体节奏；0沿用旧计时 |
| min_group | 至少几个相邻匹配陶罐可消除，默认2 |
| group_by_color | 1按基础色，0颜色和纹样都要匹配 |
| compact_empty_columns | 1收拢空列，0保留空列 |
| seed | 随机种子，相同配置和操作可复现 |

补罐数量、速度、进列、攻击、计分均集中在关卡配置。不确定的原版规则见《原游戏玩法分析.md》。特殊图标、×2功能仍未确认，没有擅自启用。详细难度表见levels/关卡配置说明.md。

## 5. 如何修改得分

points_per_pot默认10。score_mode=0时n×基础分；1时n×(n−1)×基础分；2时n²×基础分。本版全部150关用1。invalid_penalty默认0，分数最低0。补罐不直接加分，砸掉新增陶罐正常得分。每关独立计分。修改计分后需同时重新校准target_score，不能只改目标数字；旧版最高分不会被删除，较低新分不覆盖历史高分。这些为重制版规则，并非原录像已确认公式。

## 6. 如何替换陶罐图片

查看assets/素材替换说明.md。将58×49的24位BMP放入assets，命名pot_blue.bmp、pot_white.bmp、pot_beige.bmp、pot_brown.bmp。透明区域填纯洋红色RGB(255,0,255)。15列显示时完整图片自动等比缩小，不需要更改素材尺寸；缺图继续使用独立绘制的占位图形。

## 7. 如何替换角色图片

190×240的24位BMP，分别命名character_idle.bmp、character_attack.bmp、character_hit.bmp、character_fail.bmp、character_win.bmp。背景为1280×720的background.bmp。未替换的状态继续使用默认图形。

## 8. 如何增加第151关

1. 复制levels/level_150.json为level_151.json，把level_id改为151，再调整任务列、速度、目标分等。
2. 打开levels/campaign.json，把level_count从150改为151。
3. 重启游戏，自动出现第四页，仍按存档顺序解锁。

无需改核心代码或重新编译，最多256关。文件编号连续，100以后使用三位数字，例如level_150.json、level_151.json。清单不能只增加数量而缺少对应文件。

## 9. 如何重新导出EXE

玩家无需编译。开发时：Windows安装Visual Studio Build Tools，选择C++桌面开发与Windows SDK，在x64 Native Tools Command Prompt运行tools/export_windows.bat；这条路径尚未在Windows测试。Linux有g++、GNU binutils和Python3时运行python3 tools/build_linux.py，本次采用此流程。脚本不下载依赖。EXE固定装载基址，无CRT依赖。

## 工程结构

- project/game_manager.h：状态、攻击、进列、补罐时序、胜负。
- project/level_manager.h、level_data.h、level_loader.h：数据槽、参数定义、JSON加载。
- project/pot_manager.h、pot.h：Pot基础结构、类型、连通消除、重力、分布与落罐。
- project/score_manager.h：计分；player_controller.h：五种角色状态。
- project/board_layout.h：陶罐显示、点击、悬停和动画共用的布局。
- project/ui_manager.h、effect_manager.h：按钮、固定容量平移动画、下落与得分。
- project/audio_manager.h、settings_manager.h、asset_manager.h：音频、设置、素材。
- project/main.cpp：Windows入口、布局、输入、60Hz逻辑调度与双缓冲显示。
- save_system/save_manager.h：用户目录存档、校验、备份与原子替换。
- levels/：150份独立JSON配置、campaign.json关数清单和关卡表；assets/：占位声音及图片替换说明。
- tests/：真实核心逻辑、模拟Win32／文件系统、难度策略模拟与CSV结果。

陶罐使用固定16×12数组，特效也用固定数组，无无限新增节点。落罐不分配新容器；切关重置网格和特效。显示位图只在窗口尺寸变化时重建；画笔和画刷绘制后删除。完整验证范围与Windows试玩检查见《测试报告.md》。
