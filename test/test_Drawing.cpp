#include <QHash>
#include <QSet>
#include <QtTest>

#include "drawing.h"

class TestDrawing : public QObject {
    Q_OBJECT

private slots:
    // ---------- Constructors / Assignment ----------
    void defaultConstructor_createsInvalidEmptyDrawing();
    void parameterizedConstructor_setField();
    void parameterizedConstructor_setsFields();
    void parameterizedConstructor_acceptsEmptyStrings();

    // ---------- get / set ----------
    void getters_returnInitialValues();
    void setters_updateValues();
    void setters_acceptEmptyStrings();

    // ---------- isValid ----------
    void isValid_data();
    void isValid();

    // ---------- operator== / != ----------
    void equality_data();
    void equality();
    void inequality_data();
    void inequality();

    // ---------- operator< ----------
    void lessThan_data();
    void lessThan();
    void lessThan_isStrictWeakOrdering();

    // ---------- qHash ----------
    void qHash_equalObjectsHaveEqualHash();
    void qHash_worksInQHash();
    void qHash_worksInQSet();

    // ---------- QDataStream ----------
    void serialization_roundTrip_data();
    void serialization_roundTrip();
    void serialization_defaultConstructed();
    void serialization_multipleObjects();

    // ---------- QDebug ----------
    void debugOutput_containsNumberAndTitle();
    void debugOutput_invalidDrawing();
};

// =============== Implementation ===============

// ---------- Constructors / Assignment ----------

void TestDrawing::defaultConstructor_createsInvalidEmptyDrawing() {
    Drawing d;
    QVERIFY(d.getNumber().isEmpty());
    QVERIFY(d.getTitle().isEmpty());
    QVERIFY(!d.isValid());

    Drawing d2 = Drawing::Null;
    QVERIFY(d2.getNumber().isEmpty());
    QVERIFY(d2.getTitle().isEmpty());
    QVERIFY(!d2.isValid());
}

void TestDrawing::parameterizedConstructor_setField() {
    Drawing d("", "Корпус");
    QVERIFY(d.getNumber().isEmpty());
    QCOMPARE(d.getTitle(), QString("Корпус"));
    QVERIFY(!d.isValid());
}

void TestDrawing::parameterizedConstructor_setsFields() {
    Drawing d("A-001", "Корпус");
    QCOMPARE(d.getNumber(), QString("A-001"));
    QCOMPARE(d.getTitle(), QString("Корпус"));
    QVERIFY(d.isValid());
}

void TestDrawing::parameterizedConstructor_acceptsEmptyStrings() {
    Drawing d("", "");
    QVERIFY(!d.isValid());
}

// ---------- get / set ----------

void TestDrawing::getters_returnInitialValues() {
    Drawing d("B-42", "Крышка");
    QCOMPARE(d.getNumber(), QString("B-42"));
    QCOMPARE(d.getTitle(), QString("Крышка"));
}

void TestDrawing::setters_updateValues() {
    Drawing d;
    d.setNumber("C-7");
    d.setTitle("Опора");
    QCOMPARE(d.getNumber(), QString("C-7"));
    QCOMPARE(d.getTitle(), QString("Опора"));
    QVERIFY(d.isValid());
}

void TestDrawing::setters_acceptEmptyStrings() {
    Drawing d("X", "Y");
    d.setNumber("");
    QVERIFY(!d.isValid());
    d.setNumber("X");
    d.setTitle("");
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

    QTest::newRow("identical") << Drawing("A-1", "T") << Drawing("A-1", "T") << true;
    QTest::newRow("same number") << Drawing("A-1", "T1") << Drawing("A-1", "T2") << false;
    QTest::newRow("same title") << Drawing("A-1", "T") << Drawing("A-2", "T") << false;
    QTest::newRow("both empty") << Drawing() << Drawing() << true;
    QTest::newRow("empty vs filled") << Drawing() << Drawing("A", "T") << false;
    QTest::newRow("case differs") << Drawing("a-1", "T") << Drawing("A-1", "T") << false;
    QTest::newRow("null") << Drawing::Null << Drawing("", "") << true;
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

    QTest::newRow("number less") << Drawing("A-1", "Z") << Drawing("A-2", "A") << true;
    QTest::newRow("number greater") << Drawing("B-1", "A") << Drawing("A-2", "Z") << false;
    QTest::newRow("number equal, title less") << Drawing("A-1", "A") << Drawing("A-1", "B") << true;
    QTest::newRow("number equal, title greater")
        << Drawing("A-1", "B") << Drawing("A-1", "A") << false;
    QTest::newRow("fully equal") << Drawing("A-1", "T") << Drawing("A-1", "T") << false;
    QTest::newRow("empty < filled") << Drawing() << Drawing("A", "T") << true;
    QTest::newRow("case sensitive")
        << Drawing("A", "T") << Drawing("a", "T") << true; // 'A'(0x41) < 'a'(0x61)
}

