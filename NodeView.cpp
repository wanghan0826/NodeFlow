#include "NodeView.h"
#include "NodeItem.h"
#include "EdgeItem.h"
#include "NodeFactory.h"

#include <QMenu>
#include <QAction>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QQueue>
#include <QDebug>

NodeView::NodeView(QWidget *parent) : QGraphicsView(parent) {
    // 开启抗锯齿，让连线和节点边缘极其平滑
    setRenderHint(QPainter::Antialiasing);
    // 允许鼠标拖拽画布背景移动
    setDragMode(QGraphicsView::RubberBandDrag);
    // 隐藏滚动条，更像一个无限画布引擎
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

NodeView::~NodeView() {}

// ==========================================
// 1. 鼠标连线与防遮挡系统
// ==========================================
void NodeView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->pos());
        NodeItem *node = dynamic_cast<NodeItem*>(item);

        if (node && node->isHitOutputPort(mapToScene(event->pos()))) {
            _drawingSourceNode = node;
            _tempLine = new QGraphicsLineItem();
            _tempLine->setPen(QPen(Qt::cyan, 2, Qt::DashLine));
            
            QPointF startPos = _drawingSourceNode->getOutputPortPos();
            _tempLine->setLine(QLineF(startPos, startPos));
            scene()->addItem(_tempLine);
            
            event->accept();
            return;
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void NodeView::mouseMoveEvent(QMouseEvent *event) {
    if (_drawingSourceNode && _tempLine) {
        QPointF startPos = _drawingSourceNode->getOutputPortPos();
        QPointF endPos = mapToScene(event->pos());
        _tempLine->setLine(QLineF(startPos, endPos));
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void NodeView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && _drawingSourceNode && _tempLine) {
        // 核心遮挡剔除：先隐藏临时线
        _tempLine->hide();

        QGraphicsItem *hitItem = itemAt(event->pos());
        NodeItem *targetNode = dynamic_cast<NodeItem*>(hitItem);

        if (targetNode && targetNode != _drawingSourceNode) {
            if (targetNode->isHitInputPort(mapToScene(event->pos()))) {
                EdgeItem *newEdge = new EdgeItem(_drawingSourceNode, targetNode);
                scene()->addItem(newEdge);
                qDebug() << "🔗 [连线成功]";
            }
        }

        scene()->removeItem(_tempLine);
        delete _tempLine;
        _tempLine = nullptr;
        _drawingSourceNode = nullptr;

        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// ==========================================
// 2. 键盘控制中枢 (计算、删除、存读档)
// ==========================================
void NodeView::keyPressEvent(QKeyEvent *event) {
    // 【空格键】：启动 DAG 计算引擎
    if (event->key() == Qt::Key_Space) {
        executeGraph();
        event->accept();
        return;
    }

    // 【S键】：序列化存档
    if (event->key() == Qt::Key_S) {
        QJsonArray nodesArray, edgesArray;
        for (QGraphicsItem *item : scene()->items()) {
            if (NodeItem *node = dynamic_cast<NodeItem*>(item)) {
                nodesArray.append(node->toJson());
            } else if (EdgeItem *edge = dynamic_cast<EdgeItem*>(item)) {
                edgesArray.append(edge->toJson());
            }
        }
        QJsonObject saveGraph;
        saveGraph["nodes"] = nodesArray;
        saveGraph["edges"] = edgesArray;
        
        QJsonDocument doc(saveGraph);
        _memoryCard = doc.toJson();
        qDebug() << "💾 [存档成功] 画布已保存入内存！";
        event->accept();
        return;
    }

    // 【L键】：反序列化读档 (依赖工厂反射)
    if (event->key() == Qt::Key_L) {
        if (_memoryCard.isEmpty()) return;

        // 毁灭前朝
        for (QGraphicsItem *item : scene()->items()) {
            if (EdgeItem *edge = dynamic_cast<EdgeItem*>(item)) edge->safelyRemove();
        }
        for (QGraphicsItem *item : scene()->items()) {
            if (NodeItem *node = dynamic_cast<NodeItem*>(item)) delete node;
        }

        // 重建世界
        QJsonDocument doc = QJsonDocument::fromJson(_memoryCard);
        QJsonObject saveGraph = doc.object();
        QMap<QString, NodeItem*> idMap;

        for (QJsonValue val : saveGraph["nodes"].toArray()) {
            QJsonObject nodeJson = val.toObject();
            if (NodeItem *node = NodeFactory::instance().createNode(nodeJson["type"].toString())) {
                node->fromJson(nodeJson);
                scene()->addItem(node);
                idMap[node->getId()] = node;
            }
        }

        for (QJsonValue val : saveGraph["edges"].toArray()) {
            QJsonObject edgeJson = val.toObject();
            if (idMap.contains(edgeJson["source_id"].toString()) && idMap.contains(edgeJson["dest_id"].toString())) {
                scene()->addItem(new EdgeItem(idMap[edgeJson["source_id"].toString()], idMap[edgeJson["dest_id"].toString()]));
            }
        }
        qDebug() << "✨ [读档成功] 世界已完美重建！";
        event->accept();
        return;
    }

    // 【Delete键】：工业级安全级联删除
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
        if (selectedItems.isEmpty()) return;

        QList<EdgeItem*> edgesToDelete;
        QList<NodeItem*> nodesToDelete;
        for (QGraphicsItem *item : selectedItems) {
            if (EdgeItem *edge = dynamic_cast<EdgeItem*>(item)) edgesToDelete.append(edge);
            else if (NodeItem *node = dynamic_cast<NodeItem*>(item)) nodesToDelete.append(node);
        }
        for (EdgeItem *edge : edgesToDelete) edge->safelyRemove();
        for (NodeItem *node : nodesToDelete) delete node;
        
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

// ==========================================
// 3. 动态右键菜单 (依赖工厂反射)
// ==========================================
void NodeView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #2b2b2b; color: #dfdfdf; border: 1px solid #555; }"
                       "QMenu::item:selected { background-color: #0078d7; }");

    QStringList nodeTypes = NodeFactory::instance().getRegisteredTypes();
    if (nodeTypes.isEmpty()) {
        menu.addAction("没有注册任何节点")->setEnabled(false);
    } else {
        for (const QString &type : nodeTypes) {
            QAction *action = menu.addAction(type);
            connect(action, &QAction::triggered, this, [this, type, event]() {
                if (NodeItem *node = NodeFactory::instance().createNode(type)) {
                    node->setPos(mapToScene(event->pos()));
                    scene()->addItem(node);
                }
            });
        }
    }
    menu.exec(event->globalPos());
}

// ==========================================
// 4. 基础视图操作：鼠标滚轮缩放画布
// ==========================================
void NodeView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        double scaleFactor = 1.15;
        if (event->angleDelta().y() < 0) scaleFactor = 1.0 / scaleFactor;
        scale(scaleFactor, scaleFactor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

// ==========================================
// 5. 核心引擎：DAG 拓扑排序计算
// ==========================================
void NodeView::executeGraph() {
    qDebug() << "🚀 --- 引擎启动：开始执行拓扑排序 ---";
    QList<NodeItem*> allNodes;
    for (QGraphicsItem *item : scene()->items()) {
        if (NodeItem *node = dynamic_cast<NodeItem*>(item)) {
            node->clearInputs();
            allNodes.append(node);
        }
    }

    QMap<NodeItem*, int> inDegree;
    QQueue<NodeItem*> queue;

    for (NodeItem *node : allNodes) {
        int degree = node->getIncomingEdges().size();
        inDegree[node] = degree;
        if (degree == 0) queue.enqueue(node);
    }

    while (!queue.isEmpty()) {
        NodeItem *current = queue.dequeue();
        current->compute();

        QVariant outData = current->getOutputValue();
        for (EdgeItem *edge : current->getOutgoingEdges()) {
            NodeItem *target = edge->getDestNode();
            target->setInputValue(outData);
            inDegree[target]--;
            if (inDegree[target] == 0) {
                queue.enqueue(target);
            }
        }
    }
    qDebug() << "🏁 --- 引擎执行完毕 ---";
}