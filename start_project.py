import http.server
import socketserver
import webbrowser
import subprocess
import os
import time

PORT = 8000
DIRECTORY = "."

class Handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Allow Cross-Origin requests (CORS)
        self.send_header('Access-Control-Allow-Origin', '*')
        super().end_headers()

def run_server():
    print(f"--- Starting Dashboard Server on http://localhost:{PORT} ---")
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        webbrowser.open(f"http://localhost:{PORT}/dashboard_standalone.html")
        httpd.serve_forever()

if __name__ == "__main__":
    # 1. Compile the C code
    print("Compiling Logger...")
    subprocess.run(["gcc", "file_logger_windows.c", "-o", "logger_win", "-ladvapi32"])

    # 2. Start the Logger in a new terminal window
    print("Launching Logger in new window...")
    os.system("start cmd /k logger_win.exe")

    # 3. Start the Web Server
    try:
        run_server()
    except KeyboardInterrupt:
        print("\nShutting down...")
