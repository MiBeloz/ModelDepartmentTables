#ifndef STORAGE_H
#define STORAGE_H

#include <QDataStream>

class Storage {
public:
    Storage() = default;
    virtual ~Storage() noexcept = default;

    virtual void serialize(QDataStream& out) const = 0;
    virtual void deserialize(QDataStream& in) = 0;
};

#endif // STORAGE_H
