#ifndef LOG_H
#define LOG_H

#ifndef LOG_VERBOSITY
#define LOG_VERBOSITY 1
#endif

#define LOG_VERBOSITY_GIGA 1
#define LOG_VERBOSITY_EXTRA 3
#define LOG_VERBOSITY_NORMAL 5

#ifdef LOG_VERBOSITY
#define LOG(verb) if((LOG_VERBOSITY) >= (verb)) 
#else
#define LOG(verb) if (false)
#endif

#define _log LOG(LOG_VERBOSITY_NORMAL)
#define _Log LOG(LOG_VERBOSITY_EXTRA)
#define _LOG LOG(LOG_VERBOSITY_GIGA)

#endif // LOG_H