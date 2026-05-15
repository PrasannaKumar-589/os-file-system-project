# File Access Logger - Project Report

**Department of Computer Science & Engineering - AIML**  
**Vasavi College of Engineering (Autonomous)**

---

## 1. Abstract
The **File Access Logger** is an OS-based project designed to monitor and log access to specified files in a Linux/Unix system. Using the `inotify` API, the tool captures every access event (**read, write, open**) in real-time. It logs essential metadata including filenames, event types, timestamps, and User IDs (UID). This project serves as a practical exploration of system programming, audit logging, and process management.

## 2. Algorithm
1. **Initialize**: Invoke `inotify_init()` to create an inotify instance.
2. **Watch**: Define the target file and add a watch using `inotify_add_watch()` with flags `IN_OPEN`, `IN_ACCESS`, and `IN_MODIFY`.
3. **Monitor Loop**: 
   - Read event data into a buffer.
   - Iterate through events to identify the mask type.
4. **Metadata Extraction**:
   - Fetch current system time using `time()`.
   - Retrieve User ID via `getuid()` and map to username using `getpwuid()`.
5. **Logging**: Append the formatted entry to `file_access_log.txt`.
6. **Advanced Logic**: If the event is `MODIFY`, trigger the `backup_file()` function.
7. **Cleanup**: On termination, remove the watch and close file descriptors.

## 3. Tools & Technologies
- **OS**: Linux (Ubuntu recommended)
- **Language**: C
- **Compiler**: GCC
- **API**: `<sys/inotify.h>`
- **Advanced UI**: HTML5/CSS3/JS for Dashboard Visualization

## 4. Implementation Details
The implementation focuses on the `inotify` event-driven architecture, ensuring minimal CPU overhead while maintaining high precision in logging. The project also includes a Windows compatibility layer for demonstration purposes.

## 5. Advanced Features: Automatic Backup
To enhance data integrity, this project includes an **Automatic Backup** mechanism. 
- **Trigger**: Any `MODIFY` event on the target file.
- **Action**: The program creates a copy of the file in a `backups/` directory.
- **Naming Convention**: `filename_YYYYMMDD_HHMMSS.bak`

## 6. Sample Output
```text
[Mon Apr 07 13:45:01 2025] File: target.txt | Event: OPEN | User: alice (UID: 1001)
[Mon Apr 07 13:45:02 2025] File: target.txt | Event: READ | User: alice (UID: 1001)
[Mon Apr 07 13:45:05 2025] File: target.txt | Event: MODIFY | User: alice (UID: 1001)
```

## 7. Conclusion
The project successfully demonstrates real-time file monitoring. It highlights the power of system-level APIs for security auditing and user accountability. The addition of the backup feature and visual dashboard elevates the project to a functional data protection tool.
