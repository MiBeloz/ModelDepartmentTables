#ifndef IREPOSITORYSAVER_H
#define IREPOSITORYSAVER_H

#include "Repository.h"

class IRepositorySaver {
public:
    virtual ~IRepositorySaver() = default;

    virtual void save(const RepositoryService& repository) = 0;
};

#endif // IREPOSITORYSAVER_H
