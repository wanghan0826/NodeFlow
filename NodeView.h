#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QGraphicsLineItem>

class NodeItem;

class NodeView : public QGraphicsView {
    Q_OBJECT
public:
    explicit NodeView(QWidget *parent = nullptr);
    ~NodeView() override;

protected:
    // 鼠标交互三部曲：引脚连线
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // 键盘交互：触发计算、删除、存档、读档
    void keyPressEvent(QKeyEvent *event) override;

    // 视图交互：鼠标滚轮缩放
    void wheelEvent(QWheelEvent *event) override;

    // 右键交互：动态生成节点菜单
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    // 核心引擎：执行 DAG 拓扑排序计算
    void executeGraph();

    // ==========================================
    // 状态机与持久化变量
    // ==========================================
    NodeItem* _drawingSourceNode = nullptr;  // 当前拖拽连线的起点节点
    QGraphicsLineItem* _tempLine = nullptr;  // 鼠标拖拽时的临时指示线

    QByteArray _memoryCard;                  // 模拟内存卡，暂存 JSON 存档数据
};

#endif // NODEVIEW_H