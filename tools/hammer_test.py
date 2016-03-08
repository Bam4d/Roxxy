import requests
import json

if __name__ == '__main__':
    count = 0
    #Non-threaded test
    for i in range(0,1000):


        res = requests.post('http://localhost:8055?url=https://import.io')

        count += 1
        #print res.text

        if count%1000 == 0:
            print count




