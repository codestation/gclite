Game Categories Lite v1.4 - Codestation

This plugin is based and uses source code from Game Categories Revised v12 (GCR)
and Game Categories Light v1.3 (GCL), both created by Bubbletune. Compatible with
6.20, 6.3x and 6.60 CFW.

I changed the name of the project (doing a fork in the process) to avoid confusion
with GCL (this plugin doesn't use the folder categories present in firmwares 6.xx)
and GCR (the plugin is based heavily in GCL since it doesn't use a kernel/user approach).

You can configure the folder prefix (use CAT_ for folders or not), showing uncategorized
content and change the category mode in system settings.

If you want a translation of the visible options then edit the file category_lite_en.txt
and save it using the language code that first you (e.g. "es" for spanish).

Note: you must use UTF-8 encoding for the translation files (without unicode BOM).

Languages supported: "ja", "en", "fr", "es", "de", "it", "nl", "pt", "ru", "ko", "ch1", "ch2"

The source code is also available in https://github.com/codestation/gclite

TODO:
* Add folder categories like GC Light

Known issues:
Change of category in the PSPGo requires a VSH reset.

Changelog
v1.4:
[+]6.60 firmware support
[+]Allow the uncategorized folder to be sorted
[+]Multiple language support.
[+]Added ja translation by popsdeco
[+]Added de translation by KOlle
v1.3:
[+]Support for categories in contextual menu.
[+]Support for plugin configuration in system settings.
[+]Added runtime detection for ME, so category games are now shown.
[!]Fixed issues with PSPGo (big thanks to raing3 to help me with the debugging).
v1.2:
[!]Fixed PSPGo categories, again (thx RUSTII for the tests)
[!]Fixed the free space display when the psp returns from sleep
v1.1:
[!]Fixed PSPGo categories (thx RUSTII for the tests)
v1.0:
[+]First release
