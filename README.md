Dungeon
![Dun](https://user-images.githubusercontent.com/78976562/136284311-df6a5a68-7255-4937-b7e5-6bb73a4d67eb.jpg)

(a) Mindestens einem Ausgang, dargestellt durch das Zeichen ”A“.
(b) Genau einer Spielfigur, dargestellt durch das Zeichen ”S“.
(c) Wänden, dargestellt durch das Zeichen ”#“.
(d) Leeren Feldern, dargestellt durch Leerzeichen.
(e) Einer Anzahl an Monstern, dargestellt durch eines der Zeichen ”<>vˆ“.
########
# S    #
# <    #
#    A #
########


Spielfigur
Die Spielfigur (S) wird durch die folgenden Befehle gesteuert:
• ”w“: Ein Feld nach Oben gehen.
• ”a“: Ein Feld nach Links gehen.
• ”s“: Ein Feld nach Unten gehen.
• ”d“: Ein Feld nach Rechts gehen.

Wände
Wände (#) blockieren ein Spielfeld, sodass weder Spielfigur noch Monster an dieser Stelle laufen können.

Monster
Monster können in eine von vier Richtungen schauen:
• ”<“: Links.
• ”>“: Rechts.
• ”ˆ“: Oben.
• ”v“: Unten
Nach jedem verarbeiteten Befehl laufen Monster ein Feld in Blickrichtung weiter. Wenn ein Monster in eine Wand oder einen Ausgang laufen würde, dann dreht es sich stattdessen um 180 Grad
