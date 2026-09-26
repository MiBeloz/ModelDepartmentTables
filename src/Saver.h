#ifndef SAVER_H
#define SAVER_H

template<typename T>
class Saver {
public:
    virtual ~Saver() = default;

    virtual bool write(const T& storage) = 0;
    virtual bool read(T& storage) = 0;
    virtual bool save() = 0;
};

#endif // SAVER_H
