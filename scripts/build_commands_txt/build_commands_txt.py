import os
import re

WIKI_PATH = "../../../agk-steam-plugin.wiki/"
error_count = 0


# TODO Change @api, @param-api, and @return-api to @url, @param-url, @return-url.
#  url can be a list.  urls must be delimited by newlines.
#  Parse url to show anchor if available, otherwise page name.
#  ie:
#  https://partner.steamgames.com/doc/api/ISteamFriends#FriendsGetFollowerCount_t -> FriendsGetFollowerCount_t
#  https://partner.steamgames.com/doc/api/steam_gameserver -> steam_gameserver
#  Handle this?
#  [Downloadable Content (DLC)](https://partner.steamgames.com/doc/store/application/dlc) -> Downloadable Content (DLC)

def report_error(text):
    global error_count
    error_count += 1
    print("ERROR:\t" + text)


class ExportedMethodLoader:
    CPP_TO_TIER1_VAR_TYPES = {
        'void': '',
        'int': 'integer',
        'float': 'float',
        'char*': 'string',
        'constchar*': 'string',
    }

    def __init__(self, path: str):
        # self._filename = filename
        self.pages = []
        self.method_count = 0
        for (root, _, filenames) in os.walk(path):
            for filename in [f for f in filenames if f.endswith('.cpp') or f.endswith('.h')]:
                with open(os.path.join(root, filename), 'r') as f:
                    print("Loading {}...".format(filename))
                    lines = ''.join(f.readlines())
                    pages = self._get_page_tags(lines)
                    methods = self._get_methods(lines)
                    if len(methods):
                        self.method_count += len(methods)
                        self._validate_callback_tags(methods)
                        self._assign_methods_to_pages(pages, methods)
                        self._merge_page_list(pages)
        self.page_count = len(self.pages)
        # Alphabetize the pages
        self.pages.sort(key=lambda page: page['name'])

    def _merge_page_list(self, pages):
        for page in pages:
            page_index = next((i for i, p in enumerate(self.pages) if p["name"] == page['name']), None)
            if page_index is None:
                self.pages.append(page)
            else:
                self.pages[page_index]['methods'].extend(page['methods'])

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
                                 r';?'.format(type_pattern=type_pattern, id_name=name_pattern),
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
            if cls._load_method_tags(method, match['comment']):
                methods.append(method)
        return methods

    @classmethod
    def _load_method_tags(cls, method, comment):
        if not comment:
            report_error('{} has no documentation.'.format(method['name']))
            return True

        if '@ignore' in comment:
            # ignored export.
            return False

        def get_param_index(params, name):
            return next((i for i, p in enumerate(params) if p["name"] == name), None)

        def process_desc_tag(tag_text):
            method['desc'] = tag_text

        def process_return_tag(tag_text):
            method['return_desc'] = tag_text

        # def process_return_api_tag(tag_text):
        #     # TODO remove
        #     method['return_api'] = tag_text
        #
        def process_return_url_tag(tag_text):
            method['return_url'] = tag_text

        def process_param_tag(tag_text):
            param_name, sep, tag_text = tag_text.partition(' ')
            if tag_text:
                index = get_param_index(method['params'], param_name)
                if index is None:
                    report_error('{} has a description for an unknown parameter: {}'.format(method['name'], param_name))
                else:
                    method['params'][index]['desc'] = tag_text
            else:
                report_error('{} has an empty description for parameter: {}'.format(method['name'], param_name))

        # def process_param_api_tag(tag_text):
        #     # TODO remove
        #     param_name, sep, tag_text = tag_text.partition(' ')
        #     if not tag_text:
        #         report_error('{} has an empty API reference for parameter: {}'.format(method['name'], param_name))
        #     index = get_param_index(method['params'], param_name)
        #     if index is None:
        #         report_error('{} has an API reference for an unknown parameter: {}'.format(method['name'], param_name))
        #     else:
        #         method['params'][index]['api'] = tag_text

        def process_param_url_tag(tag_text):
            # TODO
            pass

        # def process_api_tag(tag_text):
        #     method['api'] = [api.strip() for api in tag_text.split(',')]

        def process_url_tag(tag_text):
            if ',' in tag_text or '\n' in tag_text:
                report_error('{} had a url tag with multiple urls.'.format(method['name']))
                return
            # TODO
            pass

        def process_plugin_name_tag(tag_text):
            method['plugin_name'] = tag_text

        def process_callback_type_tag(tag_text):
            if tag_text not in ['list', 'bool', 'callresult']:
                report_error("{} has an unknown callback type: {}".format(method['name'], tag_text))
            else:
                method['callback-type'] = tag_text

        def process_callback_getters_tag(tag_text):
            method['callback-getters'] = [name.strip() for name in tag_text.split(',')]

        def process_callbacks_tag(tag_text):
            # callbacks that fire as a result of the current method.
            method['callbacks'] = [name.strip() for name in tag_text.split(',')]

        method_apis = []
        for tag in re.finditer(r'@(?P<name>[-a-z_0-9]+)\s+(?P<text>(?:(?!@).)*)', comment, re.DOTALL | re.MULTILINE):
            tag_name = tag['name']
            process_function = locals().get('process_{}_tag'.format(tag['name'].replace("-", "_")))
            if process_function:
                process_function(tag['text'].strip())
            else:
                report_error('{} has an unknown tag: {}'.format(method['name'], tag_name))
        # Final validation checks
        if 'desc' not in method:
            report_error("{} has no description.".format(method['name']))
        if method['return_type']:
            if 'return_desc' not in method:
                report_error("{} has a return type without a return description.".format(method['name']))
        else:
            if 'return_desc' in method:
                report_error("{} has a return description without a return type.".format(method['name']))
        for param in method['params']:
            if 'desc' not in param:
                report_error("{} has a parameter without a description: {}".format(method['name'], param['name']))
        return True

    @classmethod
    def _validate_callback_tags(cls, methods):
        for method in methods:
            if 'callback-getters' in method:
                if 'callback-type' not in method:
                    report_error('{} does not have a callback type.'.format(method['name']))
                for getter in method['callback-getters']:
                    method_index = next((i for i, m in enumerate(methods) if m["name"] == getter), None)
                    if method_index is None:
                        report_error('{} has an unknown callback getter method: {}'.format(method['name'], getter))
                    else:
                        methods[method_index]['callback-parent'] = method['name']

    @classmethod
    def _assign_methods_to_pages(cls, pages, methods):
        for page in pages:
            page['methods'] = []
        for method in methods:
            try:
                page = [p for p in pages if p['start'] <= method['start']][-1]
                page['methods'].append(method)
            except IndexError as e:
                report_error("Could not find page for method: {}".format(method['name']))

    def write_commands_txt(self, out_file: str):
        var_type_letter = {
            '': '0',
            'integer': 'I',
            'float': 'F',
            'string': 'S',
        }
        with open(out_file, 'w') as f:
            f.write('#CommandName,ReturnType,ParameterTypes,Windows,Linux,Mac,Android,iOS,Windows64\n')
            for page in [p for p in self.pages if p['methods']]:
                # print('Page {} has {} methods'.format(page['name'], len(page['methods'])))
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


loader = ExportedMethodLoader('../../SteamPlugin/Common/')
loader.write_commands_txt('../../AGKPlugin/SteamPlugin/Commands.txt')
print("Error count: {}".format(error_count))
print("Page count: {}".format(len(loader.pages)))
print("Method count: {}".format(loader.method_count))
