from config_generator import ConfigGenerator

# server type =
# ConfigGenerator(
#    server_name='localhost',
#    ports=[8080],
#    client_max_body_size='10m',
#    root='/var/www/html',
#    methods_allowed='GET, POST, DELETE',
#    index='index',
#    locations={
#        'A/': {'root': 'html1', 'index': 'index1.html index1.htm', 'methods_allowed': {'GET', 'POST', 'DELETE'},
#                'autoindex': 'on', 'cgi_index': 'index.php', 'cgi_param': 'SCRIPT_FILENAME'}
#    }
# )

servers = [
    ConfigGenerator(
        server_name='localhost',
        ports=[8080],
        client_max_body_size='10m',
        root='/var/www/html',
        index='indexmain.html indexmain.htm',
        locations={
            'A/': {'root': 'html1', 'index': 'index1.html index1.htm', 'methods_allowed': 'POST', 'autoindex': 'on'},
            'B/': {'root': 'html2', 'index': 'index2.html index2.htm index2'}
        }
    ),
    ConfigGenerator(
        server_name='localhost2',
        ports=[8090],
        client_max_body_size='20m',
        methods_allowed='GET DELETE',
        root='/var/www/php/',
        locations={
            'cgi-bin/': {'cgi_index': 'index.php', 'cgi_param': 'SCRIPT_FILENAME '}
        }
    )
]
