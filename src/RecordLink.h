#ifndef RECORDLINK_H
#define RECORDLINK_H

#include <QObject>
#include <QReadWriteLock>
#include <QSet>

class RecordLink {
public:
    class Setter {
    public:
        explicit Setter(RecordLink& link);

        Setter& idDate(qint32 id);
        Setter& idDrawing(qint32 id);
        Setter& idAmount(qint32 id);

    private:
        RecordLink& m_link;
    };

    class Adder {
    public:
        explicit Adder(RecordLink& link);

        Adder& idExecutors(const QSet<qint32>& ids);
        Adder& idAuthors(const QSet<qint32>& ids);
        Adder& idCastingMaterials(const QSet<qint32>& ids);
        Adder& idModelMaterials(const QSet<qint32>& ids);
        Adder& idMachines(const QSet<qint32>& ids);
        Adder& idNotes(const QSet<qint32>& ids);

    private:
        RecordLink& m_link;
    };

    class Remover {
    public:
        explicit Remover(RecordLink& link);

        Remover& removeExecutors(const QSet<qint32>& ids);
        Remover& removeAuthors(const QSet<qint32>& ids);
        Remover& removeCastingMaterials(const QSet<qint32>& ids);
        Remover& removeModelMaterials(const QSet<qint32>& ids);
        Remover& removeMachines(const QSet<qint32>& ids);
        Remover& removeNotes(const QSet<qint32>& ids);

    private:
        RecordLink& m_link;
    };

    class Getter {
    public:
        explicit Getter(const RecordLink& link);

        qint32 idDate() const;
        qint32 idDrawing() const;
        qint32 idAmount() const;
        QSet<qint32> idExecutors() const;
        QSet<qint32> idAuthors() const;
        QSet<qint32> idCastingMaterials() const;
        QSet<qint32> idModelMaterials() const;
        QSet<qint32> idMachines() const;
        QSet<qint32> idNotes() const;

    private:
        const RecordLink& m_link;
    };

    friend class Setter;
    friend class Adder;
    friend class Remover;
    friend class Getter;

    explicit RecordLink(qint32 idDate,
                        qint32 idDrawing,
                        qint32 idAmount,
                        const QSet<qint32>& idExecutors,
                        const QSet<qint32>& idAuthors,
                        const QSet<qint32>& idCastingMaterials,
                        const QSet<qint32>& idModelMaterials,
                        const QSet<qint32>& idMachines,
                        const QSet<qint32>& idNotes);

    RecordLink(const RecordLink& other);
    RecordLink(RecordLink&& other) noexcept;
    RecordLink& operator =(const RecordLink& other);
    RecordLink& operator =(RecordLink&& other) noexcept;

    bool operator ==(const RecordLink& other) const;
    bool operator !=(const RecordLink& other) const;

    void swap(RecordLink& other);

    [[nodiscard]] Setter set();
    [[nodiscard]] Adder add();
    [[nodiscard]] Remover remove();
    [[nodiscard]] Getter get() const;

    void serialize(QDataStream& out) const;
    void deserialize(QDataStream& in);

    size_t hash(size_t seed = 0) const;

    static const RecordLink Null;

private:
    qint32 m_idDate = 0;
    qint32 m_idDrawing = 0;
    qint32 m_idAmount = 0;
    QSet<qint32> m_idExecutors;
    QSet<qint32> m_idAuthors;
    QSet<qint32> m_idCastingMaterials;
    QSet<qint32> m_idModelMaterials;
    QSet<qint32> m_idMachines;
    QSet<qint32> m_idNotes;
    mutable QReadWriteLock m_lock;

    RecordLink();

    void setIdDate(qint32 idDate);
    void setIdDrawing(qint32 idDrawing);
    void setIdAmount(qint32 idAmount);

    void addExecutors(const QSet<qint32>& idExecutors);
    void addAuthors(const QSet<qint32>& idAuthors);
    void addCastingMaterials(const QSet<qint32>& idCastingMaterials);
    void addModelMaterials(const QSet<qint32>& idModelMaterials);
    void addMachines(const QSet<qint32>& idMachines);
    void addNotes(const QSet<qint32>& idNotes);

    void removeExecutors(const QSet<qint32>& idExecutors);
    void removeAuthors(const QSet<qint32>& idAuthors);
    void removeCastingMaterials(const QSet<qint32>& idCastingMaterials);
    void removeModelMaterials(const QSet<qint32>& idModelMaterials);
    void removeMachines(const QSet<qint32>& idMachines);
    void removeNotes(const QSet<qint32>& idNotes);

    qint32 getIdDate() const;
    qint32 getIdDrawing() const;
    qint32 getIdAmount() const;

    QSet<qint32> getIdExecutors() const;
    QSet<qint32> getIdAuthors() const;
    QSet<qint32> getIdCastingMaterials() const;
    QSet<qint32> getIdModelMaterials() const;
    QSet<qint32> getIdMachines() const;
    QSet<qint32> getIdNotes() const;

    void throwStreamError(QDataStream::Status status) const;

    void deserializeVersion(QDataStream& in) const;
    qint32 deserializeId(QDataStream& in) const;
    QSet<qint32> deserializeIds(QDataStream& in) const;
};

inline const RecordLink RecordLink::Null { };

inline size_t qHash(const RecordLink& recordLink, size_t seed = 0) {
    return recordLink.hash(seed);
}

#endif // RECORDLINK_H
