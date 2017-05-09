#ifndef CSVTOQSQLITE_H
#define CSVTOQSQLITE_H

#include "csvtoqsqlite_global.h"

#include <QSharedPointer>
#include <QString>

struct CsvToQSqliteImpl;

class CSVTOQSQLITESHARED_EXPORT CsvToQSqlite
{

public:
    CsvToQSqlite();
    ~CsvToQSqlite();

    void setInput(const QString& inputPath);
    void setOutput(const QString& outputPath);
    void setSeparator(const QString& separator);

    bool process();

private:
    QSharedPointer<CsvToQSqliteImpl> pImpl;
};

#endif // CSVTOQSQLITE_H
