#include <QApplication>
#include <QGraphicsScene>
#include "NodeView.h"
#include "NumberNode.h"
#include "DisplayNode.h"
#include "AddNode.h" // 新增：引入加法节点
#include "EdgeItem.h"
#include "NodeFactory.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    NodeFactory::instance().registerNode("NumberNode", []() { return new NumberNode(); });
    NodeFactory::instance().registerNode("DisplayNode", []() { return new DisplayNode(); });
    NodeFactory::instance().registerNode("AddNode", []() { return new AddNode(); });

    QGraphicsScene scene;
    scene.setSceneRect(-50000, -50000, 100000, 100000);

    // 1. 创建源头 A (设为 10) 和源头 B (设为 20)
    // 为了能传入不同数字，你可以给 NumberNode 加个带参数的构造函数，或者简单点直接在内部写死
    NumberNode *numA = new NumberNode(); // 假设内部还是 42
    numA->setPos(-200, -100);
    scene.addItem(numA);

    NumberNode *numB = new NumberNode(); 
    numB->setPos(-200, 100);
    scene.addItem(numB);

    // 2. 创建加法节点
    AddNode *addNode = new AddNode();
    addNode->setPos(50, 0);
    scene.addItem(addNode);

    // 3. 创建显示节点
    DisplayNode *dispNode = new DisplayNode();
    dispNode->setPos(300, 0);
    scene.addItem(dispNode);

    // 4. 将它们用线缝合起来 (Y字形网络)
    scene.addItem(new EdgeItem(numA, addNode));   // NumA 连 Add
    scene.addItem(new EdgeItem(numB, addNode));   // NumB 连 Add
    scene.addItem(new EdgeItem(addNode, dispNode)); // Add 连 Display

    NodeView view;
    view.setScene(&scene);
    view.setWindowTitle("NodeFlow - DAG Engine (Add)");
    view.resize(1024, 768);
    view.show();

    return a.exec();
}