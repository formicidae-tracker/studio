# CONTRIBUTING Guidelines

## Pull Request Process

The Pull Request should be done using forked repository and feature
branches. Please observe the following guidelines :

 * Uses feature branch with `dev/` for adding new features and
   `issue/` for bug fixing.
 * Multiples commits are allowed in a branch, but try to squash
   together commits that represents partial works.
 * Do not rewrite history by rebasing the commit to the master branch

## Codestyle

TODO: write a complete one

Here are some rules to follow for this project. This are not extensive
rules, but just a list of some of the most uncommon rules that are
fo;llwed in this project. As a general guideline, always prefer code
visual consistency.

  * In any file, use [smart tabs](https://www.emacswiki.org/emacs/SmartTabs)
    for indentation: tabs for indentation, spaces for alignement.
  * Use `.cpp`/`.hpp` for C++ file and `.c`/`.h` for C file
  * Use `#pragma once` for header include guards.
  * Use capitalized CamelCase for c++ methods, at the exception of
    fort-studio C++ cases that uses minimized camelCase for methods to
    keep consistency qith the Qt code base.
  * Fields members of C++ classes should be prefixed by a `d_`
  * Static members of C++ classes should be prefixed by a `s_`
  * Always uses curly braces for single conditionnal/looping statement. By Example :
```c++
if ( condition == true ) {
	Do();
} else {
	DoOther();
}
```
