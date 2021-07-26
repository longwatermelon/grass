# grass
Grass text editor

![screenshot](https://user-images.githubusercontent.com/73869536/122617594-6b810000-d041-11eb-9dd8-005962d91034.png)

Officially supports windows and linux, not tested on mac

Tabs do not render correctly, switch to the correct method of using spaces in your code

# Enabling syntax highlighting
The name plugins is misleading, you can only configure language specific syntax highlighting through them. I was going to do more with them but then I got lazy.

Grass automatically comes with syntax highlighting for python and c++, though you should probably replace them if you want accurate syntax highlighting since they are missing some keywords.

On windows, go to C:\Users\\[user]\Program Files\grass [version]\bin\res\plugins

On linux, go to /usr/share/grass/res/plugins

Create a new file and follow the structure of the other config files in res/plugins

# Building
If you want to install grass and you:

    1. Want the most up to date version

    2. Are not on windows or a debian based linux distribution

Then follow the instructions below to install from source.
```
mkdir build
cd build
sudo cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
make
sudo make install
```

To uninstall, run:
```
cat install_manifest.txt | sudo xargs rm
```

Read install_manifest.txt first before running this command to make sure it isn't removing any important files. Everything in install_manifest.txt will be removed.

If grass gives a segmentation fault when you try to run it after installation, go in CMakeLists.txt and put `target_compile_definitions(grass PRIVATE NDEBUG)` below the add_executable() call and then rebuild. This tells grass you're not debugging so it shouldn't look for the resources folder in the current working directory. It's inconvenient but I am a cmake noob and I couldn't find anything better online.

# Resources
[Closed folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-icon.html)

[Open folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-Open-icon.html)
