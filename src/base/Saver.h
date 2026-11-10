#ifndef SAVER_H
#define SAVER_H

#include "Storage.h"

namespace mdt {
    class Saver {
    public:
        Saver(Storage* storage) : m_storage(storage) {}
        virtual ~Saver() = default;

        [[nodiscard]] virtual bool prepare() = 0;
        [[nodiscard]] virtual bool read() = 0;
        [[nodiscard]] virtual bool write() = 0;

    protected:
        Storage* m_storage;
    };
} // namespace mdt

#endif // SAVER_H
