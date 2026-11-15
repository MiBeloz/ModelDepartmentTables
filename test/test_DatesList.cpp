#include "DatesList.h"
#include "Exceptions.h"

#include <QObject>
#include <QTest>

using namespace Qt::Literals::StringLiterals;

using mdt::RuntimeError;
using mdt::work::DatesList;

class TestDatesList : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // ---------- insert ----------
    void insertByStringValid();
    void insertByStringInvalidFormat();
    void insertByStringSameValueReturnsSameId();
    void insertByExcelFormatValid();
    void insertByExcelFormatInvalid();

    // ---------- remove ----------
    void removeByStringValid();
    void removeByStringNotFound();
    void removeByStringInvalidFormat();
    void removeByExcelFormatValid();
    void removeByExcelFormatNotFound();
    void removeByExcelFormatInvalid();

    // ---------- getId ----------
    void getIdByStringValid();
    void getIdByStringNotFound();
    void getIdByStringInvalidFormat();
    void getIdByExcelFormatValid();
    void getIdByExcelFormatNotFound();

    // ---------- getValue / getStrValue ----------
    void getValueValid();
    void getValueInvalidId();
    void getStrValueValid();
    void getStrValueInvalidId();

    // ---------- static helpers ----------
    void strToDateValid();
    void strToDateInvalid();
    void dateToStrValid();
    void dateToStrInvalid();
    void checkDateValid();
    void checkDateInvalid();
    void checkDateStrValid();
    void checkDateStrInvalid();

    // ---------- draft / committed semantics ----------
    void insertLivesInDraftUntilCommit();
    void readersSeeDraft();
    void removeLivesInDraftUntilCommit();
    void resetRollsBackDraft();
    void clearAffectsOnlyDraft();
    void commitMakesDraftVisibleAsCommitted();

private:
    DatesList m_list;
};

// =============== Implementation ===============

namespace {
    const QString& kDateStr1() {
        static const QString str = u"01.01.2000"_s;
        return str;
    }
    const QString& kDateStr2() {
        static const QString str = u"15.06.2020"_s;
        return str;
    }
    const QString& kDateStr3() {
        static const QString str = u"31.12.1999"_s;
        return str;
    }
} // namespace

void TestDatesList::init() {
    m_list = DatesList();
}

void TestDatesList::cleanup() {
    m_list.clear();
    m_list.commit();
}

// ---------- insert ----------

void TestDatesList::insertByStringValid() {
    auto id = m_list.insert(kDateStr1());
    QVERIFY(id.has_value());

    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 0);

    m_list.commit();
    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 1);

    auto got = m_list.getId(kDateStr1());
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
}

void TestDatesList::insertByStringInvalidFormat() {
    auto id = m_list.insert(u"not-a-date"_s);
    QVERIFY(!id.has_value());
    QCOMPARE(m_list.size(), 0);
    QCOMPARE(m_list.sizeCommitted(), 0);
}

void TestDatesList::insertByStringSameValueReturnsSameId() {
    auto id1 = m_list.insert(kDateStr1());
    auto id2 = m_list.insert(kDateStr1());
    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());
    QCOMPARE(id1.value(), id2.value());

    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 0);
}

void TestDatesList::insertByExcelFormatValid() {
    auto excel = DatesList::strToExelDate(kDateStr1());
    QVERIFY(excel.has_value());

    auto id = m_list.insert(excel.value());
    QVERIFY(id.has_value());

    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 0);
}

void TestDatesList::insertByExcelFormatInvalid() {
    auto id = m_list.insert(qint32(-100000));
    QVERIFY(!id.has_value());
}

// ---------- remove ----------

void TestDatesList::removeByStringValid() {
    m_list.insert(kDateStr1());
    m_list.commit();
    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 1);

    QVERIFY(m_list.remove(kDateStr1()));

    QCOMPARE(m_list.size(), 0);
    QCOMPARE(m_list.sizeCommitted(), 1);

    m_list.commit();
    QCOMPARE(m_list.size(), 0);
    QCOMPARE(m_list.sizeCommitted(), 0);
}

void TestDatesList::removeByStringNotFound() {
    QVERIFY(!m_list.remove(kDateStr1()));
}

void TestDatesList::removeByStringInvalidFormat() {
    QVERIFY(!m_list.remove(u"bad-date"_s));
}

