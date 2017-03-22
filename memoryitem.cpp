#include "memoryitem.h"

memoryItem::memoryItem()
{
    this->UID = newUID++;
}

memoryItem::memoryItem(QString _name, int _memoryOccupying, bool _isFree)
{
    this->name = _name;
    this->memoryOccupying = _memoryOccupying;
    this->isFree = _isFree;
    this->UID = newUID++;
}

void memoryItem::setMemoryOccupying(int _newVal)
{
    this->memoryOccupying = _newVal;
}

QString memoryItem::Display()
{
    return this->name + "\t" + QString::number(this->memoryOccupying);
}

int memoryItem::getUID()
{
    return this->UID;
}

bool memoryItem::operator== (const memoryItem &m)
{
    return(m.UID == this->UID);
}

int memoryItem::newUID = 0;
