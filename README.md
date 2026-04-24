# 🌌 NodeFlow - 准工业级节点化可视化计算引擎

![C++](https://img.shields.io/badge/C++-11%2F14%2F17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-5.x%20%7C%206.x-41CD52.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

NodeFlow 是一个基于 **C++** 与 **Qt Graphics View Framework** 从零底层构建的无环有向图（DAG）可视化节点计算引擎。本项目未依赖任何庞大的第三方节点库，纯手搓实现了完整的图形学交互、拓扑执行排序以及对象反射持久化机制。

## ✨ 核心特性 (Core Features)

* **🧠 DAG 拓扑执行引擎**：底层基于 **Kahn 算法 (拓扑排序)**，一键计算多输入/多输出的复杂节点网络，完美处理数据流向与执行顺序。
* **🏭 C++ 反射与工厂模式**：克服 C++ 缺乏原生反射的缺陷，手搓 `NodeFactory` 单例。只需一行代码注册新类，系统即可自动实现“字符串 -> 实体对象”的动态映射。
* **💾 深度序列化与反序列化 (Save/Load)**：借助 JSON 将内存中的节点实例、坐标、UUID、内部数据及连线拓扑关系进行全局快照。读档时通过反射机制瞬间重建整个内存网络。
* **🖱️ 工业级图形交互**：
  * **动态右键菜单**：基于工厂模式的数据驱动 UI，菜单内容根据已注册的节点动态生成，极致解耦。
  * **物理遮挡剔除连线**：极其严谨的射线检测（Raycasting）逻辑，丝滑的拖拽连线体验，彻底消灭临时线遮挡 Bug。
  * **级联安全销毁**：完美处理 C++ 野指针与 Qt 场景刷新的生命周期冲突，实现节点与连线的极度安全删除。

## 🛠️ 技术栈 (Tech Stack)

* **语言**：C++ (重度运用多态、虚函数、Lambda、函数指针、智能指针概念)
* **GUI 框架**：Qt (QGraphicsScene, QGraphicsView, QGraphicsItem)
* **设计模式**：工厂模式 (Factory)、单例模式 (Singleton)
* **核心算法**：图论拓扑排序 (Kahn's Algorithm)

## 🚀 快速上手 (Quick Start)

### 交互操作指南
* **添加节点**：在画布空白处 `右键` 呼出动态菜单，选择并生成节点。
* **数字交互**：`双击` NumberNode 节点，可弹出输入框动态修改数值。
* **连接网络**：拖拽节点的彩色输入/输出引脚，将其相互连接。
* **执行计算**：按下 `Space (空格键)`，引擎将顺着连线拓扑，自动执行数据流转与计算。
* **保存与读取**：
  * 按下 `S 键`：将当前画布状态序列化为 JSON 并存入内存。
  * 按下 `L 键`：销毁当前画布，根据 JSON 存档瞬间完美重建拓扑网络。
* **删除操作**：框选节点或连线，按下 `Delete 键` 安全销毁。

## 🎯 架构扩展演示

如果需要新增节点（例如乘法节点 `MulNode`），只需三步，无需修改任何框架底层逻辑：
1. 继承 `NodeItem` 并重写 `compute()` 及 `toJson()` 函数。
2. 在 `main.cpp` 启动时将其注册进工厂：`NodeFactory::instance().registerNode("MulNode", [](){ return new MulNode(); });`
3. 右键菜单会自动长出 `MulNode` 选项，读档引擎也会自动学会反序列化该节点。
