# QTail

This is a home made tool for monitoring log files with some cool features.

## About License

You may use it as you want to, but I, in no way, am not responsible for you usage, damage or anithing.

It's built in C++20 with [QT Community](https://www.qt.io/download-open-source) and [QCoro](https://qcoro.dvratil.cz/)

Any License, obligation in using it or whatever, read their sites.

## Objective

This tool aims to be blazing fast and consuming the minimum resources to:

- Load as fast as it can be the log file;
- Continue monitoring the file and append new lines to the end of visualization;
- Highlight control chars just like VSCode does;
- Identify FIX protocol strings in the selected line of log and display a human readable table with tag names and field description when available. I'm using the [public FIX dictionary release by B3](https://www.b3.com.br/en_us/solutions/connectivity/fix-suite/) as the source of information. But you can exchange it by your own dictionary changing the `FIX44.xml` in de assets directory.

## Wanna help?

Cool, thanks for that. This project is more a tool for me, but if you like it you can ask in the [issues](https://github.com/Bigous/QTail/issues) for features or report bugs or even better - do a [pull request](https://github.com/Bigous/QTail/pulls) 😀!
