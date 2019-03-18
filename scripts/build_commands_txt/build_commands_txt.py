import re


class ExportedMethodLoader:
    CPP_TO_TIER1_VAR_TYPES = {
        'void': '',
        'int': 'integer',
        'float': 'float',
        'char*': 'string',
        'constchar*': 'string',
    }

    def __init__(self, filename: str):
        self._filename = filename
        with open(filename, 'r') as f:
            lines = ''.join(f.readlines())
        self.pages = self._get_page_tags(lines)
        methods = self._get_methods(lines)
        self._assign_methods_to_pages(self.pages, methods)
        print(self.pages)

    @classmethod
    def _get_tier_1_var_type(cls, cpp_var_type):
        return cls.CPP_TO_TIER1_VAR_TYPES[cpp_var_type.replace(' ', '')]

    @classmethod
    def _get_page_tags(cls, lines: str):
        pages = []
        for match in re.finditer(r'(?:/\*+\s*@page\s+)(?P<tag_info>(?:(?!\*+/).)*)\*+/',
                                 lines, re.DOTALL | re.MULTILINE):
            name, sep, header = match.group('tag_info').partition('\n')
            pages.append({
                'name': name.strip(),
                'header': header.strip(),
                'start': match.start(),
                'filename': '{}.md'.format(name.strip().replace(' ', '-')),
            })
        return pages

    @classmethod
    def _get_methods(cls, lines: str):
        type_pattern = r'void\s+|int\s+|(?:const\s+)?char\s*\*\s*|float\s+'
        name_pattern = r'[A-Za-z0-9_]+'
        methods = []
        for match in re.finditer(r'(?:/\*+(?P<comment>(?:(?!\*+/).)*)\*+/\n)?'
                                 r'^extern "C" DLL_EXPORT '
                                 # type name (params)
                                 r'(?P<type>{type_pattern})(?P<name>{id_name})\((?P<params>[^)]+?)?\)'
                                 r';'.format(type_pattern=type_pattern, id_name=name_pattern),
                                 lines, re.DOTALL | re.MULTILINE):
            # Parse method name and return type.
            # method_declaration = declaration_pattern.match(match['method'])
            method = {
                'name': match['name'],
                'return_type': cls._get_tier_1_var_type(match['type']),
                'start': match.start(),
                'params': [],
            }
            # Parse parameter names and types.
            if match['params']:
                for param in match['params'].split(','):
                    param_match = re.match(r'(?P<type>{type_pattern})(?P<name>{name_pattern})'.
                                           format(type_pattern=type_pattern, name_pattern=name_pattern),
                                           param.strip())
                    method['params'].append({
                        'name': param_match['name'],
                        'type': cls._get_tier_1_var_type(param_match['type'])
                    })
            # Parse comment tags
            cls._load_method_tags(method, match['comment'])
            methods.append(method)
        return methods

    @classmethod
    def _load_method_tags(cls, method, comment):
        if not comment:
            print('{} has no documentation.'.format(method['name']))
            return

        def get_param_index(params, name):
            return next((i for i, p in enumerate(params) if p["name"] == name), None)

        def process_desc_tag(tag_text):
            method['desc'] = tag_text

        def process_return_tag(tag_text):
            method['return_desc'] = tag_text

        def process_param_tag(tag_text):
            param_name, sep, tag_text = tag_text.partition(' ')
            index = get_param_index(method['params'], param_name)
            if index is None:
                print('{} has a description for an unknown parameter: {}'.format(method['name'], param_name))
            elif not tag_text:
                print('{} has an empty description for parameter: {}'.format(method['name'], param_name))
            else:
                method['params'][index]['desc'] = tag_text

        def process_param_api_tag(tag_text):
            param_name, sep, tag_text = tag_text.partition(' ')
            index = get_param_index(method['params'], param_name)
            if index is None:
                print('{} has an API reference for an unknown parameter: {}'.format(method['name'], param_name))
            elif not tag_text:
                print('{} has an empty API reference for parameter: {}'.format(method['name'], param_name))
            else:
                method['params'][index]['api'] = tag_text

        def process_api_tag(tag_text):
            method['api'] = [api.strip() for api in tag_text.split(',')]

        def process_plugin_name_tag(tag_text):
            method['plugin_name'] = tag_text

        method_apis = []
        for tag in re.finditer(r'@(?P<name>[-a-z_0-9]+)\s+(?P<text>(?:(?!@).)*)', comment, re.DOTALL | re.MULTILINE):
            tag_name = tag['name']
            process_function = locals().get('process_{}_tag'.format(tag['name'].replace("-", "_")))
            if process_function:
                process_function(tag['text'].strip())
            else:
                print('{} has an unknown tag: {}'.format(method['name'], tag_name))
            # # elif tag_name == 'page':
            # #     # Rather than spend time finding a way to do things such that page tags don't get captured here,
            # #     # just ignore them when found.
            # #     pass
        # Final validation checks
        if not method['desc']:
            print("{} has no description.".format(method['name']))
        if bool(method['return_type']) != bool('return_desc' in method):
            print("{} has a return type without a return description.".format(method['name']))
        for param in method['params']:
            if 'desc' not in param:
                print("{} has a parameter without a description: {}".format(method['name'], param['name']))

    @classmethod
    def _assign_methods_to_pages(cls, pages, methods):
        for page in pages:
            page['methods'] = []
        for method in methods:
            page = [p for p in pages if p['start'] <= method['start']][-1]
            page['methods'].append(method)

    def write_commands_txt(self, out_file: str):
        var_type_letter = {
            '': '0',
            'integer': 'I',
            'float': 'F',
            'string': 'S',
        }
        with open(out_file, 'w') as f:
            f.write('#CommandName,ReturnType,ParameterTypes,Windows,Linux,Mac,Android,iOS,Windows64\n')
            for page in self.pages:
                f.write('#\n')
                f.write('# {0}\n'.format(page['name']))
                f.write('#\n')
                for method in page['methods']:  # type: dict
                    param_types = ''.join([var_type_letter[v['type']] for v in method['params']]) \
                        if method['params'] else '0'
                    f.write('{plugin_method_name},{return_type},{param_types},'
                            '{windows},{linux},{mac},{android},{ios},{windows64}\n'
                            .format(plugin_method_name=method.get('plugin_name', method['name']),
                                    return_type=var_type_letter[method['return_type']],
                                    param_types=param_types,
                                    windows=method['name'],
                                    linux='0',
                                    mac='0',
                                    android='0',
                                    ios='0',
                                    windows64=method['name'],
                                    )
                            )


loader = ExportedMethodLoader('../../SteamPlugin/Common/DllMain.h')
# ../../AGKPlugin/SteamPlugin/
loader.write_commands_txt('Commands.txt')
