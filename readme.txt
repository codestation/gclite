Game Categories Lite v1.0 - Codestation

This plugin is based and uses source code from Game Categories Revised v12 (GCR)
and Game Categories Light v1.3 (GCL), both created by Bubbletune. Compatible with
6.20 and 6.3x CFW.

I changed the name of the project (doing a fork in the process) to avoid confusion
with GCL (this plugin doesn't use the folder categories present in firmwares 6.xx)
and GCR (the plugin is based heavily in GCL since it doesn't use a kernel/user approach).

To create categories just make a directory called CAT_<name_of_the_category> in the 
/PSP/GAME folder, you can also create categories inside /ISO and they are going to be
merged with the one in /PSP/GAME (if the category exists).

To hide the "Uncategorized" category create a file named hide_uncategorized.txt and
place in the /SEPLUGINS directory.

TODO:
* Restore support for contextual categories (from GCR v12).

Changelog
v0.1.0:
[+]First release
