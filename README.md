# grass
![Screenshot_20210726_131750](https://user-images.githubusercontent.com/73869536/127053377-710eb3b1-f9ba-4a5d-b994-e215e4ece837.png)

Officially supports windows and linux, not tested on mac

Tabs do not render correctly, switch to the correct method of using spaces in your code

# Enabling syntax highlighting
The name plugins is misleading, you can only configure language specific syntax highlighting through them. I was going to do more with them but then I got lazy.

Grass automatically comes with syntax highlighting for python and c++, though you should probably replace them if you want accurate syntax highlighting since they are missing some keywords.

On windows, go to C:\Users\\[user]\Program Files\grass [version]\bin\res\plugins

On linux, go to /usr/share/grass/res/plugins

Create a new file and follow the structure of the other config files in res/plugins

# Building
You need [vcpkg](https://github.com/microsoft/vcpkg) to build grass.

If none of the releases work or you want the most updated version, just build it from source.
```
git clone https://github.com/longwatermelon/grass
cd grass
mkdir build
cd build
sudo cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
make
sudo make install
```

You should be able to run grass by either typing `grass` into the terminal, grass is also available through the start menu but it doesn't have an icon.

To uninstall, run:
```
cat install_manifest.txt | sudo xargs rm
```

Read install_manifest.txt first before running this command to make sure it isn't removing any important files. Everything in install_manifest.txt will be removed.

If grass gives a segmentation fault when you try to run it after installation, go in CMakeLists.txt and put `target_compile_definitions(grass PRIVATE NDEBUG)` below the add_executable() call and then rebuild. This tells grass you're not debugging so it shouldn't look for the resources folder in the current working directory. It's inconvenient but I am a cmake noob and I couldn't find anything better online.

# Bugs
Anything that's not in the list below should be reported in issues.
* Grass crashes when an image file is open and you change directories
* Open file dialog doesn't respond to moving up a directory sometimes

# Resources
[Closed folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-icon.html)

[Open folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-Open-icon.html)
