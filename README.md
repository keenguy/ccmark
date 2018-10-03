# ccmark
A c++ markdown engine. Based on [markdown-it](https://github.com/markdown-it/markdown-it)

## Todo
- [x] basic functionality. (Commonmark)
- [x] math (only add delimiters)
- [x] task list (only disabled)
- [ ] footnote
- [ ] anchor (heading, block)
- [ ] file link (subpage)
- [ ] toggled list
- [ ] toc
- [ ] yaml (date, tag, category)
- [ ] search
- [ ] block comment or tag
- [ ] block menu



## development log
> 2018-10-03

Added task list extension: only with disabled attribute. It's implemented
in paragraph block-rule.
Added mathjax extension: only add delimiters. Need script in html file.

> 2018-10-03

Finished translating built-in functionality of markdown-it. Several things are excluded:
* dynamic plugin customization (ruler)
* postprocesses: linkify, replacements, smartquotes
* presets
* highlight


