#ifndef CLAMLOG_H
#define CLAMLOG_H

#include <QStringList>

#define cInfo Clam::LogContext(__FILE__,__LINE__,__FUNCTION__).info
#define cDebug Clam::LogContext(__FILE__,__LINE__,__FUNCTION__).debug
#define cWarning Clam::LogContext(__FILE__,__LINE__,__FUNCTION__).warning
#define cCritical Clam::LogContext(__FILE__,__LINE__,__FUNCTION__).critical
#define cFatal Clam::LogContext(__FILE__,__LINE__,__FUNCTION__).fatal

namespace Clam {

typedef enum {
    INFO = 0,   // Analysis information directed to supporters
    DEBUG,      // Analysis debug information directed to developers
    WARNING,    // A warning, signalizing a deformity, without challenging the core operation
    CRITICAL,   // An critical, maybe that challenges the core operation
    FATAL       // Fatal error, the program execution has to be aborted
}Level;
const QStringList CLAMLOG_LEVEL_STRINGLIST = {QString("info,debg,warn,crit,fatl").split(",")};

class LogContext
{
public:
    LogContext(const char *file_in, int line_in, const char *function_in);

    LogContext& info();
    LogContext& debug();
    LogContext& warning();
    LogContext& critical();
    LogContext& fatal();

    void operator << (int number);
    void operator << (QString message);

    const char *file{nullptr};
    int line{0};
    const char *function{nullptr};
    Level lvl{INFO};
};

class Log
{
public:
    ~Log();
    static Log &instance();
    void log(const QString &message, const LogContext &context);
    void quit();

protected:
    QString format(const QString &message, const LogContext &context);

private:
    explicit Log();
    explicit Log(const Log &) = delete;
    Log & operator=(const Log &) = delete;

    class LogPrivate;
    LogPrivate *d_ptr;
};

} // namespace Clam

#endif // CLAMLOG_H
