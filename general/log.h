#ifndef LOG_H
#define LOG_H

#ifndef LOG_VERBOSITY
#define LOG_VERBOSITY 1
#endif

#define LOG_VERBOSITY_EXTRA 1
#define LOG_VERBOSITY_NORMAL 5

#ifdef LOG_VERBOSITY
#define LOG(verb) if((LOG_VERBOSITY) >= (verb)) 
#else
#define LOG(verb) if (false)
#endif

#define _log LOG(LOG_VERBOSITY_NORMAL)
#define _LOG LOG(LOG_VERBOSITY_EXTRA)

#endif // LOG_H