#print("HTTP/1.1 200 OK\r\n Content-Length: 1235\r\nContent-Type: text/plain\r\n\r\nHello, World!\n")

import cgi
import http.cookies as cookies

# create a cookie
c = cookies.SimpleCookie()
c['my_cookie'] = 'my_value'

# create a response
response_body = 'Cookie set!'
response_headers = [
    ('Content-type', 'text/plain'),
    ('Set-Cookie', c['my_cookie'].OutputString())
]

# format the response
response = 'HTTP/1.1 200 OK\r\n'
for header in response_headers:
    response += '{}: {}\r\n'.format(header[0], header[1])
response += '\r\n{}'.format(response_body)

# send the response
print(response)
