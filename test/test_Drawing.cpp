#include <QHash>
#include <QSet>
#include <QTest>

#include "Drawing.h"

using mdt::Drawing;
using namespace Qt::Literals::StringLiterals;

class TestDrawing : public QObject {
    Q_OBJECT

private slots:
    // ---------- Constructors / Assignment ----------
    void defaultConstructorCreatesInvalidEmptyDrawing();
    void parameterizedConstructorSetField();
    void parameterizedConstructorSetsFields();
    void parameterizedConstructorAcceptsEmptyStrings();

    // ---------- get / set ----------
    void gettersReturnInitialValues();
    void settersUpdateValues();
    void settersAcceptEmptyStrings();

    // ---------- isValid ----------
    void isValid_data();
    void isValid();

    // ---------- operator== / != ----------
    void equality_data();
    void equality();
    void inequality_data();
    void inequality();

    // ---------- operator< / > / <= / >= ----------
    void lessThan_data();
    void lessThan();
    void lessThanIsStrictWeakOrdering();
    void greaterThan_data();
    void greaterThan();
    void greaterThanIsStrictWeakOrdering();
    void lessOrEqual_data();
    void lessOrEqual();
    void lessOrEqualIsLessOrEqual();
    void greaterOrEqual_data();
    void greaterOrEqual();
    void greaterOrEqualIsGreaterOrEqual();
    void comparisonOperatorsAreConsistent();

    // ---------- qHash ----------
    void qHashEqualObjectsHaveEqualHash();
    void qHashWorksInQHash();
    void qHashWorksInQSet();

    // ---------- serialize / deserialize ----------
    void serializationRoundTrip_data();
    void serializationRoundTrip();
    void serializationDefaultConstructed();
    void serializationMultipleObjects();
};

// =============== Implementation ===============

// ---------- Constructors / Assignment ----------

void TestDrawing::defaultConstructorCreatesInvalidEmptyDrawing() {
    Drawing d;
    QVERIFY(d.getNumber().isEmpty());
    QVERIFY(d.getTitle().isEmpty());
    QVERIFY(!d.isValid());

    Drawing d2 = Drawing::Null;
    QVERIFY(d2.getNumber().isEmpty());
    QVERIFY(d2.getTitle().isEmpty());
    QVERIFY(!d2.isValid());
}

void TestDrawing::parameterizedConstructorSetField() {
    Drawing d(u""_s, u"Корпус"_s);
    QVERIFY(d.getNumber().isEmpty());
    QCOMPARE(d.getTitle(), u"Корпус"_s);
    QVERIFY(!d.isValid());
}

void TestDrawing::parameterizedConstructorSetsFields() {
    Drawing d(u"A-001"_s, u"Корпус"_s);
    QCOMPARE(d.getNumber(), u"A-001"_s);
    QCOMPARE(d.getTitle(), u"Корпус"_s);
    QVERIFY(d.isValid());
}

void TestDrawing::parameterizedConstructorAcceptsEmptyStrings() {
    Drawing d(u""_s, u""_s);
    QVERIFY(!d.isValid());
}

// ---------- get / set ----------

void TestDrawing::gettersReturnInitialValues() {
    Drawing d(u"B-42"_s, u"Крышка"_s);
    QCOMPARE(d.getNumber(), u"B-42"_s);
    QCOMPARE(d.getTitle(), u"Крышка"_s);
}

void TestDrawing::settersUpdateValues() {
    Drawing d;
    d.setNumber(u"C-7"_s);
    d.setTitle(u"Опора"_s);
    QCOMPARE(d.getNumber(), u"C-7"_s);
    QCOMPARE(d.getTitle(), u"Опора"_s);
    QVERIFY(d.isValid());
}

void TestDrawing::settersAcceptEmptyStrings() {
    Drawing d(u"X"_s, u"Y"_s);
    d.setNumber(u""_s);
    QVERIFY(!d.isValid());
    d.setNumber(u"X"_s);
    d.setTitle(u""_s);
    QVERIFY(!d.isValid());
}

// ---------- isValid ----------

