#include "log.h"
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

static FILE *m_log_fp        = NULL;
static t_log_level  m_log_threshold = LOG_I;
static t_log_config m_log_config;

int log_init()
{
    char* options;

    // m_log_config.LOG_LEVEL = LOG_I;
    m_log_config.LOG_LEVEL = LOG_D;
    m_log_config.LOG_FILE_PATH = LOG_FILE;
    m_log_config.LOG_ERASE = true;

    options = m_log_config.LOG_ERASE == true ? "w+" : "a";
    m_log_threshold = m_log_config.LOG_LEVEL;
    
    m_log_fp = fopen(m_log_config.LOG_FILE_PATH, options);
    if (!m_log_fp)
    {
        perror("fopen");
        return -1;
    }
    return 0;
}

void log_close(void)
{
    if (m_log_fp)
    {
        fclose(m_log_fp);
        m_log_fp = NULL;
    }
}

static void get_timestamp(char *buf, size_t len)
{
    time_t t = time(NULL);
    struct tm tm_info;
#ifdef _WIN32
    localtime_s(&tm_info, &t);
#else
    localtime_r(&t, &tm_info);
#endif
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm_info);
}

void log_msg(t_log_level level, const char *fmt, ...)
{
    va_list args, args_copy;
    static bool inside_logger = false;

    if (!m_log_fp) return;

    /* avoid circle reference with time api!! */
    if (inside_logger)
        return;
    
    inside_logger = true;

    va_start(args, fmt);
    va_copy(args_copy, args);

    vfprintf(m_log_fp, fmt, args);
    fflush(m_log_fp);

    if ((level <= m_log_threshold) || (level == LOG_B))
    {
        switch (level)
        {
            case LOG_E:
                fprintf(stderr, "\033[1;31m");
                vfprintf(stderr, fmt, args_copy);
                fprintf(stderr, "\033[0m");
                break;
            case LOG_W: /* TODO make it yellow */
                fprintf(stderr, "\033[1;31m");
                vfprintf(stderr, fmt, args_copy);
                fprintf(stderr, "\033[0m");
                break;
            default:
                vfprintf(stdout, fmt, args_copy);
                break;
        }
    }

    va_end(args_copy);
    va_end(args);
    inside_logger = false;
}

void log_msg_time(t_log_level level, const char *fmt, ...)
{
    va_list args, args_copy;
    static bool inside_logger = false;
    char ts[32];

    if (!m_log_fp) return;

    /* avoid circle reference with time api!! */
    if (inside_logger)
        return;
    
    inside_logger = true;

    va_start(args, fmt);
    va_copy(args_copy, args);

    get_timestamp(ts, sizeof(ts));
    fprintf(m_log_fp, "[%s] ", ts);
    vfprintf(m_log_fp, fmt, args);
    fflush(m_log_fp);

    if ((level <= m_log_threshold) || (level == LOG_B))
    {
        switch (level)
        {
            case LOG_E:
                fprintf(stderr, "\033[1;31m[%s] ", ts);
                vfprintf(stderr, fmt, args_copy);
                fprintf(stderr, "\033[0m");
                break;
            case LOG_W: /* TODO make it yellow */
                fprintf(stderr, "\033[1;31m[%s] ", ts);
                vfprintf(stderr, fmt, args_copy);
                fprintf(stderr, "\033[0m");
                break;
            default:
                fprintf(stdout, "[%s] ", ts);
                vfprintf(stdout, fmt, args_copy);
                break;
        }
    }

    va_end(args_copy);
    va_end(args);
    inside_logger = false;
}
