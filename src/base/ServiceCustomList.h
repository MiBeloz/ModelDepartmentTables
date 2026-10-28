#ifndef SERVICECUSTOMLIST_H
#define SERVICECUSTOMLIST_H

#include "CustomList.h"

namespace mdt {
    namespace work {
        template<typename ValueType, typename CustomListType = CustomList<ValueType>>
        class ServiceCustomList {
        public:
            ServiceCustomList();
            virtual ~ServiceCustomList() = default;

            virtual std::optional<qint32> add(const ValueType &value);
            virtual bool remove(const ValueType &value);
            virtual std::optional<qint32> findId(const ValueType &value) const;
            virtual std::optional<ValueType> findValue(qint32 id) const;
            virtual QList<ValueType> findAllValues() const;

            void clear();
            void reset();
            void commit();

            qsizetype count() const;
            qsizetype countCommitted() const;

            void serialize(QDataStream &out) const;
            void deserialize(QDataStream &in);

        protected:
            CustomListType m_list;
        };

        template<typename ValueType, typename CustomListType>
        inline ServiceCustomList<ValueType, CustomListType>::ServiceCustomList() = default;

        template<typename ValueType, typename CustomListType>
        inline std::optional<qint32> ServiceCustomList<ValueType, CustomListType>::add(
            const ValueType &value) {
            return m_list.insert(value);
        }

        template<typename ValueType, typename CustomListType>
        inline bool ServiceCustomList<ValueType, CustomListType>::remove(const ValueType &value) {
            return m_list.remove(value);
        }

        template<typename ValueType, typename CustomListType>
        inline std::optional<qint32> ServiceCustomList<ValueType, CustomListType>::findId(
            const ValueType &value) const {
            return m_list.getId(value);
        }

        template<typename ValueType, typename CustomListType>
        inline std::optional<ValueType> ServiceCustomList<ValueType, CustomListType>::findValue(
            qint32 id) const {
            return m_list.getValue(id);
        }

        template<typename ValueType, typename CustomListType>
        inline QList<ValueType> ServiceCustomList<ValueType, CustomListType>::findAllValues() const {
            return m_list.getAllValues();
        }

        template<typename ValueType, typename CustomListType>
        inline void ServiceCustomList<ValueType, CustomListType>::clear() {
            return m_list.clear();
        }

        template<typename ValueType, typename CustomListType>
        inline void ServiceCustomList<ValueType, CustomListType>::reset() {
            m_list.reset();
        }

        template<typename ValueType, typename CustomListType>
        inline void ServiceCustomList<ValueType, CustomListType>::commit() {
            m_list.commit();
        }

        template<typename ValueType, typename CustomListType>
        inline qsizetype ServiceCustomList<ValueType, CustomListType>::count() const {
            return m_list.size();
        }

        template<typename ValueType, typename CustomListType>
        inline qsizetype ServiceCustomList<ValueType, CustomListType>::countCommitted() const {
            return m_list.sizeCommitted();
        }

        template<typename ValueType, typename CustomListType>
        inline void ServiceCustomList<ValueType, CustomListType>::serialize(QDataStream &out) const {
            m_list.serialize(out);
        }

        template<typename ValueType, typename CustomListType>
        inline void ServiceCustomList<ValueType, CustomListType>::deserialize(QDataStream &in) {
            m_list.deserialize(in);
        }
    } // namespace work
} // namespace mdt

#endif // SERVICECUSTOMLIST_H
