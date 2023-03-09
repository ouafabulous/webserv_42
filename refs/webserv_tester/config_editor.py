from config_generator import ConfigGenerator

# server type =
# ConfigGenerator(
#    server_name='localhost',
#    ports=[8080],
#    client_max_body_size='10m',
#    root='/var/www/html',
#    allowed_methods='GET, POST, DELETE',
#    index='index',
#    locations={
#        'A/': {'root': 'html1', 'index': 'index1.html index1.htm', 'allowed_methods': {'GET', 'POST', 'DELETE'},
#                'autoindex': 'on', 'cgi_index': 'index.php', 'cgi_param': 'SCRIPT_FILENAME'}
#    }
# )

servers = [
     ConfigGenerator(
         server_names='localhost',
         port=8080,
         client_max_body_size='1000000',
         root='../../webapp/www/',
         index='indexmain.html',
         locations={
             'A/': {'root': 'html1', 'index': 'index1.html', 'allowed_methods': 'POST', 'autoindex': 'on'},
             'B/': {'root': 'html2', 'index': 'index2.html'}
         }
     ),
    ConfigGenerator(
        server_names='localhost2',
        port=8090,
        client_max_body_size='2000000',
        allowed_methods='GET',
        root='../../webapp/basic_cgi',
        locations={
            'cgi-bin/': {'cgi_setup': '.py    /bin/python'}
        }
    ),
]
