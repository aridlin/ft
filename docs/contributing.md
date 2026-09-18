# Maintaining the documentation

[Documentation home](index.md)

## Scope and source of truth

This is a standalone documentation set, readable directly from `docs/index.md`. It documents the checked-out header, including local changes; it does not assert that every API has already shipped in a tagged release. Keep documentation updates with the corresponding source changes.

The public facade is the final `namespace ft` in `ft.hpp`. Its aliases refer to FTUI declarations near the start of the header. The embedded FTUI, FTTI, and FTHT declaration sections define direct-backend APIs. Read the forwarding implementation as well as a declaration before claiming parity.

`api.md` records public facade signatures and supporting type definitions, excluding `detail` internals. The usage guides describe behavior and limitations. When adding a public function, update both the signature inventory and the relevant guide. When changing a default, update the configuration table and copied type definition.

## Validation

Compile both files in `docs/examples` against the current header with C++17 and the platform dependencies described in [Getting started](getting-started.md). These are complete applications; other guide snippets intentionally depend on surrounding state and a frame.

Check every relative Markdown link and image path after moving pages. Review configuration field coverage against `ft::Config` and function coverage against the public facade. Check GUI, terminal, and web claims separately. Do not claim runtime testing for a backend merely because its code compiled.

The repository's existing regression commands are listed in [Examples and troubleshooting](examples-and-troubleshooting.md). Changes confined to prose do not require rebuilding every demo; executable documentation examples should be compiled when modified.

## Build and preview the website

The source pages are Markdown. A small Python builder turns them into a static site with local search, responsive navigation, copyable snippets, and six interactive teaching models. No JavaScript framework, CDN, or runtime server is required.

```sh
python -m venv build/docs-venv
build/docs-venv/bin/pip install -r docs/requirements.txt
build/docs-venv/bin/python docs/build.py
python -m http.server 8765 --directory build/docs
```

Open `http://localhost:8765/`. The generated `build/docs` directory can be published under a subpath such as `/ft/docs/`; all internal asset links are relative. The source header, credentials, and deployment scripts are not included. C++ examples link to the public library; compile them from the repository using the documented include path.

The animated frame lesson starts only on user request, can be paused or stepped, and pauses when offscreen or in a hidden tab. CSS honors reduced-motion preferences. Other demos respond directly to interaction. Keep teaching models clearly labeled: they demonstrate FT concepts using HTML/JavaScript, not an FT/WASM implementation.
