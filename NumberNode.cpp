#include "NumberNode.h"
#include <QInputDialog>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

NumberNode::NumberNode() {
    _val = 42.0; // 默认给它注入灵魂数字 42
}

void NumberNode::compute() {
    // 作为源头节点，它的计算就是把内部的值丢给输出端口
    _outputValue = _val; 
}

void NumberNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    
    drawNodeShell(painter, option); // 画通用壳子

    // 用黄色的字显示当前的数字
    painter->setPen(Qt::yellow);
    QString text = QString("Num: %1").arg(_val);
    painter->drawText(boundingRect(), Qt::AlignCenter, text);
}

void NumberNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    bool ok;
    // 弹出一个极其原生的 Qt 数字输入框
    double newVal = QInputDialog::getDouble(
        nullptr,                    // 父窗口
        "修改数值",                  // 弹窗标题
        "请输入新的数字:",            // 提示文本
        _val,                       // 默认值是当前值
        -9999999.0, 9999999.0,      // 最小值，最大值
        2,                          // 小数点后保留 2 位
        &ok                         // 记录用户是点了确定还是取消
    );

    // 如果用户点了“确定”
    if (ok) {
        _val = newVal;        // 更新内部数据
        _outputValue = _val;  // 更新输出缓存
        update();             // 强制节点重新绘制自己，让新数字显示出来！
        
        qDebug() << "📝 [NumberNode] 数值已修改为:" << _val;
    }

    // 拦截事件，不再往下传
    event->accept();
}

QJsonObject NumberNode::toJson() const {
    // 先调用父类的函数，把 id、x、y 拿到手
    QJsonObject json = NodeItem::toJson(); 
    // 再加上自己的特有属性
    json["type"] = "NumberNode"; 
    json["value"] = _val; // 把内部的灵魂数字 42 也存起来
    return json;
}

void NumberNode::fromJson(const QJsonObject &json) {
    // 先让父类把 ID 和坐标恢复了
    NodeItem::fromJson(json); 
    
    // 再恢复自己特有的数据
    if (json.contains("value")) {
        _val = json["value"].toDouble(); 
    }
}
