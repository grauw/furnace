# Contributing

contributions to Furnace are welcome!

# Getting ready

log into your Github account, and click the Fork button in the header of the project's page.

then open a terminal and clone your fork:

```
git clone git@github.com:USERNAME/furnace.git
```

(replace `USERNAME` with your username)

# Working

## Code

bug fixes, improvements and several other things accepted.

the coding style is described here:

- indentation: two spaces
- modified 1TBS style:
  - no spaces in function calls
  - spaces between arguments in function declarations
  - no spaces in operations except for `||` and `&&`
  - no space between variable name and assignment
  - space between macro in string literals
  - C++ pointer style: `void* variable` rather than `void *variable`
  - indent switch cases
  - preprocessor directives not intended
  - if macro comprises more than one line, indent
- prefer built-in types:
  - `bool`
  - `signed char` or `unsigned char` are 8-bit
    - when the type is `char`, **always** specify whether it is signed or not.
    - unspecified `char` is signed on x86 and unsigned on ARM, so yeah.
    - the only situation in where unspecified `char` is allowed is for C strings (`const char*`).
  - `short` or `unsigned short` are 16-bit
  - `int` or `unsigned int` are 32-bit
  - `float` is 32-bit
  - `double` is 64-bit
  - `long long int` or `unsigned long long int` are 64-bit
    - avoid using 64-bit numbers as I still build for 32-bit systems.
    - two `long`s are required to make Windows happy.
  - `size_t` are 32-bit or 64-bit, depending on architecture.
- in float/double operations, always use decimal and `f` if single-precision.
  - e.g. `1.0f` or `1.0` instead of `1`.
- don't use `auto` unless needed.

some files (particularly the ones in `src/engine/platform/sound` and `extern/`) don't follow this style.

you don't have to follow this style. I will fix it after I accept your contribution.

## Demo Songs

just put your demo song in `demos/`!

# Finishing

after you've done your modifications, commit the changes and push.
then open your fork on GitHub and send a pull request.

# I don't know how to use Git but I want to contribute with a demo song

you can also contact me directly! [find me here.](https://tildearrow.org/?p=contact)