void TestDrawing::isValid_data() {
    QTest::addColumn<QString>("number");
    QTest::addColumn<QString>("title");
    QTest::addColumn<bool>("expected");

    QTest::newRow("both filled") << "A-1" << "Title" << true;
    QTest::newRow("empty number") << "" << "Title" << false;
    QTest::newRow("empty title") << "A-1" << "" << false;
    QTest::newRow("both empty") << "" << "" << false;
    QTest::newRow("whitespace only") << " " << " " << false;
}

void TestDrawing::isValid() {
    QFETCH(QString, number);
    QFETCH(QString, title);
    QFETCH(bool, expected);

    Drawing d(number, title);
    QCOMPARE(d.isValid(), expected);
}

// ---------- operator== / != ----------

void TestDrawing::equality_data() {
    QTest::addColumn<Drawing>("a");
    QTest::addColumn<Drawing>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("identical") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s) << true;
    QTest::newRow("same number") << Drawing(u"A-1"_s, u"T1"_s) << Drawing(u"A-1"_s, u"T2"_s)
                                 << false;
    QTest::newRow("same title") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-2"_s, u"T"_s) << false;
    QTest::newRow("both empty") << Drawing() << Drawing() << true;
    QTest::newRow("empty vs filled") << Drawing() << Drawing(u"A"_s, u"T"_s) << false;
    QTest::newRow("case differs") << Drawing(u"a-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s)
                                  << false;
    QTest::newRow("null") << Drawing::Null << Drawing(u""_s, u""_s) << true;
}

void TestDrawing::equality() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a == b, expected);
    QCOMPARE(b == a, expected);
}

void TestDrawing::inequality_data() {
    equality_data();
}

void TestDrawing::inequality() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a != b, !expected);
    QCOMPARE(b != a, !expected);
}

// ---------- operator< ----------

void TestDrawing::lessThan_data() {
    QTest::addColumn<Drawing>("a");
    QTest::addColumn<Drawing>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("number less") << Drawing(u"A-1"_s, u"Z"_s) << Drawing(u"A-2"_s, u"A"_s) << true;
    QTest::newRow("number greater")
        << Drawing(u"B-1"_s, u"A"_s) << Drawing(u"A-2"_s, u"Z"_s) << false;
    QTest::newRow("number equal, title less")
        << Drawing(u"A-1"_s, u"A"_s) << Drawing(u"A-1"_s, u"B"_s) << true;
    QTest::newRow("number equal, title greater")
        << Drawing(u"A-1"_s, u"B"_s) << Drawing(u"A-1"_s, u"A"_s) << false;
    QTest::newRow("fully equal") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s) << false;
    QTest::newRow("empty < filled") << Drawing() << Drawing(u"A"_s, u"T"_s) << true;
    QTest::newRow("case sensitive")
        << Drawing(u"A"_s, u"T"_s) << Drawing(u"a"_s, u"T"_s) << true; // 'A'(0x41) < 'a'(0x61)
}

void TestDrawing::lessThan() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a < b, expected);
}

void TestDrawing::lessThanIsStrictWeakOrdering() {
    //  1) irreflexivity:  !(a < a)
    //  2) asymmetry:      a < b  =>  !(b < a)
    //  3) transitivity:   a < b && b < c  =>  a < c
    const QVector<Drawing> items = {
        Drawing(),
        Drawing(u"A"_s, u"A"_s),
        Drawing(u"A"_s, u"B"_s),
        Drawing(u"B"_s, u"A"_s),
        Drawing(u"B"_s, u"B"_s),
    };

    for (const Drawing& a : items) {
        QVERIFY2(!(a < a), "irreflexivity violated");
        for (const Drawing& b : items) {
            if (a < b) {
                QVERIFY2(!(b < a), "asymmetry violated");
            }
            for (const Drawing& c : items) {
                if (a < b && b < c) {
                    QVERIFY2(a < c, "transitivity violated");
                }
            }
        }
    }

    QVector<Drawing> sorted = items;
    std::sort(sorted.begin(), sorted.end());
    for (int i = 1; i < sorted.size(); ++i) {
        QVERIFY(!(sorted[i] < sorted[i - 1]));
    }
}

