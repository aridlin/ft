// Run against examples/ft_paper_ink.cpp --web. Only the localhost demo is mutated.
const assert = require('node:assert/strict');
const { chromium } = require(process.env.PLAYWRIGHT_MODULE || 'playwright');
(async () => {
  const browser=await chromium.launch({headless:true,args:['--no-sandbox']});
  try {
    const page=await browser.newPage({viewport:{width:1100,height:1000},colorScheme:'dark'});
    const errors=[];page.on('pageerror',e=>errors.push(e.message));
    await page.goto('http://127.0.0.1:18764');
    await page.getByRole('button',{name:'WORKSPACE',exact:true}).click();
    await page.evaluate(()=>document.fonts.ready);
    assert(await page.evaluate(()=>document.fonts.check('16px "Paper Ink"')));
    const bg=()=>page.locator('body').evaluate(e=>getComputedStyle(e).backgroundColor);
    assert.equal(await bg(),'rgb(255, 255, 255)');
    const appearance=page.getByRole('combobox').first();
    await appearance.selectOption({label:'One Dark'});
    await page.waitForFunction(()=>!document.querySelector('style#ftht-theme').textContent.includes('--pi-paper'));
    assert.equal(await page.locator('style#ftht-theme').count(),1);
    await appearance.selectOption({label:'Magenta Ink'});
    await page.waitForFunction(()=>document.querySelector('style#ftht-theme').textContent.includes('--ft-ink-action-fg:#000'));
    assert.equal(await bg(),'rgb(255, 255, 255)');
    await appearance.selectOption({label:'Paper Ink'});
    await page.waitForFunction(()=>document.querySelector('style#ftht-theme').textContent.includes('--pi-paper')&&!document.querySelector('style#ftht-theme').textContent.includes('--ft-ink-action-fg:#000'));
    await page.locator('h1').click();
    await page.keyboard.press(':');await page.keyboard.type('deep-theme pa');
    assert.equal(await page.locator('.ft-command-text').evaluate(e=>getComputedStyle(e).color),'rgb(0, 0, 0)');
    assert.equal(await page.locator('.ft-command-preview').evaluate(e=>getComputedStyle(e).color),'rgb(107, 107, 107)');
    assert((await page.locator('.ft-command-preview').textContent()).length>0);
    await page.keyboard.press('Escape');
    async function command(text) {
      await page.locator('h1').click();
      await page.keyboard.press(':');
      await page.keyboard.type(text);
      await page.keyboard.press('Enter');
      await page.waitForTimeout(220);
    }
    await command('deep-theme blueprint-demo');
    await page.waitForFunction(()=>getComputedStyle(document.body).backgroundColor==='rgb(1, 43, 68)');
    await command('deep-theme standard');
    await page.waitForFunction(()=>!document.querySelector('style#ftht-theme').textContent.includes('--pi-paper'));
    await command('deep-theme paper-ink');
    await page.waitForFunction(()=>document.querySelector('style#ftht-theme').textContent.includes('--pi-paper'));
    const field=page.getByLabel('WORKSPACE',{exact:true});
    await field.fill('Żółć – persistent theme');await field.press('Tab');
    await page.waitForTimeout(350);
    assert.equal(await field.inputValue(),'Żółć – persistent theme');
    await page.getByRole('button',{name:'PREVIEW',exact:true}).click();
    await page.getByRole('button',{name:'CLOSE',exact:true}).waitFor();
    await page.getByRole('button',{name:'CLOSE',exact:true}).click();
    await page.getByRole('button',{name:'CLOSE',exact:true}).waitFor({state:'hidden'});
    assert(await page.getByRole('button',{name:'WAITING',exact:true}).isDisabled());
    assert.equal(await page.getByRole('progressbar').getAttribute('aria-valuenow'),'62.0');
    await page.getByText('CONTROLS',{exact:true}).click();
    await page.getByRole('slider',{name:/opacity/i}).waitFor();
    await page.getByText('DATA / LOGS',{exact:true}).click();
    await page.locator('table').waitFor();
    await page.getByText('WORKSPACE',{exact:true}).first().click();
    await page.getByLabel('WORKSPACE',{exact:true}).waitFor();
    await page.screenshot({path:process.env.INK_WEB_SCREENSHOT || 'build/paper-ink-web.png',fullPage:true});
    await page.setViewportSize({width:390,height:844});
    assert(await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
    await page.screenshot({path:process.env.INK_MOBILE_SCREENSHOT || 'build/paper-ink-web-mobile.png',fullPage:true});
    assert.deepEqual(errors,[]);
    console.log('Web deep themes: offline font, dark browser, round-trip switching, editable input, modal, disabled button and mobile width passed.');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1});