void TestDatesList::removeByExcelFormatValid() {
    auto excel = DatesList::strToExelDate(kDateStr1());
    QVERIFY(excel.has_value());
    m_list.insert(excel.value());
    m_list.commit();

    QVERIFY(m_list.remove(excel.value()));

    m_list.commit();
    QCOMPARE(m_list.size(), 0);
    QCOMPARE(m_list.sizeCommitted(), 0);
}

void TestDatesList::removeByExcelFormatNotFound() {
    QVERIFY(!m_list.remove(qint32(12345)));
}

void TestDatesList::removeByExcelFormatInvalid() {
    QVERIFY(!m_list.remove(qint32(-1)));
}

// ---------- getId ----------

void TestDatesList::getIdByStringValid() {
    auto id = m_list.insert(kDateStr1());
    QVERIFY(id.has_value());
    m_list.commit();

    auto got = m_list.getId(kDateStr1());
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
}

void TestDatesList::getIdByStringNotFound() {
    auto got = m_list.getId(kDateStr1());
    QVERIFY(!got.has_value());
}

void TestDatesList::getIdByStringInvalidFormat() {
    auto got = m_list.getId(u"bad-date"_s);
    QVERIFY(!got.has_value());
}

void TestDatesList::getIdByExcelFormatValid() {
    auto excel = DatesList::strToExelDate(kDateStr2());
    QVERIFY(excel.has_value());

    auto id = m_list.insert(excel.value());
    QVERIFY(id.has_value());
    m_list.commit();

    auto got = m_list.getId(excel.value());
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
}

void TestDatesList::getIdByExcelFormatNotFound() {
    auto got = m_list.getId(qint32(999999));
    QVERIFY(!got.has_value());
}

// ---------- getValue / getStrValue ----------

void TestDatesList::getValueValid() {
    auto id = m_list.insert(kDateStr1());
    QVERIFY(id.has_value());
    m_list.commit();

    auto value = m_list.getExelDate(id.value());
    QVERIFY(value.has_value());
    QCOMPARE(value.value(), DatesList::strToExelDate(kDateStr1()).value());
}

void TestDatesList::getValueInvalidId() {
    auto value = m_list.getExelDate(qint32(42));
    QVERIFY(!value.has_value());
}

void TestDatesList::getStrValueValid() {
    auto id = m_list.insert(kDateStr1());
    QVERIFY(id.has_value());
    m_list.commit();

    auto str = m_list.getStrDate(id.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr1());
}

void TestDatesList::getStrValueInvalidId() {
    auto str = m_list.getStrDate(qint32(42));
    QVERIFY(!str.has_value());
}

// ---------- static helpers ----------

void TestDatesList::strToDateValid() {
    auto excel = DatesList::strToExelDate(kDateStr1());
    QVERIFY(excel.has_value());

    QDate expected = QDate::fromString(kDateStr1());
    QDate base = QDate::fromJulianDay(mdt::work::EXCEL_EPOCH_JULIAN_DAY);
    QCOMPARE(excel.value(), qint32(expected.toJulianDay() - base.toJulianDay()));
}

void TestDatesList::strToDateInvalid() {
    QVERIFY(!DatesList::strToExelDate(u"bad-date"_s).has_value());
    QVERIFY(!DatesList::strToExelDate(u"2020-13-45"_s).has_value());
}

void TestDatesList::dateToStrValid() {
    auto excel = DatesList::strToExelDate(kDateStr2());
    QVERIFY(excel.has_value());

    auto str = DatesList::exelDateToStr(excel.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr2());
}

void TestDatesList::dateToStrInvalid() {
    auto str = DatesList::exelDateToStr(std::numeric_limits<qint32>::max());
    Q_UNUSED(str);
}

void TestDatesList::checkDateValid() {
    qint32 excel1 = 0;
    qint32 excel2 = 543446;
    qint32 excel3 = 2958465;

    QVERIFY(DatesList::checkExelDate(excel1));
    QVERIFY(DatesList::checkExelDate(excel2));
    QVERIFY(DatesList::checkExelDate(excel3));
}

void TestDatesList::checkDateInvalid() {
    qint32 excel1 = -1;
    qint32 excel2 = 2958466;

    QVERIFY(!DatesList::checkExelDate(excel1));
    QVERIFY(!DatesList::checkExelDate(excel2));
}

