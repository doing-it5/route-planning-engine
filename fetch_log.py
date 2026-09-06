import urllib.request
import json
import ssl

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE

# Get the ubuntu job that failed at Build
job_id = 101537351595

req = urllib.request.Request(f'https://api.github.com/repos/sathviknalla/route-planning-engine/actions/jobs/{job_id}/logs')
req.add_header('User-Agent', 'Mozilla/5.0')
try:
    with urllib.request.urlopen(req, context=ctx) as response:
        log = response.read().decode('utf-8', errors='replace')
        with open('ubuntu_build_log.txt', 'w', encoding='utf-8') as f:
            f.write(log)
        print("Log saved.")
except Exception as e:
    print(f"Error: {e}")
