#ifndef NODEFACTORY_H
#define NODEFACTORY_H

#include <QString>
#include <QMap>
#include <functional> // 极其重要：C++ 的函数指针容器
#include "NodeItem.h"

// ==========================================
// 工业级设计模式：单例工厂类
// ==========================================
class NodeFactory {
public:
    // 1. 获取全局唯一的造物主实例
    static NodeFactory& instance();

    // 2. 注册节点类型：告诉工厂“如果收到这个字符串，请用这个函数生成对象”
    void registerNode(const QString &type, std::function<NodeItem*()> creator);

    // 3. 终极反射接口：传入字符串，返回一个新的 C++ 对象！
    NodeItem* createNode(const QString &type);

    QStringList getRegisteredTypes() const;

private:
    // 私有化构造函数，强制大家只能用 instance() 获取，这就是单例模式的精髓！
    NodeFactory() = default; 

    // 核心花名册：【字符串名字 -> 生成该对象的函数】
    QMap<QString, std::function<NodeItem*()>> _registry;
};

#endif // NODEFACTORY_H