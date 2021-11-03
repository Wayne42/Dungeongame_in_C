# Dungeongame_in_C
Ein simples Dungeonspiel (Konsole), an einem Tag übereilig zusammengehackt in C

Entwickelt und getestet über Linux, aber übers WSL (Linux Subsystem über Windows) scheint es auch gut genug zu funktionieren. 

# Projekt selbst bauen:
1.) Dateien runterladen
2.) ```make compile``` bzw. ```make compile_windows``` ausführen
3.) ???
4.) Profit.

Wichtig hierbei ist, dass eventuell nötige Packages wie "valgrind" installiert sein müssen, falls nicht bereits vorhanden.

# Spiel spielen (Linux)

```./dungeon```
Verschiedene Level auswählen: 
```./dungeon.exe level/1.txt```

# Spiel spielen (Windows)

```./dungeon.exe```
Verschiedene Level auswählen: 
```./dungeon.exe level/1.txt```

# Steuerung
Über die 
W, A, S, D 
Tasten

# Spielziel
Man spielt die Spielfigur "S", und versucht den Ausgang "A" zu erreichen. 
Ab dem 2. Level sind Monster, welche das Spiel etwas "anspruchsvoller" machen...

# Würde ich das nochmal so machen?
Nein, das ist eine besondere Art "Spaghetti Bolognese" 🍝, und auch, wenn ich da echt Spaß daran hatte es so schnell es geht fertig zu bekommen... bereue ich ein paar Entscheidungen, die das ganze etwas hässlich gemacht haben. 

Dafür, dass es mein erstes "größeres" C Projekt war, ist es in Ordnung. 
