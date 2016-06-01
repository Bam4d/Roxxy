import requests
from multiprocessing import Pool
from progressbar import ProgressBar, SimpleProgress
from time import sleep
import numpy as np


def do_request(rand):
    
    url = "http://doom.import.io"
    
    req = requests.post("http://localhost:8055",
            json = {
                    "url": url,
                    "png": True,
                })
    
    
    return req

rands = np.random.random((400,))
pool = Pool(50)

results = []

pbar = ProgressBar(widgets=[SimpleProgress()], maxval=len(rands)).start()

r = [pool.apply_async(do_request, (x,), callback=results.append) for x in rands]

while len(results) != len(rands):
    pbar.update(len(results))
    sleep(0.5)
pbar.finish()

success = 0
error = 0
busy = 0
for r in results:
    if r.status_code == 200:
        assert 'png' in r.json()
        success += 1
    elif r.status_code == 400:
        error += 1
    elif r.status_code == 420:
        busy += 1
        
print "success %d" % success
print "busy %d" % busy
print "error %d" % error

print "complete!"