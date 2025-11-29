#pragma once

#include "id.h"
#include "swarm.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qwidget.h"
#include <QTimer>
#include <QPlainTextEdit>

// опционально: чтобы можно было добраться до поля извне
extern QPlainTextEdit* g_lookupPathEdit;

bool validateInput(QString str);

void setWindow(QWidget& window);

// опциональная удобная функция для обновления пути извне
// опциональная удобная функция для обновления пути извне
void updateLookupPathFromOutside(const QString& pathText);
