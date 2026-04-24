#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QList>
// ==========================================
// 新增：引入 Qt 的万能数据容器 QVariant
#include <QVariant>
// ==========================================
#include <QJsonObject>
#include <QUuid>

class EdgeItem;

class NodeItem : public QGraphicsItem {
public:
    NodeItem();
    virtual ~NodeItem(); // 确保析构函数是虚函数，这在多态里很重要

    // 核心边界与绘制
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 连线管理
    void addEdge(EdgeItem *edge);
    void removeEdge(EdgeItem *edge);

    // 获取端口坐标 (为了简化，目前假设每个节点只有 1 个输入和 1 个输出)
    QPointF getInputPortPos() const;
    QPointF getOutputPortPos() const;
    bool isHitOutputPort(const QPointF &scenePos) const;
    bool isHitInputPort(const QPointF &scenePos) const;

    // ==========================================
    // 实战第一关核心：数据流转接口
    // ==========================================

    // 1. 设置节点的输入数据 (由上游节点或者连线调用)
    void setInputValue(const QVariant &value);

    // 2. 获取节点的输出结果 (供下游节点或者引擎读取)
    QVariant getOutputValue() const;

    // 3. 纯虚函数：极其关键！强制所有子节点必须实现自己的计算逻辑！
    virtual void compute() = 0; 

    // 新增：获取所有连进来的线 (上游)
    QList<EdgeItem *> getIncomingEdges() const;
    // 新增：获取所有连出去的线 (下游)
    QList<EdgeItem *> getOutgoingEdges() const;
// ==========================================
    // 新增：序列化与身份证系统
    // ==========================================
    QString getId() const;
    
    // 虚函数：让节点把自己的数据翻译成 JSON
    virtual QJsonObject toJson() const;
   
    virtual void fromJson(const QJsonObject& json);

    void clearInputs();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    // 新增：受保护的虚函数，专门负责绘制节点壳子（背景、边框、端口）
    // 子类可以通过调用它，然后覆盖默认文本
    virtual void drawNodeShell(QPainter *painter, const QStyleOptionGraphicsItem *option);
    QString _id; // 节点的唯一身份证号
    // ==========================================
    // 供子类使用的数据存储
    // ==========================================
    //QVariant _inputValue;  // 暂存收到的输入数据
    QList<QVariant> _inputValues; // 新增：能装下多个输入数据的容器！
    QVariant _outputValue; // 暂存计算出的结果数据

private:
    QList<EdgeItem *> edgeList;
    
};

#endif // NODEITEM_H1