import urllib.request
import json
import ssl

ctx = ssl.create_default_context()
ctx.check_hostname = False
ctx.verify_mode = ssl.CERT_NONE

run_id = 34051914370

req = urllib.request.Request(f'https://api.github.com/repos/sathviknalla/route-planning-engine/actions/runs/{run_id}/jobs')
req.add_header('User-Agent', 'Mozilla/5.0')
with urllib.request.urlopen(req, context=ctx) as response:
    jobs = json.loads(response.read().decode())
    for job in jobs['jobs']:
        if job['conclusion'] == 'failure':
            print(f"\n=== FAILED JOB: {job['name']} (ID: {job['id']}) ===")
            for step in job['steps']:
                if step.get('conclusion') == 'failure':
                    print(f"  FAILED STEP: {step['name']} (#{step['number']})")
