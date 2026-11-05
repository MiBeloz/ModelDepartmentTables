#include "NaturalSortListModel.h"

#include <QSignalSpy>
#include <QTest>

using namespace Qt::Literals::StringLiterals;

class TestNaturalSortListModel : public QObject {
    Q_OBJECT

private slots:
    // ---------- setStringList ----------
    void setStringListSortsNaturally();
    void setStringListEmptyList();
    void setStringListDeduplicatesCaseInsensitive();
    void setStringListPreservesFirstOccurrence();

    // ---------- stringList ----------
    void stringListReturnsSortedOrder();
    void stringListEmptyModel();

    // ---------- appendString ----------
    void appendStringAddsAndSorts();
    void appendStringRejectsDuplicateCaseInsensitive();
    void appendStringReturnsTrueOnSuccess();
    void appendStringEmitsRowsInserted();

    // ---------- removeString ----------
    void removeStringRemovesExisting();
    void removeStringCaseInsensitive();
    void removeStringReturnsFalseForMissing();
    void removeStringEmitsRowsRemoved();

    // ---------- lessThan / natural order ----------
    void naturalOrderNumbersInStrings();
    void naturalOrderLeadingZeros();

    // ---------- edge cases ----------
    void emptyStringsHandled();
    void unicodeStrings();

    // ---------- idempotency ----------
    void setStringListReplacesPrevious();
};

// ---------- setStringList ----------
void TestNaturalSortListModel::setStringListSortsNaturally() {
    NaturalSortListModel model;
    model.setStringList({ u"file10"_s, u"file2"_s, u"file1"_s });

    QCOMPARE(model.sortedStringList(), QStringList({ u"file1"_s, u"file2"_s, u"file10"_s }));
}

void TestNaturalSortListModel::setStringListEmptyList() {
    NaturalSortListModel model;
    model.setStringList({});
    QVERIFY(model.sortedStringList().isEmpty());
    QCOMPARE(model.rowCount(), 0);
}

void TestNaturalSortListModel::setStringListDeduplicatesCaseInsensitive() {
    NaturalSortListModel model;
    model.setStringList({ u"Apple"_s, u"apple"_s, u"APPLE"_s });
    QCOMPARE(model.sortedStringList(), QStringList({ u"Apple"_s }));
}

void TestNaturalSortListModel::setStringListPreservesFirstOccurrence() {
    NaturalSortListModel model;
    model.setStringList({ u"b"_s, u"B"_s, u"a"_s });
    QCOMPARE(model.sortedStringList(), QStringList({ u"a"_s, u"b"_s }));
}

// ---------- stringList ----------
void TestNaturalSortListModel::stringListReturnsSortedOrder() {
    NaturalSortListModel model;
    model.setStringList({ u"c"_s, u"a"_s, u"b"_s });
    QCOMPARE(model.sortedStringList(), QStringList({ u"a"_s, u"b"_s, u"c"_s }));
}

void TestNaturalSortListModel::stringListEmptyModel() {
    NaturalSortListModel model;
    QVERIFY(model.sortedStringList().isEmpty());
}

// ---------- appendString ----------
void TestNaturalSortListModel::appendStringAddsAndSorts() {
    NaturalSortListModel model;
    model.setStringList({ u"b"_s, u"d"_s });
    QVERIFY(model.appendString(u"c"_s));
    QCOMPARE(model.sortedStringList(), QStringList({ u"b"_s, u"c"_s, u"d"_s }));
}

void TestNaturalSortListModel::appendStringRejectsDuplicateCaseInsensitive() {
    NaturalSortListModel model;
    model.setStringList({ u"Hello"_s });
    QVERIFY(!model.appendString(u"hello"_s));
    QVERIFY(!model.appendString(u"HELLO"_s));
    QCOMPARE(model.sortedStringList(), QStringList({ u"Hello"_s }));
}

void TestNaturalSortListModel::appendStringReturnsTrueOnSuccess() {
    NaturalSortListModel model;
    QVERIFY(model.appendString(u"x"_s));
    QCOMPARE(model.rowCount(), 1);
}