void TestDrawing::greaterThan_data() {
    QTest::addColumn<Drawing>("a");
    QTest::addColumn<Drawing>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("number less") << Drawing(u"A-1"_s, u"Z"_s) << Drawing(u"A-2"_s, u"A"_s) << false;
    QTest::newRow("number greater")
        << Drawing(u"B-1"_s, u"A"_s) << Drawing(u"A-2"_s, u"Z"_s) << true;
    QTest::newRow("number equal, title less")
        << Drawing(u"A-1"_s, u"A"_s) << Drawing(u"A-1"_s, u"B"_s) << false;
    QTest::newRow("number equal, title greater")
        << Drawing(u"A-1"_s, u"B"_s) << Drawing(u"A-1"_s, u"A"_s) << true;
    QTest::newRow("fully equal") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s) << false;
    QTest::newRow("empty > filled") << Drawing() << Drawing(u"A"_s, u"T"_s) << false;
    QTest::newRow("case sensitive")
        << Drawing(u"A"_s, u"T"_s) << Drawing(u"a"_s, u"T"_s) << false; // 'A'(0x41) > 'a'(0x61)
}

void TestDrawing::greaterThan() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a > b, expected);
}

void TestDrawing::greaterThanIsStrictWeakOrdering() {
    //  1) irreflexivity:  !(a > a)
    //  2) asymmetry:      b > a  =>  !(a > b)
    //  3) transitivity:   b > a && c > b  =>  c > a
    const QVector<Drawing> items = {
        Drawing(),
        Drawing(u"A"_s, u"A"_s),
        Drawing(u"A"_s, u"B"_s),
        Drawing(u"B"_s, u"A"_s),
        Drawing(u"B"_s, u"B"_s),
    };

    for (const Drawing& a : items) {
        QVERIFY2(!(a > a), "irreflexivity violated");
        for (const Drawing& b : items) {
            if (b > a) {
                QVERIFY2(!(a > b), "asymmetry violated");
            }
            for (const Drawing& c : items) {
                if (b > a && c > b) {
                    QVERIFY2(c > a, "transitivity violated");
                }
            }
        }
    }

    QVector<Drawing> sorted = items;
    std::sort(sorted.begin(), sorted.end());
    for (int i = 1; i < sorted.size(); ++i) {
        QVERIFY(!(sorted[i] < sorted[i - 1]));
    }
}

void TestDrawing::lessOrEqual_data() {
    QTest::addColumn<Drawing>("a");
    QTest::addColumn<Drawing>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("number less") << Drawing(u"A-1"_s, u"Z"_s) << Drawing(u"A-2"_s, u"A"_s) << true;
    QTest::newRow("number greater")
        << Drawing(u"B-1"_s, u"A"_s) << Drawing(u"A-2"_s, u"Z"_s) << false;
    QTest::newRow("number equal, title less")
        << Drawing(u"A-1"_s, u"A"_s) << Drawing(u"A-1"_s, u"B"_s) << true;
    QTest::newRow("number equal, title greater")
        << Drawing(u"A-1"_s, u"B"_s) << Drawing(u"A-1"_s, u"A"_s) << false;
    QTest::newRow("fully equal") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s) << true;
    QTest::newRow("empty < filled") << Drawing() << Drawing(u"A"_s, u"T"_s) << true;
    QTest::newRow("filled > empty") << Drawing(u"A"_s, u"T"_s) << Drawing() << false;
    QTest::newRow("case sensitive") << Drawing(u"A"_s, u"T"_s) << Drawing(u"a"_s, u"T"_s) << true;
}

void TestDrawing::lessOrEqual() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a <= b, expected);
}

void TestDrawing::lessOrEqualIsLessOrEqual() {
    const QVector<Drawing> items = {
        Drawing(),
        Drawing(u"A"_s, u"A"_s),
        Drawing(u"A"_s, u"B"_s),
        Drawing(u"B"_s, u"A"_s),
        Drawing(u"B"_s, u"B"_s),
        Drawing(u"A-1"_s, u"T"_s),
    };

    for (const Drawing& a : items) {
        for (const Drawing& b : items) {
            QCOMPARE(a <= b, (a < b) || (a == b));
        }
    }
}

