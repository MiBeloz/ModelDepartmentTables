#ifndef ISAVER_H
#define ISAVER_H

template<typename T>
class ISaver {
public:
    virtual ~ISaver() = default;

    virtual bool save(const T& storage) = 0;

    virtual bool load(T& storage) = 0;

    virtual bool commit() = 0;
};

#endif // ISAVER_H
