# ccmark
A c++ markdown engine. Based on [markdown-it](https://github.com/markdown-it/markdown-it)

## Dev note
1. In block parsing, footnote is checked before reference.
Footnote labels begin with ^, reference labels should not begin with ^.
There are two kinds of footnotes: block and inline. Blocks in footnote
references are inline-parsed separately: after the inline-parsing of the
main content. (see Processor.cpp)

2.
## Todo
- [x] basic functionality. (Commonmark)
- [x] math (only add delimiters)
- [x] task list (only disabled)
- [x] footnote
- [ ] anchor (heading, block), data-line (scroll sync)
- [ ] file link (subpage)
- [ ] toggled list
- [ ] toc
- [ ] yaml (date, tag, category)
- [ ] search
- [ ] block comment or tag
- [ ] block menu
- [ ] miscs: container, imsize, emoji, fontawesome



## development log
> 2018-10-04

Restructured source. Added footnotes(both block and inline).

> 2018-10-03

Added task list extension: only with disabled attribute. It's implemented
in paragraph block-rule. Added mathjax extension: only add delimiters.
It needs script in html file.

> 2018-10-03

Finished translating built-in functionality of markdown-it. Several things are excluded:
* dynamic plugin customization (ruler)
* postprocesses: linkify, replacements, smartquotes
* presets
* highlight


