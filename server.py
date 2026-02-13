import http.server
import socketserver
import json
import sqlite3
import hashlib
import os
import urllib.parse
import urllib.request
from urllib.parse import urlparse, parse_qs

PORT = 8082
DB_FILE = "/usr/local/VitalNet/vitalnet.db"

# reCAPTCHA Keys
RECAPTCHA_SECRET_KEY = "6LdtIWosAAAAAI57haSL3IZQn0iQWrXBQGi0evBM"

def init_db():
    print(f"Initializing database at {DB_FILE}...")
    conn = sqlite3.connect(DB_FILE)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS users
                 (username TEXT PRIMARY KEY, password TEXT, name TEXT, license TEXT)''')
    conn.commit()
    conn.close()
    print("Database initialized successfully.")

def hash_password(password):
    return hashlib.sha256(password.encode()).hexdigest()

class VitalNetHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        # Redirect root to login.html
        if path == '/' or path == '':
            self.path = '/login.html'
            return super().do_GET()
            
        # Handle API endpoints
        if path == '/login':
            self.handle_login_get(parsed_path)
        elif path == '/air-info':
            self.send_json({"temperature": 24.5, "humidity": 45, "quality": "Good"})
        elif path == '/water-info':
            self.send_json({"ph": 7.2, "turbidity": 0.5, "temp": 18.2})
        elif path == '/system-info':
            self.send_json({"status": "running", "uptime": "PC Runtime", "version": "PC-1.0"})
        else:
            # Clean URLs: /inicio -> /inicio.html
            full_path = os.path.join('/usr/local/VitalNet/data/pages', path.lstrip('/'))
            if not os.path.exists(full_path) and os.path.exists(full_path + '.html'):
                self.path = path + '.html'
            
            # Serve static files
            return super().do_GET()

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        
        if self.path == '/register':
            self.handle_register(post_data)
        elif self.path == '/login':
            self.handle_login_post(post_data)

    def verify_recaptcha(self, response_token):
        if not response_token:
            return False
            
        try:
            url = "https://www.google.com/recaptcha/api/siteverify"
            params = urllib.parse.urlencode({
                'secret': RECAPTCHA_SECRET_KEY,
                'response': response_token
            }).encode('utf-8')
            
            request = urllib.request.Request(url, data=params)
            with urllib.request.urlopen(request) as response:
                result = json.loads(response.read().decode())
                return result.get('success', False)
        except Exception as e:
            print(f"Error verifying reCAPTCHA: {e}")
            return False

    def handle_login_get(self, parsed_path):
        params = parse_qs(parsed_path.query)
        username = params.get('username', [None])[0]
        password = params.get('password', [None])[0]
        recaptcha_response = params.get('g-recaptcha-response', [None])[0]
        
        if not self.verify_recaptcha(recaptcha_response):
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"reCAPTCHA verification failed")
            return

        if self.verify_user(username, password):
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"OK")
        else:
            self.send_response(401)
            self.end_headers()
            self.wfile.write(b"Invalid credentials")

    def handle_login_post(self, data):
        try:
            params = json.loads(data)
            username = params.get('username')
            password = params.get('password')
            recaptcha_response = params.get('g-recaptcha-response')

            if not self.verify_recaptcha(recaptcha_response):
                self.send_error_response(400, "reCAPTCHA verification failed")
                return

            if self.verify_user(username, password):
                self.send_json({"status": "success"})
            else:
                self.send_error_response(401, "Invalid credentials")
        except:
            self.send_error_response(400, "Invalid JSON")

    def handle_register(self, data):
        try:
            params = json.loads(data)
            username = params.get('email')
            password = params.get('password')
            name = params.get('name')
            license = params.get('license')
            recaptcha_response = params.get('g-recaptcha-response')

            if not self.verify_recaptcha(recaptcha_response):
                self.send_error_response(400, "reCAPTCHA verification failed")
                return

            conn = sqlite3.connect(DB_FILE)
            c = conn.cursor()
            try:
                c.execute("INSERT INTO users VALUES (?, ?, ?, ?)", 
                          (username, hash_password(password), name, license))
                conn.commit()
                self.send_response(201)
                self.end_headers()
                self.wfile.write(b"User created")
            except sqlite3.IntegrityError:
                self.send_error_response(409, "User already exists")
            finally:
                conn.close()
        except Exception as e:
            self.send_error_response(400, str(e))

    def verify_user(self, username, password):
        if not username or not password: return False
        conn = sqlite3.connect(DB_FILE)
        c = conn.cursor()
        c.execute("SELECT password FROM users WHERE username=?", (username,))
        row = c.fetchone()
        conn.close()
        if row and row[0] == hash_password(password):
            return True
        return False

    def send_json(self, data):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def send_error_response(self, code, message):
        self.send_response(code)
        self.end_headers()
        self.wfile.write(message.encode())

if __name__ == "__main__":
    init_db()
    os.chdir('/usr/local/VitalNet/data/pages')
    with socketserver.TCPServer(("", PORT), VitalNetHandler) as httpd:
        print("-" * 50)
        print(f"VitalNet PC Edition is now running!")
        print(f"Access it at: http://localhost:{PORT}")
        print(f"Database: {os.path.abspath(os.path.join('/usr/local/VitalNet', DB_FILE))}")
        print("-" * 50)
        httpd.serve_forever()




