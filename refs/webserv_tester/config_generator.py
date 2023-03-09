import os


class ConfigGenerator:
    def __init__(self, server_names, port, client_max_body_size, root, locations, index=None, cgi_setup=None, allowed_methods=None,num_servers=1):
        self.num_servers = num_servers
        self.server_names = server_names
        self.port = port
        self.client_max_body_size = client_max_body_size
        self.allowed_methods = allowed_methods
        self.root = root
        self.locations = locations
        self.index = index
        self.cgi_setup = cgi_setup

    def generate(self):
        configs = []
        for i in range(self.num_servers):


            # Generate server config
            server_block = [
                f'server\n{{',
                f'\tlisten {self.port};',
                f'\tserver_names {self.server_names};',
                f'\tclient_max_body_size {self.client_max_body_size};',
                f'\troot {self.root};',
            ]
            if self.allowed_methods is not None:
                server_block.append(
                    f'\tallowed_methods {self.allowed_methods};')
            if self.index is not None:
                server_block.append(f'\tindex {self.index};')


            for location, params in self.locations.items():
                # Generate location config
                location_block = [
                    f'\n\tlocation {location} {{',
                ]
                root = params.get("root")
                if root is not None:
                    location_block.append(f'\t\troot\t{root};')
                index = params.get("index")
                if index is not None:
                    location_block.append(f'\t\tindex\t{index};')
                autoindex = params.get("autoindex")
                if autoindex is not None:
                    location_block.append(f'\t\tautoindex\t{autoindex};')
                allowed_methods = params.get("allowed_methods")
                if allowed_methods is not None:
                    location_block.append(
                        f'\t\tallowed_methods\t{allowed_methods};')
                cgi_setup = params.get("cgi_setup")
                if cgi_setup is not None:
                    location_block.append(f'\t\tcgi_setup\t{cgi_setup};')
                location_block.append('\t}')
                server_block += location_block
            server_block.append('}\n\n')
            configs += server_block
        return '\n'.join(configs)
