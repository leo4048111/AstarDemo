#ifndef LOG_H
#define LOG_H

#include <QObject>

class Log: public QObject
{
    Q_OBJECT

private:
    Log();

    static Log* m_instance;

public:
    enum class LogType
    {
        normal = 0,
        error,
        runtime
    };

public:
    static Log* getInstance();

    void log(const QString str, const LogType type);

signals:
    void signal_log(const QString str, const Log::LogType type);

};

#define LOG(str, type) \
{\
    Log::getInstance()->log(str, type);\
}

#endif // LOG_H
