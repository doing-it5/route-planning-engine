import urllib.request
import json
import ssl
import sys

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE

url = 'https://api.github.com/repos/sathviknalla/route-planning-engine/actions/jobs/101527959660/logs'
req = urllib.request.Request(url)
req.add_header('User-Agent', 'Mozilla/5.0')
# Note: we might get a 302 redirect here. urllib handles it by default.
try:
    with urllib.request.urlopen(req, context=ctx) as response:
        logs = response.read().decode('utf-8')
        with open('job_log.txt', 'w', encoding='utf-8') as f:
            f.write(logs)
        print("Logs downloaded successfully.")
except Exception as e:
    print(f"Error: {e}")
