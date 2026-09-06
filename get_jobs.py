import urllib.request
import json
import ssl

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE

req = urllib.request.Request('https://api.github.com/repos/sathviknalla/route-planning-engine/actions/runs/34048578070/jobs')
req.add_header('User-Agent', 'Mozilla/5.0')
with urllib.request.urlopen(req, context=ctx) as response:
    jobs_data = json.loads(response.read().decode())
    for job in jobs_data['jobs']:
        print(f"Job Name: {job['name']}, ID: {job['id']}, Conclusion: {job['conclusion']}")
