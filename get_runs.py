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
    latest_run = runs['workflow_runs'][0]
    print('Latest Run ID:', latest_run['id'])
    print('Conclusion:', latest_run['conclusion'])
