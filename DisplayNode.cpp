#include "DisplayNode.h"
#include <QDebug>

DisplayNode::DisplayNode() {}

void DisplayNode::compute() {
    // 【核心修复】：先判断列表是不是空的，再去拿 first()
    if (!_inputValues.isEmpty()) {
        qDebug() << "🔥 [DisplayNode] 收到上游数据，值为:" << _inputValues.first();
        _outputValue = _inputValues.first(); 
    } else {
        qDebug() << "⚠️ [DisplayNode] 没有收到有效输入!";
    }
}

void DisplayNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    NodeItem::paint(painter, option, widget); 

    painter->setPen(Qt::green);
    
    // 【核心修复】：极其安全的三元运算符
    // 如果列表不为空，才取 first()；如果为空，直接显示 "No Data"
    QString text = !_inputValues.isEmpty() ? _inputValues.first().toString() : "No Data";
    
    painter->drawText(boundingRect().adjusted(0, 20, 0, 0), Qt::AlignHCenter | Qt::AlignTop, text);
}

QJsonObject DisplayNode::toJson() const {
    QJsonObject json = NodeItem::toJson(); 
    json["type"] = "DisplayNode"; 
    return json;
}
