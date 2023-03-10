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
import http.cookies as cookies

c = cookies.SimpleCookie()
c['myCookie'] = "hello"

form = cgi.FieldStorage()

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print('set-Cookie', c['myCookie'].OutputString())
print()

if "myVariable" in form:
    name = form["myVariable"].value
    c['myCookie'] = name
print(f"<h1>Hello, {c['myCookie'].OutputString()}!</h1>")
