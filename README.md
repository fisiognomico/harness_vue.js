# Vue.js harness example

This project aims to be a practical example of an harness that employs
[AFL++ Injection fuzzing](https://github.com/AFLplusplus/AFLplusplus/blob/stable/instrumentation/README.injections.md) on a web application.
In particular we target a Vue.js web app that is dynamically compiled server side.
The harness is made of three components:
    1) An executor that injects tainted input in the template engine.
    2) A parser, in this case we use Libxml capabilities to read HTML
       input.
    3) The actual harness component that uses LLVM instrumentation to add
       to the parser a function that when detects an injection sample
       crashes to signal the fuzzer.

