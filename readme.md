# fast as fuck

ive always wanted to write the fastest [brainfuck](https://en.wikipedia.org/wiki/Brainfuck) interpreter

this is my attempt

#### Build

```sh
cmake --preset all
cmake --build --preset all
```

#### Run

```
./build/src/standalone/brainfuck
./build/src/benchmarks/benchmarks
./build/src/tests/tests
```

## References

- [brainfuck optimization strategies](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html)
- [some brainfuck fluff](http://braifuck.org/)
- [Brainf***](http://www.iwriteiam.nl/Ha_BF.html)

### Notable Competitors

- [Tritium "This is the fastest brainfuck interpreter around"](https://github.com/rdebath/Brainfuck/tree/master/tritium)

### Random References

- [Lambda + shared_ptr<> = memory leak](https://floating.io/2017/07/lambda-shared_ptr-memory-leak/)
