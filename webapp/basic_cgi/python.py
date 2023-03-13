#print("HTTP/1.1 200 OK\r\nSet-Cookie: first_cookie=first_value\r\nContent-Type: text/plain\r\n Host: TestHost\r\n\r\nHello, World!\n")


#########################################################################


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


########################################################################


#!/usr/bin/env python3
import os
import cgi
import http.cookies as cookies
from datetime import datetime, timedelta

c = cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))

form = cgi.FieldStorage()


if "name" in form:
    name = form["name"].value
    c['myCookie'] = name
    expires = datetime.utcnow() + timedelta(hours=10)
    c['myCookie']['expires'] = expires.strftime('%a, %d-%b-%Y %H:%M:%S GMT')

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
if "name" in form:
    print(c.output())


print()

print("""
<html>
  <head>
    <title>My Python Form</title>
  </head>
  <body>
    """)
if 'myCookie' in c:
    print(f"<h1>Hello {c['myCookie'].value} from Python script</h1>")
print("""<form method="post" action="php.php">
      <label for="name">Enter your name:</label>
      <input type="text" name="name" id="name">
      <input type="submit" value="Submit">
    </form>
    <br>
    <p>""")
#if 'myCookie' in cookies.SimpleCookie(os.environ.get("HTTP_COOKIE")):
#    print("Set-Cookie: myCookie=value")

print("""    </p>
  </body>
</html>
""")
