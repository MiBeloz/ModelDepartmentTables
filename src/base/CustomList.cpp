#include "CustomList.h"

#include "Exceptions.h"

namespace mdt {
    namespace work {
        bool CustomList::operator==(const CustomList& other) const {
            if (this == &other) {
                return true;
            }

            return m_list == other.m_list && m_listTmp == other.m_listTmp && m_id == other.m_id &&
                   m_idTmp == other.m_idTmp && m_emptyId == other.m_emptyId &&
                   m_emptyIdTmp == other.m_emptyIdTmp && m_commit == other.m_commit;
        }

        bool CustomList::operator!=(const CustomList& other) const {
            return !(*this == other);
        }

        void CustomList::swap(CustomList& other) {
            if (this == &other) {
                return;
            }

            m_list.swap(other.m_list);
            m_listTmp.swap(other.m_listTmp);
            std::swap(m_id, other.m_id);
            std::swap(m_idTmp, other.m_idTmp);
            m_emptyId.swap(other.m_emptyId);
            m_emptyIdTmp.swap(other.m_emptyIdTmp);
            std::swap(m_commit, other.m_commit);
        }

        std::optional<qint32> CustomList::insert(const QVariant& data) {
            for (const auto& [k, v] : m_listTmp.asKeyValueRange()) {
                if (v == data) {
                    return k;
                }
            }

            qint32 id = -1;
            if (m_emptyIdTmp.isEmpty()) {
                ++m_idTmp;
                id = m_idTmp;
            } else {
                id = m_emptyIdTmp.pop();
            }
            m_listTmp.insert(id, data);

            m_commit = false;
            return id;
        }

        bool CustomList::remove(const QVariant& data) {
            for (auto it = m_listTmp.begin(); it != m_listTmp.end(); ++it) {
                if (it.value() == data) {
                    m_emptyIdTmp.push(it.key());
                    m_listTmp.erase(it);

                    m_commit = false;
                    return true;
                }
            }
            return false;
        }

        std::optional<qint32> CustomList::getId(const QVariant& data) const {
            if (auto id = m_listTmp.key(data, -1); id != -1) {
                return id;
            }
            return std::nullopt;
        }

        std::optional<QVariant> CustomList::getValue(qint32 id) const {
            if (auto it = m_listTmp.find(id); it != m_listTmp.end()) {
                return *it;
            }
            return std::nullopt;
        }

        QVariantList CustomList::getAllValues() const {
            return m_listTmp.values();
        }

        void CustomList::clear() {
            m_listTmp.clear();
            m_emptyIdTmp.clear();
            m_idTmp = 0;
            m_commit = false;
        }

        void CustomList::reset() {
            m_listTmp = m_list;
            m_idTmp = m_id;
            m_emptyIdTmp = m_emptyId;
            m_commit = true;
        }

        void CustomList::commit() {
            m_list = m_listTmp;
            m_id = m_idTmp;
            m_emptyId = m_emptyIdTmp;
            m_commit = true;
        }

        qsizetype CustomList::size() const {
            return m_listTmp.size();
        }

        qsizetype CustomList::sizeCommitted() const {
            return m_list.size();
        }

        void CustomList::serialize(QDataStream& out) const {
            out << out.version();

            out << static_cast<qint32>(m_list.size());
            for (auto [id, value] : std::as_const(m_list).asKeyValueRange()) {
                out << id << value;
            }
            out << m_id;
            out << static_cast<qint32>(m_emptyId.size());
            for (auto& id : std::as_const(m_emptyId)) {
                out << id;
            }

            out << static_cast<qint32>(m_listTmp.size());
            for (auto [id, value] : std::as_const(m_listTmp).asKeyValueRange()) {
                out << id << value;
            }
            out << m_idTmp;
            out << static_cast<qint32>(m_emptyIdTmp.size());
            for (auto& id : std::as_const(m_emptyIdTmp)) {
                out << id;
            }

            out << m_commit;

            if (out.status() != QDataStream::Ok) {
                throwStreamError(out.status());
            }
        }

        void CustomList::deserialize(QDataStream& in) {
            deserializeVersion(in);
            QHash<qint32, QVariant> list = deserializeList(in);
            qint32 id = deserializeId(in);
            QStack<qint32> emptyId = deserializeEmptyId(in);
            QHash<qint32, QVariant> listTmp = deserializeList(in);
            qint32 idTmp = deserializeId(in);
            QStack<qint32> emptyIdTmp = deserializeEmptyId(in);
            bool commit = deserializeCommit(in);

            m_list = list;
            m_id = id;
            m_emptyId = emptyId;
            m_listTmp = listTmp;
            m_idTmp = idTmp;
            m_emptyIdTmp = emptyIdTmp;
            m_commit = commit;
        }

        void CustomList::throwStreamError(QDataStream::Status status) const {
            throw RuntimeError(
                QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
        }

        void CustomList::deserializeVersion(QDataStream& in) const {
            int version {};
            in >> version;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (version != in.version()) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throw RuntimeError(
                    QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                        .arg(QString::number(in.version()), QString::number(version)));
            }
        }

        QHash<qint32, QVariant> CustomList::deserializeList(QDataStream& in) const {
            qint32 size;
            in >> size;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (size < 0) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throwStreamError(in.status());
            }

            QHash<qint32, QVariant> list;
            for (qint32 i = 0; i < size; ++i) {
                qint32 key;
                QVariant value;

                in >> key >> value;
                if (in.status() != QDataStream::Ok) {
                    throwStreamError(in.status());
                }
                list.insert(key, value);
            }
            return list;
        }

        qint32 CustomList::deserializeId(QDataStream& in) const {
            qint32 id {};
            in >> id;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (id < 0) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throwStreamError(in.status());
            }
            return id;
        }

        QStack<qint32> CustomList::deserializeEmptyId(QDataStream& in) const {
            qint32 size;
            in >> size;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (size < 0) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throwStreamError(in.status());
            }

            QStack<qint32> ids;
            for (qint32 i = 0; i < size; ++i) {
                qint32 id;
                in >> id;
                if (in.status() != QDataStream::Ok) {
                    throwStreamError(in.status());
                }
                ids.push(id);
            }
            return ids;
        }

        bool CustomList::deserializeCommit(QDataStream& in) const {
            bool commit {};
            in >> commit;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            return commit;
        }
    } // namespace work
} // namespace mdt
