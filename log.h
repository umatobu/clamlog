#ifndef LOG_H
#define LOG_H

#include <QScopedPointer>

#define ClamLogInfo Clam::Log(__FILE__,__LINE__,__FUNCTION__).info

namespace Clam {


class Log
{
public:
    explicit Log(const char *file, int line, const char *function);
    ~Log();

    Log& info();

    void operator << (int nummber);

protected:
    void log(const QString &message);

private:
    class LogPrivate;
    LogPrivate *d_ptr;
};

} // namespace Clam

#endif // LOG_H
