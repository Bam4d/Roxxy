import requests
import json
import base64
import uuid
import pytest

def pytest_generate_tests(metafunc):
    # called once per each test function
    funcarglist = metafunc.cls.params[metafunc.function.__name__]
    argnames = list(funcarglist[0])
    metafunc.parametrize(argnames, [[funcargs[name] for name in argnames]
            for funcargs in funcarglist])

class TestRoxxyAPI:
    
    params = {
        'test_httpbin_status_code': [dict(code=401),dict(code=400),dict(code=403),dict(code=500),dict(code=200)],
        'test_httpbin_redirect': [dict(code=301),dict(code=302)],
         'test_html_endpoint': [{}],
         'test_png_endpoint': [{}],
         'test_render_png': [{}],
        #'test_visual_features_flag': [dict(visualFeatures=True),dict(visualFeatures=False)],
        #'test_user_agent': [dict(user_agent="Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_3) AppleWebKit/601.4.4 (KHTML, like Gecko) Version/9.0.3 Safari/601.4.4")],
        #'test_response_time': [dict(response_time=1, resource_timeout=2, error_reason=None),
        #                    dict(response_time=2, resource_timeout=1, error_reason="network5")],
        #'test_global_timeout': [dict(response_time=40, resource_timeout=60)],
        #'test_js_timeout': [{}]
    }

    @pytest.fixture
    def test_request(self, url, png=None, visualFeatures=False,
                    user_agent="Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_3) AppleWebKit/601.4.4 (KHTML, like Gecko) Version/9.0.3 Safari/601.4.4",
                    resource_timeout=10):

        req = requests.post("http://localhost:8055",
            json = {
                    "url": url,
                    "png": png,
                })
        return req
    
    def test_httpbin_status_code(self, code):
        req = self.test_request("http://httpbin.org/status/%d" % code)
        roxxy_response = json.loads(req.text)
        # Assert that roxxy returns 200 as the request is complete
        assert req.status_code == 200

        # Assert that roxxy reports the correct response code from the website being rendered
        assert roxxy_response['statusCode'] == code

    def test_httpbin_redirect(self, code):
        req = self.test_request("http://httpbin.org/status/%d" % code)
        roxxy_response = json.loads(req.text)
        # Assert that roxxy returns 200 as the request is complete
        assert req.status_code == 200

        # Assert that roxxy reports the correct response code from the website being rendered
        assert roxxy_response['statusCode'] == 200
    
    def test_html_endpoint(self):
        # Test that when the png is requested that it gets rendered and sent back on the API as base64
        req = requests.get("http://localhost:8055/html", 
            params = {
                    "url": "http://doom.import.io?q=" + str(uuid.uuid4())
                })
        
        assert req.status_code == 200
        
    def test_png_endpoint(self):
        # Test that when the png is requested that it gets rendered and sent back on the API as base64
        req = requests.get("http://localhost:8055/png",
            params = {
                    "url": "http://doom.import.io?q=" + str(uuid.uuid4())
                })
        
        assert req.status_code == 200

    def test_render_png(self):
        # Test that when the png is requested that it gets rendered and sent back on the API as base64
        req = self.test_request("http://doom.import.io?q=" + str(uuid.uuid4()), png=False)
         
        assert req.status_code == 200
        req_json = json.loads(req.text)
         
        assert 'png' not in req_json 
        
        # Should not render a png by default if not specified
        req = self.test_request("http://doom.import.io?q=" + str(uuid.uuid4()))
         
        assert req.status_code == 200
        req_json = json.loads(req.text)
         
        assert 'png' not in req_json 
         
        req = self.test_request("http://doom.import.io?q=" + str(uuid.uuid4()), png=True)
         
        assert req.status_code == 200
        req_json = json.loads(req.text)
         
        assert 'png' in req_json
        
        with open("png_test.png", 'w') as png_test:
         
            binary_png = bytearray(base64.b64decode(req_json['png']))
             
            png_test.write(binary_png)
            
#     def test_user_agent(self, user_agent):
#         req = self.test_request("http://doom.import.io/php/miscellaneous%20tools/header_reflector.php", user_agent=user_agent)
#         req_json = json.loads(req.text)
# 
#         # Assert that roxxy return 200 as the request is complete
#         assert req.status_code == 200
# 
#         # search for the user agent in the content of the page
#         assert re.search(re.escape(user_agent), req_json['html'])
        
    
    