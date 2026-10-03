#ifndef RECORDLINK_H
#define RECORDLINK_H

#include <QObject>
#include <QReadWriteLock>
#include <QSet>

class RecordLink {
public:
    class Adder {
    public:
        explicit Adder(RecordLink& link);

        Adder& idsExecutors(const QSet<qint32>& ids);
        Adder& idsAuthors(const QSet<qint32>& ids);
        Adder& idsCastingMaterials(const QSet<qint32>& ids);
        Adder& idsModelMaterials(const QSet<qint32>& ids);
        Adder& idsMachines(const QSet<qint32>& ids);
        Adder& idsNotes(const QSet<qint32>& ids);

    private:
        RecordLink& m_link;
    };

    class Remover {
    public:
        explicit Remover(RecordLink& link);

        Remover& idsExecutors(const QSet<qint32>& ids);
        Remover& idsAuthors(const QSet<qint32>& ids);
        Remover& idsCastingMaterials(const QSet<qint32>& ids);
        Remover& idsModelMaterials(const QSet<qint32>& ids);
        Remover& idsMachines(const QSet<qint32>& ids);
        Remover& idsNotes(const QSet<qint32>& ids);

    private:
        RecordLink& m_link;
    };

    class Getter {
    public:
        explicit Getter(const RecordLink& link);

        qint32 idDate() const;
        qint32 idDrawing() const;
        qint32 idAmount() const;
        QSet<qint32> idsExecutors() const;
        QSet<qint32> idsAuthors() const;
        QSet<qint32> idsCastingMaterials() const;
        QSet<qint32> idsModelMaterials() const;
        QSet<qint32> idsMachines() const;
        QSet<qint32> idsNotes() const;

    private:
        const RecordLink& m_link;
    };

    class Updater {
    public:
        explicit Updater(RecordLink& link);

        Updater& idDate(qint32 newIdDate);
        Updater& idDrawing(qint32 newIdDrawing);
        Updater& idAmount(qint32 newIdAmount);
        Updater& idsExecutors(const QSet<qint32>& newIdsExecutors);
        Updater& idsAuthors(const QSet<qint32>& newIdsAuthors);
        Updater& idsCastingMaterials(const QSet<qint32>& newIdsCastingMaterials);
        Updater& idsModelMaterials(const QSet<qint32>& newIdsModelMaterials);
        Updater& idsMachines(const QSet<qint32>& newIdsMachines);
        Updater& idsNotes(const QSet<qint32>& newIdsNotes);

    private:
        RecordLink& m_link;
    };

    friend class Adder;
    friend class Remover;
    friend class Getter;
    friend class Updater;

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

    [[nodiscard]] Adder add();
    [[nodiscard]] Remover remove();
    [[nodiscard]] Getter get() const;
    [[nodiscard]] Updater update();

    void serialize(QDataStream& out) const;
    void deserialize(QDataStream& in);

    size_t hash(size_t seed = 0) const;

    static const RecordLink Null;

private:
    qint32 m_idDate = 0;
    qint32 m_idDrawing = 0;
    qint32 m_idAmount = 0;
    QSet<qint32> m_idsExecutors;
    QSet<qint32> m_idsAuthors;
    QSet<qint32> m_idsCastingMaterials;
    QSet<qint32> m_idsModelMaterials;
    QSet<qint32> m_idsMachines;
    QSet<qint32> m_idsNotes;
    mutable QReadWriteLock m_lock;

    RecordLink();

    void setIdDate(qint32 idDate);
    void setIdDrawing(qint32 idDrawing);
    void setIdAmount(qint32 idAmount);

    void addExecutors(const QSet<qint32>& idsExecutors);
    void addAuthors(const QSet<qint32>& idsAuthors);
    void addCastingMaterials(const QSet<qint32>& idsCastingMaterials);
    void addModelMaterials(const QSet<qint32>& idsModelMaterials);
    void addMachines(const QSet<qint32>& idsMachines);
    void addNotes(const QSet<qint32>& idsNotes);

    void removeExecutors(const QSet<qint32>& idsExecutors);
    void removeAuthors(const QSet<qint32>& idsAuthors);
    void removeCastingMaterials(const QSet<qint32>& idsCastingMaterials);
    void removeModelMaterials(const QSet<qint32>& idsModelMaterials);
    void removeMachines(const QSet<qint32>& idsMachines);
    void removeNotes(const QSet<qint32>& idsNotes);

    void updateExecutors(const QSet<qint32>& idsExecutors);
    void updateAuthors(const QSet<qint32>& idsAuthors);
    void updateCastingMaterials(const QSet<qint32>& idsCastingMaterials);
    void updateModelMaterials(const QSet<qint32>& idsModelMaterials);
    void updateMachines(const QSet<qint32>& idsMachines);
    void updateNotes(const QSet<qint32>& idsNotes);

    qint32 getIdDate() const;
    qint32 getIdDrawing() const;
    qint32 getIdAmount() const;

    QSet<qint32> getIdsExecutors() const;
    QSet<qint32> getIdsAuthors() const;
    QSet<qint32> getIdsCastingMaterials() const;
    QSet<qint32> getIdsModelMaterials() const;
    QSet<qint32> getIdsMachines() const;
    QSet<qint32> getIdsNotes() const;

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
