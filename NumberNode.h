#ifndef NUMBERNODE_H
#define NUMBERNODE_H

#include "NodeItem.h"

class NumberNode : public NodeItem {
public:
    NumberNode();
    
    // 强制实现的虚函数：数字节点的计算逻辑
    void compute() override;
    
    // 重写绘制函数，在节点上画出这个数字
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QJsonObject toJson() const override;
    
    void fromJson(const QJsonObject& json) override;
    
private:
    double _val; // 这个节点专属的内部参数
    
protected:
    // ==========================================
    // 新增：拦截鼠标双击事件
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    // ==========================================
};

#endif // NUMBERNODE_H