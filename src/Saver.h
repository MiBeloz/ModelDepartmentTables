#ifndef SAVER_H
#define SAVER_H

template<typename T>
class Saver {
public:
    virtual ~Saver() = default;

    [[nodiscard]] virtual bool prepare(const T& storage) = 0;
    [[nodiscard]] virtual bool read(T& storage) = 0;
    [[nodiscard]] virtual bool write() = 0;
};

#endif // SAVER_H
