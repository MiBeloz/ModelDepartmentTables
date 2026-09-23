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

    // ---------- format ----------
    void setGetDateFormat();
    void customFormatRoundTrip();

    // ---------- static helpers ----------
    void strToDateValid();
    void strToDateInvalid();
    void dateToStrValid();
    void dateToStrInvalid();
    void checkDateValid();
    void checkDateInvalid();
    void checkDateStrValid();
    void checkDateStrInvalid();

    // ---------- lastError ----------
    void lastErrorResetOnSuccess();

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

void TestDatesList::insertByStringValid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 1);

    m_list.commit();
    QCOMPARE(m_list.getId(kDateStr1), id);
    QCOMPARE(m_list.size(), 1);
}

void TestDatesList::insertByStringInvalidFormat() {
    auto id = m_list.insert("not-a-date");
    QVERIFY(!id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
    QCOMPARE(m_list.sizeNotCommitted(), 0);
}

void TestDatesList::insertByStringSameValueReturnsSameId() {
    auto id1 = m_list.insert(kDateStr1);
    auto id2 = m_list.insert(kDateStr1);
    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());
    QCOMPARE(id1.value(), id2.value());
    QCOMPARE(m_list.sizeNotCommitted(), 1);
}

void TestDatesList::insertByExcelFormatValid() {
    auto excel = DatesList::strToDate(kDateStr1);
    QVERIFY(excel.has_value());

    auto id = m_list.insert(excel.value());
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 1);
}

void TestDatesList::insertByExcelFormatInvalid() {
    auto id = m_list.insert(qint32(-100000));
    QVERIFY(!id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

// ---------- remove ----------

void TestDatesList::removeByStringValid() {
    m_list.insert(kDateStr1);
    m_list.commit();
    QVERIFY(m_list.remove(kDateStr1));
    QCOMPARE(m_list.lastError(), DatesList::NoError);
    QCOMPARE(m_list.sizeNotCommitted(), 0);
}

void TestDatesList::removeByStringNotFound() {
    QVERIFY(!m_list.remove(kDateStr1));
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

void TestDatesList::removeByStringInvalidFormat() {
    QVERIFY(!m_list.remove("bad-date"));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::removeByExcelFormatValid() {
    auto excel = DatesList::strToDate(kDateStr1);
    QVERIFY(excel.has_value());
    m_list.insert(excel.value());
    m_list.commit();

    QVERIFY(m_list.remove(excel.value()));
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::removeByExcelFormatNotFound() {
    QVERIFY(!m_list.remove(qint32(12345)));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::removeByExcelFormatInvalid() {
    QVERIFY(!m_list.remove(qint32(-1)));
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

// ---------- getId ----------

void TestDatesList::getIdByStringValid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto got = m_list.getId(kDateStr1);
    QVERIFY(got.has_value());
    QCOMPARE(got.value(), id.value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getIdByStringNotFound() {
    auto got = m_list.getId(kDateStr1);
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

void TestDatesList::getIdByStringInvalidFormat() {
    auto got = m_list.getId("bad-date");
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::FormatError);
}

void TestDatesList::getIdByExcelFormatValid() {
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

void TestDatesList::getIdByExcelFormatNotFound() {
    auto got = m_list.getId(qint32(999999));
    QVERIFY(!got.has_value());
    QCOMPARE(m_list.lastError(), DatesList::DateError);
}

// ---------- getValue / getStrValue ----------

void TestDatesList::getValueValid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto value = m_list.getValue(id.value());
    QVERIFY(value.has_value());
    QCOMPARE(value.value(), DatesList::strToDate(kDateStr1).value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getValueInvalidId() {
    auto value = m_list.getValue(qint32(42));
    QVERIFY(!value.has_value());
    QCOMPARE(m_list.lastError(), DatesList::IdError);
}

void TestDatesList::getStrValueValid() {
    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    m_list.commit();

    auto str = m_list.getStrValue(id.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr1);
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

void TestDatesList::getStrValueInvalidId() {
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

void TestDatesList::customFormatRoundTrip() {
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

void TestDatesList::strToDateValid() {
    auto excel = DatesList::strToDate(kDateStr1, kDefaultFormat);
    QVERIFY(excel.has_value());

    QDate expected = QDate::fromString(kDateStr1, kDefaultFormat);
    QDate base = QDate::fromJulianDay(START_EXCEL_DATE);
    QCOMPARE(excel.value(), qint32(expected.toJulianDay() - base.toJulianDay()));
}

void TestDatesList::strToDateInvalid() {
    QVERIFY(!DatesList::strToDate("bad-date").has_value());
    QVERIFY(!DatesList::strToDate("2020-13-45").has_value());
}

void TestDatesList::dateToStrValid() {
    auto excel = DatesList::strToDate(kDateStr2);
    QVERIFY(excel.has_value());

    auto str = DatesList::dateToStr(excel.value());
    QVERIFY(str.has_value());
    QCOMPARE(str.value(), kDateStr2);
}

void TestDatesList::dateToStrInvalid() {
    auto str = DatesList::dateToStr(std::numeric_limits<qint32>::max());
    Q_UNUSED(str);
}

void TestDatesList::checkDateValid() {
    qint32 excel1 = 0;
    qint32 excel2 = 543446;
    qint32 excel3 = 2958465;

    QVERIFY(DatesList::checkDate(excel1));
    QVERIFY(DatesList::checkDate(excel2));
    QVERIFY(DatesList::checkDate(excel3));
}

void TestDatesList::checkDateInvalid() {
    qint32 excel1 = -1;
    qint32 excel2 = 2958466;

    QVERIFY(!DatesList::checkDate(excel1));
    QVERIFY(!DatesList::checkDate(excel2));
}

void TestDatesList::checkDateStrValid() {
    auto str1 = QString("30.12.1899");
    auto str2 = QString("31.12.9999");
    auto str3 = QString("20.06.2072");

    QVERIFY(DatesList::checkDate(str1));
    QVERIFY(DatesList::checkDate(str2));
    QVERIFY(DatesList::checkDate(str3));
}

void TestDatesList::checkDateStrInvalid() {
    auto str1 = QString("29.12.1899");
    auto str2 = QString("32.12.9999");
    auto str3 = QString("15.15.1935");

    QVERIFY(!DatesList::checkDate(str1));
    QVERIFY(!DatesList::checkDate(str2));
    QVERIFY(!DatesList::checkDate(str3));
}

// ---------- lastError ----------

void TestDatesList::lastErrorResetOnSuccess() {
    m_list.insert("bad-date");
    QCOMPARE(m_list.lastError(), DatesList::FormatError);

    auto id = m_list.insert(kDateStr1);
    QVERIFY(id.has_value());
    QCOMPARE(m_list.lastError(), DatesList::NoError);
}

QTEST_APPLESS_MAIN(TestDatesList)
#include "test_DatesList.moc"
