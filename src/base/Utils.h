#ifndef UTILS_H
#define UTILS_H

#include <QString>

static bool naturalLess(const QString &a, const QString &b) {
    int i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        QChar ca = a[i];
        QChar cb = b[j];

        if (ca.isDigit() && cb.isDigit()) {
            // находим границы числовых блоков
            int si = i, sj = j;
            while (i < a.size() && a[i].isDigit()) {
                ++i;
            }
            while (j < b.size() && b[j].isDigit()) {
                ++j;
            }

            // сравниваем как числа (без ведущих нулей)
            QString na = a.mid(si, i - si);
            QString nb = b.mid(sj, j - sj);
            // убираем ведущие нули для числового сравнения
            int za = 0, zb = 0;
            while (za < na.size() - 1 && na[za] == u'0') {
                ++za;
            }
            while (zb < nb.size() - 1 && nb[zb] == u'0') {
                ++zb;
            }
            QString ta = na.mid(za), tb = nb.mid(zb);

            if (ta.size() != tb.size()) {
                return ta.size() < tb.size();
            }
            int c = QString::compare(ta, tb);
            if (c != 0) {
                return c < 0;
            }
        } else {
            if (ca != cb) {
                return ca < cb;
            }
            ++i;
            ++j;
        }
    }
    return (a.size() - i) < (b.size() - j);
}

#endif // UTILS_H
