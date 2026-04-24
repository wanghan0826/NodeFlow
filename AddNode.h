#ifndef ADDNODE_H
#define ADDNODE_H

#include "NodeItem.h"

class AddNode : public NodeItem {
public:
    AddNode();
    void compute() override;
    QJsonObject toJson() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // ADDNODE_H