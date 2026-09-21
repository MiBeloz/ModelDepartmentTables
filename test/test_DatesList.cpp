#include <QDebug>
#include <QObject>
#include <QTest>

#include "DatesList.h"

class TestDatesList : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // ---------- insert ----------
    void insertByString_valid();
    void insertByString_invalidFormat();
    void insertByString_sameValueReturnsSameId();
    void insertByExcelFormat_valid();
    void insertByExcelFormat_invalid();

    // ---------- remove ----------
    void removeByString_valid();
    void removeByString_notFound();
    void removeByString_invalidFormat();
    void removeByExcelFormat_valid();
    void removeByExcelFormat_notFound();
    void removeByExcelFormat_invalid();

    // ---------- getId ----------
    void getIdByString_valid();
    void getIdByString_notFound();
    void getIdByString_invalidFormat();
    void getIdByExcelFormat_valid();
    void getIdByExcelFormat_notFound();

    // ---------- getValue / getStrValue ----------
    void getValue_valid();
    void getValue_invalidId();
    void getStrValue_valid();
    void getStrValue_invalidId();

    // ---------- format ----------
    void setGetDateFormat();
    void customFormat_roundTrip();

    // ---------- static helpers ----------
    void strToDate_valid();
    void strToDate_invalid();
    void dateToStr_valid();
    void dateToStr_invalid();
    void checkDate_valid();
    void checkDate_invalid();
    void checkDateStr_valid();
    void checkDateStr_invalid();

    // ---------- lastError ----------
    void lastError_resetOnSuccess();

private:
    DatesList m_list;
};

// =============== Implementation ===============

namespace {
    const QString kDefaultFormat = "dd.MM.yyyy";
    const QString kDateStr1 = "01.01.2000";
    const QString kDateStr2 = "15.06.2020";
    const QString kDateStr3 = "31.12.1999";
} // namespace

void TestDatesList::init() {
    m_list = DatesList();
    m_list.setDateFormat(kDefaultFormat);
}

void TestDatesList::cleanup() {
    m_list.clear();
    m_list.commit();
}

// ---------- insert ----------

void TestDatesList::insertByString_valid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 1);

    m_list.commit();
    QCOMPARE(m_list.getId(kDateStr1), id);
    QCOMPARE(m_list.size(), 1);
}

void TestDatesList::insertByString_invalidFormat() {
    auto id = m_list.insert("not-a-date");
    QVERIFY(!id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
    QCOMPARE(m_list.sizeNotCommitted(), 0);
}

void TestDatesList::insertByString_sameValueReturnsSameId() {
    auto id1 = m_list.insert(kDateStr1);
    auto id2 = m_list.insert(kDateStr1);
    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());
    QCOMPARE(id1.value(), id2.value());
    QCOMPARE(m_list.sizeNotCommitted(), 1);
}

void TestDatesList::insertByExcelFormat_valid() {
    auto excel = DatesList::strToDate(kDateStr1);
    QVERIFY(excel.has_value());

    auto id = m_list.insert(excel.value());
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 1);
}

