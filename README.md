# peony-extensions

## Getting Start
- Make sure all build dependencies have been installed in your machine. You can reference debian/control for building depends.

* Create *build* directory and change to it, then build with cmake. The follow
  commands will auto build extensions as library.

``` sh
cmake ..
make
```
- Put the generated libraries into /usr/lib/peony-qt-extensions.
  
- Test the extension with peony.

## Current extensions

- **terminal**, open terminal with right click menu.
- **shared**, a share properties page in properties window.
- **admin**, open a directory or a file as root with menu.
- **parchives**, compress/uncompress files with menu.
- **computer view**, a special view for displaying computer:///.

> Note that gtk2-style-enhanced-plugin and create-link is deprecated and will be removed later, they are not recommended to be installed in to plugins directory.

## Translations
Unlike Peony, Peony Extensions translations job is relatively primitive and crude. The main idea is every plugin translates itself, and compile translations resources with code (qrc).

When you are going to translate peony extensions (I created), here are the mainly steps.

- use lupdate to generate .ts file for your language, the naming rules reference existing files.
- use lrelease to publish .ts file to .qm file.
- modify the project's .qrc file, add your .qm file into recources to be compiled.
