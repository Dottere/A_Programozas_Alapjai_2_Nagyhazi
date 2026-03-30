1.	Feladat
A feladat lényege C++ nyelven objektum orientált paradigmákkal megvalósítani egy olyan parancssoros interfészen keresztül játszható sakkjátékot, amely ezen felül képes általános PGN formátumban megadott akár harmadik félnél (chess.com, lichess.org, etc…) játszott régebbi játékokat, vagy ezen program által generált játékokat visszajátszani. Minden véget ért meccs végén – legyen az lejátszott, vagy akár félbeszakított – megadja az addigi lépésekből álló PGN-t.

2.	Feladatspecifikáció
A feladat egy olyan sakkjáték elkészítése, amely képes a játék szabályait betartatni. Ennek alapjául két atomi osztály fog társulni, egy a táblát kezelő osztály mely kezeli a tábla állapotát, és végrehajtja a lépéseket, ha azok lehetségesek. A második pedig egy minden bábuhoz tartozó ősosztály, szimplán csak bábu, amely tartalmazni fog egy tisztán virtuális függvényt, amely a lépés logikáját valósítja meg.
A fentebb említett oldalakon van módunk változtatni a visszajátszott játékok menetén, ez most itt nem lesz lehetséges, viszont a lépések között oda-vissza lehet majd lépkedni, és azt személyesen elemezni.
A program rendelkezni fog egyetlen egy opcionális parancssoros bemenettel, az pedig vagy egy a PGN-t tartalmazó fájl elérési útja, vagy maga a PGN lesz. Ezek közül kapcsolókkal (flag) lehet majd választani.
