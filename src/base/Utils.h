#ifndef UTILS_H
#define UTILS_H

#include <QString>

namespace mdt {
    namespace utils {
        bool naturalLess(const QString &a, const QString &b) {
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
    } // namespace utils
} // namespace mdt

#endif // UTILS_H
