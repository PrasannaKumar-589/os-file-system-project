#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pwd.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))
#define LOG_FILE    "file_access_log.txt"

/**
 * Creates a timestamped backup of the target file (Linux version).
 */
void backup_file(const char *filename) {
    char timestamp[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", t);

    char command[512];
    sprintf(command, "mkdir -p backups && cp %s backups/%s_%s.bak", filename, filename, timestamp);
    
    if (system(command) == 0) {
        printf("[BACKUP] Created backup in backups/ folder\n");
    }
}

/**
 * Logs the file access event to a text file with timestamp and user info.
 */
void log_event(const char *filename, const char *event_type) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0; // Remove trailing newline from ctime

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    
    // Log format: [Timestamp] File: name | Event: TYPE | User: name (UID: id)
    fprintf(log, "[%s] File: %s | Event: %s | User: %s (UID: %d)\n",
            time_str, filename, event_type, pw ? pw->pw_name : "Unknown", uid);
    
    // Also print to console for real-time feedback
    printf("[%s] %s event detected on %s by %s\n", time_str, event_type, filename, pw ? pw->pw_name : "Unknown");

    fclose(log);

    // Trigger backup on modification
    if (strcmp(event_type, "MODIFY") == 0) {
        backup_file(filename);
    }
}

int main() {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init failed");
        return 1;
    }

    const char *file_to_watch = "target.txt";
    
    // Add watch for Open, Access (Read), and Modify (Write)
    int wd = inotify_add_watch(fd, file_to_watch, IN_OPEN | IN_MODIFY | IN_ACCESS);

    if (wd == -1) {
        printf("Error: Could not watch file '%s'. Ensure the file exists.\n", file_to_watch);
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    printf("--- File Access Logger Started ---\n");
    printf("Monitoring: %s\n", file_to_watch);
    printf("Logs will be saved to: %s\n", LOG_FILE);
    printf("Press Ctrl+C to stop.\n\n");

    char buffer[BUF_LEN];

    while (1) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read failed");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->mask & IN_OPEN) {
                log_event(file_to_watch, "OPEN");
            } else if (event->mask & IN_ACCESS) {
                log_event(file_to_watch, "READ");
            } else if (event->mask & IN_MODIFY) {
                log_event(file_to_watch, "MODIFY");
            }

            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
