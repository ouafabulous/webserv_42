import os
import cgi
import http.cookies as cookies
from datetime import datetime, timedelta

c = cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))

form = cgi.FieldStorage()

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")

if "name" in form and form["name"].value:
    name = form["name"].value
    c['myCookie'] = name
    expires = datetime.utcnow() + timedelta(hours=10)
    c['myCookie']['expires'] = expires.strftime('%a, %d-%b-%Y %H:%M:%S GMT')
    print(c.output())

print()

print("""
<html>
  <head>
    <title>My Python Form</title>
    <link rel="stylesheet" href="style.css">
  </head>
  <body class="python-body">
    """)
if 'myCookie' in c:
    print(f"<h1>Hello {c['myCookie'].value} from Python script</h1>")
print("""<form method="post" action="php.php">
      <label for="name" class="python-button">Enter your name:</label>
      <input type="text" name="name" id="name" class="python-button">
      <input type="submit" value="Submit" class="python-button">
    </form>
  </body>
</html>
    """)
