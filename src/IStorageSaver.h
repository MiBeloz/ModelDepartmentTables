#ifndef ISTORAGESAVER_H
#define ISTORAGESAVER_H

#include "StorageService.h"

class IStorageSaver {
public:
    virtual ~IStorageSaver() = default;

    virtual bool save(const StorageService& storage) = 0;

    virtual bool load(StorageService& storage) = 0;

    virtual bool commit() = 0;
};

#endif // ISTORAGESAVER_H
