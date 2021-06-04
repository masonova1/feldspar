# Feldspar
Mason Watmough's MCU-oriented "game engine".

# Huh?

Feldspar is a (will be?) fast ECS (entity-component-system) and collision detection library built using only 32-bit fixed-point math, and is generally optimized for microcontrollers, and other low-power systems without floating-point support.

# Where's the documentation, Lebowski?

Here's the plain english. All of this is WIP.

https://docs.google.com/document/d/1mNFkinqF4Yg5yPYUeK-OSPH0S3f0Z0xs6FM0fecTzE4/edit?usp=sharing

# A word on practicality

While Feldspar is written in pure C++ and should port to just about anything on its' own, the chef reccommends not using this on something _too_ weak; I have no idea where the line is, but this thing probably won't be much help for making a real-time game on anything weaker than an ATMEGA. 

The implemented structures and functionality are an honest attempt to crunch down a lot of modern game engine data and parameters into the smallest relevant information possible, but most abstracted structures and objects inside Feldspar are still on the order of tens of bytes. A Feldspar game world with about 60 objects in it would easily eat the free SRAM of an Arduino UNO, and flash fetching is annoying and slow -- consider whether your game _needs_ convex polygon collision response when very important gameplay elements can and have been done very well with 6-byte sprite objects. 
