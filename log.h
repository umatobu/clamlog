#ifndef LOG_H
#define LOG_H

#include <QScopedPointer>

#define ClamLogInfo Clam::Log(__FILE__,__LINE__,__FUNCTION__).info
#define ClamLogDebug Clam::Log(__FILE__,__LINE__,__FUNCTION__).debug
#define ClamLogWarning Clam::Log(__FILE__,__LINE__,__FUNCTION__).warning
#define ClamLogCritical Clam::Log(__FILE__,__LINE__,__FUNCTION__).critical
#define ClamLogInfoFatal Clam::Log(__FILE__,__LINE__,__FUNCTION__).fatal

namespace Clam {


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

protected:
    void log(const QString &message);
    QString format(const QString &message);

private:
    class LogPrivate;
    LogPrivate *d_ptr;
};

} // namespace Clam

#endif // LOG_H
