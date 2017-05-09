#include "csvtoqsqlite.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>

using HeaderPair = QPair<QString, QString>;

struct CsvToQSqliteImpl
{
    QString inputPath;
    QString outputPath;
    QString separator = ";";

    QSqlDatabase db;
};

bool createTable(CsvToQSqliteImpl* pImpl, const QString& tableName, const QList<HeaderPair>& fullHeaders)
{
    QString query = QString("CREATE TABLE '%0'(").arg(tableName);
    QString parameterSeparator = ",";
    for(auto pair: fullHeaders)
        query += QString("'%0' %1%2").arg(pair.first).arg(pair.second).arg(parameterSeparator);

    query.remove(query.lastIndexOf(parameterSeparator), parameterSeparator.length());
    query += ")";

    pImpl->db.exec(query);
    return !(pImpl->db.lastError().isValid());
}

bool insertData(CsvToQSqliteImpl* pImpl, const QString& tableName, const QString& columns, const QStringList& values)
{
    QString parameterSeparator = ",";
    QString valueList;
    for(auto value: values)
        valueList += QString("'%0'%1").arg(value).arg(parameterSeparator);
    valueList.remove(valueList.lastIndexOf(parameterSeparator), parameterSeparator.length());

    QString query = QString("INSERT INTO '%0' (%1) VALUES (%2)").arg(tableName).arg(columns).arg(valueList);

    pImpl->db.exec(query);
    qDebug() << query << pImpl->db.lastError();

    return !(pImpl->db.lastError().isValid());
}


CsvToQSqlite::CsvToQSqlite()
 : pImpl(new CsvToQSqliteImpl)
{
}

CsvToQSqlite::~CsvToQSqlite()
{

}

void CsvToQSqlite::setInput(const QString &inputPath)
{
    if(QFileInfo(inputPath).exists())
        pImpl->inputPath = inputPath;
}

void CsvToQSqlite::setOutput(const QString &outputPath)
{
    pImpl->outputPath = outputPath;
}

void CsvToQSqlite::setSeparator(const QString &separator)
{
    pImpl->separator = separator;
}

bool CsvToQSqlite::process()
{
    if(pImpl->inputPath.isEmpty() || pImpl->outputPath.isEmpty() || pImpl->separator.isEmpty())
        return false;

    QFile input(pImpl->inputPath);
    if(!input.open(QIODevice::ReadOnly))
        return false;

    QFile output(pImpl->outputPath);
    if(output.exists())
        output.remove();

    pImpl->db = QSqlDatabase::addDatabase("QSQLITE");
    pImpl->db.setDatabaseName(pImpl->outputPath);
    if(!pImpl->db.open())
        return false;

    QTextStream stream(&input);
    QString headerLine = stream.readLine();
    QStringList headers = headerLine.split(pImpl->separator);
    QString firstDataLine = stream.readLine();
    QStringList firstLineValues = firstDataLine.split(pImpl->separator);

    if(headerLine.isEmpty() || firstDataLine.isEmpty() || headers.size() != firstLineValues.size())
        return false;

    QList<HeaderPair> fullHeaders;
    for(int index = 0 ; index < headers.size() ; index++)
    {
        HeaderPair currentHeader;
        currentHeader.first = headers[index];
        bool ok = true;
        auto currentValue = firstLineValues[index];
        if(currentValue.contains(","))
            currentHeader.second = "REAL";
        else if(currentValue.toInt(&ok) && ok)
            currentHeader.second = "NUMERIC";
        else
            currentHeader.second = "TEXT";

        fullHeaders += currentHeader;
    }

    QString tableName = QFileInfo(pImpl->inputPath).baseName();

    if(!createTable(pImpl.data(), tableName, fullHeaders))
        return false;

    QString parameterSeparator = ",";
    QString columnNames;
    for(auto column: headers)
        columnNames += QString("'%0'%2").arg(column).arg(parameterSeparator);
    columnNames.remove(columnNames.lastIndexOf(parameterSeparator), parameterSeparator.length());

    if(pImpl->db.driver()->hasFeature(QSqlDriver::Transactions))
        pImpl->db.transaction();

    insertData(pImpl.data(), tableName, columnNames, firstLineValues);

    while(!stream.atEnd())
    {
        QString currentLine = stream.readLine();
        insertData(pImpl.data(), tableName, columnNames, currentLine.split(pImpl->separator));
    }

    if(pImpl->db.driver()->hasFeature(QSqlDriver::Transactions))
        if(!pImpl->db.commit())
            return false;


    return true;
}
