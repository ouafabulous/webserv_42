import os


class ConfigGenerator:
    def __init__(self, server_name, ports, client_max_body_size, root, locations, index=None, cgi_index=None, cgi_param=None, methods_allowed=None,num_servers=1):
        self.num_servers = num_servers
        self.server_name = server_name
        self.ports = ports
        self.client_max_body_size = client_max_body_size
        self.methods_allowed = methods_allowed
        self.root = root
        self.locations = locations
        self.index = index
        self.cgi_index = cgi_index
        self.cgi_param = cgi_param

    def generate(self):
        configs = []
        for i in range(self.num_servers):


            # Generate server config
            server_block = [
                f'server\n{{',
                f'\tlisten {self.ports[i]};',
                f'\tserver_name {self.server_name};',
                f'\tclient_max_body_size {self.client_max_body_size};',
                f'\troot {self.root};',
            ]
            if self.methods_allowed is not None:
                server_block.append(
                    f'\tmethods_allowed {self.methods_allowed};')
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
                methods_allowed = params.get("methods_allowed")
                if methods_allowed is not None:
                    location_block.append(
                        f'\t\tmethods_allowed\t{methods_allowed};')
                cgi_index = params.get("cgi_index")
                if cgi_index is not None:
                    location_block.append(f'\t\tcgi_index\t{cgi_index};')
                cgi_param = params.get("cgi_param")
                if cgi_param is not None:
                    location_block.append(f'\t\tcgi_param\t{cgi_param};')
                location_block.append('\t}')
                server_block += location_block
            server_block.append('}\n\n')
            configs += server_block
        return '\n'.join(configs)
