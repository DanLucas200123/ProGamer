# DIY Gamer Kit Arduino Library: ProGamer Edition!

Now that Tech Will Save Us can no longer save us and the DIY Gamer is becoming rarer to buy, I feel that it is the appropriate time to rewrite the hobby handheld's code library.
This is a small project to try and give the Gamer more features and attempt to make it function more similarly to a modern game engine.

## Installation

1. Download the project as a .zip file.

2. Install the latest version of **Arduino**. You can get this software [here](https://www.arduino.cc/en/Main/Software).

3. Open the Arduino software on your computer and navigate to **Sketch** → **Include Library** → **Add .ZIP library...**, then select the .zip file of the project.

## Usage

In your project with the ProGamer library included, create a global instance of the ProGamer class.

```c++
#include <ProGamer.h>

ProGamer gamer;
```

At the top of the `setup()` function, call `ProGamer::begin()`

```c++
void setup() {
  gamer.begin();
  ...
}
```

At the end of the `loop()` function, call `ProGamer::update()`

```c++
void loop() {
  ...
  gamer.update();
}
```

`ProGamer::update()` causes the main loop to halt between frames, so unlike in the original library, you shouldn't use the `delay()` function in your main loop.

## Features

- Frame updates are automatically handled by the `update()` function, you can change the framerate/game speed using `ProGamer::setFramelength(long)`.
- 4-colour display!
- Display scores that are more than 2 digits long
- Better printString function (speed is controlled by the framelength)
- Dynamic memory usage reductions from the original library, meaning more space for images, animation and music! Which brings me to...
- Music and sound effects, with predefined buzzer values so you can play a range of notes just shy of 2 octaves. Single-channel only, SFX have priority over music but should be kept shorter, music will continue playing once SFX stops.

# Original README

# DIY Gamer Kit Arduino Library

This software is for the [DIY Gamer Kit](http://www.techwillsaveus.com/shop/diy-kits/diy-gamer-kit-arduino/).

The library is essential for using your DIY Gamer Kit to:

- Draw graphics on the screen
- Make animations
- Code apps and games
- Play sounds on the buzzer
- Scroll text across the screen

## Installation

1. Install the latest version of **Arduino**. You can get this software [here](https://www.arduino.cc/en/Main/Software).

2. Open the Arduino software on your computer and navigate to **Sketch** → **Include Library** → **Manage Libraries...**

3. Type `Gamer` into the search bar. Two libraries should appear: **Gamer** and **GamerIR**.

4. Click on each library to highlight it and click the **Install** button. Do this for both libraries!

### Testing Your Installation

The easiest way to test that the library has been successfully installed, is to follow these steps:

1. Go to **File &#8594; Examples &#8594; Gamer** and choose one of the example programs (e.g., **SnakeGame**)

2. Click on the **Verify** button to compile the example, and if it compiles correctly (it should say "Done compiling"), you're good to go!

## Contributions and Bug Fixes

For contributing to the library or games, first fork the master branch, modify, and open a pull request. We will evaluate end test your contributions and merge into this repo.

For anything else, write to us at `support@techwillsaveus.com`.