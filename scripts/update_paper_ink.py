#!/usr/bin/env python3
"""Rebuild the offline web font/source CSS inside ft.hpp (dev-only fontTools).
The browser-independent C++ native painter is maintained in ft.hpp itself.
"""
from pathlib import Path
from fontTools.ttLib import TTFont
from fontTools import subset
import base64
import io

root = Path(__file__).resolve().parents[1]
assets = root / 'themes' / 'paper-ink'
font = TTFont(assets / 'paper-ink-regular.woff2')
options = subset.Options()
options.flavor = 'woff2'
options.recalc_timestamp = False
subsetter = subset.Subsetter(options=options)
subsetter.populate(unicodes=(list(range(0x20, 0x250)) + list(range(0x370, 0x530))
                            + list(range(0x2000, 0x2800))))
subsetter.subset(font)
font.flavor = 'woff2'
output = io.BytesIO()
font.save(output)
encoded = base64.b64encode(output.getvalue()).decode('ascii')
css = (assets / 'reference.css').read_text()
css = css.replace('url("paper-ink-regular.woff2")',
                  'url("data:font/woff2;base64,' + encoded + '")')
# Avoid the MSVC per-string-literal size limit while preserving byte-exact CSS.
chunks = [css[i:i + 12000] for i in range(0, len(css), 12000)]
body = '    // Original v2 CSS, with its OFL font subset embedded for offline use.\n'
body += '\n'.join('    ctx().out += R"FTINKSOURCE(' + chunk + ')FTINKSOURCE";'
                  for chunk in chunks) + '\n'
header = root / 'ft.hpp'
text = header.read_text()
start = text.index('    // Original v2 CSS, with its OFL font subset embedded for offline use.')
end = text.index('    if (ctx().style.accent.r', start)
header.write_text(text[:start] + body + text[end:])
print(f'Embedded {len(output.getvalue())} font bytes and original CSS in {len(chunks)} chunks')
