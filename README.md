# chip-8-emulator

This repo contains worked out tutorials to learn software emulation. The main goal is to write a C based function CHIP-8 emulator. Post which we will move to emulation Nintendo's GameBoy in a separate Repo.

For now, let's get going with the tutorials on [Emulator 101](http://emulator101.com).
> Note: These might changes as I learn more and currently may not be the best way to go about things. `But really what is?`


## Emulating 8080

Let's start by following the Steps on [Emulator 101](http://emulator101.com).

> Critical: Do not look at this until necessary, but here's a [completed version](https://github.com/herrecito/invaders) I came across this tutorial. But the **Useful links** section is actually quite useful.

### OpCode
Emulator 101's Opcode is linked [here](http://www.emulator101.com/8080-by-opcode.html). But IMO, it gives too much away. There's always the [8080 system programming manual](http://www.nj7p.info/Manuals/PDFs/Intel/9800153B.pdf), but that might be a little too much so let's keep it aside for now. I think referring to below links would be more work and fruitful:
- https://gist.github.com/joefg/634fa4a1046516d785c9
- https://pastraiser.com/cpu/i8080/i8080_opcodes.html
- http://www.classiccmp.org/dunfield/r/8080.txt

**To top it off**, this [programming manual](https://drakeor.com/uploads/8080-Programmers-Manual.pdf) seems quite useful, and small enough.

The tutorial starts by mentioning guides such as above, then says to go find the ROM. Here's what I got:
- ROM Mirror : [C code](https://drakeor.com/uploads/invaders.zip)

Next, I would recommend you to read the programming manual (91 Page one). The first 2 chapters should be enough for now. By now it would have struck you:
- The ROM contains the 8080 code we want to execute after some setup (like Input and display)
- We want to apply identical effects to those components, which a real system would notice. Basically software emulation!

#### RoadMap:
1. Emulate the basic commands of 8080, `write a disassembler -> call C equivalents for those assembly instructions`.
1. Write the IO (Graphical Interface) to actually do the GUI updates
1. Implement Input
1. Implement Sound driver 

> Note: While these are important individual steps, we'll go in a manner where we just implement enough to get started with the next step and then keep coming back for extensions. This is an important part of rapid prototype dev which I've been lacking in and is very much needed in today's world. So let's practice that!


### Disassembling 8080 (JIT style) :laughing: 

Well, it's not really JIT. But we'll only extend it just in time like we ROM tries to use an instruction that hasn't been implemented yet. This would help us with debugging (smaller scope to look for bugs) and visible progress!

> Note: While I would highly recommend starting from scratch since you'll have to give the code organization and management some thought.

> To make getting started easier, I can share my workspace setup with basic file structure, makefile etc which you can extend as we go together :smile:.

## Emulation Bookmarks
- [Emulator 101 - Welcome](http://www.emulator101.com/)
- [HOWTO: Writing a Computer Emulator](http://fms.komkon.org/EMUL8/HOWTO.html)
- [Writing a CHIP-8 emulator with Rust and WebAssembly](https://blog.scottlogic.com/2017/12/13/chip8-emulator-webassembly-rust.html)
- [How to build an emulator - Codemotion Magazine](https://www.codemotion.com/magazine/dev-hub/gamedev/how-to-build-an-emulator/)
- [LemonBoy/Space-Invaders-Emulator: Fuck PS3 we haz Space Invaders!](https://github.com/LemonBoy/Space-Invaders-Emulator)
1. JS
    - [How to Create Your Very Own Chip-8 Emulator](https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/)
    - [Imran Nazar: GameBoy Emulation in JavaScript: Timers](http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-Timers)
1. C++
    - [How to write an emulator (CHIP-8 interpreter) — Multigesture.net](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
1. GameBoy
    - [GameBoyProgManVer1.1.pdf](https://ia803208.us.archive.org/9/items/GameBoyProgManVer1.1/GameBoyProgManVer1.1.pdf)
    - [Cinoop](https://cturt.github.io/cinoop.html)
1.NES
    - [Programming guide - Nesdev wiki](http://wiki.nesdev.com/w/index.php/Programming_guide)
    - [nesdev.com/NES emulator development guide.txt](http://nesdev.com/NES%20emulator%20development%20guide.txt)
    - [Emulators 101: How To Write A Program That Functions Like An NES CPU | Hackaday](https://hackaday.com/2012/10/12/emulators-101-how-to-write-a-program-that-functions-like-an-nes-cpu/)
    - [Writing your own NES emulator - overview | yizhang82’s blog](https://yizhang82.dev/nes-emu-overview)
1. SDL C
    - [Lazy Foo' Productions - Beginning Game Programming v2.0](https://lazyfoo.net/tutorials/SDL/index.php#Hello%20SDL)
    - [SDL 2.0 Tutorial-00: The Basic Sturcture | Stephen Meier](https://stephenmeier.net/2014/08/10/sdl-2-0-tutorial-00-the-basic-sturcture/)