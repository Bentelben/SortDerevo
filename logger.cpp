#include "logger.h"

#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

static void SPrintTime(char *buffer, size_t n, char const *timespec);
static int MakeLogDirectory(logger_t *logger);
static int UpdateLatestLog(logger_t logger);

static void SPrintTime(char *const buffer, size_t const n, char const *const timespec) {
    assert(buffer);
    assert(timespec);

    time_t const t = time(NULL);
    struct tm *const time = localtime(&t);
    strftime(buffer, n, timespec, time);
}

static int MakeLogDirectory(logger_t *const logger) {
    assert(logger);

    char buffer[512+6] = "";
    snprintf(buffer, 512+6, "mkdir %s", logger->dirPath);
    return system(buffer);
}

int LoggerInitialize(logger_t *const logger) {
    assert(logger);

    size_t id = (size_t)logger;
    logger->id = id;

    char timeStringBuffer[256] = "";
    SPrintTime(timeStringBuffer, 256, "%Y_%m_%d__%H_%M_%S");
    snprintf(logger->dirPath, 512, LOGGER_DIR "/%s__id%zu", timeStringBuffer, id);
    snprintf(logger->logPath, 1024, "%s/log.html", logger->dirPath);
    return MakeLogDirectory(logger);
}

FILE *LoggerStartEvent(logger_t logger, char const *title) {
    assert(title);
     
    FILE *const file = fopen(logger.logPath, "a");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Unable to open log file\n");
        return NULL;
    }

    fprintf(file, "<h2>Event: `%s`</h4>\n", title);
    char timeStringBuffer[256] = "";
    SPrintTime(timeStringBuffer, 256, "%d %B %Y, %H:%M:%S");
    fprintf(file, "<h4>%s</h7>\n", timeStringBuffer);
    fprintf(file, "<pre>\n");
    return file;
}

static int UpdateLatestLog(logger_t logger) {
    char buffer[1024+19];
    snprintf(buffer, 1024+19, "cp %s latest_log.html", logger.logPath);
    return system(buffer);
}
 

void LoggerEndEvent(logger_t logger, FILE *file) {
    assert(file);

    fprintf(file, "\n</pre>\n");
    fclose(file);
    UpdateLatestLog(logger);
}
