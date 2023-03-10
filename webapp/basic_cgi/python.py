#print("HTTP/1.1 200 OK\r\nSet-Cookie: first_cookie=first_value\r\nContent-Type: text/plain\r\n Host: TestHost\r\n\r\nHello, World!\n")

#import cgi
#import http.cookies as cookies

#c = cookies.SimpleCookie()
#c['second_cookie'] = 'second_value'

#response_body = 'Cookie set!'
#response_headers = [
#    ('Content-type', 'text/plain'),
#    ('Set-Cookie', c['second_cookie'].OutputString())
#]

#response = 'HTTP/1.1 200 OK\r\n'
#for header in response_headers:
#    response += '{}: {}\r\n'.format(header[0], header[1])
#response += '\r\n{}'.format(response_body)

#print(response)

#!/usr/bin/env python3
import cgi

form = cgi.FieldStorage()

if "name" in form:
    name = form["name"].value

    print("Content-Type: text/html")
    print()
    print(f"<h1>Hello, {name}!</h1>")
else:
    print("Content-Type: text/html")
    print()
    print("<h1>Error: Missing name field in POST request</h1>")
