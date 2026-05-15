const dummyLogs = [
    { time: "Mon Apr 07 13:45:01 2025", file: "target.txt", event: "OPEN", user: "alice", uid: 1001 },
    { time: "Mon Apr 07 13:45:02 2025", file: "target.txt", event: "READ", user: "alice", uid: 1001 },
    { time: "Mon Apr 07 13:45:05 2025", file: "target.txt", event: "MODIFY", user: "alice", uid: 1001 },
    { time: "Mon Apr 07 13:48:12 2025", file: "target.txt", event: "OPEN", user: "root", uid: 0 },
    { time: "Mon Apr 07 13:48:15 2025", file: "target.txt", event: "READ", user: "root", uid: 0 },
];

function renderLogs(logs) {
    const body = document.getElementById('logBody');
    body.innerHTML = '';

    let reads = 0, writes = 0, opens = 0;

    logs.forEach(log => {
        const tr = document.createElement('tr');
        
        if (log.event === 'READ') reads++;
        if (log.event === 'MODIFY') writes++;
        if (log.event === 'OPEN') opens++;

        const tagClass = `tag-${log.event.toLowerCase()}`;
        
        tr.innerHTML = `
            <td style="color: var(--text-dim); font-family: 'JetBrains Mono'">${log.time}</td>
            <td><strong>${log.file}</strong></td>
            <td><span class="event-tag ${tagClass}">${log.event}</span></td>
            <td>
                <div class="user-context">
                    <div class="avatar">${log.user[0].toUpperCase()}</div>
                    <span>${log.user} <span style="color: var(--text-dim)">(ID: ${log.uid})</span></span>
                </div>
            </td>
        `;
        body.appendChild(tr);
    });

    document.getElementById('totalEvents').innerText = logs.length;
    document.getElementById('readEvents').innerText = reads;
    document.getElementById('writeEvents').innerText = writes;
    document.getElementById('openEvents').innerText = opens;
}

// Parser for the actual log file
document.getElementById('uploadBtn').addEventListener('click', () => {
    document.getElementById('logFileInput').click();
});

document.getElementById('logFileInput').addEventListener('change', function(e) {
    const file = e.target.files[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = function(e) {
        const content = e.target.result;
        const lines = content.split('\n');
        const parsedLogs = [];

        lines.forEach(line => {
            // Regex to parse: [Timestamp] File: name | Event: TYPE | User: name (UID: id)
            const regex = /\[(.*?)\] File: (.*?) \| Event: (.*?) \| User: (.*?)(?: \(UID: (\d+)\))?$/;
            const match = line.match(regex);
            if (match) {
                parsedLogs.unshift({
                    time: match[1],
                    file: match[2],
                    event: match[3],
                    user: match[4],
                    uid: match[5] || 'N/A'
                });
            }
        });
        renderLogs(parsedLogs);
    };
    reader.readAsText(file);
});

document.getElementById('refreshBtn').addEventListener('click', () => {
    // Add a random log to simulate activity
    const types = ['OPEN', 'READ', 'MODIFY'];
    const type = types[Math.floor(Math.random() * types.length)];
    const newLog = {
        time: new Date().toString().split(' ').slice(0, 5).join(' '),
        file: "target.txt",
        event: type,
        user: "prasanna",
        uid: 1002
    };
    dummyLogs.unshift(newLog);
    renderLogs(dummyLogs);
});

// Initial Render
renderLogs(dummyLogs);