void TestDrawing::lessThan() {
    QFETCH(Drawing, a);
    QFETCH(Drawing, b);
    QFETCH(bool, expected);

    //qDebug() << "case:" << a << b << expected;

    QCOMPARE(a < b, expected);
}

void TestDrawing::lessThan_isStrictWeakOrdering() {
    //  1) irreflexivity:  !(a < a)
    //  2) asymmetry:      a < b  =>  !(b < a)
    //  3) transitivity:   a < b && b < c  =>  a < c
    const QVector<Drawing> items = {
        Drawing(), // пустое
        Drawing("A", "A"), Drawing("A", "B"), Drawing("B", "A"), Drawing("B", "B"),
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

// ---------- qHash ----------

void TestDrawing::qHash_equalObjectsHaveEqualHash() {
    Drawing a("A-1", "T");
    Drawing b("A-1", "T");
    QCOMPARE(a, b);
    QCOMPARE(qHash(a), qHash(b));

    Drawing aa;
    Drawing bb(Drawing::Null);
    QCOMPARE(aa, bb);
    QCOMPARE(qHash(aa), qHash(bb));
}

void TestDrawing::qHash_worksInQHash() {
    QHash<Drawing, int> map;
    map.insert(Drawing("A-1", "T1"), 1);
    map.insert(Drawing("A-2", "T2"), 2);

    QCOMPARE(map.size(), 2);
    QCOMPARE(map.value(Drawing("A-1", "T1")), 1);
    QCOMPARE(map.value(Drawing("A-2", "T2")), 2);

    map.insert(Drawing("A-1", "T1"), 10);
    QCOMPARE(map.size(), 2);
    QCOMPARE(map.value(Drawing("A-1", "T1")), 10);
}

void TestDrawing::qHash_worksInQSet() {
    QSet<Drawing> set;
    set.insert(Drawing("A-1", "T"));
    set.insert(Drawing("A-1", "T"));
    set.insert(Drawing("A-1", "U"));

    QCOMPARE(set.size(), 2);
    QVERIFY(set.contains(Drawing("A-1", "T")));
    QVERIFY(set.contains(Drawing("A-1", "U")));
    QVERIFY(!set.contains(Drawing("A-1", "V")));
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

void TestDrawing::serialization_roundTrip_data() {
    QTest::addColumn<Drawing>("drawing");

    QTest::newRow("regular") << Drawing("A-001", "Корпус");
    QTest::newRow("empty") << Drawing();
    QTest::newRow("only number") << Drawing("A-001", "");
    QTest::newRow("only title") << Drawing("", "Корпус");
    QTest::newRow("unicode") << Drawing("№1", "Чертёж «Основа»");
    QTest::newRow("long strings") << Drawing(QString(1000, 'n'), QString(1000, 't'));
    QTest::newRow("special chars") << Drawing("a\tb\nc", "x\"y\\z");
}

void TestDrawing::serialization_roundTrip() {
    QFETCH(Drawing, drawing);

    const QByteArray data = serialize(drawing);
    const Drawing restored = deserialize(data);

    QCOMPARE(restored, drawing);
    QCOMPARE(restored.getNumber(), drawing.getNumber());
    QCOMPARE(restored.getTitle(), drawing.getTitle());
}

void TestDrawing::serialization_defaultConstructed() {
    Drawing original;
    Drawing restored = deserialize(serialize(original));
    QCOMPARE(restored, original);
    QVERIFY(!restored.isValid());

    Drawing original2(Drawing::Null);
    Drawing restored2 = deserialize(serialize(original2));
    QCOMPARE(restored2, original2);
    QVERIFY(!restored2.isValid());
}

void TestDrawing::serialization_multipleObjects() {
    const QVector<Drawing> src = {
        Drawing("A-1", "Alpha"),
        Drawing("A-2", "Beta"),
        Drawing(),
        Drawing("B-1", "Gamma"),
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

// ---------- QDebug ----------

static QString debugToString(const Drawing& d) {
    QString s;
    QDebug dbg(&s);
    dbg << d;
    return s;
}

void TestDrawing::debugOutput_containsNumberAndTitle() {
    Drawing d("A-001", "Корпус");
    const QString out = debugToString(d);

    QVERIFY2(out.contains("A-001"), qPrintable(out));
    QVERIFY2(out.contains("Корпус"), qPrintable(out));
    QVERIFY2(out.contains("-"), qPrintable(out));
}

void TestDrawing::debugOutput_invalidDrawing() {
    Drawing d;
    const QString out = debugToString(d);
    QVERIFY2(out.contains("-"), qPrintable(out));
}

QTEST_APPLESS_MAIN(TestDrawing)
#include "test_Drawing.moc"
