# QTail

QTail is a homemade tool for monitoring log files with some powerful features.

## License

You are free to use this tool as you like, but I take no responsibility for how you use it, any damage caused, or any other issues that may arise from its use.

This project is built with C++20 using [Qt Community](https://www.qt.io/download-open-source) and [QCoro](https://qcoro.dvratil.cz/).

For any licensing details, terms of use, or obligations, please refer to the respective websites of these projects.

## Objective

This tool aims to be blazing fast while consuming minimal resources to:

- Load log files as quickly as possible;
- Continuously monitor the file and append new lines to the view;
- Highlight control characters, similar to VSCode;
- Identify FIX protocol strings in the selected log line and display a human-readable table with tag names and field descriptions when available.

The tool uses the [public FIX dictionary released by B3](https://www.b3.com.br/en_us/solutions/connectivity/fix-suite/) as the source of information. However, you can replace it with your own dictionary by updating the `FIX44.xml` file in the assets directory.

## Want to contribute?

Awesome! This project started as a personal tool, but if you find it useful, feel free to contribute. You can request new features or report bugs in the [issues](https://github.com/Bigous/QTail/issues) section, or even better — submit a [pull request](https://github.com/Bigous/QTail/pulls) 😀!
