#include <QObject>
#include <QTest>

#include "StorageRecordsLinks.h"

class TestStorageRecordLink final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestStorageRecordLink)

public:
    TestStorageRecordLink() = default;

private slots:
};

QTEST_APPLESS_MAIN(TestStorageRecordLink)
#include "test_StorageRecordLink.moc"
