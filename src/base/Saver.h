#ifndef SAVER_H
#define SAVER_H

#include "Storage.h"

namespace mdt {
    class Saver {
    public:
        virtual ~Saver() = default;

        [[nodiscard]] virtual bool prepare(const Storage& storage) = 0;
        [[nodiscard]] virtual bool read(Storage& storage) = 0;
        [[nodiscard]] virtual bool write() = 0;
    };
} // namespace mdt

#endif // SAVER_H
