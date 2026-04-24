#ifndef EDGEITEM_H
#define EDGEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QJsonObject>


// 前向声明：告诉编译器有 NodeItem 这个东西，避免循环包含
class NodeItem; 

class EdgeItem : public QGraphicsItem {
public:
    // 构造函数：出生即绑定源节点和目标节点
    EdgeItem(NodeItem *sourceNode, NodeItem *destNode);

    // 让线根据它绑定的节点，自己重新计算坐标
    void updatePosition();

    // 必须重写的两个基础绘图函数
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void safelyRemove();

    NodeItem* getSourceNode() const { return source; }
    NodeItem* getDestNode() const { return dest; }
    QJsonObject toJson() const;
    
    // 新增：析构函数
    ~EdgeItem();

private:
    NodeItem *source; // 指向源节点的指针
    NodeItem *dest;   // 指向目标节点的指针
    
    QPointF sourcePoint; // 缓存起点的绝对坐标
    QPointF destPoint;   // 缓存终点的绝对坐标
};

#endif // EDGEITEM_H