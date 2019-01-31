#ifndef BASICREADWRITE_H
#define BASICREADWRITE_H
#include <QString>
#include <QIODevice>
#include <Segments/qcubicsegment1d.h>
#include "castmacros.h"

extern bool gRead(QIODevice* src, QString& targetStr);
extern bool gWrite(QIODevice* dst, const QString& strToWrite);

extern bool gRead(QIODevice* src, bool& value);
extern bool gWrite(QIODevice* dst, const bool& value);

extern bool gRead(QIODevice* src, qCubicSegment1D& value);
extern bool gWrite(QIODevice* dst, const qCubicSegment1D& value);
#endif // BASICREADWRITE_H