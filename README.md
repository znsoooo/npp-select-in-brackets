# Select in Brackets

A Notepad++ plugin that supports 2 functions, quickly selecting content within matching brackets, swapping selections when multiple areas are selected.


## Features

### Select in Brackets

In Notepad++'s "Setting - Preferences - Delimiter", you can set up to select content within matching brackets. However, it only supports ONE type of bracket. For example, if you set it for round brackets (as default), it won't work for square brackets or curly brackets, and it doesn't support any Unicode brackets.

So I designed this plugin. It can select content within matching several types of brackets, and supports Unicode brackets also.

Press **Alt-Q** (**Q**uote) to select text within matching brackets, supports multiple selections.

Pressing it repeatedly will expand the selection area, until to the whole text.

### Swap Selections

Swap selections is a feature that was [rejected by the Notepad++ author](https://github.com/notepad-plus-plus/notepad-plus-plus/issues/6226#issuecomment-569478880). It is mainly used to swap the content of two selections, especially when swapping two multi-line contents from very far distance positions.

Press **Alt-X** (e**X**change) to swap selections.

Press **Alt-Shift-X** to swap selections in the reverse direction.


## Build

1. Install MinGW.
2. Run the "[build.bat](build.bat)" file.
3. The DLL files will be generated in the "output" folder, including both 32-bit and 64-bit versions.


## Usage

1. Get the DLL file in two ways:
   1. Compile it with [GCC](https://sourceforge.net/projects/mingw-w64/files).
   2. Download the latest release version: <https://github.com/znsoooo/npp-select-in-brackets/releases>
2. Copy the DLL file to the directory of "~\Notepad++\plugins".
3. Restart the Notepad++ program.
4. (Recommended) Set up with Notepad++:
   1. In "Setting - Preferences - Editing - Multi-Editing", set it to "Enable".
   2. In "Setting - Preferences - Delimiter - Delimiter selection settings", set the "Open" and "Close" options to empty.
5. Enjoy the plugin!


## FAQ

This plugin doesn't have syntax parsing, so if there are escape chars or chars in strings, it might select the wrong area, you need to adjust it manually.

The types of brackets supported were written in the "[matcher.cpp](src/matcher.cpp)" file. The current supported brackets include:

> {Hello[my(world)]}\
> "Hello'my\`world\`'"\
> ｛你好［我的（世界）］｝\
> 《你好《世界》》\
> 『你好「世界」』\
> 〖你好【世界】〗\
> “你好‘世界’”

If you need to add or remove symbols, you need to modify the source file and recompile. Configuring through a settings file is not currently supported (because it is troublesome).

However, if many people request this feature, I will reconsider it. But I think it's simpler to just write it in the source file.

Welcome to suggest any advices in the issues:

<https://github.com/znsoooo/npp-select-in-brackets/issues>


## License

- __Author:__ Lishixian
- __QQ:__ 11313213
- __Email:__ lsx7@sina.com
- __GitHub:__ https://github.com/znsoooo/npp-select-in-brackets
- __License:__ MIT License. Copyright (c) 2024-2025 Shixian Li (znsoooo). All rights reserved.

