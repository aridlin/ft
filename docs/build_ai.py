#!/usr/bin/env python3
"""Build a plain-text, dependency-free FT documentation bundle for LLM retrieval."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
from urllib.parse import urljoin

DOCS = Path(__file__).resolve().parent
REPO = DOCS.parent
BASE = 'https://aridlin.pl/ft/ai/'
TOPICS = ['api', 'configuration', 'lifecycle', 'widgets', 'layout-and-style',
          'backends', 'web', 'getting-started', 'examples-and-troubleshooting']

def git(*args):
    return subprocess.check_output(['git', *args], cwd=REPO, text=True).strip()

def normalize_links(text):
    def replace(match):
        label, target = match.groups()
        if re.match(r'https?://', target):
            return match[0]
        file, separator, anchor = target.partition('#')
        if file == 'index.md':
            url = BASE
        elif file in ('learn.md', 'contributing.md'):
            url = 'https://aridlin.pl/ft/docs/' + file.replace('.md', '.html')
        elif file.startswith('../examples/'):
            url = 'https://aridlin.pl/ft/docs/repository-examples/' + file[len('../examples/'):]
        elif file.startswith('screenshots/'):
            url = 'https://aridlin.pl/ft/docs/' + file
        else:
            url = urljoin(BASE, file)
        return f'[{label}]({url}{separator}{anchor})'
    # A C++ lambda such as [](const char*) is not a Markdown link.
    parts = re.split(r'(```.*?```)', text, flags=re.S)
    return ''.join(part if i % 2 else re.sub(r'\[([^\]]+)\]\(([^)]+)\)', replace, part)
                   for i, part in enumerate(parts))

def build(output):
    output.mkdir(parents=True, exist_ok=True)
    intro = (DOCS/'ai/index.md').read_text()
    api = (DOCS/'api.md').read_text()
    fields = []
    for line in (DOCS/'configuration.md').read_text().splitlines():
        m = re.match(r'\| `([^`]+)` \| `([^`]+)` \| `([^`]+)` \| (.*?) \|$', line)
        if m:
            name, kind, default, meaning = m.groups()
            fields.append(dict(name=name, type=kind.strip(), default=default, description=meaning))
    functions, types = [], []
    for section in re.split(r'^## ', api, flags=re.M)[1:]:
        title = section.splitlines()[0]
        if title == 'Types':
            for name, decl in re.findall(r'^### (\w+)\n\n```cpp\n(.*?)\n```', section, re.M|re.S):
                types.append(dict(name=name, declaration=decl))
            continue
        for code in re.findall(r'```cpp\n(.*?)\n```', section, re.S):
            for signature in code.splitlines():
                match = re.search(r'(\w+)\(', signature)
                if match:
                    functions.append(dict(name=match[1], signature=signature, category=title))
    # Fail loudly if the hand-maintained reference no longer describes the source surface.
    header = (REPO/'ft.hpp').read_text()
    facade = header[header.rindex('\nnamespace ft {'):]
    config = re.search(r'^struct Config \{(.*?)^\};', facade, re.M|re.S)[1]
    source_fields = re.findall(r'^\s*[\w:* ]+\s+(\w+)\s*=.*;', config, re.M)
    assert set(source_fields) == {f['name'] for f in fields}, 'Config reference drift'
    for function in functions:
        assert re.search(r'\b'+re.escape(function['name'])+r'\(', facade), function['name']
    snapshot = {
        'repository': 'https://github.com/aridlin/ft',
        'git_head': git('rev-parse', 'HEAD'),
        'header_sha256': hashlib.sha256((REPO/'ft.hpp').read_bytes()).hexdigest(),
        'header_differs_from_head': bool(git('diff', 'HEAD', '--', 'ft.hpp')),
        'scope': 'Checked-out ft.hpp; may include unpublished local changes. Compare header_sha256, not just git_head.',
    }
    stamp = ('Snapshot: `'+snapshot['git_head']+'`; header SHA-256: `'+snapshot['header_sha256']+'`.\n\n'
             'This reference describes the checked-out header'+(' including local changes' if snapshot['header_differs_from_head'] else '')+'. Consult manifest.json before assuming release compatibility.\n\n')
    for name in ('index.txt', 'llms.txt'):
        (output/name).write_text(intro+'\n## Source snapshot\n\n'+stamp)
    full = [intro, '\n## Source snapshot\n\n'+stamp]
    for topic in TOPICS:
        text = normalize_links((DOCS/f'{topic}.md').read_text())
        (output/f'{topic}.md').write_text(text)
        full.append(f'\n---\n\nSource: {BASE}{topic}.md\n\n{text}')
    (output/'examples').mkdir(exist_ok=True)
    for source in sorted((DOCS/'examples').glob('*.cpp')):
        shutil.copy2(source, output/'examples'/source.name)
        full.append(f'\n---\n\n# Complete example: {source.name}\n\n```cpp\n{source.read_text()}```\n')
    (output/'llms-full.txt').write_text('\n'.join(full))
    inventory = {'schema_version':1, 'namespace':'ft', 'snapshot':snapshot,
                 'functions':functions, 'configuration':fields, 'types':types,
                 'aliases': {'TableRows':'std::vector<std::vector<std::string>>',
                             'CommandHandler':'bool (*)(const char*)'},
                 'behavior_reference':BASE+'llms-full.txt'}
    (output/'api.json').write_text(json.dumps(inventory, indent=2)+'\n')
    (output/'.htaccess').write_text('DirectoryIndex index.txt\nOptions -Indexes\nAddType text/plain .txt .md .cpp\nAddType application/json .json\nAddCharset UTF-8 .txt .md .cpp .json\n')
    manifest = {'schema_version':1, 'base_url':BASE, 'snapshot':snapshot,
                'function_overloads':len(functions), 'config_fields':len(fields), 'type_declarations':len(types),
                'files':[]}
    for path in sorted(output.rglob('*')):
        if path.is_file() and path.name not in ('manifest.json', '.htaccess'):
            manifest['files'].append({'path':path.relative_to(output).as_posix(),
                                     'bytes':path.stat().st_size,
                                     'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
    (output/'manifest.json').write_text(json.dumps(manifest, indent=2)+'\n')
    print(f'Built {len(functions)} overloads, {len(fields)} fields, {len(types)} types in {output}')

if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,default=REPO/'build/ai')
    build(parser.parse_args().output.resolve())