void TestNaturalSortListModel::appendStringEmitsRowsInserted() {
    NaturalSortListModel model;
    QSignalSpy spy(&model, &QAbstractItemModel::rowsInserted);
    QVERIFY(model.appendString(u"x"_s));
    QCOMPARE(spy.count(), 1);
}

// ---------- removeString ----------
void TestNaturalSortListModel::removeStringRemovesExisting() {
    NaturalSortListModel model;
    model.setStringList({ u"a"_s, u"b"_s, u"c"_s });
    QVERIFY(model.removeString(u"b"_s));
    QCOMPARE(model.sortedStringList(), QStringList({ u"a"_s, u"c"_s }));
}

void TestNaturalSortListModel::removeStringCaseInsensitive() {
    NaturalSortListModel model;
    model.setStringList({ u"Alpha"_s });
    QVERIFY(model.removeString(u"alpha"_s));
    QVERIFY(model.sortedStringList().isEmpty());
}

void TestNaturalSortListModel::removeStringReturnsFalseForMissing() {
    NaturalSortListModel model;
    model.setStringList({ u"a"_s });
    QVERIFY(!model.removeString(u"zzz"_s));
}

void TestNaturalSortListModel::removeStringEmitsRowsRemoved() {
    NaturalSortListModel model;
    model.setStringList({ u"a"_s, u"b"_s });
    QSignalSpy spy(&model, &QAbstractItemModel::rowsRemoved);
    QVERIFY(model.removeString(u"a"_s));
    QCOMPARE(spy.count(), 1);
}

// ---------- natural order ----------
void TestNaturalSortListModel::naturalOrderNumbersInStrings() {
    NaturalSortListModel model;
    model.setStringList({ u"item12"_s, u"item2"_s, u"item1"_s, u"item10"_s, u"item3"_s });
    QCOMPARE(model.sortedStringList(),
             QStringList({ u"item1"_s, u"item2"_s, u"item3"_s, u"item10"_s, u"item12"_s }));
}

void TestNaturalSortListModel::naturalOrderLeadingZeros() {
    NaturalSortListModel model;
    model.setStringList({ u"v010"_s, u"v2"_s, u"v001"_s });
    // numeric mode: v001 == v1 < v2 < v010 == v10
    const QStringList sorted = model.sortedStringList();
    QCOMPARE(sorted.size(), 3);
    QCOMPARE(sorted.at(0), QString(u"v001"_s));
    QCOMPARE(sorted.at(1), QString(u"v2"_s));
    QCOMPARE(sorted.at(2), QString(u"v010"_s));
}

// ---------- edge cases ----------
void TestNaturalSortListModel::emptyStringsHandled() {
    NaturalSortListModel model;
    model.setStringList({ u""_s, u"a"_s, u""_s });
    const QStringList list = model.sortedStringList();
    QCOMPARE(list.size(), 2);
    QVERIFY(list.contains(u""_s));
    QVERIFY(list.contains(u"a"_s));
}

void TestNaturalSortListModel::unicodeStrings() {
    NaturalSortListModel model;
    model.setStringList({ u"ёж"_s, u"ель"_s, u"яблоко"_s, u"Ёлка"_s });
    const QStringList list = model.sortedStringList();
    QCOMPARE(list.size(), 4);
    QVERIFY(list.contains(u"ёж"_s));
    QVERIFY(list.contains(u"ель"_s));
    QVERIFY(list.contains(u"яблоко"_s));
    QVERIFY(list.contains(u"Ёлка"_s));
}

void TestNaturalSortListModel::setStringListReplacesPrevious() {
    NaturalSortListModel model;
    model.setStringList({ u"a"_s, u"b"_s });
    model.setStringList({ u"x"_s, u"y"_s, u"z"_s });
    QCOMPARE(model.sortedStringList(), QStringList({ u"x"_s, u"y"_s, u"z"_s }));
}

QTEST_MAIN(TestNaturalSortListModel)
#include "test_NaturalSortListModel.moc"
