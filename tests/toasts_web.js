// Run after polish_regressions emits build/toast-regression.html. No packages needed.
const fs=require('node:fs'),vm=require('node:vm'),assert=require('node:assert/strict');
const html=fs.readFileSync('build/toast-regression.html','utf8');
const script=html.match(/<script>([\s\S]*?)<\/script>/)[1];
class Element extends EventTarget {
    constructor(classes='') {super();this.attrs={};this.dataset={};this.children=[];const set=new Set(classes.split(' '));this.classList={contains:x=>set.has(x),add:x=>set.add(x)};}
    getAttribute(k){return this.attrs[k]||null;}
    setAttribute(k,v){this.attrs[k]=v;}
    appendChild(e){if(e.parentElement)e.parentElement.children=e.parentElement.children.filter(c=>c!==e);this.children.push(e);e.parentElement=this;}
    remove(){if(this.parentElement)this.parentElement.children=this.parentElement.children.filter(c=>c!==this);this.removed=true;}
    querySelectorAll(sel){let matches=[];for(const c of this.children){if(sel.startsWith('.')&&c.classList.contains(sel.slice(1)))matches.push(c);matches.push(...c.querySelectorAll(sel));}return matches;}
    querySelector(sel){if(sel.startsWith('[data-ft-toast-id=')){const id=sel.match(/"([^"]+)"/)[1];return this.children.find(c=>c.getAttribute('data-ft-toast-id')===id)||null;}return this.querySelectorAll(sel)[0]||null;}
}
const body=new Element(),stack=new Element('ft-toast-stack');body.appendChild(stack);
function toast(id){let item=new Element('ft-toast');item.setAttribute('data-ft-toast-id',id);item.setAttribute('data-ft-toast-duration','1000');item.appendChild(new Element('ft-toast-close'));return item;}
const first=toast('1');stack.appendChild(first);
const document={body,querySelectorAll:s=>body.querySelectorAll(s),createElement:()=>new Element()},window={},timers=[];
vm.runInNewContext(script,{window,document,setTimeout:(fn,ms)=>{timers.push({fn,ms});return timers.length;}});
assert.equal(timers.filter(t=>t.ms===1000).length,1,'initial page toast must get an expiry timer');
window.__fthtInitToasts(document);assert.equal(timers.length,1,'reinitialization must not duplicate timers');
first.querySelector('.ft-toast-close').dispatchEvent(new Event('click'));
assert(first.classList.contains('ft-toast-out'),'initial toast close button works');
timers.find(t=>t.ms===240).fn();assert(first.removed);
const fetched=new Element(),newStack=new Element('ft-toast-stack');fetched.appendChild(newStack);
const duplicate=toast('1'),second=toast('2');newStack.appendChild(duplicate);newStack.appendChild(second);
window.__fthtInitToasts(fetched);assert(duplicate.removed,'dismissed toast must not return');assert.equal(second.parentElement,stack);
const expirations=timers.filter(t=>t.ms===1000);assert.equal(expirations.length,2);expirations[1].fn();assert(second.classList.contains('ft-toast-out'),'newly fetched toasts expire');
console.log('Initial/fetched toast dismissal, expiry, and duplicate-init tests passed.');
