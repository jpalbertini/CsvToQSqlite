#ifndef CSVTOQSQLITE_GLOBAL_H
#define CSVTOQSQLITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CSVTOQSQLITE_LIBRARY)
#  define CSVTOQSQLITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define CSVTOQSQLITESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CSVTOQSQLITE_GLOBAL_H
