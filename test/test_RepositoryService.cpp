#include <QObject>
#include <QTest>

#include "Repository.h"

RepositoryService repo;

class TestRepositoryService final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestRepositoryService)

public:
    TestRepositoryService() = default;

private slots:
    void testAdd() {
        QCOMPARE(repo.dates().add("25.08.2025"), 1);
        QCOMPARE(repo.dates().count(), 1);

        QCOMPARE(repo.dates().add("15.07.2024"), 2);
        QCOMPARE(repo.dates().add("18.03.2020"), 3);
        QCOMPARE(repo.dates().add("05.11.2021"), 4);
        QCOMPARE(repo.dates().count(), 4);

        QCOMPARE(repo.dates().add("18.03.2020"), 3);
        QCOMPARE(repo.dates().count(), 4);

        QCOMPARE(repo.dates().add("28.08.2019"), 5);
        QCOMPARE(repo.dates().count(), 5);

        QCOMPARE(repo.dates().add("55.08.2019"), std::nullopt);
    }

    void testRemove() {
        QCOMPARE(repo.dates().remove("05.11.2021"), true);
        QCOMPARE(repo.dates().count(), 4);

        QCOMPARE(repo.dates().remove("25.09.2008"), false);
        QCOMPARE(repo.dates().count(), 4);

        QCOMPARE(repo.dates().remove("18.03.2020"), true);
        QCOMPARE(repo.dates().count(), 3);

        QCOMPARE(repo.dates().remove("55.08.2019"), false);
    }

    void testaddWithQueue() {
        QCOMPARE(repo.dates().add("13.06.2015"), 4);
        QCOMPARE(repo.dates().count(), 4);

        QCOMPARE(repo.dates().add("26.03.2018"), 3);
        QCOMPARE(repo.dates().count(), 5);

        QCOMPARE(repo.dates().add("09.01.2019"), 6);
        QCOMPARE(repo.dates().count(), 6);
    }

    void testGetId() {
        QCOMPARE(repo.dates().findId("13.06.2015"), 4);
        QCOMPARE(repo.dates().findId("15.07.2024"), 2);
        QCOMPARE(repo.dates().findId("26.03.2018"), 3);
        QCOMPARE(repo.dates().findId("28.08.2019"), 5);
        QCOMPARE(repo.dates().findId("09.01.2019"), 6);
        QCOMPARE(repo.dates().findId("25.08.2025"), 1);

        QCOMPARE(repo.dates().findId("01.05.2017"), std::nullopt);
        QCOMPARE(repo.dates().findId("55.05.2017"), std::nullopt);
    }

    void testGetDate() {
        QCOMPARE(repo.dates().findValue(4), "13.06.2015");
        QCOMPARE(repo.dates().findValue(2), "15.07.2024");
        QCOMPARE(repo.dates().findValue(3), "26.03.2018");
        QCOMPARE(repo.dates().findValue(5), "28.08.2019");
        QCOMPARE(repo.dates().findValue(6), "09.01.2019");
        QCOMPARE(repo.dates().findValue(1), "25.08.2025");

        QCOMPARE(repo.dates().findValue(7), std::nullopt);
    }

    void clear() {
        QCOMPARE(repo.dates().count(), 6);
        repo.dates().clear();
        QCOMPARE(repo.dates().count(), 0);
    }
};

QTEST_APPLESS_MAIN(TestRepositoryService)
#include "test_RepositoryService.moc"
