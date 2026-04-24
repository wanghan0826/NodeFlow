#include "EdgeItem.h"
#include "NodeItem.h" // 这里引入节点的真正定义
#include <QDebug> // 用于打印死亡信息
#include <QGraphicsScene>

EdgeItem::EdgeItem(NodeItem *sourceNode, NodeItem *destNode)
    : source(sourceNode), dest(destNode) {
    
    // 保证连线永远画在节点的下层，不遮挡节点文字
    setZValue(-1); 
    
    // 双向绑定核心：把自己注册到两个节点的“关联列表”中
    source->addEdge(this);
    dest->addEdge(this);
    
    // 出生时，立刻计算一次初始位置
    updatePosition();
}

void EdgeItem::updatePosition() {
    if (!source || !dest) return;

    // 获取两个节点当前的绝对中心坐标
    // 核心变动：线段的起点是【源节点的输出端口】，终点是【目标节点的输入端口】
    // ==========================================
    QPointF start = source->getOutputPortPos(); 
    QPointF end = dest->getInputPortPos();

    // 性能优化：如果坐标根本没变，就不需要重绘
    if (start == sourcePoint && end == destPoint) return;

    // 通知 Qt 图形引擎：“我的形状和位置要发生改变了，请做好刷新准备！”
    // 这一句非常关键，不写的话线拖动时会在屏幕上留下残影
    prepareGeometryChange(); 

    sourcePoint = start;
    destPoint = end;
}

QRectF EdgeItem::boundingRect() const {
    if (!source || !dest) return QRectF();
    
    // 连线的包围盒就是起点和终点组成的矩形
    // adjusted(-5, -5, 5, 5) 是为了往外扩充一点边缘，防止贝塞尔曲线的弧度被粗暴裁剪
    return QRectF(sourcePoint, destPoint).normalized().adjusted(-5, -5, 5, 5);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!source || !dest) return;

    painter->setRenderHint(QPainter::Antialiasing);

    // 绘制优美的贝塞尔曲线 (Cubic Bezier Curve)
    QPainterPath path(sourcePoint);
    
    // 计算贝塞尔曲线的控制点：让线条从起点的右边平滑延展，汇入终点的左边
    qreal deltaX = destPoint.x() - sourcePoint.x();
    QPointF control1(sourcePoint.x() + deltaX * 0.5, sourcePoint.y());
    QPointF control2(destPoint.x() - deltaX * 0.5, destPoint.y());

    path.cubicTo(control1, control2, destPoint);

    // 设置画笔：一条灰白色的粗线，端点圆润
    painter->setPen(QPen(QColor(150, 150, 150), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path);
}

void EdgeItem::safelyRemove() {
    // 1. 如果源节点还在，告诉它：“把我从你的线缆列表里删掉”
    if (source) {
        source->removeEdge(this); 
        source = nullptr;
    }
    
    // 2. 如果目标节点还在，也告诉它
    if (dest) {
        dest->removeEdge(this);
        dest = nullptr;
    }
    
    // 3. 把自己从画布场景中移除
    if (scene()) {
        //scene()->removeItem(this);
    }
    
    // 4. 最后，挥刀自宫（删除自己的内存）
    delete this;
}

EdgeItem::~EdgeItem() {
    qDebug() << "EdgeItem(线缆) 已被安全销毁，内存释放完毕！";
}

QJsonObject EdgeItem::toJson() const {
    QJsonObject json;
    if (source && dest) {
        // 线不需要记录坐标，只记录“源节点 ID”和“目标节点 ID”即可完美复原关系！
        json["source_id"] = source->getId();
        json["dest_id"] = dest->getId();
    }
    return json;
}
