import os
import unittest
import requests
import subprocess
import time
import threading
from config_editor import servers

# can you process a post request without CGI for a form
# add a test where we try an HTTP request with a different version

# Generate config file
config_file = [server.generate() for server in servers]

# Write config file to disk
with open('default.conf', 'w') as f:
    for server in servers:
        config_file = server.generate()
        f.write(config_file)

class TestWebServer(unittest.TestCase):
    SERVER_EXECUTABLE = '../../webserv'
    DEFAULT_CONF_PATH = 'default.conf'
    CGI_DIR = '/var/www/cgi-bin'
    NUMBER_REQUESTS = 100
    PORTS = [server.ports[0] for server in servers]

    @classmethod
    def setUp(cls):
        # Start the server
        # Change path if other conf file
        cls.webserv = subprocess.Popen(
            [cls.SERVER_EXECUTABLE, cls.DEFAULT_CONF_PATH])
        time.sleep(1)

    def tearDown(cls):
        # Stop the web server after running the tests
        cls.webserv.kill()

######################################################################

    def test_server_running(self):
        # Assert that the web server is running
        response = requests.get(f'http://localhost:{self.PORTS[0]}/')
        self.assertEqual(response.status_code, 200)

        response = requests.get(f'http://localhost:{self.PORTS[1]}/')
        self.assertEqual(response.status_code, 200)

########################################################################

    #def test_default_error_pages(self):
    #    # Test that the server serves default error pages for 404 and 500 errors
    #    response = requests.get(
    #        f'http://localhost:{self.PORTS[0]}/nonexistent_page.html')
    #    self.assertEqual(response.status_code, 404)
    #    self.assertIn('404 Not Found', response.text)

    #    response = requests.get(f'http://localhost:{self.PORTS[0]}/error')
    #    self.assertEqual(response.status_code, 500)
    #    self.assertIn('500 Internal Server Error', response.text)

    #    response = requests.put(f'http://localhost:{self.PORTS[0]}/index.html')
    #    self.assertEqual(response.status_code, 405)
    #    self.assertIn('405 Method Not Allowed', response.text)

########################################################################

#    def test_http_methods(self):
#        # Test that the server can handle POST, GET, and DELETE requests
#        response = requests.post(
#            f'http://localhost:{self.PORTS[0]}/', data={'name': 'Alice'})
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Hello, Alice!', response.text)

#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/', params={'name': 'Bob'})
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Hello, Bob!', response.text)

#        response = requests.delete(
#            f'http://localhost:{self.PORTS[0]}/', params={'name': 'Charlie'})
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Goodbye, Charlie!', response.text)

#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/directory.html')
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Directory listing for /var/www/html', response.text)

#        response = response.get(f'http://localhost:{self.PORTS[1]}/test.cgi')
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('CGI output', response.text)

########################################################################

#    def test_large_file_get_handling(self):
#        # Send a request to the server for a large file
#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/large_file.txt')

#        # Check that the server responds with a 200 OK status code
#        self.assertEqual(response.status_code, 200)

#        # Check that the server streams the file back to the client
#        content_length = int(response.headers.get('Content-Length', 0))
#        self.assertGreater(content_length, 0)

#        # Choose a reasonable limit for the file size
#        self.assertLess(content_length, 1000000)

#        # Check that the response content is correct
#        expected_content = b'Lorem ipsum dolor sit amet'
#        self.assertEqual(response.content, expected_content)

########################################################################

#    def test_chunked_encoding(self):
#        # Send a request with chunked encoding header to the server
#        headers = {'Transfer-Encoding': 'chunked'}
#        response = requests.post(f'http://localhost:{self.PORTS[0]}/chunked',
#                                 headers=headers, data='5\r\nHello\r\n6\r\n, world\r\n0\r\n\r\n')

#        # Check that the response is correct
#        self.assertEqual(response.status_code, 200)
#        self.assertEqual(response.text, 'Hello, world')

########################################################################

#    def test_redirection(self):
#        # Send a request to the server for a redirected URL
#        response = requests.get(f'http://localhost:{self.PORTS[0]}/redirect')

#        # Check that the server responds with a 302 Found status code
#        self.assertEqual(response.status_code, 302)

#        # Check that the server redirected to the correct URL
#        self.assertEqual(
#            response.headers['Location'], 'http://localhost:8080/new_location')

########################################################################

#    def test_large_file_post_handling(self):
#        # Create a large file to send as the request body
#        with open('large_file.txt', 'wb') as f:
#            f.write(os.urandom(1024 * 1024 * 50))  # 50 MB file

#        # Send a POST request to the server with the large file as the body
#        with open('large_file.txt', 'rb') as f:
#            response = requests.post(
#                f'http://localhost:{self.PORTS[0]}/upload', data=f)

#        # Check that the server responded with a 200 OK status code
#        self.assertEqual(response.status_code, 200)

#        # Check that the server returned the correct response body
#        self.assertIn('Successfully uploaded', response.text)

#        # Clean up the test file
#        os.remove('large_file.txt')

#        # get the file again to compare if it's the same


########################################################################

#    def test_http_version(self):
#        # Test that the server is HTTP 1.1 compliant
#        response = requests.get(f'http://localhost:{self.PORTS[0]}/',
#                                headers={'Connection': 'close'})
#        self.assertEqual(response.status_code, 200)
#        self.assertNotIn('Connection: keep-alive', response.headers)

#        response = requests.get(f'http://localhost:{self.PORTS[0]}/',
#                                headers={'Connection': 'keep-alive'})
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Connection: keep-alive', response.headers)

########################################################################

#    def test_persistent_connection(self):
#        # Send a GET request to the server with HTTP 1.1 headers and a persistent connection
#        headers = {'Host': 'localhost:8080', 'Connection': 'keep-alive'}
#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/', headers=headers)

#        # Check that the server responds correctly with a 200 OK status code and the expected response body
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Hello, world!', response.text)

#        # Send a second GET request over the same connection and check the response
#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/', headers=headers)

#        # Check that the server responds correctly with a 200 OK status code and the expected response body
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Hello, world!', response.text)

#        # Send a third GET request over the same connection and check the response
#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/', headers=headers)

#        # Check that the server responds correctly with a 200 OK status code and the expected response body
#        self.assertEqual(response.status_code, 200)
#        self.assertIn('Hello, world!', response.text)

########################################################################

#    def test_request_timeout(self):
#        # Test that the server does not hang forever on a request
#        response = requests.get(
#            f'http://localhost:{self.PORTS[0]}/slow_request', timeout=1)
#        self.assertEqual(response.status_code, 500)

########################################################################

#    def test_concurrent_requests(self):
#        # Define a function that sends a single request
#        def send_request():
#            response = requests.get(f'http://localhost:{self.PORT[0]}/')
#            self.assertEqual(response.status_code, 200)
#            self.assertIn('Hello, world!', response.text)

#        # Create a list of threads that will send the requests
#        threads = []
#        for i in range(self.NUMBER_REQUESTS):
#            threads.append(threading.Thread(target=send_request))

#        # Start the threads
#        for thread in threads:
#            thread.start()

#        # Wait for all threads to finish
#        for thread in threads:
#            thread.join()


if __name__ == '__main__':
    unittest.main()
