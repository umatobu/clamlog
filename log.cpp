#include "log.h"
#include <QDateTime>
#include <QThread>
#include <QTextStream>
#include <QWaitCondition>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#ifdef ULOG_COLOR_UNIX_STYLE
#include <iostream>
#elif ULOG_COLOR_CONSOLE_STYLE
#include <Windows.h>
#elif ULOG_COLOR_MSVC_STYLE
#include <Windows.h>
#else
#include <iostream>
#endif

namespace UmaTobu {

class FileAppender : public QThread
{
public:
    ~FileAppender()
    {
        is_activation_.store(false);
        wait_condition_.notify_all();
        quit();
        wait();
    }

    static FileAppender &instance()
    {
        static FileAppender instance;
        return instance;
    }

    void addMessage(const QString &message)
    {
        QMutexLocker lock(&msg_list_mutex_);
        message_list_.push_back(message);
        wait_condition_.notify_all();
    }

protected:
    void run()
    {
        QString file_pathname = QCoreApplication::applicationDirPath()+"/log/log.txt";
        QFile log_file(file_pathname);
        if(!QFileInfo(file_pathname).absoluteDir().exists())
        {
            QDir().mkpath(QFileInfo(file_pathname).path());
        }
        bool is_open = log_file.open(QIODevice::WriteOnly | QIODevice::Append);

        while(is_activation_.load())
        {

            if(is_open)
            {
                QString one_message;
                {
                    QMutexLocker lock(&(msg_list_mutex_));
                    if(message_list_.size() > 0)
                    {
                        one_message = message_list_.takeFirst();
                    }
                    else
                    {
                        wait_condition_.wait(&(msg_list_mutex_));
                    }
                }

                if(!one_message.isEmpty())
                {
                    while (one_message.right(1) == "\n")
                    {
                        one_message = one_message.mid(0, one_message.size() - 1);
                    }
                    while (one_message.right(1) == "\r")
                    {
                        one_message = one_message.mid(0, one_message.size() - 1);
                    }
#ifdef Q_OS_WIN32
                    one_message.append("\r");
#endif
                    one_message.append("\n");

                    QTextStream out(&log_file);
                    out.setCodec("UTF-8");
                    out << one_message;
                }
            }
            else
            {
                is_open = log_file.open(QIODevice::WriteOnly | QIODevice::Append);
                msleep(1000);
            }


            is_open = log_file.isOpen();
            if(is_open && log_file.size() > 1024*1024*10)
            {
                if(QFile::copy(file_pathname,
                               file_pathname + "." +
                               QDateTime::currentDateTime().toString("yyyy-MM-dd hh_mm_ss") + ".txt"))
                {
                    log_file.close();
                    log_file.remove();
                    is_open = log_file.open(QIODevice::WriteOnly | QIODevice::Append);
                }
            }
        }

        log_file.close();
    }

private:
    explicit FileAppender()
        : is_activation_(true)
    {
        start();
    }

    explicit FileAppender(const FileAppender &) = delete;
    FileAppender & operator=(const FileAppender &) = delete;

