#ifndef SAVER_H
#define SAVER_H

template<typename T>
class Saver {
public:
    virtual ~Saver() = default;

    virtual bool save(const T& storage) = 0;

    virtual bool load(T& storage) = 0;

    virtual bool commit() = 0;
};

#endif // SAVER_H
