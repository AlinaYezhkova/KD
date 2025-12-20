#include "window.h"

#include <QPlainTextEdit>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QFont>
#include <QEasingCurve>

std::vector<uint64_t> path;
bool                  g_uiActive = false;

bool validateInput(QString str) {
    bool     ok    = false;
    uint64_t srcId = str.toULongLong(&ok, 10);
    if (!ok || str.isEmpty() || srcId > kSwarmSize) {
        return false;
    }
    return true;
}

void setWindow(QWidget& window) {
    g_uiActive = true;
    window.setWindowTitle("Kademlia Lookup");

    // --- Немного стиля всему окну ---
 window.setStyleSheet(R"(
        QWidget {
            background-color: #f8fafc;       /* светлый фон */
            color: #0f172a;                  /* тёмный текст */
            font-family: 'Segoe UI', 'DejaVu Sans', sans-serif;
            font-size: 12px;
        }
        QLineEdit, QPlainTextEdit {
            background-color: #ffffff;
            border: 1px solid #cbd5e1;       /* светло-серая рамка */
            border-radius: 6px;
            padding: 4px 6px;
            selection-background-color: #93c5fd;   /* нежно-голубой */
            selection-color: #1e293b;
        }
        QLabel#TitleLabel {
            color: #0f172a;
            font-size: 20px;
            font-weight: 700;
        }
        QLabel#SectionLabel {
            color: #475569;
            font-size: 11px;
            font-weight: 600;
            text-transform: uppercase;
        }
        QPushButton {
            background-color: #3b82f6;       /* голубая кнопка */
            border-radius: 6px;
            padding: 6px 16px;
            color: #ffffff;
            font-weight: 600;
            border: none;
        }
        QPushButton:hover {
            background-color: #2563eb;       /* тёмнее при наведении */
        }
        QPushButton:pressed {
            background-color: #1d4ed8;       /* ещё темнее при клике */
        }
    )");
    
    // --- Верхний заголовок ---
    auto* titleLabel = new QLabel("Kademlia Lookup Explorer", &window);
    titleLabel->setObjectName("TitleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    // --- Поля ввода ---
    auto* srcEdit  = new QLineEdit(&window);
    auto* dstEdit  = new QLineEdit(&window);
    auto* startBtn = new QPushButton("Search", &window);

    srcEdit->setPlaceholderText("Source ID");
    dstEdit->setPlaceholderText("Target ID");

    // --- Лейблы статуса ---
    auto* actConvLabel = new QLabel("Result: -", &window);

    auto* theoryConvLabel = new QLabel(
        QString::fromStdString(
            fmt::format("You should reach convergence at {:.5f} hops",
                        (1.0 / harmonic(kBucketSize)) * std::log2(kSwarmSize))
        ),
        &window
    );

    auto* netSizeLabel =
        new QLabel("Network size: " + QString::number(kSwarmSize), &window);

    auto* boolLabel = new QLabel("Found: -", &window);
    boolLabel->setTextFormat(Qt::RichText);

    // --- Лейбл и поле для пути поиска ---
    auto* pathSectionLabel = new QLabel("Lookup path", &window);
    pathSectionLabel->setObjectName("SectionLabel");

    auto* pathEdit = new QPlainTextEdit(&window);
    pathEdit->setReadOnly(true);
    pathEdit->setPlaceholderText("...");
    QFont monoFont = pathEdit->font();
    monoFont.setFamily("Consolas, 'DejaVu Sans Mono', monospace");
    pathEdit->setFont(monoFont);
    pathEdit->setMinimumHeight(80);

    // --- Размещение: сначала сетка для ID и кнопки ---
    auto* topGrid = new QGridLayout();
    topGrid->setColumnStretch(1, 1);
    topGrid->setHorizontalSpacing(8);
    topGrid->setVerticalSpacing(6);

    auto* srcLabel = new QLabel("Source peer:", &window);
    auto* dstLabel = new QLabel("Target peer:", &window);

    topGrid->addWidget(srcLabel, 0, 0, Qt::AlignRight);
    topGrid->addWidget(srcEdit, 0, 1);
    topGrid->addWidget(dstLabel, 1, 0, Qt::AlignRight);
    topGrid->addWidget(dstEdit, 1, 1);
    topGrid->addWidget(startBtn, 0, 2, 2, 1, Qt::AlignVCenter);

    // --- Блок статуса ---
    auto* statusLayout = new QVBoxLayout();
    statusLayout->setSpacing(2);
    statusLayout->addWidget(theoryConvLabel);
    statusLayout->addWidget(netSizeLabel);
    statusLayout->addWidget(actConvLabel);
    statusLayout->addWidget(boolLabel);

    // --- Главный layout ---
    auto* layout = new QVBoxLayout(&window);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    layout->addWidget(titleLabel);
    layout->addSpacing(4);
    layout->addLayout(topGrid);
    layout->addSpacing(8);
    layout->addLayout(statusLayout);
    layout->addSpacing(8);
    layout->addWidget(pathSectionLabel);
    layout->addWidget(pathEdit);

    // --- Логика кнопки ---
    QObject::connect(
        startBtn,
        &QPushButton::released,
        [srcEdit,
         dstEdit,
         actConvLabel,
         boolLabel,
         pathEdit,
         &window] {
            QString yes =
                "<span style='color:#22c55e; font-size:22px; font-weight:bold;'>✓</span>";
            QString no  =
                "<span style='color:#ef4444; font-size:22px; font-weight:bold;'>✗</span>";

            path.clear();

            QString srcText = srcEdit->text().trimmed();
            QString dstText = dstEdit->text().trimmed();

            pathEdit->clear();

            if (!validateInput(srcText)) {
                actConvLabel->setText("Error: wrong source ID");
                boolLabel->setText("Found: " + no);
                pathEdit->setPlainText("Invalid source ID.");
                return;
            }

            if (!validateInput(dstText)) {
                actConvLabel->setText("Error: wrong target ID");
                boolLabel->setText("Found: " + no);
                pathEdit->setPlainText("Invalid target ID.");
                return;
            }

            auto& swarm = Swarm::getInstance();
            bool  ok    = false;

            uint64_t srcId = srcText.toULongLong(&ok, 10);
            auto     srcPeer = swarm.getPeer(srcId);
            if (!srcPeer) {
                actConvLabel->setText("Error: source peer not found");
                boolLabel->setText("Found: " + no);
                pathEdit->setPlainText("Source peer not found in swarm.");
                return;
            }

            auto stats = srcPeer->getStats();
            stats->resetHopCount();
            stats->resetFoundNodes();
            stats->resetTotalHopCounts();

            uint64_t dstId = dstText.toULongLong(&ok, 10);

            path.push_back(srcId);  // стартовый peer
            srcPeer->find(dstId);

            QTimer::singleShot(
                kMsBetweenPeers,
                &window,
                [stats,
                 dstId,
                 actConvLabel,
                 boolLabel,
                 yes,
                 no,
                 pathEdit] {
                    uint64_t hops  = stats->getTotalHopCounts();
                    uint64_t found = stats->getFoundNodes();
                    actConvLabel->setText("Actual convergence: " +
                                          QString::number(hops));

                    QString symbol = (found == 1 ? yes : no);
                    boolLabel->setText("Found: " + symbol);

                    // финальную точку тоже добавим
                    path.push_back(dstId);

                    // собираем красивую строку вида:
                    // Peer 2 → Peer 15 → Peer 18
                    QString arrow = " \u2192 "; // →
                    QString line;
                    for (std::size_t i = 0; i < path.size(); ++i) {
                        line += "Peer " + QString::number(path[i]);
                        if (i + 1 < path.size())
                            line += arrow;
                    }
                    pathEdit->setPlainText(line);

                    // простая анимация: "подмигивающий" boolLabel
                    auto* effect = new QGraphicsOpacityEffect(boolLabel);
                    boolLabel->setGraphicsEffect(effect);
                    auto* anim =
                        new QPropertyAnimation(effect, "opacity", boolLabel);
                    anim->setDuration(300);
                    anim->setStartValue(0.3);
                    anim->setEndValue(1.0);
                    anim->setEasingCurve(QEasingCurve::OutCubic);
                    anim->start(QAbstractAnimation::DeleteWhenStopped);
                });
        });
}

void updateLookupPathFromOutside(uint64_t interm) {
    if (!g_uiActive)
        return;
    path.push_back(interm);
}
