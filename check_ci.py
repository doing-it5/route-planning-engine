import urllib.request
import json
import ssl

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE

req = urllib.request.Request('https://api.github.com/repos/sathviknalla/route-planning-engine/actions/runs')
req.add_header('User-Agent', 'Mozilla/5.0')
with urllib.request.urlopen(req, context=ctx) as response:
    runs = json.loads(response.read().decode())
    latest = runs['workflow_runs'][0]
    print('Run ID:', latest['id'])
    print('Status:', latest['status'])
    print('Conclusion:', latest['conclusion'])

req2 = urllib.request.Request(f"https://api.github.com/repos/sathviknalla/route-planning-engine/actions/runs/{latest['id']}/jobs")
req2.add_header('User-Agent', 'Mozilla/5.0')
with urllib.request.urlopen(req2, context=ctx) as response:
    jobs = json.loads(response.read().decode())
    for job in jobs['jobs']:
        print(f"  Job: {job['name']} -> {job['conclusion']}")
