#ifndef MOMENTOLIST_H
#define MOMENTOLIST_H

template<typename T>
class MementoList final {
public:
    T getState() const {
        return m_state;
    }

private:
    explicit MementoList(const T& state) : m_state(state) { }

    T m_state;

    friend T;
};

#endif // MOMENTOLIST_H
