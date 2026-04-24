#ifndef DISPLAYNODE_H
#define DISPLAYNODE_H

#include "NodeItem.h"

class DisplayNode : public NodeItem {
public:
    DisplayNode();
    
    void compute() override;
    QJsonObject toJson() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // DISPLAYNODE_H