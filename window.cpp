#pragma once

#include "window.h"

QPlainTextEdit* g_lookupPathEdit = nullptr;  // ОДНА дефиниция

bool validateInput(QString str) {
    bool ok = false;
    uint64_t srcId = str.toULongLong(&ok, 10);
    if (!ok || str.isEmpty() || srcId < 0 || srcId > kSwarmSize) {
        return false;
    }
    return true;
}

void setWindow(QWidget& window) {
    window.setWindowTitle("Kademlia Lookup");

    auto* srcEdit   = new QLineEdit(&window);
    auto* dstEdit   = new QLineEdit(&window);
    auto* startBtn  = new QPushButton("Search", &window);
    auto* actConvLabel  = new QLabel("Result: -", &window);
    auto* theoryConvLabel  = new QLabel(QString::fromStdString(
                                           fmt::format("You should reach convergence at {:.5f} hops",
                                                       (1.0 / harmonic(kBucketSize)) * std::log2(kSwarmSize))

                                           ), &window);
    auto* netSizeLabel  = new QLabel("Network Size: " + QString::number(kSwarmSize), &window);
    auto* boolLabel  = new QLabel("Found: -", &window);
    boolLabel->setTextFormat(Qt::RichText);

    srcEdit->setPlaceholderText("Source ID");
    dstEdit->setPlaceholderText("Target ID");

    // новое поле для пути поиска
    auto* pathLabel = new QLabel("Lookup path:", &window);
    auto* pathEdit  = new QPlainTextEdit(&window);
    pathEdit->setReadOnly(true);
    pathEdit->setPlaceholderText("...");

    // сохраним указатель глобально, чтобы можно было обновлять путь снаружи
    g_lookupPathEdit = pathEdit;

    auto* layout   = new QVBoxLayout(&window);
    auto* row1     = new QHBoxLayout();
    auto* row2     = new QHBoxLayout();

    row1->addWidget(new QLabel("Source Peer:", &window));
    row1->addWidget(srcEdit);

    row2->addWidget(new QLabel("Target Peer:", &window));
    row2->addWidget(dstEdit);

    layout->addLayout(row1);
    layout->addLayout(row2);
    layout->addWidget(startBtn);
    layout->addWidget(theoryConvLabel);
    layout->addWidget(netSizeLabel);
    layout->addWidget(actConvLabel);
    layout->addWidget(boolLabel);
    layout->addWidget(pathLabel);
    layout->addWidget(pathEdit);

    QObject::connect(startBtn, &QPushButton::released, [srcEdit, dstEdit, actConvLabel, theoryConvLabel, boolLabel, &window] {
        QString yes = "<span style='color:#2ECC71; font-size:22px; font-weight:bold;'>✓</span>";
        QString no  = "<span style='color:#E74C3C; font-size:22px; font-weight:bold;'>✗</span>";
        QString arrow = " \u27F6 ";

        QString srcText = srcEdit->text();
        if(!validateInput(srcText)) {
            actConvLabel->setText("Error: wrong source ID");
            boolLabel->setText("Found: " + no);
            return;
        }

        QString dstText = dstEdit->text();
        if(!validateInput(dstText)) {
            actConvLabel->setText("Error: wrong target ID");
            boolLabel->setText("Found: " + no);
            return;
        }

        auto& swarm = Swarm::getInstance();
        bool ok = false;
        uint64_t srcId = srcText.toULongLong(&ok, 10);
        auto srcPeer = swarm.getPeer(srcId);

        auto stats = srcPeer->getStats();
        stats->resetHopCount();
        stats->resetFoundNodes();
        stats->resetTotalHopCounts();

        // перед запуском поиска можно очистить поле пути
        if (g_lookupPathEdit) {
            g_lookupPathEdit->setPlainText("Peer " + QString::number(srcId));
        }
        uint64_t dstId = dstText.toULongLong(&ok, 10);
        srcPeer->find(dstId);

        QTimer::singleShot(kMsBetweenSearches, &window, [stats,dstId, actConvLabel, boolLabel, yes, no] {
            uint64_t hops  = stats->getTotalHopCounts();
            uint64_t found = stats->getFoundNodes();
            actConvLabel->setText("Actual convergence: " + QString::number(hops));

            QString symbol = (found == 1 ? yes : no);
            boolLabel->setText("Found: " + symbol);
            g_lookupPathEdit->appendPlainText("Peer " + QString::number(dstId));
        });
    });
}

void updateLookupPathFromOutside(const QString& pathText) {
    if (!g_lookupPathEdit) return;

    // чтобы безопасно дернуть из других потоков
    QMetaObject::invokeMethod(
        g_lookupPathEdit,
        [p = pathText]() {
            g_lookupPathEdit->appendPlainText(p);
        },
        Qt::QueuedConnection
        );
}