    QAtomicInt          is_activation_;
    QWaitCondition      wait_condition_;
    QMutex              msg_list_mutex_;
    QStringList         message_list_;
};

typedef enum {
    INFO = 0,   // Analysis information directed to supporters
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
    {
#ifdef ULOG_COLOR_CONSOLE_STYLE
        out_console_handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
    }

#ifdef ULOG_COLOR_CONSOLE_STYLE
    HANDLE      out_console_handle_;
#endif
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
    d_ptr = nullptr;
}

Log &Log::info()
{
    d_ptr->m_level = Level::INFO;
    return *this;
}

Log &Log::debug()
{
    d_ptr->m_level = Level::DEBUG;
    return *this;
}

Log &Log::warning()
{
    d_ptr->m_level = Level::WARNING;
    return *this;
}

Log &Log::critical()
{
    d_ptr->m_level = Level::CRITICAL;
    return *this;
}

Log &Log::fatal()
{
    d_ptr->m_level = Level::FATAL;
    return *this;
}

void Log::operator <<(int nummber)
{
    log(QString::number(nummber));
}

void Log::operator <<(QString message)
{
    log(message);
}

void Log::log(const QString &message)
{
    QString first_process_msg = format(message);

    QString final_msg = first_process_msg;
    final_msg.replace("%{l}", CLAMLOG_LEVEL_STRINGLIST[d_ptr->m_level]);
    FileAppender::instance().addMessage(final_msg);

#ifdef ULOG_COLOR_UNIX_STYLE
    QString lvl_console_str;
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
    QString unix_msg = first_process_msg.replace("%{l}", lvl_console_str);
    std::cout << unix_msg.toStdString() << std::endl;
#elif ULOG_COLOR_CONSOLE_STYLE
    QString lvl_console_str;
    QStringList msg_str_list = first_process_msg.split("%{l}");
    if(msg_str_list.size() <= 1)
    {
        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 7);
        auto size = static_cast<DWORD>(first_process_msg.size());
        ::WriteConsoleA(d_ptr->out_console_handle_, first_process_msg.toLocal8Bit().data(), size, nullptr, nullptr);
    }
    else
    {
        for (int i = 0; i < msg_str_list.size(); i++)
        {
            auto size = static_cast<DWORD>(msg_str_list.at(i).size());
            ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 7);
            ::WriteConsoleA(d_ptr->out_console_handle_, msg_str_list.at(i).toLocal8Bit().data(), size, nullptr, nullptr);

            if( i != (msg_str_list.size()-1))
            {
                switch (d_ptr->m_level)
                {
                    case Level::INFO :
                        lvl_console_str = CLAMLOG_LEVEL_STRINGLIST[Level::INFO];
                        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 10);//green
                        break;
                    case Level::DEBUG :
                        lvl_console_str = CLAMLOG_LEVEL_STRINGLIST[Level::DEBUG];
                        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 14);//yellow
                        break;
                    case Level::WARNING :
                        lvl_console_str = CLAMLOG_LEVEL_STRINGLIST[Level::WARNING];
                        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 11);//cyan
                        break;
                    case Level::CRITICAL :
                        lvl_console_str = CLAMLOG_LEVEL_STRINGLIST[Level::CRITICAL];
                        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 13);//magenta
                        break;
                    case Level::FATAL :
                        lvl_console_str = CLAMLOG_LEVEL_STRINGLIST[Level::FATAL];
                        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 12);//red
                        break;
                }
                auto size_lvl = static_cast<DWORD>(lvl_console_str.size());
                ::WriteConsoleA(d_ptr->out_console_handle_, lvl_console_str.toLocal8Bit().data(), size_lvl, nullptr, nullptr);
            }
        }
        ::SetConsoleTextAttribute(d_ptr->out_console_handle_, 7);
    }
    ::WriteConsoleA(d_ptr->out_console_handle_, QString("\n").toLocal8Bit().data(), 1, nullptr, nullptr);
#elif ULOG_COLOR_MSVC_STYLE
    final_msg.append("\n");
    OutputDebugStringA(final_msg.toStdString().c_str());
#else
    std::cout << final_msg.toStdString() << std::endl;
#endif
}

QString Log::format(const QString &message)
{
    QString format_msg("[%{yyyy}-%{MM}-%{dd} %{hh}:%{mm}:%{ss}:%{zzz}][%{t}][%{l}]: %{d}");
    QDateTime curDataTime = QDateTime::currentDateTime();
    format_msg.replace("%{yyyy}", curDataTime.toString("yyyy"));
    format_msg.replace("%{MM}", curDataTime.toString("MM"));
    format_msg.replace("%{dd}", curDataTime.toString("dd"));
    format_msg.replace("%{hh}", curDataTime.toString("hh"));
    format_msg.replace("%{mm}", curDataTime.toString("mm"));
    format_msg.replace("%{ss}", curDataTime.toString("ss"));
    format_msg.replace("%{zzz}", curDataTime.toString("zzz"));
    format_msg.replace("%{t}", "0x" + QString("%1").arg(
                           qulonglong(QThread::currentThread()), 10, 16, QLatin1Char('0')));
    format_msg.replace("%{f}", QString(d_ptr->m_context.file));
    format_msg.replace("%{r}", QString::number(d_ptr->m_context.line));
    format_msg.replace("%{n}", QString(d_ptr->m_context.function));
    format_msg.replace("%{d}", message);
    return format_msg;
}

} // namespace Clam
