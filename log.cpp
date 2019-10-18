#include "log.h"
#include <QDateTime>
#include <QThread>
#include <QTextStream>

namespace Clam {

typedef enum {
    INFO,       // Analysis information directed to supporters
    DEBUG,      // Analysis debug information directed to developers
    WARNING,    // A warning, signalizing a deformity, without challenging the core operation
    CRITICAL,   // An critical, maybe that challenges the core operation
    FATAL       // Fatal error, the program execution has to be aborted
}Level;
const QStringList CLAMLOG_LEVEL_STRINGLIST = {QString("info,debg,warn,crit,fatl").split(",")};

struct LogContext
{
    LogContext() = default;
    LogContext(const char *file_in, int line_in, const char *function_in)
        : file{file_in}
        , line{line_in}
        , function{function_in}
    {}
    LogContext(const LogContext &context)
        : file{context.file}
        , line{context.line}
        , function{context.function}
    {}

    LogContext& operator = (const LogContext& rhs)
    {
        file = rhs.file;
        line = rhs.line;
        function = rhs.function;
        return *this;
    }

    const char *file{nullptr};
    int line{0};
    const char *function{nullptr};
};

class Log::LogPrivate
{
public:
    LogPrivate()
        : m_context()
    {}

    Level       m_level;
    LogContext  m_context;
};

Log::Log(const char *file, int line, const char *function)
    : d_ptr(new LogPrivate)
{
    d_ptr->m_context = LogContext(file, line, function);
}

Log::~Log()
{
    delete d_ptr;
    d_ptr = NULL;
}

Log &Log::info()
{
    d_ptr->m_level = Level::INFO;
    return *this;
}

void Log::operator <<(int nummber)
{
    log(QString::number(nummber));
}

void Log::log(const QString &message)
{
    QString lvl_console_str;
#ifdef CLAMLOG_COLOR_UNIX_STYLE
    switch (d_ptr->m_level)
    {
    case Level::INFO :
        lvl_console_str = "\033[0;32;1m"+CLAMLOG_LEVEL_STRINGLIST[Level::INFO]+"\033[0m";//green
        break;
    case Level::DEBUG :
        lvl_console_str = "\033[0;33;1m"+CLAMLOG_LEVEL_STRINGLIST[Level::DEBUG]+"\033[0m";//yellow
        break;
    case Level::WARNING :
        lvl_console_str = "\033[0;36;1m"+CLAMLOG_LEVEL_STRINGLIST[Level::WARNING]+"\033[0m";//cyan
        break;
    case Level::CRITICAL :
        lvl_console_str = "\033[0;35;1m"+CLAMLOG_LEVEL_STRINGLIST[Level::CRITICAL]+"\033[0m";//magenta
        break;
    case Level::FATAL :
        lvl_console_str = "\033[0;31;1m"+CLAMLOG_LEVEL_STRINGLIST[Level::FATAL]+"\033[0m";//red
        break;
    }
#endif

    QString final_msg("[%{yyyy}-%{MM}-%{dd} %{hh}:%{mm}:%{ss}:%{zzz}][%{t}][%{l}]: %{d}");
    QDateTime curDataTime = QDateTime::currentDateTime();
    final_msg.replace("%{yyyy}", curDataTime.toString("yyyy"));
    final_msg.replace("%{MM}", curDataTime.toString("MM"));
    final_msg.replace("%{dd}", curDataTime.toString("dd"));
    final_msg.replace("%{hh}", curDataTime.toString("hh"));
    final_msg.replace("%{mm}", curDataTime.toString("mm"));
    final_msg.replace("%{ss}", curDataTime.toString("ss"));
    final_msg.replace("%{zzz}", curDataTime.toString("zzz"));
    final_msg.replace("%{t}", "0x" + QString("%1").arg(qulonglong(QThread::currentThread()), 10, 16, QLatin1Char('0')));
    final_msg.replace("%{f}", QString(d_ptr->m_context.file));
    final_msg.replace("%{r}", QString::number(d_ptr->m_context.line));
    final_msg.replace("%{n}", QString(d_ptr->m_context.function));
    final_msg.replace("%{l}", lvl_console_str);
    final_msg.replace("%{d}", message);

    QTextStream out(stdout);
    out << final_msg;

#ifdef Q_OS_WIN32
    out << "\r";
#endif
    out << "\n";
}

} // namespace Clam
