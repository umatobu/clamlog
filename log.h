#ifndef LOG_H
#define LOG_H

#include <QScopedPointer>

#define uInfo UmaTobu::Log(__FILE__,__LINE__,__FUNCTION__).info
#define uDebug UmaTobu::Log(__FILE__,__LINE__,__FUNCTION__).debug
#define uWarning UmaTobu::Log(__FILE__,__LINE__,__FUNCTION__).warning
#define uCritical UmaTobu::Log(__FILE__,__LINE__,__FUNCTION__).critical
#define uFatal UmaTobu::Log(__FILE__,__LINE__,__FUNCTION__).fatal

namespace UmaTobu {


class Log
{
public:
    explicit Log(const char *file, int line, const char *function);
    ~Log();

    Log& info();
    Log& debug();
    Log& warning();
    Log& critical();
    Log& fatal();

    void operator << (int nummber);
    void operator << (QString message);

protected:
    void log(const QString &message);
    QString format(const QString &message);

private:
    class LogPrivate;
    LogPrivate *d_ptr;
};

} // namespace Clam

#endif // LOG_H
