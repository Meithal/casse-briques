![Use cases](use%20case%20v2.png)

https://trello.com/b/UwwzTs6c/kanban

## Installation

* Télécharger cmake https://cmake.org/
* Installer VisualStudio avec compilateur c++ https://visualstudio.microsoft.com/fr/ , activer charge de travail "developpement desktop en c++", et outils C++ cmake pour windows. cmake a des difficultés pour détecter les versions récentes de visual studio. Soit utiliser une version ancienne de VS, ou bien mettre à jour cmake. Voir aussi https://stackoverflow.com/a/71418520/13577241
* Redémarrer la console
* `mkdir build`
* `cd build`
* `cmake ..`
* `cmake --build .`
* `./Debug/casses_briques-(cli|gui)[.exe]`
