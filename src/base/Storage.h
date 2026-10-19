#ifndef STORAGE_H
#define STORAGE_H

#include <QDataStream>

namespace mdt {
    class Storage {
    public:
        Storage() = default;
        virtual ~Storage() noexcept = default;

        virtual void serialize(QDataStream& out) const = 0;
        virtual void deserialize(QDataStream& in) = 0;
    };
} // namespace mdt

#endif // STORAGE_H
