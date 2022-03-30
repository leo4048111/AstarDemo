#include "log.h"

Log* Log::m_instance = nullptr;

Log::Log()
{
}

Log* Log::getInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new Log();
    }

    return m_instance;
}

void Log::log(const QString str, const Log::LogType type)
{
    emit signal_log(str, type);
}
