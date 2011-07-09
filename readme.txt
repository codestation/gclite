Game Categories Lite v1.3 - Codestation

This plugin is based and uses source code from Game Categories Revised v12 (GCR)
and Game Categories Light v1.3 (GCL), both created by Bubbletune. Compatible with
6.20 and 6.3x CFW.

I changed the name of the project (doing a fork in the process) to avoid confusion
with GCL (this plugin doesn't use the folder categories present in firmwares 6.xx)
and GCR (the plugin is based heavily in GCL since it doesn't use a kernel/user approach).

You can configure the folder prefix (use CAT_ for folders or not), showing uncategorized
content and change the category mode in system settings.

The source code is also available in https://github.com/codestation/gclite

TODO:
* Restore support for contextual categories (from GCR v12).
* Add option to use the categories "new" format (without CAT_ prefix).
* Restore options menu.

Changelog
v1.3:
[+]Support for categories in contextual menu
[+]Support for categories configuration in system settings
[+]Added runtime detection for ME, so category games are now shown
v1.2:
[!]Fixed PSPGo categories, again (thx RUSTII for the tests)
[!]Fixed the free space display when the psp returns from sleep
v1.1:
[!]Fixed PSPGo categories (thx RUSTII for the tests)
v1.0:
[+]First release
