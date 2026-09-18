# Your first interface

You do not need GUI experience. If you can follow a C++ variable, a loop, and an `if` statement, you can build this interface. This lesson introduces each GUI idea before using it.

<div class="lesson-note"><strong>Try it, then build it.</strong> The interactive examples on this page are browser teaching models. They animate the concepts; they do not compile or execute FT. The downloadable C++ examples run against the real header.</div>

## 1. A widget is an interactive building block

A **widget** is a part of the interface: a button, checkbox, text field, or slider. Your application owns the information those widgets show. That information is called **state**.

Try the slider and checkbox below. The preview changes because you changed a value. Watch the corresponding C++ state on the right.

<div data-demo="widgets"></div>

In `ft::checkbox("Enabled", &enabled)`, `"Enabled"` is the visible label. `&enabled` means “the address of my variable”: FT can update the original boolean when you click. For a slider, pass the address of a floating-point variable and the allowed minimum and maximum.

```cpp
// Define these before your frame loop.
bool enabled = true;
float volume = 0.5f;

// Draw these inside begin()/end().
ft::checkbox("Enabled", &enabled);
ft::slider_float("Volume", &volume, 0.0f, 1.0f);
```

**Try next:** set the slider to the far right. Its value becomes `1.0`, not `100`, because this example uses a range from zero to one. A progress bar uses the same range.

## 2. A frame is one pass through your interface

FT is an **immediate-mode** library. You describe the widgets again each frame. You do not construct a permanent button object and later attach an event listener to it.

A frame is one opportunity to process input and render output. The steps below are repeated. Play the animation or use **Next step** to move at your own pace.

<div data-demo="frames"></div>

1. `pump()` processes events or waits for work. If it returns false, stop the loop.
2. `begin()` starts a new frame.
3. Your widget calls describe what should be visible and react to interaction.
4. `end()` finishes the frame and presents local output or completes a web response.

```cpp
while (ft::pump()) {
    ft::begin();
    ft::text("Hello!");
    ft::end();
}
ft::shutdown();
```

The animation is deliberately slow so you can follow it. Real local frames may run many times per second. Web frames run when a request arrives; they are not a continuous animation clock.

## 3. An event changes state

An **event** is something that happened: a click, a keystroke, or a selection. `button()` returns true when activated. That lets a normal C++ `if` statement handle the action.

Click the counter. The highlighted line shows the action that changed the value. Toggle the mistake switch to see why resetting state inside the loop makes a counter fail to accumulate.

<div data-demo="counter"></div>

```cpp
int count = 0; // Before the loop: keep the value between frames.
while (ft::pump()) {
    ft::begin();
    if (ft::button("Add one")) ++count;
    std::string message = "Count: " + std::to_string(count);
    ft::text(message.c_str());
    ft::end();
}
```

`++count` adds one. `std::to_string()` converts the number into text. `.c_str()` gives FT a C-style string to display. The temporary `message` remains alive during the `text()` call.

**Common mistake:** putting `int count = 0;` inside the loop resets it on each frame. The faulty preview models a button frame; on a following frame without a click, that version would return to zero.

## 4. Layout decides where widgets go

By default, you call widgets in the order you want them laid out. A **row** places widgets next to each other. **Weights** say how to divide the available width: `{2, 1}` gives twice as much space to the first column.

<div data-demo="layout"></div>

```cpp
ft::row({2.0f, 1.0f}, [&] {
    ft::text("Main content");
    ft::button("Action");
});
```

`[&] { ... }` is a C++ **lambda**: a small function defined right where it is used. FT calls it while the row layout is active. The `&` lets it use variables from the surrounding scope. You do not need a separate class to group controls.

The sandbox uses browser grid to explain ratios. Exact pixel sizes, font metrics, and terminal cell dimensions differ in FT. Use weighted layouts when possible; hardcoded pixel widths do not translate directly into terminal cells.

## 5. Selection is another piece of state

A table displays rows of information. When you select a record, you usually want the source record's identity, even if the screen is sorted differently.

Click a column header to sort, then select Alice. The stored `selected_row` still points into the original source list.

<div data-demo="table"></div>

```cpp
// Keep these before the frame loop.
ft::TableRows people = {{"Cara", "Ready"}, {"Alice", "Busy"}, {"Bob", "Ready"}};
ft::TableState state;

// Inside the frame:
ft::table("People", {"Name", "Status"}, people, state);
// Alice is source index 1, even if sorting moves her to the first visible row.
```

An **index** starts at zero in C++. `-1` means no row is selected. If you insert or reorder the source data yourself, update the selection to match. [The widget guide](widgets.md) explains sorting, pagination, and table options.

## 6. One application, three backends

A **backend** is the part that handles drawing and input for an environment. GUI means a desktop window. TUI means a terminal interface. Web means an HTTP page in a browser.

<div data-demo="backends"></div>

The code calls the same `ft::` functions. The backend decides how those calls look and behave. This does not mean every feature is identical: file dialogs are native-only; terminal dimensions use cells; browsers render HTML forms and process HTTP requests.

Pass `argc, argv` to `create_window()` so FT can read the launch flags. Without it, adding `--tui` to a command does not change the application mode.

## 7. Build your first real FT program

You now know the four essentials: widgets display state, events change state, a frame repeats the UI description, and layout arranges it.

Follow [Getting started](getting-started.md) to compile the complete [hello example](examples/hello.cpp). First run it in one backend. Change the title, add a checkbox, then try another backend.

### A small exercise

Add a `bool accepted = false;` before the loop. Draw `ft::checkbox("I agree", &accepted)` inside it. Display `"Thank you"` only when `accepted` is true.

<details><summary>Show a solution</summary>

```cpp
// Before the loop:
bool accepted = false;

// Between begin() and end():
ft::checkbox("I agree", &accepted);
if (accepted) ft::text("Thank you");
```

The `if` controls whether the text is included in that frame. On the next frame, FT evaluates the condition again.

</details>

### Where to go next

- [Build and dependencies](getting-started.md) — make the examples run on your machine.
- [Widgets](widgets.md) — choose a control and learn its return value.
- [Layout and style](layout-and-style.md) — arrange controls and change the theme.
- [Configuration](configuration.md) — understand each option before changing it.
- [Backend behavior](backends.md) — check what changes in terminal or browser mode.
