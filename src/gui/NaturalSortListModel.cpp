#include "NaturalSortListModel.h"

NaturalSortListModel::NaturalSortListModel(const QStringList &strings, QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_stringListModel(new QStringListModel(strings, this)) {
    setSourceModel(m_stringListModel);
    sort(0, Qt::AscendingOrder);
}

QStringList NaturalSortListModel::stringList() const {
    return m_stringListModel->stringList();
}

void NaturalSortListModel::setStringList(const QStringList &strings) {
    m_stringListModel->setStringList(strings);
}

bool NaturalSortListModel::lessThan(const QModelIndex &source_left,
                                    const QModelIndex &source_right) const {
    QVariant leftData = sourceModel()->data(source_left, sortRole());
    QVariant rightData = sourceModel()->data(source_right, sortRole());

    if (leftData.userType() == QMetaType::QString) {
        return naturalLess(leftData.toString(), rightData.toString());
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool NaturalSortListModel::naturalLess(const QString &a, const QString &b) {
    int i = 0, j = 0;
    const int na_len = a.size(), nb_len = b.size();
    while (i < na_len && j < nb_len) {
        const QChar ca = a[i], cb = b[j];
        if (ca.isDigit() && cb.isDigit()) {
            const int si = i, sj = j;
            while (i < na_len && a[i].isDigit()) {
                ++i;
            }
            while (j < nb_len && b[j].isDigit()) {
                ++j;
            }

            int za = si, zb = sj;
            while (za < i - 1 && a[za] == u'0') {
                ++za;
            }
            while (zb < j - 1 && b[zb] == u'0') {
                ++zb;
            }

            const int la = i - za, lb = j - zb;
            if (la != lb) {
                return la < lb;
            }
            for (int k = 0; k < la; ++k) {
                if (a[za + k] != b[zb + k]) {
                    return a[za + k] < b[zb + k];
                }
            }
        } else {
            if (ca != cb) {
                return ca < cb;
            }
            ++i;
            ++j;
        }
    }
    return (na_len - i) < (nb_len - j);
}
