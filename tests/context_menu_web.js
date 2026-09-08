// Run after data_widgets_test emits its web fixture. No browser package required.
const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const html = fs.readFileSync('build/data-widgets-web.html', 'utf8');
const code = html.match(/<script data-ft-context>([\s\S]*?)<\/script>/)[1];
let document;
class Element extends EventTarget {
    constructor() { super(); this.attrs = {}; this.style = {}; this.dataset = {}; this.children = []; }
    setAttribute(k, v) { this.attrs[k] = v; }
    append(child) { child.parentElement = this; this.children.push(child); }
    contains(other) { return other === this || this.children.some(c => c.contains(other)); }
    closest(selector) { return selector === '.ft-menu-panel' && this === panel ? panel : null; }
    focus() { document.activeElement = this; }
    remove() { this.removed = true; }
    getBoundingClientRect() { return {left: 20, bottom: 40}; }
}
const window = new Element();
document = new Element(); document.body = new Element();
const summary = new Element(), target = new Element(), menu = new Element(), panel = new Element();
const form = new Element(); form.id = 'test-form';
menu.previousElementSibling = target;
menu.closest = selector => selector === 'form' ? form : null;
menu.querySelector = selector => selector === 'summary' ? summary : panel;
const buttons = [new Element(), new Element(), new Element()];
buttons[1].disabled = true;
for (let i = 0; i < buttons.length; i++) {
    const row = new Element(); row.dataset = {checkable: i === 0 ? 'true' : 'false', checked: i === 0 ? 'true' : 'false'};
    row.append(buttons[i]); panel.append(row);
}
panel.querySelectorAll = () => buttons;
panel.hidden = true; panel.offsetWidth = 220; panel.offsetHeight = 104;
const animations = [];
panel.animate = (frames, options) => { animations.push({frames, options}); return {cancel() {}}; };
document.currentScript = {previousElementSibling: menu}; document.activeElement = summary;
let reducedMotion = false;
class CustomEvent extends Event {}
vm.runInNewContext(code, {document, window, innerWidth: 640, innerHeight: 480,
    AbortController, CustomEvent, matchMedia: () => ({matches: reducedMotion})});
assert.equal(buttons[0].attrs['aria-checked'], 'true');
assert.equal(buttons[0].attrs.role, 'menuitemcheckbox');
assert.equal(buttons[0].attrs.form, 'test-form');
assert.equal(panel.parentElement, document.body);
function send(el, type, props = {}) {
    const event = new Event(type, {cancelable: true});
    for (const [key, value] of Object.entries(props)) Object.defineProperty(event, key, {value});
    el.dispatchEvent(event); return event;
}
assert(send(document, 'contextmenu', {target, clientX: 635, clientY: 470}).defaultPrevented);
assert.equal(panel.hidden, false); assert.equal(panel.style.left, '420px');
assert.equal(panel.style.top, '376px');
assert.equal(animations[0].frames[0].transform, 'scale(1)');
assert.equal(animations[0].frames[1].transform, 'scale(1.025)');
assert.equal(animations[0].frames[2].transform, 'scale(1)');
assert.equal(animations[0].options.duration, 120);
assert.equal(document.activeElement, buttons[0]);
send(panel, 'keydown', {key: 'ArrowDown'});
assert.equal(document.activeElement, buttons[2]);
send(panel, 'keydown', {key: 'Escape'});
assert.equal(panel.hidden, true); assert.equal(document.activeElement, summary);
reducedMotion = true;
send(document, 'contextmenu', {target, clientX: 10, clientY: 10});
assert.equal(animations.length, 1);
send(document, 'pointerdown', {target});
assert.equal(panel.hidden, true);
for (const cleanup of window.__fthtMenuCleanup) cleanup();
assert.equal(panel.removed, true);
assert.equal(send(document, 'contextmenu', {target, clientX: 10, clientY: 10}).defaultPrevented, false);
console.log('Web menu interaction tests passed.');
