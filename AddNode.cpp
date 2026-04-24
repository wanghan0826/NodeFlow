#include "AddNode.h"

AddNode::AddNode() {}

void AddNode::compute() {
    // 作为加法节点，我必须集齐至少两个输入，才能召唤神龙（进行计算）
    if (_inputValues.size() >= 2) {
        double a = _inputValues[0].toDouble();
        double b = _inputValues[1].toDouble();
        _outputValue = a + b;
    }
}

void AddNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // 极其重要：设置抗锯齿，保证文本和圆角滑溜丝滑
    painter->setRenderHint(QPainter::Antialiasing);

    // ==========================================
    // 【核心修复】：不调用父类的 paint（防文本重叠）
    // 而是直接调用受保护的 drawNodeShell，只画背景和端口
    // ==========================================
    drawNodeShell(painter, option); 

    // 接着，画子类自己特有的 UI：霸气的青色加号
    painter->setPen(QPen(Qt::cyan, 3));
    painter->drawText(boundingRect(), Qt::AlignCenter, "+ ADD +");
}

// 补上刚才漏掉的实现代码
QJsonObject AddNode::toJson() const {
    // 先调用父类的函数，把 id、x、y 拿到手
    QJsonObject json = NodeItem::toJson(); 
    // 加上属于自己的类型标签，这在将来读档时极其重要！
    json["type"] = "AddNode"; 
    return json;
}