void TestDatesList::insertByExcelFormat_invalid() {
    auto id = m_list.insert(qint32(-100000));
    QVERIFY(!id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

// ---------- remove ----------

void TestDatesList::removeByString_valid() {
    m_list.insert(kDateStr1);
    m_list.commit();
    QVERIFY(m_list.remove(kDateStr1));
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 0);
}

void TestDatesList::removeByString_notFound() {
    QVERIFY(!m_list.remove(kDateStr1));
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

void TestDatesList::removeByString_invalidFormat() {
    QVERIFY(!m_list.remove("bad-date"));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::removeByExcelFormat_valid() {
    auto excel = DatesList::strToDate(kDateStr1);
    QVERIFY(excel.has_value());
    m_list.insert(excel.value());
    m_list.commit();

    QVERIFY(m_list.remove(excel.value()));
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::removeByExcelFormat_notFound() {
    QVERIFY(!m_list.remove(qint32(12345)));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::removeByExcelFormat_invalid() {
    QVERIFY(!m_list.remove(qint32(-1)));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

// ---------- getId ----------

void TestDatesList::getIdByString_valid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto got = m_list.getId(kDateStr1);
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getIdByString_notFound() {
    auto got = m_list.getId(kDateStr1);
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

void TestDatesList::getIdByString_invalidFormat() {
    auto got = m_list.getId("bad-date");
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::getIdByExcelFormat_valid() {
    auto excel = DatesList::strToDate(kDateStr2);
    QVERIFY(excel.has_value());

    auto id = m_list.insert(excel.value());
    QVERIFY(id.has_value());
    m_list.commit();

    auto got = m_list.getId(excel.value());
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getIdByExcelFormat_notFound() {
    auto got = m_list.getId(qint32(999999));
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

// ---------- getValue / getStrValue ----------

void TestDatesList::getValue_valid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto value = m_list.getValue(id.value());
    QVERIFY(value.has_value());
    QCOMPARE(value.value(), DatesList::strToDate(kDateStr1).value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getValue_invalidId() {
    auto value = m_list.getValue(qint32(42));
    QVERIFY(!value.has_value());
    QCOMPARE(m_list.lastError(), DatesList::IdError);
}

void TestDatesList::getStrValue_valid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto str = m_list.getStrValue(id.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr1);
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getStrValue_invalidId() {
    auto str = m_list.getStrValue(qint32(42));
    QVERIFY(!str.has_value());
    QCOMPARE(m_list.lastError(), DatesList::IdError);
}

// ---------- format ----------

void TestDatesList::setGetDateFormat() {
    const QString newFormat = "yyyy-MM-dd";
    m_list.setDateFormat(newFormat);
    QCOMPARE(m_list.getDateFormat(), newFormat);
}

void TestDatesList::customFormat_roundTrip() {
    const QString newFormat = "yyyy-MM-dd";
    m_list.setDateFormat(newFormat);

    const QString dateStr = "2021-03-15";
    auto id = m_list.insert(dateStr);
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    m_list.commit();

    auto str = m_list.getStrValue(id.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), dateStr);

    auto bad = m_list.insert("15.03.2021");
    QVERIFY(!bad.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

// ---------- static helpers ----------

void TestDatesList::strToDate_valid() {
    auto excel = DatesList::strToDate(kDateStr1, kDefaultFormat);
    QVERIFY(excel.has_value());

    QDate expected = QDate::fromString(kDateStr1, kDefaultFormat);
    QDate base = QDate::fromJulianDay(START_EXCEL_DATE);
    QCOMPARE(excel.value(), qint32(expected.toJulianDay() - base.toJulianDay()));
}

void TestDatesList::strToDate_invalid() {
    QVERIFY(!DatesList::strToDate("bad-date").has_value());
    QVERIFY(!DatesList::strToDate("2020-13-45").has_value());
}

void TestDatesList::dateToStr_valid() {
    auto excel = DatesList::strToDate(kDateStr2);
    QVERIFY(excel.has_value());

    auto str = DatesList::dateToStr(excel.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr2);
}

void TestDatesList::dateToStr_invalid() {
    auto str = DatesList::dateToStr(std::numeric_limits<qint32>::max());
    Q_UNUSED(str);
}

void TestDatesList::checkDate_valid() {
    qint32 excel1 = 0;
    qint32 excel2 = 543446;
    qint32 excel3 = 2958465;

    QVERIFY(DatesList::checkDate(excel1));
    QVERIFY(DatesList::checkDate(excel2));
    QVERIFY(DatesList::checkDate(excel3));
}

void TestDatesList::checkDate_invalid() {
    qint32 excel1 = -1;
    qint32 excel2 = 2958466;

    QVERIFY(!DatesList::checkDate(excel1));
    QVERIFY(!DatesList::checkDate(excel2));
}

void TestDatesList::checkDateStr_valid() {
    auto str1 = QString("30.12.1899");
    auto str2 = QString("31.12.9999");
    auto str3 = QString("20.06.2072");

    QVERIFY(DatesList::checkDate(str1));
    QVERIFY(DatesList::checkDate(str2));
    QVERIFY(DatesList::checkDate(str3));
}

void TestDatesList::checkDateStr_invalid() {
    auto str1 = QString("29.12.1899");
    auto str2 = QString("32.12.9999");
    auto str3 = QString("15.15.1935");

    QVERIFY(!DatesList::checkDate(str1));
    QVERIFY(!DatesList::checkDate(str2));
    QVERIFY(!DatesList::checkDate(str3));
}

// ---------- lastError ----------

void TestDatesList::lastError_resetOnSuccess() {
    m_list.insert("bad-date");
    QCOMPARE(m_list.lastError(), DatesList::FormatError);

    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

QTEST_APPLESS_MAIN(TestDatesList)
#include "test_DatesList.moc"
