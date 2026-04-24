# Movement

Movement is a mate search chess program.

## Usage

```
Movement [OPTIONS]
```

Movement reads problems
as [Extended Position Description](https://www.chessprogramming.org/Extended_Position_Description)
records (with one operation: `dm` for direct mate or `acd` for perft) from standard input until
end-of-file, then solves them and writes solutions to standard output.

## Options

- `-h`, `--help` Shows help and exits.
- `-V`, `--version` Shows version and exits.
- `-d`, `--detailed` Enables detailed analysis.
- `-v`, `--verbose` Enables verbose logging.

## Example

> Sam Loyd, The Detroit Free Press 1878

### Input

```
8/2N5/8/k2NQ3/P1R5/Pp6/n2p4/1K1B4 w - - dm 2;
```

### Output (default)

```
Qe5-a1 [#2]
```

### Output with `--detailed`

```
1.Qe5-a1 Na2-b4 2.a3xb4#
1...Na2-c1 2.Qa1-c3#
1...Na2-c3+ 2.Qa1xc3#
1...b3-b2 2.Rc4-c5#
```

## Author

Ivan Denkovski is the author of Movement.
