#!/usr/bin/env python3
"""Build the static docs. Dependency: Python-Markdown (see requirements.txt)."""
from pathlib import Path
import argparse
import html
import hashlib
import json
import re
import shutil
import markdown

ROOT = Path(__file__).resolve().parent
PAGES = [
    ('Start here', [('index', 'Overview'), ('learn', 'Your first interface'), ('getting-started', 'Install & build')]),
    ('Guides', [('lifecycle', 'Lifecycle & state'), ('widgets', 'Widgets'), ('layout-and-style', 'Layout & style'), ('web', 'Web integration')]),
    ('Reference', [('api', 'API reference'), ('configuration', 'Configuration'), ('backends', 'Backend behavior'), ('examples-and-troubleshooting', 'Examples & troubleshooting'), ('contributing', 'Maintaining the docs')]),
]

def build(output):
    output.mkdir(parents=True, exist_ok=True)
    for folder in ('assets', 'examples', 'screenshots'):
        shutil.copytree(ROOT / folder, output / folder, dirs_exist_ok=True)
    # Preserve repository-relative example links, without copying source outside docs.
    repo_examples = ROOT.parent / 'examples'
    shutil.copytree(repo_examples, output / 'repository-examples', dirs_exist_ok=True)
    items = [item for _, group in PAGES for item in group]
    search = []
    asset_version = hashlib.sha256((ROOT/'assets/site.css').read_bytes() + (ROOT/'assets/site.js').read_bytes() + b''.join((ROOT/f'{s}.md').read_bytes() for s,t in items)).hexdigest()[:12]
    for index, (slug, title) in enumerate(items):
        source = (ROOT / f'{slug}.md').read_text()
        source = re.sub(r'\]\(../examples/', '](repository-examples/', source)
        source = re.sub(r'\]\(([^)]+)\.md(#[^)]*)?\)', lambda m: '](' + m[1] + '.html' + (m[2] or '') + ')', source)
        converter = markdown.Markdown(extensions=['fenced_code', 'tables', 'toc', 'md_in_html'])
        body = converter.convert(source)
        if slug not in ('index', 'learn'):
            body = re.sub(r'(</h1>)', lambda m: m[1] + '<details class="page-contents"><summary>On this page</summary>' + converter.toc + '</details>', body, count=1)
        if slug == 'index':
            intro = '''<section class="hero"><div class="eyebrow">Small library. A clear place to start.</div><h2>Build an interface.<br>Understand every line.</h2><p>New to GUI programming? Start with a button, watch state change, and follow a frame — then build the same ideas in C++.</p><a class="cta" href="learn.html">Start the interactive guide →</a><div class="cards"><a class="card" href="learn.html#2-a-frame-is-one-pass-through-your-interface"><strong>Watch a frame</strong><span>Play, pause, and step through the loop.</span></a><a class="card" href="learn.html#4-layout-decides-where-widgets-go"><strong>Try a layout</strong><span>Change the ratios. See the code update.</span></a><a class="card" href="api.html"><strong>Find a function</strong><span>113 signatures, organized by purpose.</span></a></div></section>'''
            body = re.sub(r'(</h1>)', lambda m: m[1] + intro, body, count=1)
        nav = ''.join('<div class="navlabel">'+group+'</div>'+''.join(f'<a href="{s}.html"'+(' class="active" aria-current="page"' if s==slug else '')+'>'+html.escape(t)+'</a>' for s,t in entries) for group,entries in PAGES)
        next_link = f'<a href="{items[index+1][0]}.html">Next: {html.escape(items[index+1][1])} →</a>' if index+1<len(items) else '<a href="index.html">Back to overview →</a>'
        page = f'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><meta name="description" content="FT C++ library documentation: {html.escape(title, quote=True)}. Learn immediate-mode GUI, terminal, and web interfaces with interactive examples."><meta name="theme-color" content="#17684f"><title>{html.escape(title)} · FT Docs</title><link rel="icon" href="assets/favicon.svg" type="image/svg+xml"><link rel="stylesheet" href="assets/site.css?v={asset_version}"><script defer src="assets/search-index.js?v={asset_version}"></script><script defer src="assets/site.js?v={asset_version}"></script></head>
<body><a class="skip" href="#main">Skip to content</a><header class="topbar"><a class="brand" href="index.html"><span class="brandmark">ft</span><span>Documentation</span></a><div class="toplinks"><span class="pill">C++17 · Single header</span><a href="https://github.com/aridlin/ft">GitHub ↗</a><button class="mobile-nav" aria-expanded="false" aria-controls="navigation">Menu</button></div></header><div class="shell"><aside class="sidebar" id="navigation"><label for="doc-search" class="navlabel">Search documentation</label><input class="search" id="doc-search" type="search" placeholder="Search concepts or functions…" autocomplete="off"><div class="search-results" aria-live="polite"></div><nav class="nav-groups" aria-label="Documentation">{nav}</nav></aside><main class="main" id="main"><article class="article"><div class="eyebrow">FT / {'Learning path' if slug=='learn' else 'Developer documentation'}</div>{body}<footer class="page-footer"><span>FT · GUI / TUI / Web<br>Interactive previews are browser teaching models.</span>{next_link}</footer></article></main></div></body></html>'''
        (output/f'{slug}.html').write_text(page)
        text = re.sub(r'<[^>]+>', ' ', body)
        search.append({'title':title,'url':f'{slug}.html','text':re.sub(r'\s+',' ',html.unescape(text))})
    (output/'assets/search-index.js').write_text('window.FT_SEARCH = '+json.dumps(search, ensure_ascii=False)+';\n')
    print(f'Built {len(items)} pages in {output}')

if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT.parent/'build/docs')
    args=parser.parse_args()
    build(args.output.resolve())
