#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "file_access_log.txt"

/**
 * Logs the file access event to a text file with timestamp.
 * Note: Getting specific user info on Windows via C is more complex than Linux,
 * so we use the current environment username.
 */
/**
 * Creates a timestamped backup of the target file.
 */
void backup_file(const char *filename) {
    char timestamp[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", t);

    char backup_name[MAX_PATH];
    sprintf(backup_name, "backups/%s_%s.bak", filename, timestamp);

    if (CopyFileA(filename, backup_name, FALSE)) {
        printf("[BACKUP] Created: %s\n", backup_name);
    } else {
        printf("[BACKUP] Failed to create backup. Error: %lu\n", GetLastError());
    }
}

void log_event(const char *filename, const char *event_type) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0; // Remove newline

    char username[256];
    DWORD username_len = sizeof(username);
    GetUserNameA(username, &username_len);

    fprintf(log, "[%s] File: %s | Event: %s | User: %s\n",
            time_str, filename, event_type, username);
    
    printf("[%s] %s event detected on %s by %s\n", time_str, event_type, filename, username);

    fclose(log);

    // Trigger backup on modification
    if (strcmp(event_type, "MODIFY") == 0) {
        backup_file(filename);
    }
}

int main() {
    const char *file_to_watch = "target.txt";
    HANDLE hDir = CreateFileA(
        ".", 
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        printf("Error: Could not open current directory for monitoring.\n");
        return 1;
    }

    printf("--- File Access Logger (Windows Port) Started ---\n");
    printf("Monitoring folder for changes to: %s\n", file_to_watch);
    printf("Press Ctrl+C to stop.\n\n");

    char buffer[1024];
    DWORD bytesReturned;

    while (ReadDirectoryChangesW(
        hDir, buffer, sizeof(buffer), TRUE,
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE,
        &bytesReturned, NULL, NULL)) 
    {
        FILE_NOTIFY_INFORMATION *pNotify;
        int offset = 0;

        do {
            pNotify = (FILE_NOTIFY_INFORMATION *)&buffer[offset];
            int fileNameLen = pNotify->FileNameLength / sizeof(WCHAR);
            char fileName[MAX_PATH];
            wcstombs(fileName, pNotify->FileName, fileNameLen);
            fileName[fileNameLen] = '\0';

            if (strcmp(fileName, file_to_watch) == 0) {
                switch (pNotify->Action) {
                    case FILE_ACTION_MODIFIED:
                        log_event(fileName, "MODIFY");
                        break;
                    case FILE_ACTION_ADDED:
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        log_event(fileName, "CREATED/OPENED");
                        break;
                }
            }

            offset += pNotify->NextEntryOffset;
        } while (pNotify->NextEntryOffset != 0);
    }

    CloseHandle(hDir);
    return 0;
}
