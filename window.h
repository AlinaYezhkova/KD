#pragma once

#include "id.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qwidget.h"
#include "swarm.h"
#include <QPlainTextEdit>
#include <QTimer>

extern std::vector<uint64_t> path;
extern bool                  g_uiActive;

bool validateInput(QString str);

void setWindow(QWidget& window);

void updateLookupPathFromOutside(uint64_t inter);