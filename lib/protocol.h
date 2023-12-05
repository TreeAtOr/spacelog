#include <sys/types.h>
#define MAX 255
enum LOG_LEVEL {
    info = 1,
    warn = 2,
    error= 3
};

struct LoggingRequest {
    unsigned int cid;
    LOG_LEVEL log_level;
    char msg[MAX];
};

struct SuccessRequest {
    long double result;
};

struct Request {
    bool isLogging;
    struct LoggingRequest logging;
    struct SuccessRequest success;
};

