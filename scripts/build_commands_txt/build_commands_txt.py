import os
import re

WIKI_PATH = "../../../agk-steam-plugin.wiki/"
error_count = 0


def report_error(text):
    global error_count
    error_count += 1
    print("ERROR:\t" + text)


class ExportedMethodLoader:
    CPP_TO_TIER1_VAR_TYPES = {
        'void': None,
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
                    # print("Loading {}...".format(filename))
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
            name, sep, desc = match.group('tag_info').partition('\n')
            pages.append({
                'name': name.strip(),
                'desc': desc.strip(),
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
                'start': match.start(),
                'params': [],
            }
            return_type = cls._get_tier_1_var_type(match['type'])
            if return_type:
                method['return_type'] = return_type
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
            if 'desc' in method:
                report_error('{} has multiple desc tags.'.format(method['name']))
                return
            method['desc'] = tag_text

        def process_return_tag(tag_text):
            if 'return_desc' in method:
                report_error('{} has multiple return-desc tags.'.format(method['name']))
                return
            method['return_desc'] = tag_text

        def process_return_url_tag(tag_text):
            if 'return_url' in method:
                report_error('{} has multiple return-url tags.'.format(method['name']))
                return
            method['return_url'] = tag_text

        def process_param_tag(tag_text):
            param_name, sep, tag_text = tag_text.partition(' ')
            if not tag_text:
                report_error('{} has an empty description for parameter: {}'.format(method['name'], param_name))
                return
            index = get_param_index(method['params'], param_name)
            if index is None:
                report_error('{} has a description for an unknown parameter: {}'.format(method['name'], param_name))
                return
            if 'desc' in method['params'][index]:
                report_error('{} has multiple param tags for: {}'.format(method['name'], param_name))
                return
            method['params'][index]['desc'] = tag_text

        def process_param_url_tag(tag_text):
            param_name, sep, tag_text = tag_text.partition(' ')
            if not tag_text:
                report_error('{} has an empty URL for parameter: {}'.format(method['name'], param_name))
                return
            index = get_param_index(method['params'], param_name)
            if index is None:
                report_error('{} has a URL for an unknown parameter: {}'.format(method['name'], param_name))
                return
            if 'url' in method['params'][index]:
                report_error('{} has multiple param-url tags for: {}'.format(method['name'], param_name))
                return
            method['params'][index]['url'] = tag_text

        # def process_api_tag(tag_text):
        #     method['api'] = [api.strip() for api in tag_text.split(',')]

        def process_url_tag(tag_text):
            if ',' in tag_text or '\n' in tag_text:
                report_error('{} had a url tag with multiple urls.'.format(method['name']))
                return
            if 'url' not in method:
                method['url'] = []
            if tag_text in method['url']:
                report_error('{} had a duplicate url entry: {}'.format(method['name'], tag_text))
                return
            method['url'].append(tag_text)

        def process_plugin_name_tag(tag_text):
            if 'plugin_name' in method:
                report_error('{} already had a plugin-name tag.'.format(method['name']))
                return
            method['plugin_name'] = tag_text

        def process_callback_type_tag(tag_text):
            if tag_text not in ['list', 'bool', 'callresult']:
                report_error("{} has an unknown callback type: {}".format(method['name'], tag_text))
                return
            if 'callback-type' in method:
                report_error('{} has multiple callback-type tags.'.format(method['name']))
                return
            method['callback-type'] = tag_text

        def process_callback_getters_tag(tag_text):
            if 'callback-getters' in method:
                report_error('{} has multiple callback-getters tags.'.format(method['name']))
                return
            method['callback-getters'] = [name.strip() for name in tag_text.split(',')]

        def process_callbacks_tag(tag_text):
            if 'callbacks' in method:
                report_error('{} has multiple callbacks tags.'.format(method['name']))
                return
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
        if 'return_type' in method:
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
                if method['callback-getters'] == '':
                    report_error('{} has a empty callback-getters tag.'.format(method['name']))
                    return
                for getter in method['callback-getters']:
                    method_index = next((i for i, m in enumerate(methods) if m["name"] == getter), None)
                    if method_index is None:
                        report_error('{} has an unknown callback getter method: {}'.format(method['name'], getter))
                    else:
                        if 'callback-parents' not in methods[method_index]:
                            methods[method_index]['callback-parents'] = []
                        methods[method_index]['callback-parents'].append(method['name'])
                        methods[method_index]['callback-parent-type'] = method['callback-type']
                        # callback parents attach their @url tags to each @callback-getters method.
                        if 'url' in method:
                            if 'url' not in methods[method_index]:
                                methods[method_index]['url'] = []
                            for url in method['url']:
                                if url not in methods[method_index]['url']:
                                    methods[method_index]['url'].append(url)

    @classmethod
    def _assign_methods_to_pages(cls, pages, methods):
        for page in pages:
            page['methods'] = []
        for method in methods:
            try:
                page = [p for p in pages if p['start'] <= method['start']][-1]
                page['methods'].append(method)
                method['page'] = page
            except IndexError as e:
                report_error("Could not find page for method: {}".format(method['name']))

    def write_commands_txt(self, out_file: str):
        var_type_letter = {
            None: '0',
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
                                    return_type=var_type_letter[method.get('return_type')],
                                    param_types=param_types,
                                    windows=method['name'],
                                    linux='0',
                                    mac='0',
                                    android='0',
                                    ios='0',
                                    windows64=method['name'],
                                    )
                            )

    @classmethod
    def _get_url_title(cls, url: str):
        # TODO Handle this?
        #  [Downloadable Content (DLC)](https://partner.steamgames.com/doc/store/application/dlc)
        #  -> Downloadable Content (DLC)
        base, sep, title = url.rpartition('/')
        # base, sep, title = title.rpartition('#')
        return title

    @classmethod
    def _create_comma_list(cls, text_list):
        if len(text_list) == 0:
            return ""
        if len(text_list) == 1:
            return text_list[0]
        return ', '.join(text_list[0:-1]) + ' or ' + text_list[-1]

    def _get_method_link(self, method_name):
        for page in self.pages:
            if method_name in [m['name'] for m in page['methods']]:
                return '[{0}]({1}#{0})'.format(method_name, page['name'].replace(' ', '-'))
        report_error("Could not generate method link for {}".format(method_name))
        return method_name

    def write_wiki_files(self, base_path: str):
        for page in [p for p in self.pages if p['methods']]:
            with open(base_path + page['filename'], 'w') as f:
                if 'desc' in page:
                    f.write(page['desc'])
                    f.write('\n\n')
                for method in page['methods']:  # type: dict
                    params_list = ', '.join(
                            ('[{name} as {type}]({url})' if 'url' in param else '{name} as {type}').format(
                                    **param) for param in method['params'])
                    syntax = '{name}({params_list}){return_type}'.format(name=method['name'],
                                                                         params_list=params_list,
                                                                         return_type=(' as ' + method['return_type'])
                                                                         if 'return_type' in method else '')
                    # print(syntax)
                    # Output the method wiki.
                    wiki_entry = '## {0}\n'.format(method['name'])
                    wiki_entry += '> {0}\n'.format(syntax)
                    if method['params']:
                        wiki_entry += ''.join('> * _{name}_ - {desc}  \n'.format(**param) for param in method['params'])
                    wiki_entry += '\n'
                    if 'desc' in method:
                        wiki_entry += '{0}\n\n'.format(method['desc'])
                    if 'callback-parents' in method:
                        if method['callback-parent-type'] == 'callresult':
                            parent_links = [self._get_method_link(name) for name in method['callback-parents']]
                            comma_list = self._create_comma_list(parent_links)
                            wiki_entry += '_This method should only be used when the call result returned by {} has ' \
                                          'reported a GetCallResultCode of 1._'.format(comma_list)
                        else:
                            # Only one callback parent for other types.
                            wiki_entry += '_This method should only be used ' \
                                          'when {} has returned 1._'.format(method['callback-parents'][0])
                        wiki_entry += '\n\n'
                    if 'return_desc' in method:
                        if 'return_url' in method:
                            wiki_entry += '**Returns:** [{return_desc}]({return_url})\n\n'.format(**method)
                        else:
                            wiki_entry += '**Returns:** {return_desc}\n\n'.format(**method)
                    if 'callback-getters' in method:
                        wiki_entry += '**Associated Methods:**  \n'
                        wiki_entry += '  \n'.join(self._get_method_link(name) for name in method['callback-getters'])
                        wiki_entry += '\n\n'
                    if 'url' in method:
                        wiki_entry += '**Reference:**  \n'
                        wiki_entry += '  \n'.join(('[{0}]({1})'.format(self._get_url_title(url), url)
                                                   for url in method['url']))
                        wiki_entry += '\n\n'
                    f.write(wiki_entry)
                    # print(wiki_entry)


loader = ExportedMethodLoader('../../SteamPlugin/Common/')
loader.write_commands_txt('../../AGKPlugin/SteamPlugin/Commands.txt')
print("Error count: {}".format(error_count))
print("Page count: {}".format(len(loader.pages)))
print("Method count: {}".format(loader.method_count))
loader.write_wiki_files(WIKI_PATH)
