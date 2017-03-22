#ifndef MEMORYITEM_H
#define MEMORYITEM_H
#include <QString>

class memoryItem
{
public:
    memoryItem();
    memoryItem(QString _name, int _memoryOccupying, bool _isFree);
    QString Display();

    memoryItem *next;

    QString name;
    void setMemoryOccupying(int _newVal);
    int memoryOccupying;
    bool isFree;

    bool operator== (const memoryItem &m);

    int getUID();
private:
    int UID;
    static int newUID;
};

#endif // MEMORYITEM_H
