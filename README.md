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
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=path_to_vcpkg/scripts/buildsystems/vcpkg.cmake
```

# Resources
[Closed folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-icon.html)

[Open folder](https://iconarchive.com/show/sleek-xp-basic-icons-by-hopstarter/Folder-Open-icon.html)
