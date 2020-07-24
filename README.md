# C-Chess

[Work in progress]

ASCII Chess game in pure C

Compatible with UNIX-based OS, but only tested on Mac so far.

![Terminal Screenshot](../master/screenshots/demo2.png?raw=true "Looks like this")

# Build

## Make
```bash
  cd src
  chmod 711 make
  ./make
```
## Run
```bash
  ./chess-b [args]
```
## Arguments
### 1 args
```bash
  ./chess-b 5
```
Run perft(5) (count leaf nodes of game tree up to 5 ply)
### 2 args
```bash
  ./chess-b [PLAYER-1] [PLAYER-2]
```
Start a game, with AI players using default settings
* `0`: Human player
* `1`: Fixed depth alpha-beta AI
* `2`: Iterative deepening AI (**recommended**)
* `3`: Quick-and-dirty minimax AI

### 4 args
```bash
  ./chess-b [PLAYER-1] [PLAYER-2] [DEPTH] [EXTENSION]
```
Same as 2 args, but specifies depth of search and maximum extension depth for forcing lines.
