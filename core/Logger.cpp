#include "Logger.h"
#include <QDebug>

void Logger::info(const QString& msg)  { qInfo().noquote()     << "[Game_macro_elite]" << msg; }
void Logger::warn(const QString& msg)  { qWarning().noquote()  << "[Game_macro_elite]" << msg; }
void Logger::error(const QString& msg) { qCritical().noquote() << "[Game_macro_elite]" << msg; }
void Logger::debug(const QString& msg) { qDebug().noquote()    << "[Game_macro_elite][DEBUG]" << msg; }
