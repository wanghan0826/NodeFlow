#include "NodeItem.h"
#include "EdgeItem.h" // 必须在这里引入 EdgeItem 的真身
#include <QLineF> // 加在文件顶部，用于计算线段的长度
#include <QDebug>

NodeItem::NodeItem() {

    _id = QUuid::createUuid().toString();

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    // 【极其致命的一句】必须开启这个标志，节点移动时才会触发 itemChange 事件！
    setFlag(QGraphicsItem::ItemSendsGeometryChanges); 
    
}

void NodeItem::addEdge(EdgeItem *edge) {
    edgeList.append(edge);
}

// 当节点发生任何状态改变时，Qt 引擎会自动调用这个函数
QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    // 如果是因为鼠标拖拽导致位置发生了变化
    if (change == ItemPositionHasChanged) {
        // 遍历所有连在自己身上的线，命令它们更新坐标
        for (EdgeItem *edge : qAsConst(edgeList)) {
            edge->updatePosition(); 
        }
    }
    // 把事件继续交还给底层处理
    return QGraphicsItem::itemChange(change, value);
}

QRectF NodeItem::boundingRect() const {
    // 原来是 (-60, -40, 120, 80)
    // 现在把 X 往左扩 10 像素，总宽度加 20 像素，确保完全包住引脚
    return QRectF(-70, -40, 140, 80); 
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    
    // 1. 调用通用绘制壳子的函数
    drawNodeShell(painter, option);

    // 2. 绘制基础节点默认的白色文本 "Node"
    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, "Node");
}

// 新增：实现获取引脚坐标的函数
// mapToScene 会将节点内部的局部坐标，精准转换为整个画布的全局坐标
// ==========================================
QPointF NodeItem::getInputPortPos() const {
    return mapToScene(QPointF(-60, 0)); 
}

QPointF NodeItem::getOutputPortPos() const {
    return mapToScene(QPointF(60, 0));
}

bool NodeItem::isHitOutputPort(const QPointF &scenePos) const {
    // 把原来的 10.0 改成 15.0，让判定区域更大，手感更好
    return QLineF(scenePos, getOutputPortPos()).length() < 15.0; 
}

bool NodeItem::isHitInputPort(const QPointF &scenePos) const {
    // 计算鼠标点击位置，到输入端口中心的绝对距离
    // 如果距离小于 10 个像素，我们就认为“命中了！”
    return QLineF(scenePos, getInputPortPos()).length() < 10.0;
}

void NodeItem::removeEdge(EdgeItem *edge) {
    // 从列表中移除这根线
    edgeList.removeAll(edge);
}

// ==========================================
// 极其核心的析构函数：节点死亡前的最后防线
// ==========================================
NodeItem::~NodeItem() {
    qDebug() << "NodeItem(节点) 开始执行临终清理...";
    
    // 复制一份列表，因为在遍历时直接删原列表会导致迭代器崩溃
    QList<EdgeItem *> edgesCopy = edgeList;
    
    for (EdgeItem *edge : edgesCopy) {
        if (edge) {
            // 命令连在身上的线，立刻执行安全自毁！
            edge->safelyRemove(); 
        }
    }
    
    qDebug() << "NodeItem(节点) 彻底死亡，内存释放完毕！";
}

// ==========================================
// 数据管线接口实现
// ==========================================
void NodeItem::setInputValue(const QVariant &value) {
    // 把直接赋值改为追加到列表中
    _inputValues.append(value);
    update(); 
}

// 新增清空函数的实现
void NodeItem::clearInputs() {
    _inputValues.clear();
    update(); // 刷新画面，让 Qt 引擎重新绘制节点
}

QVariant NodeItem::getOutputValue() const {
    return _outputValue;
}

// 注意：compute() 是纯虚函数，不需要在这里实现，交给子类去写！

QList<EdgeItem *> NodeItem::getIncomingEdges() const {
    QList<EdgeItem *> inEdges;
    for (EdgeItem *edge : edgeList) {
        // 如果这根线的终点是我，那就是输入线
        if (edge->getDestNode() == this) { 
            inEdges.append(edge);
        }
    }
    return inEdges;
}

QList<EdgeItem *> NodeItem::getOutgoingEdges() const {
    QList<EdgeItem *> outEdges;
    for (EdgeItem *edge : edgeList) {
        // 如果这根线的起点是我，那就是输出线
        if (edge->getSourceNode() == this) { 
            outEdges.append(edge);
        }
    }
    return outEdges;
}

QString NodeItem::getId() const {
    return _id;
}

QJsonObject NodeItem::toJson() const {
    QJsonObject json;
    // 基础节点只负责保存最通用的属性：ID 和 坐标
    json["id"] = _id;
    json["x"] = pos().x();
    json["y"] = pos().y();
    return json;
}

void NodeItem::fromJson(const QJsonObject &json) {
    // 1. 极其关键：用存档里的老 ID，覆盖掉出生时随机生成的新 ID！
    _id = json["id"].toString(); 
    
    // 2. 恢复坐标
    setPos(json["x"].toDouble(), json["y"].toDouble());
}

// ==========================================
// 新增：实现基础壳子绘制 (背景和端口)
// ==========================================
void NodeItem::drawNodeShell(QPainter *painter, const QStyleOptionGraphicsItem *option) {
    // 1. 画节点主体（深灰背景）
    painter->setBrush(QColor(45, 45, 45));
    if (isSelected()) {
        painter->setPen(QPen(QColor(0, 150, 255), 2));
    } else {
        painter->setPen(QPen(QColor(20, 20, 20), 1));
    }
    painter->drawRoundedRect(boundingRect(), 5, 5);

    // 2. 绘制左侧输入端口 (青色小圆圈)
    painter->setBrush(QColor(0, 200, 150));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QRectF(-65, -5, 10, 10));

    // 3. 绘制右侧输出端口 (橙色小圆圈)
    painter->setBrush(QColor(255, 100, 50));
    painter->drawEllipse(QRectF(55, -5, 10, 10));
}