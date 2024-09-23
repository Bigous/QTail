# QTail

This is a homemade tool for monitoring log files with some cool features.

## License

You are free to use this tool as you wish, but I take no responsibility for how you use it, any damage caused, or any other issues that may arise.

This project is built with C++20 using [Qt Community](https://www.qt.io/download-open-source) and [QCoro](https://qcoro.dvratil.cz/).

For any licensing details, terms of use, or obligations, please refer to their respective websites.

## Objective

This tool aims to be blazing fast while consuming minimal resources to:

- Load the log file as quickly as possible;
- Continuously monitor the file and append new lines to the visualization;
- Highlight control characters just like VSCode;
- Identify FIX protocol strings in the selected log line and display a human-readable table with tag names and field descriptions when available.

I'm using the [public FIX dictionary release by B3](https://www.b3.com.br/en_us/solutions/connectivity/fix-suite/) as the source of information, but you can replace it with your own dictionary by updating the `FIX44.xml` file in de assets directory.

## Wanna help?

Cool, thanks for that. This project is more a tool for me, but if you like it you can ask in the [issues](https://github.com/Bigous/QTail/issues) for features or report bugs or even better - do a [pull request](https://github.com/Bigous/QTail/pulls) 😀!