void TestDatesList::checkDateStrValid() {
    auto str1 = QString(u"30.12.1899"_s);
    auto str2 = QString(u"31.12.9999"_s);
    auto str3 = QString(u"20.06.2072"_s);

    QVERIFY(DatesList::checkExelDate(str1));
    QVERIFY(DatesList::checkExelDate(str2));
    QVERIFY(DatesList::checkExelDate(str3));
}

void TestDatesList::checkDateStrInvalid() {
    auto str1 = QString(u"29.12.1899"_s);
    auto str2 = QString(u"32.12.9999"_s);
    auto str3 = QString(u"15.15.1935"_s);

    QVERIFY(!DatesList::checkExelDate(str1));
    QVERIFY(!DatesList::checkExelDate(str2));
    QVERIFY(!DatesList::checkExelDate(str3));
}

// ---------- draft / committed semantics ----------

void TestDatesList::insertLivesInDraftUntilCommit() {
    m_list.insert(kDateStr1());
    m_list.insert(kDateStr2());
    m_list.insert(kDateStr3());

    QCOMPARE(m_list.size(), 3);
    QCOMPARE(m_list.sizeCommitted(), 0);

    m_list.commit();

    QCOMPARE(m_list.size(), 3);
    QCOMPARE(m_list.sizeCommitted(), 3);
}

void TestDatesList::readersSeeDraft() {
    m_list.insert(kDateStr1());
    m_list.commit();

    m_list.insert(kDateStr2());

    QVERIFY(m_list.getId(kDateStr1()).has_value());
    QVERIFY(m_list.getId(kDateStr2()).has_value());

    QCOMPARE(m_list.getAllExelDates().size(), 2);
    QCOMPARE(m_list.size(), 2);
    QCOMPARE(m_list.sizeCommitted(), 1);
}

void TestDatesList::removeLivesInDraftUntilCommit() {
    m_list.insert(kDateStr1());
    m_list.insert(kDateStr2());
    m_list.commit();

    QVERIFY(m_list.remove(kDateStr1()));

    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 2);

    QVERIFY(!m_list.getId(kDateStr1()).has_value());
    QVERIFY(m_list.getId(kDateStr2()).has_value());

    m_list.commit();
    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 1);
}

void TestDatesList::resetRollsBackDraft() {
    m_list.insert(kDateStr1());
    m_list.commit();

    m_list.insert(kDateStr2());
    m_list.insert(kDateStr3());

    QCOMPARE(m_list.size(), 3);
    QCOMPARE(m_list.sizeCommitted(), 1);

    m_list.reset();

    QCOMPARE(m_list.size(), 1);
    QCOMPARE(m_list.sizeCommitted(), 1);
    QVERIFY(m_list.getId(kDateStr1()).has_value());
    QVERIFY(!m_list.getId(kDateStr2()).has_value());
    QVERIFY(!m_list.getId(kDateStr3()).has_value());
}

void TestDatesList::clearAffectsOnlyDraft() {
    m_list.insert(kDateStr1());
    m_list.insert(kDateStr2());
    m_list.commit();

    m_list.clear();

    QCOMPARE(m_list.size(), 0);
    QCOMPARE(m_list.sizeCommitted(), 2);

    QVERIFY(!m_list.getId(kDateStr1()).has_value());
    QVERIFY(!m_list.getId(kDateStr2()).has_value());
    QCOMPARE(m_list.getAllExelDates().size(), 0);
}

void TestDatesList::commitMakesDraftVisibleAsCommitted() {
    m_list.insert(kDateStr1());
    m_list.insert(kDateStr2());

    QCOMPARE(m_list.size(), 2);
    QCOMPARE(m_list.sizeCommitted(), 0);

    m_list.commit();

    QCOMPARE(m_list.size(), 2);
    QCOMPARE(m_list.sizeCommitted(), 2);

    auto id1 = m_list.getId(kDateStr1());
    auto id2 = m_list.getId(kDateStr2());
    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());

    auto v1 = m_list.getStrDate(id1.value());
    auto v2 = m_list.getStrDate(id2.value());
    QVERIFY(v1.has_value());
    QVERIFY(v2.has_value());
    QCOMPARE(v1.value(), kDateStr1());
    QCOMPARE(v2.value(), kDateStr2());
}

QTEST_APPLESS_MAIN(TestDatesList)
#include "test_DatesList.moc"
