# Parancssoros Sakkjáték (CLI Chess) ♟️

Egy C++ nyelven, objektumorientált paradigmákkal megvalósított parancssoros sakkjáték. A program nem csak a hagyományos sakk szabályait ismeri és tartatja be, de képes szabványos PGN (Portable Game Notation) és FEN sztringek beolvasására, lejátszására és generálására is.

## 🌟 Főbb funkciók

* **Teljes körű sakkszabályzat:** Támogatja a speciális lépéseket is (en passant, sáncolás, gyalogátváltozás).
* **Játékállapot felügyelete:** Sakk, matt, patt, 50 lépéses szabály és 3 lépéses ismétlés felismerése.
* **PGN és FEN Támogatás:** Korábbi (pl. chess.com, lichess.org) PGN fájlok beolvasása és visszajátszása (oda-vissza léptetéssel), valamint játék indítása FEN sztringből.
* Lejátszott vagy félbeszakított mérkőzések exportálása PGN formátumba.

## 🖼️ Előnézet

A játék úgy lett megcsinálva CLI-n belül, hogy az a chess.com alapértelmezett felületére hasonlítson.
![Előnézeti kép](assets/chess-cli-preview.png)

## 🚀 Telepítés és Futtatás

**Előfeltételek:**

* C++17 fordító (pl. GCC, Clang)

* Make

**Fordítás Make használatával:**

    make all        # A játék lefordítása
    make test       # Tesztek futtatása
    make sanitize   # Fordítás AddressSanitizer bekapcsolásával memóriaszivárgások kereséséhez
    make debug      # Fordítás debug módhoz
    make clean      # Object fájlok és futtatható állományok törlése

**Futtatás:**

A játék indítása normál módban (alapértelmezett FEN állapotból):
    ```bash
    ./cli-chess
    ```

Játék indítása egyedi FEN sztringből:
    ```bash
    ./cli-chess --fen "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    ```

### (Rövidített forma: -f)

PGN fájl betöltése visszajátszáshoz:
    ```bash
    ./cli-chess --pgn [fájl_elérési_útja.pgn]
    ```

### (Rövidített forma: -p)

Súgó megjelenítése:
    ```bash
    ./cli-chess --help
    ```

## 📖 Fejlesztői Dokumentáció

Ha érdekel a program belső felépítése (osztálydiagramok, algoritmusok, és a Piece ősosztály logikája), kérlek olvasd el a részletes [Feladatspecifikációt és Tervezetet](docs/Feladatspecifikacio.pdf).

## 👨‍💻 Készítette

* **Balogh Tamás** - *Programozás alapjai II. Nagyházi*
