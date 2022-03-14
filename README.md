# RobotCommander

Robocon比赛中用于方便调试和控制机器人运动的小软件

## TODOs:
接下来要做的任务（2.27）

##### 主要部分

- [ ] 通讯协议，规范内容完善
- [ ] 完善通讯协议的实现，分开实现(pure C or C++)，方便添加新组件
- [ ] 主窗体的放大缩小，缩放点的精确控制（改用double记录位置）
- [ ] 考虑新功能的添加（绘制曲线的功能）

##### 舒适使用

- [ ] 自动保存设置内容
- [ ] 界面美化
- [ ] 安装包&自动更新

##### 按时间顺序的计划列表（3.10）

- [x] 自动保存设置
- [x] 检测更新并提示
- [x] 改用double记录位置座标
- [x] 新的设置界面（横向）
- [x] 将浮动窗口独立出单独的UI类
- [x] 未连接串口时不要发送信息
- [x] 简化about页面内容
- [x] 关闭时保存窗口大小位置
- [x] 记住dockWidget的位置和大小
- [ ] 主窗体的放大缩小
- [ ] 优化compositor接发消息的流程&Console
- [ ] 新的命令队列和参数概览小窗口
- [ ] 用CSS美化界面
- [ ] checkbutton改为switchbutton
- [ ] version.rc的问题

同时，新的协议完善
