Game Categories Lite

This plugin is based and uses source code from Game Categories Revised v12 (GCR)
and Game Categories Light v1.3 (GCL), both created by Bubbletune. Compatible with
6.20, 6.3x and 6.60 CFW.

I changed the name of the project (doing a fork in the process) to avoid confusion
with GCL and GCR (the plugin is based heavily in GCL since it doesn't use a
kernel/user approach).

You can configure the folder prefix (use CAT_ for folders or not), showing uncategorized
content and change the category mode in system settings.

If you want to hide certain homebrew/game/dlc from the category listing just create a file
named gclite_filter.txt and add the folders name, one per line (make sure that a newline is
added at the end of the file). Next, put the file in your seplugins folder.

If you want a translation of the visible options then edit the file category_lite_en.txt
and save it using the language code that first you (e.g. "es" for spanish).

Note: you must use UTF-8 encoding for the translation files (without unicode BOM).

Languages supported: "ja", "en", "fr", "es", "de", "it", "nl", "pt", "ru", "ko", "ch1", "ch2"

The source code is also available in https://github.com/codestation/gclite

Notes: make sure that this is the 1st plugin listed in vsh.txt

Known issues:
>> Unknown if fixable:
* Change of category in the PSPGo requires a VSH reset.

>> Unrelated to gclite:
* ME doesn't merge the categories with the same name between /ISO and /PSP/GAME.

>> Folder mode limitations/bugs:
* Max categories: 8 (included uncategorized).
* Folder name + homebrew folder name: 30 character, e.g.: My homebrews/AwesomeBigHomebrew
  is valid but My homebrews/AwesomeBigHomebrews isn't (i am not counting the "/").
  Note: the japanese and other non-ascii characters are 2 bytes wide, e.g.: カラフル counts
  as 8 chars.

Changelog
v1.7-js1 (October 17, 2017):
[!]Fix labels not showing on PSP go internal storage.
v1.6:
[+]Added new option to sort categories: Use CAT_XX or XXcategory_name (XX between 00 and 99).
v1.5-r4
[+]Added polish translation.
v1.5-r3
[!]Fix duplicated entries on iso category in folder mode (PRO).
v1.5:
[+]Support for categories in folder mode like Bubbletune's GCL (thx Nekmo for betatesting).
[+]Support to hide certain homebrews/games/dlc from the categories.
[+]Added subtitles to the config options.
[+]Empty categories are hidden by default.
[+]Non game folders are hidden by default on uncategorized content.
[+]Added folder mode benchmark (compile with BENCHMARK=1)
[+]Added bulgarian translation by Xian Nox.
[+]Added simple chinese translation by phoe-nix.
[+]Added Traditional-Chinese translation by Raiyou.
[+]Added Russian translation by Frostegater.
[+]Added italian translation by stevealexanderames.
[!]Force the uncategorized content to be the last item by default.
[!]Fixed UMD icon malfunction bug introduced in 1.4-r2.
v1.4:
[+]6.60 firmware support
[+]Allow the uncategorized folder to be sorted with your favorite app.
[+]Multiple language support.
[+]Added ja translation by popsdeco.
[+]Added de translation by KOlle and The Z.
v1.3:
[+]Support for categories in contextual menu.
[+]Support for plugin configuration in system settings.
[+]Added runtime detection for ME, so category games are now shown.
[!]Fixed issues with PSPGo (big thanks to raing3 to help me with the debugging).
v1.2:
[!]Fixed PSPGo categories, again (thx RUSTII for the tests).
[!]Fixed the free space display when the psp returns from sleep.
v1.1:
[!]Fixed PSPGo categories (thx RUSTII for the tests).
v1.0:
[+]First release.