void TestDrawing::greaterOrEqual_data() {
    QTest::addColumn<Drawing>("a");
    QTest::addColumn<Drawing>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("number less") << Drawing(u"A-1"_s, u"Z"_s) << Drawing(u"A-2"_s, u"A"_s) << false;
    QTest::newRow("number greater")
        << Drawing(u"B-1"_s, u"A"_s) << Drawing(u"A-2"_s, u"Z"_s) << true;
    QTest::newRow("number equal, title less")
        << Drawing(u"A-1"_s, u"A"_s) << Drawing(u"A-1"_s, u"B"_s) << false;
    QTest::newRow("number equal, title greater")
        << Drawing(u"A-1"_s, u"B"_s) << Drawing(u"A-1"_s, u"A"_s) << true;
    QTest::newRow("fully equal") << Drawing(u"A-1"_s, u"T"_s) << Drawing(u"A-1"_s, u"T"_s) << true;
    QTest::newRow("empty < filled") << Drawing() << Drawing(u"A"_s, u"T"_s) << false;
    QTest::newRow("filled > empty") << Drawing(u"A"_s, u"T"_s) << Drawing() << true;
    QTest::newRow("case sensitive") << Drawing(u"A"_s, u"T"_s) << Drawing(u"a"_s, u"T"_s) << false;
}

void TestDrawing::greaterOrEqual() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    QCOMPARE(a >= b, expected);
}

void TestDrawing::greaterOrEqualIsGreaterOrEqual() {
    const QVector<Drawing> items = {
        Drawing(),
        Drawing(u"A"_s, u"A"_s),
        Drawing(u"A"_s, u"B"_s),
        Drawing(u"B"_s, u"A"_s),
        Drawing(u"B"_s, u"B"_s),
        Drawing(u"A-1"_s, u"T"_s),
    };

    for (const Drawing& a : items) {
        for (const Drawing& b : items) {
            QCOMPARE(a >= b, (a > b) || (a == b));
        }
    }
}

void TestDrawing::comparisonOperatorsAreConsistent() {
    const QVector<Drawing> items = {
        Drawing(),
        Drawing(u"A"_s, u"A"_s),
        Drawing(u"A"_s, u"B"_s),
        Drawing(u"B"_s, u"A"_s),
        Drawing(u"B"_s, u"B"_s),
        Drawing(u"A-1"_s, u"T"_s),
        Drawing(u"A-2"_s, u"T"_s),
        Drawing(u"B-1"_s, u"A"_s),
    };

    for (const Drawing& a : items) {
        for (const Drawing& b : items) {
            // exactly one of <, ==, > must be true
            int count = 0;
            if (a < b) {
                ++count;
            }
            if (a == b) {
                ++count;
            }
            if (a > b) {
                ++count;
            }
            QCOMPARE(count, 1);

            // consistency of <= and >=
            QCOMPARE(a <= b, (a < b) || (a == b));
            QCOMPARE(a >= b, (a > b) || (a == b));
            QCOMPARE(a != b, !(a == b));

            // duality
            QCOMPARE(a<b, b> a);
            QCOMPARE(a <= b, b >= a);
            QCOMPARE(a > b, b < a);
            QCOMPARE(a >= b, b <= a);
            QCOMPARE(a == b, b == a);
            QCOMPARE(a != b, b != a);
        }
    }
}

// ---------- qHash ----------

void TestDrawing::qHashEqualObjectsHaveEqualHash() {
    Drawing a(u"A-1"_s, u"T"_s);
    Drawing b(u"A-1"_s, u"T"_s);
    QCOMPARE(a, b);
    QCOMPARE(qHash(a), qHash(b));

    Drawing aa;
    Drawing bb(Drawing::Null);
    QCOMPARE(aa, bb);
    QCOMPARE(qHash(aa), qHash(bb));
}

