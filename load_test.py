import requests
from multiprocessing import Pool
import numpy as np


def do_request(rand):
    
    print "sending request"
    url = "http://doom.import.io"
    
    req = requests.post("http://localhost:8055",
            json = {
                    "url": url,
                    "png": True,
                })
    
    print "recieved response"
    
    return req

rands = np.random.random((1000,))
pool = Pool(5)
results = pool.map(do_request, rands.tolist())


success = 0
error = 0
busy = 0
for r in results:
    if r.status_code == 200:
        success += 1
    elif r.status_code == 400:
        error += 1
    elif r.status_code == 420:
        busy += 1
        
print "success %d" % success
print "busy %d" % busy
print "error %d" % error

print "complete!"