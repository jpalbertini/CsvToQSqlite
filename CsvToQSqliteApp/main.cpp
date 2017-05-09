#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "csvtoqsqlite.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;

    QCommandLineOption inputOption("input", QCoreApplication::translate("main", "input file."), "test");
    parser.addOption(inputOption);

    QCommandLineOption outputOption("output", QCoreApplication::translate("main", "output file."));
    parser.addOption(outputOption);

    parser.process(a);

    CsvToQSqlite converter;
    converter.setInput(parser.value(inputOption));

    if(parser.isSet(outputOption))
        converter.setOutput(parser.value(outputOption));
    else
        converter.setOutput(parser.value(inputOption).replace("csv", "sqlite"));

    converter.setSeparator(";");

    if(converter.process())
        qDebug() << "ok";
    else
        qDebug() << "ko";
    return a.exec();
}