void TestDrawing::qHashWorksInQHash() {
    QHash<Drawing, int> map;
    map.insert(Drawing(u"A-1"_s, u"T1"_s), 1);
    map.insert(Drawing(u"A-2"_s, u"T2"_s), 2);

    QCOMPARE(map.size(), 2);
    QCOMPARE(map.value(Drawing(u"A-1"_s, u"T1"_s)), 1);
    QCOMPARE(map.value(Drawing(u"A-2"_s, u"T2"_s)), 2);

    map.insert(Drawing(u"A-1"_s, u"T1"_s), 10);
    QCOMPARE(map.size(), 2);
    QCOMPARE(map.value(Drawing(u"A-1"_s, u"T1"_s)), 10);
}

void TestDrawing::qHashWorksInQSet() {
    QSet<Drawing> set;
    set.insert(Drawing(u"A-1"_s, u"T"_s));
    set.insert(Drawing(u"A-1"_s, u"T"_s));
    set.insert(Drawing(u"A-1"_s, u"U"_s));

    QCOMPARE(set.size(), 2);
    QVERIFY(set.contains(Drawing(u"A-1"_s, u"T"_s)));
    QVERIFY(set.contains(Drawing(u"A-1"_s, u"U"_s)));
    QVERIFY(!set.contains(Drawing(u"A-1"_s, u"V"_s)));
}

// ---------- QDataStream ----------

static QByteArray serialize(const Drawing& d) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    out << d;
    return data;
}

static Drawing deserialize(const QByteArray& data) {
    Drawing d;
    QDataStream in(data);
    in.setVersion(QDataStream::Qt_6_0);
    in >> d;
    return d;
}

void TestDrawing::serializationRoundTrip_data() {
    QTest::addColumn<Drawing>("drawing");

    QTest::newRow("regular") << Drawing(u"A-001"_s, u"Корпус"_s);
    QTest::newRow("empty") << Drawing();
    QTest::newRow("only number") << Drawing(u"A-001"_s, u""_s);
    QTest::newRow("only title") << Drawing(u""_s, u"Корпус"_s);
    QTest::newRow("unicode") << Drawing(u"№1"_s, u"Чертёж «Основа»"_s);
    QTest::newRow("long strings") << Drawing(QString(1000, u'n'), QString(1000, u't'));
    QTest::newRow("special chars") << Drawing(u"a\tb\nc"_s, u"x\"y\\z"_s);
}

void TestDrawing::serializationRoundTrip() {
    QFETCH(Drawing, drawing);

    const QByteArray _data = serialize(drawing);
    const Drawing restored = deserialize(_data);

    QCOMPARE(restored, drawing);
    QCOMPARE(restored.getNumber(), drawing.getNumber());
    QCOMPARE(restored.getTitle(), drawing.getTitle());
}

void TestDrawing::serializationDefaultConstructed() {
    Drawing original;
    Drawing restored = deserialize(serialize(original));
    QCOMPARE(restored, original);
    QVERIFY(!restored.isValid());

    Drawing original2(Drawing::Null);
    Drawing restored2 = deserialize(serialize(original2));
    QCOMPARE(restored2, original2);
    QVERIFY(!restored2.isValid());
}

void TestDrawing::serializationMultipleObjects() {
    const QVector<Drawing> src = {
        Drawing(u"A-1"_s, u"Alpha"_s),
        Drawing(u"A-2"_s, u"Beta"_s),
        Drawing(),
        Drawing(u"B-1"_s, u"Gamma"_s),
    };

    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_0);
        for (const Drawing& d : src) {
            out << d;
        }
    }

    QVector<Drawing> restored;
    {
        QDataStream in(data);
        in.setVersion(QDataStream::Qt_6_0);
        for (int i = 0; i < src.size(); ++i) {
            Drawing d;
            in >> d;
            restored.append(d);
        }
        QCOMPARE(in.status(), QDataStream::Ok);
    }

    QCOMPARE(restored, src);
}

QTEST_APPLESS_MAIN(TestDrawing)
#include "test_Drawing.moc"
