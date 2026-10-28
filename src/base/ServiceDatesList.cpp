#include "ServiceDatesList.h"

namespace mdt {
    namespace work {
        ServiceDatesList::ServiceDatesList() : ServiceCustomList() { }

        std::optional<qint32> ServiceDatesList::add(const QString &date) {
            return m_list.insert(date);
        }

        bool ServiceDatesList::remove(const QString &date) {
            return m_list.remove(date);
        }

        std::optional<qint32> ServiceDatesList::findId(const QString &date) const {
            return m_list.getId(date);
        }

        std::optional<QString> ServiceDatesList::findStrValue(qint32 id) const {
            return m_list.getStrValue(id);
        }

        QList<QString> ServiceDatesList::findAllStrValues() const {
            return m_list.getAllStrValues();
        }

        DatesList::ErrorType ServiceDatesList::lastError() const {
            return m_list.lastError();
        }
    } // namespace work
} // namespace mdt
