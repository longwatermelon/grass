# grass
Grass text editor

![screenshot](https://user-images.githubusercontent.com/73869536/122597274-01a42e80-d020-11eb-9863-db7c73e3f10d.png)

Officially supports windows and linux, not tested on mac

Tabs do not render correctly, switch to the correct method of using spaces in your code

# Enabling syntax highlighting
Grass automatically comes with syntax highlighting for python and c++, though you should probably replace them if you want accurate syntax highlighting since they are missing some keywords.

On windows, go to C:\Users\\[user]\Program Files\grass [version]\bin\res\plugins

On linux, go to /usr/share/grass/res/plugins

Create a new file and follow the structure of the other config files

# Building
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path_to_vcpkg/scripts/buildsystems/vcpkg.cmake
```

# Resources
[Closed folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-icon.html)

[Open folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-Open-icon.html)
