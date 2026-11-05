#include "NaturalSortComboBox.h"

#include <QComboBox>
#include <QSignalSpy>
#include <QTest>

using namespace Qt::Literals::StringLiterals;

class TestNaturalSortComboBox : public QObject {
    Q_OBJECT

private slots:
    void constructorNoItems();
    void constructorNotEditable();

    void setStringListPopulatesItems();
    void setStringListResetsCurrentIndex();
    void setStringListEmpty();

    void stringListMatchesModel();

    void addStringAppendsAndSorts();
    void addStringDuplicateRejected();
    void addStringDoesNotChangeCurrent();

    void removeStringRemovesItem();
    void removeStringMissingReturnsFalse();

    void comboSelectionWorksOnSortedItems();

    void modelIsProxy();
};

void TestNaturalSortComboBox::constructorNoItems() {
    NaturalSortComboBox combo;
    QCOMPARE(combo.count(), 0);
}

void TestNaturalSortComboBox::constructorNotEditable() {
    NaturalSortComboBox combo;
    QVERIFY(!combo.isEditable());
}

void TestNaturalSortComboBox::setStringListPopulatesItems() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"b"_s, u"a"_s, u"c"_s });
    QCOMPARE(combo.count(), 3);
    QCOMPARE(combo.itemText(0), QString(u"a"_s));
    QCOMPARE(combo.itemText(1), QString(u"b"_s));
    QCOMPARE(combo.itemText(2), QString(u"c"_s));
}

void TestNaturalSortComboBox::setStringListResetsCurrentIndex() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"a"_s, u"b"_s });
    combo.setCurrentIndex(1);
    QCOMPARE(combo.currentIndex(), 1);

    combo.setStringList({ u"x"_s, u"y"_s });
    QCOMPARE(combo.currentIndex(), -1);
}

void TestNaturalSortComboBox::setStringListEmpty() {
    NaturalSortComboBox combo;
    combo.setStringList({});
    QCOMPARE(combo.count(), 0);
    QCOMPARE(combo.currentIndex(), -1);
}

void TestNaturalSortComboBox::stringListMatchesModel() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"c"_s, u"a"_s, u"b"_s });
    QCOMPARE(combo.sortedStringList(), QStringList({ u"a"_s, u"b"_s, u"c"_s }));
}

void TestNaturalSortComboBox::addStringAppendsAndSorts() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"a"_s, u"c"_s });
    QVERIFY(combo.addString(u"b"_s));
    QCOMPARE(combo.sortedStringList(), QStringList({ u"a"_s, u"b"_s, u"c"_s }));
}

void TestNaturalSortComboBox::addStringDuplicateRejected() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"Apple"_s });
    QVERIFY(!combo.addString(u"apple"_s));
    QCOMPARE(combo.count(), 1);
}

void TestNaturalSortComboBox::addStringDoesNotChangeCurrent() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"a"_s, u"b"_s });
    combo.setCurrentIndex(0);
    QVERIFY(combo.addString(u"c"_s));
    QCOMPARE(combo.currentIndex(), 0);
    QCOMPARE(combo.currentText(), QString(u"a"_s));
}

void TestNaturalSortComboBox::removeStringRemovesItem() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"a"_s, u"b"_s, u"c"_s });
    QVERIFY(combo.removeString(u"b"_s));
    QCOMPARE(combo.count(), 2);
    QVERIFY(!combo.sortedStringList().contains(u"b"_s));
}

void TestNaturalSortComboBox::removeStringMissingReturnsFalse() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"a"_s });
    QVERIFY(!combo.removeString(u"z"_s));
    QCOMPARE(combo.count(), 1);
}

void TestNaturalSortComboBox::comboSelectionWorksOnSortedItems() {
    NaturalSortComboBox combo;
    combo.setStringList({ u"file10"_s, u"file2"_s, u"file1"_s });
    // прокси отсортировал
    QCOMPARE(combo.itemText(0), QString(u"file1"_s));
    QCOMPARE(combo.itemText(1), QString(u"file2"_s));
    QCOMPARE(combo.itemText(2), QString(u"file10"_s));

    combo.setCurrentIndex(2);
    QCOMPARE(combo.currentText(), QString(u"file10"_s));
}

void TestNaturalSortComboBox::modelIsProxy() {
    NaturalSortComboBox combo;
    QVERIFY(qobject_cast<QSortFilterProxyModel *>(combo.model()) != nullptr);
}

QTEST_MAIN(TestNaturalSortComboBox)
#include "test_NaturalSortComboBox.moc"
