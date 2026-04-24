#include "NodeFactory.h"
#include <QDebug>

// 实现单例模式：利用 C++11 的局部静态变量特性，这是最安全、最优雅的写法
NodeFactory& NodeFactory::instance() {
    static NodeFactory factory;
    return factory;
}

void NodeFactory::registerNode(const QString &type, std::function<NodeItem*()> creator) {
    _registry[type] = creator;
    qDebug() << "🏭 [NodeFactory] 成功注册节点类型:" << type;
}

NodeItem* NodeFactory::createNode(const QString &type) {
    // 查字典，看看有没有这个类型
    if (_registry.contains(type)) {
        // 调用里面存的函数，把对象造出来并返回！
        return _registry[type](); 
    }
    
    qDebug() << "❌ [NodeFactory] 灾难：未知的节点类型" << type << "，你是不是忘记注册了？";
    return nullptr;
}

// 只需要一行代码，把字典（QMap）里所有的 Key（名字）提取出来变成列表
QStringList NodeFactory::getRegisteredTypes() const {
    return _registry.keys();
}