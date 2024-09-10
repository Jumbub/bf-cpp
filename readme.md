# a pretty fast brainfuck interpreter

it's possible that this is the fastest non-jit interpreter (at time of writing)

it's also possible that this is just tuned to my system and won't be fast for anyone else

<br>

### running the interpreter

build:

```sh
cmake --preset release
cmake --build --preset all
```

run:

```
./build/src/standalone/brainfuck samples/mandelbrot.b
./build/src/tests/tests
```

<br>

### the competition

mine:
- (0.77s) `./build/src/standalone/brainfuck samples/mandelbrot.b  0.77s user 0.00s system 99% cpu 0.769 total`

other non-jit entries:
- (0.98s) https://github.com/JohnCGriffin/BrainForked `./bf ../bf-cpp/samples/mandelbrot.b  0.98s user 0.00s system 99% cpu 0.978 total`
- (1.14s) https://github.com/rinoldm/sbfi `./a.out ../bf-cpp/samples/mandelbrot.b  1.14s user 0.00s system 99% cpu 1.145 total`
- (1.16s) https://github.com/rdebath/Brainfuck `./tritium/bfi.out ../bf-cpp/samples/mandelbrot.b -r  1.16s user 0.01s system 99% cpu 1.170 total`
- (1.96s) https://copy.sh/brainfuck/?file=https://copy.sh/brainfuck/prog/mandelbrot.b
- (2.22s) https://github.com/primo-ppcg/bfci `./bfci ../bf-cpp/samples/mandelbrot.b  2.22s user 0.00s system 99% cpu 2.226 total`
- (3.72s) https://github.com/apankrat/bff `./a.out ../bf-cpp/samples/mandelbrot.b  3.72s user 0.00s system 99% cpu 3.722 total`
- (4.08s) https://github.com/dmitmel/brainwhat `./target/release/brainwhat ../bf-cpp/samples/mandelbrot.b  4.08s user 0.00s system 99% cpu 4.085 total`
- (4.49s) https://github.com/phunanon/Barebrain `./C/Barebrain ../bf-cpp/samples/mandelbrot.b  4.49s user 0.00s system 99% cpu 4.494 total`

the fastest jit implementation for reference:
- (0.30s) https://github.com/rdebath/Brainfuck `./tritium/bfi.out ../bf-cpp/samples/mandelbrot.b  0.30s user 0.00s system 99% cpu 0.304 total`

<br>

### notable micro-optimizations

- The interpreter loop is a good reminder that C++ is still actually a relatively high level language
  - 5% performance gains by using branchless looping
    - https://github.com/Jumbub/bf-cpp/commit/b722a046532dd8e3c8c86855452dd39ffc56522c
  - 2% performance gains by using sequential enum values
    - https://github.com/Jumbub/bf-cpp/commit/eb9b1714bd0e1bed281d94bde227510293d466cd
    - Generates better switch statement jump tables
  - 3% performance gains by telling the compiler that it will never reach the default switch case
    - https://github.com/jumbub/bf-cpp/commit/cafb46b4e5239c24966859327d6d6369cf06201a
    - https://mmore500.com/2021/03/24/switch-default.html
  - 5% performance gains by using computed gotos ([godbolt link](https://compiler-explorer.com/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahUAUgBMAIUtXyKxqiIEh1ZpgDC6egFc2TA3cAGQImbAA5PwAjbFIpAGYATnIAB3QlYhcmL19/AzSM5yEQsMi2GLieJPtsRyKmESIWUiIcvwCeGrqsxuaiEojo2ITkpSaWtrzOsb6BsoqRgEp7dB9SVE4uS3jQ1F8cAGozeI9sdRTsJ2xMY9wzLQBBbd397COTwVI2FiJb%2B6eLDsmHsfIdjh4XGNSNh2H9Hs9ga93h4oaFgHCAUCQWCTgA3S4kUgYhHYt7g1BjQjCYmAl6gskWCyxUhkAB0SEsFn%2B3MerA4ShSLA2ByipBYoRoPlQAGsjgB2Gzwx4HA7MPwHPYsJRKA64Ugs0jyqwHcIAeXCuHeABEDlpyAdVAAlU0AcUdDwAsgB9M0AFS9ADFTQpwlb7Y7cA8PAAJXBWr0ASXCIl9joUHl9CfNXqCCY9Cd99pDHoevpjca9VndHktZjlVuOiqejzVbAOvoAnhcDiADj5QkQAGzSL1EI3/FVm02qcgTg5W824WfK%2Belh5eh5WsMqne7vf7g8qgD0R6O1gOJ4OAFo51a116RLhC4eX6/j6ezABWKw3z8Nld3r666pg8yYBrgjr2m%2B0GXl%2BP5/NY4Jfv%2BDwqoB64egoQSZqoQQAJpQdBr6wd%2BN7xHciGIScyG3vemHYQmuF4RuoZelaCYAGoJlaS5ESRP5kRRNiUTY1F/rRQEPk%2BgbOt6SaqAoz5Ebul7Lqhq6SU6SZKcpb6XqyEnrqopraRBG5boRuk7rB5EXh%2B1EAfexmmY65nxgA6tGCZBLgPqmv6ABaEGmpZ75HN%2Bfx/nZ4WiR4NErkmKZphmWbhF6znhL6ZmPv6CYBl6QXOqF0VwXOiWpummbZhlWWuTlib5X6BXBVBJEoXWDbxE2/w%2BBkJgHBxEg%2BGS8Q2gOw6jo2PV9cABymjQNAqGOxxjcIE2/F1PLNg8UJSmOCZMLtThZOOK6dt2RBdiNNpTqoU0roNvhvLiQ3Xba93qfNi3YGO6ALUt1rvZtSrqQdR31BA51vJdFyLB9Kpg0QpB7VkkNXQcMPYPaj3DQcL1PXDwOg4dSMo0IaMXVd2OvXjr32l9AN/d9RCE919YfdNaIHFYHZENgXg4jalIgCA%2BJOGQ4KI8jx1CH8RO9VzAtvcLosEhLJyoEgzRy91K7/HqBqquolw%2BHzEA83zSsHKE4NZEoiynep40jmOmA/CwXppAOsSA1o8Malrhpu00cHxFo4d%2B1FK1Gn77NEwjJPS/UABU1uJ2TTCA5Yg426TMuHXBkedbr6mXv2a0u9bfNivUvv%2B5eKti4S4Ll0Q8QWJNtm50nWResjTBehgPjCBA3cZ4PqzCPaWis3ODdEJgIvfCk4JQ/are3GnttCKOV0T8PG0lzMzioHj6AEJgqe6H4KS%2BiwUSMKVUejY7%2B7Z9nt1qW/FiDtnC4Wl/Pc78f7oTcoA3cwDBygJyuAnckDQEgTAhBWBKp4F0SwjhfCKCzy/xAeghiTEWLxnYlxHi2C0GSXqgGWSiZwgKULHOCBP8/5OUdNpchzC8GaRMplMym4wyMLgZwqBTkeG1Tcl6Ty3lfJNUKiFQRqDhHlWSlVNKNVsrSTys1IqCicHZ2UZVVK6UxEaNyo1fy2j5F6zjiXFUwB0AkCvjfO%2BD9sBwTHvnQSmMaJE3%2BKA9Rrl%2BEgDnMHD2XthA%2B1sNHDx9RBJMyWv7exJADgPADLVDmjwDEpWqiYuqmjzGBWCr2OsTYVQYBJgcFgpt0AHHiT9L0oTPbnwiYaaOjTwnVzPMaGJWQ4n/R%2Bv7AgNADgQFCXBOpRAGnuyad7IkUcLCfgOAswcBwwBgGjjPV%2BO4elCCzj/HZBdvzHwIKgQeWoNoogIAAL2wJ3XAo906eNuPjYacNxLxx3Ek9ATi2C33vo/b8ByvFXR8aU%2BUKE7EOJqak9Jvj2YgwQe6JBjpikKjnOUsYlTqm1P6ZM9pzTOltOmR0yJ54gW3Amf7MZ34JlTKaDMlpXSt551ic816yEulEpDjS3FdKwkEtiKCkJ7txm8vxbMjlGzElQpSWkiCHN4XbVAUErZ1KrDisZbYZlPchB9OZpKqiq1t5MEEi8txHyvmythWzdqjxQH0UwXhVFYKMVjiqck2lGrCUvy9aS7pjzWXkUpR811WKPWmxSKbHU0SA29LZU9f2nwRnupqQQOuXVrbIlqRGqNjZrZROsA7EpgiDkFtEmClUarPXEoFa0slsbdUUtxQa0SRqWVxvImag4qcq1iprRK95YKOrCu5VYat9KSVzJtFKi1MqYXyrhbazc94crOvRUITFKacXMz5Qy71NpfV1v9cavVCSPm9p3YeyVL9yWdvZbO5J87HQKqXVk1RxiXJSTMZYtdK5Q1bvHfy2ZgND1MtvbgYNYKhkjIvUtXdk6DWLOWdaDZRa0UrhVAcvZOcG2HKsMc054gxjggyDcu5DyT3xteUKjDBwvk/L%2Ba49xuHgUXBo%2BpYdK5LVPpfVtfxuSJFSJ8n5QpzpgkrgAO5IAYG8UZIrvyga1eByl8ykOfhWWs1DWzK39s1Ya7VGdTX3qHYqyFj65XPt8Xa1h2lf3qX/diwDe7Ik%2Bt04S%2BtlGg3No%2BUmh5Y400zu6Vm8DZq80EDLWhitBwVZD2WtRE40WmgnzOcRjWgdbhydHU5hDqmlnqdntYiFdG50Wd4wild0lqGmjknQxSdmykbrdY5vtE7a0gbc36gzTyvP6p82QEZA5M2BaGwlkLxn80iUi4IzWzQ06RsPoImLoR3h3FsirJg6RpQEBSBJ6N5FN6hHm/7TDwyIBrKW0wVk2A/oQEWFN2jOmsstaAy069q0jsfNQaZ8Fc5uOlcXXx/BjqiFsU4txXA9WNSNbDTU7LbWuUvfc8e9tjaetnpddDgDOaiB7bbTqk1VHzVQdw2W/2OM3jEFiD8O26awVSZkzB%2BTY7nvOanWeNTGn1kv02cW2jsH6lXvma2rrgbcChc%2B1XKn9QlCk4%2BZx7aKpfNboC9zsLWbVhEHmzLjN4XJvaZF1kWXUX%2Bd4o60eg3qOIPNqFzGzzYuaap0pzXO2/t5dmehf9m1fHFzidLqeHAUQfDAFZJgIQ2Bbv%2B2hEQNYmcDZkBFmaC0GTQKmmnL793VqF0lyfen6KAeg%2BsiRkKaUUsM4QAIyli5pHbkbXuVhq8FvDoh/drdu7gySciWlY4g419fkuIBVYcD3jB3chsVtDqYU%2BTYAFEKN4opxRMElDKf4XBlj0G4J%2BfgAReD8HQNwOKIlourHWAyeIfByBEG0Kv5Y0oQDxHiKyRIcpByfmWZ%2BeIcpPw8AsAADkMNwaQfgNgEAT8O0bfHQcgPfLgfgJQEAO0S/LgHQZYOAWAFADAHAfAYgMgSgagOgRgKfbgc/QQYQMQCQTgGQOQYQZQNQTQRAvQEACwQwYwUwMtLoAkLINwJgTwbwdoQILguYIYSoT8OUVIdITIIQSYDoUQwoLIAQ8oYYHgYQtg/OXoCYHgqYZQ%2BoVQ/oUIQYeQyoewcYVodQqQmYFoOQhYHgZYJQY/DYbgEkJEcEM4C4K4G4fbeEWkREekZET4b4WvbkTw0kZESEJGGENgGkLERwk4VEEwCIukHEDwJudWO4DwyI7w8kSkAcOIrwsERkZkNkDkRkcfXkdgafQUYUefCUKUWUXnBXVUJgdUTUbUXUfUfrEpE0RcX2e0J0V0d0b0JqIMEMbcCMKMWMeMN9IxXMfMZ8YsUscseMKsKMWsMfEGVsdsdGXsVudaLZT%2BW8RcQBZVCyKyPiD8c8S8G8RyShJ8YqY4/cfiX8IrRFUCEQcCSCW4u4j8UiBCWKeKdSe1DBRiLBd4vce474qJMSR4oHQE5iUCYhMHMhE4mKK8QSUnKiOKcSS49cKhGheSRSG4vSU8A4mzTKfE94/SQyD9XhQJI44EmyS0GyOKTEyk8RfhSRLyYTWRFqD4mKSKG0fiJCDE0GZMCqbJNRATeqLRORYqfkhKYUlRIxAJL9BqETSxVqT49qFY7aBWfqcnQGZ2TuJsbU2aBmH6PUiuA04onaFlA4UvfOLZKGDGdGaOW6MnGmLtQLOcE036XFWnLaBOY1CmaGK6ArYmAMh0zGamJ6WmAmf2W0iGcMqmAaN0umOaH0iZEM4dKzB4I07mXmfmLAZWBeEWJIokE4OMw3fbeWGaA4K2aORuNWUsjwGbUs1bWxI4R4OPQ0M4E2M2C2fMw4A5e2LZfU12M3X0v9QOA4NVMOCON7GOTMsFcsoQR3XDbDQcwudjOeU8LYyuJ3anXZD0lceeReVWcWRs1uduO5RvPuYefeEeA5O8ogaeEMsKFWZeVeRMjeLuXDXeC4R85PFUMIdQS/RbCTYgTWAbFjW4TGe7dSMpLUN4fjT9IJXRRTfTZTbzKLFUUUGEaUY7DUBCm0uUwxHJT9CUgpH9fXBrCpADFnSddrVrYDJTKCtHAZCXE7RnJ7S9M3DlTTbnWCw8LDaObOdco5JLE5SvEja5GvDLMbGMkfB7T5GVIChbRSt3PcHClgPCiXdS%2BClQDSYCJFF4iCSHXcBzD1Oi%2BHVzRivTZHfHU9NirCqcpnOHJijzFHAnO9BNG3aylgUVbimy6udjfcE3eDWtOcv2dikUaELS/C3Sgi/Sw4q0UyncNVNCuywzJtfVHyoWcSwjc5ckdLciCjDyozGMqKzS7SqLeKojRCqEpiFK6izdZrAKxHFzA9Mc5iu3SDXRcy2HbHXHRvMq4afCxXfrCAZXWnEbDwbNTXXNHXCLKi7ZDvYSctXRR7PynlVq1nMDFiq3bK6dQ1davcCvIjC5Zs2SvartHtFyyygdK0F8oBb7XcUKwXadVXCqmKqqwRGqwi6BJ8RqqHGilquDRTXynarq0qrK9HVC267anLd63K6nfK1LJsoquvK616R63cSquK56hKt4CY0iqkpUyUopJavq7dUGzq8G%2BiyG%2By6Gxy3RaDTLTa5neG8K5%2BTqG0HndDOCvcISl%2BES3DGXMS5GyS6I6S8jOS6jBS/m3cS1FS0an7RS3GnS/G2qgy5kvhLcNk6RFUuRXPXcenRgTitm9Koaxm34KKPinmgSg8dK23KGrykaqK%2BK7Cr6/CzW/xNhTKQGym1yzVWm%2BHdyhm1i1S%2BWg4XzQbFXHmtXcES2l2onCbVa%2B2/cGLDXZEBO06gqtLbWYq16niv8bGuBfGj23CvGorPSuqq4/0KrGrehf2zHEGgXGmjqwKzrDC3rJy6O4QEbOOnXYLTGhNBavXWow8ZsubU2ZWlUC7FbTedbTbbbXbee78j7Jyjis7MAC7K7G7O7Jal6uG6mjutnGNde4692lWyOtW6qjWv6%2Bq/CEHEhcHJu4Giyjmpi4OtyjK7rfamGxSgOgas0u3cXJy0tTvKK3Uvc6XccyOk22TQuk%2B3irnO2g%2B5yri4%2Btq0%2B/THO1Gi64qmW7AEu3caBl3CBpy%2BXXRJXbFWOoGILBOjXLXMLRa8eg8cB1ame9BtmwOpHROv%2BgZHKxLcWs6wq/OjGkBh3SXZ3DdYhq%2B/cG%2Bn6u%2BxKn3XRKPGPFog0BPRcL2wi26I2ncBRvnbAGgKpegECxSr0L0QPBgZwAeYeGKmbVxCPdWpdEhla8EkuRWs4COhc8fesSfUomfCosUKo5fR4Vff/LgTfcgcA3fffMtI/NYYUbYCwfgBApA5YJAGEHAOIW7SJwA8gYA0AmJq/SA7gGAuAi/K/O7cgW/CwLQR/OULQT8H/dTHgRIH/LQaQT8dTSJ%2BILfUpqAtJ6p8gFAhAeACANA9AX5GTCgKgCADAGZxgOIYAGQToPA6uWAiAKIUpqIUIZoDsQg/gPZ1gUgDsU0KIXQAkI58gRZjgYQU0JgegQ5ug8gfPYADwCQegWAnfN57Ab4EwSQV5wgaEY6fEH5iA7s1AU2TYc/b2dfV5%2BgAgefM5pWUppGAgYA35/EUgKIdIbAK0f55gpFkwEZmgIwYAJQDiAgbACTU0C4bfIg%2BQUgyQCg4gxQFQDQUp/QToIwUl8wESQwZF2AyAZYdAFIaXbgK8U0eIa8K0UMXAKwBQF0a8SkFaYABostQEXfHF0gC%2BafeAaw2odg1wCAdwSQhgu0YIXQ%2BYYYe/aQ8Q7IEwy1h1%2BoCwu1%2BITQnoIwi1%2Bpr1oQbQ91uIe1sw4w3IAIP10NoNu/eIaw2w8gtfDfAZ15qAg4dQH/QcK8YcOjVAU%2BGQVkHgEZTAwkM8M/RYYZugmprJlgHJ6gG/Bg6QVkZ/OURIRIQcRIT8RIaQLQOUD/EQhFgpopsAwZ8p%2BwSp9JiJsZyZtAaZlIWZnAhZ2d2ZkAVZ6QdZmx2ILZnZ15k5g5m53ds5i5q5pwG5u55gIgR5550p95z5%2Bgb5m5nAAF4AIFiAkF9g8F0pqFmFm5%2BF0ppFlFjsNF15jFrF8/HFvFlQQlp9kl0AStgQClqlmlulhlm59lll8g2Qdl6grl15/QRgvllgwV/9kVvJ8VyVrgaV2Vq8eVniJVlVq8NV0aDVnwLV/pyA3V/Vkjo17oU18151iwToa10oQQl1goR131zoMTt1m1kTyN41lQn1/j6YeTrQow6NyNxT8Nhg6YNTmT/Qy1uNpJzgCwRNqJ5NiA1N9NzN7N4AXNg4fNwtiAYttowEEzitjJ8gat2tvJgdoAkA4dlN0d2A%2BA6p%2BthZVkJICwJ/dpxIM/HtighFtj2Jsp6AqpuDqdlAQgBaBdtD8QVlzD%2BQbD2giA%2BgJAWA3lsrrLmgTGSp0rlQYCsUMjjg1L0gMrhg8gVrpQKrmru0Tr7xxriV5riJhF6J5LqAq0IZYZal2ln2KzrN6QHNvNxtwt3EHUObmzuzhz9z6/WpkAh/QceIYcaQQEH/Jpn/H/KL2QXzkpwL1L4LtLjzhF1Jm7iz0didmpnFjIVwaQIAA%3D%3D))
    - https://github.com/Jumbub/bf-cpp/commit/c26dec0dc0ff1661c6a151fca66b733d65ea789f
    - Simpler instruction jump table
- This was the first project that I was really conscious of the idea of code locality impacting performance
  - [2016 LLVM Developers’ Meeting: Z. Ansari "Causes of Performance Instability due to Code ..."](https://www.youtube.com/watch?v=IX16gcX4vDQ)
  - ["Performance Matters" by Emery Berger ](https://www.youtube.com/watch?v=r-TLSBdHe1A)
  - 4% performance gains by changing the order of the cpp files in the CMakelists.txt
    - https://github.com/Jumbub/bf-cpp/commit/44017042f7515248d655477616b7ac0e224861bd
  - 8% performance loss by removing dead code
    - https://github.com/Jumbub/bf-cpp/commit/a4e1c4d73862980b61900bfece9528b0df31aa00
  - 15% performance loss by changing value of a constant
    - https://github.com/Jumbub/bf-cpp/commit/adfb837e8103fb54aa39f6109d65b609607c3297
- Optimising for CPU instruction speed over data compression
  - 11% overall performance gains by switching from 8 bit data types to 64
    - https://github.com/Jumbub/bf-cpp/commit/0e52ab9c875a291a1e3ba4bf2d8b520ede0da11b
    - https://github.com/Jumbub/bf-cpp/commit/039f096668d2f592e41f8033bb16ecd579074cbe
    - https://github.com/Jumbub/bf-cpp/commit/53e31164d85d45b4e94c394c210fa60a00ca58c5
    - https://github.com/Jumbub/bf-cpp/commit/1bc35a79799e14713de64ab7d2dec016060afd1c
    - Current generation CPUs are just much faster when operating on 64 bit data
- 13% performance gains by ensuring an odd multiple of 64 bits in the Instruction struct
  - https://github.com/Jumbub/bf-cpp/commit/0f2aa88c0d0a37dcc314bc111f6ad272ebbab2f2
  - I have literally no idea what is going on here, data locality? instruction locality changes?
- 12% performance gains by using pointer arithmetic rather than using a dynamic index
  - https://github.com/Jumbub/bf-cpp/commit/113145cbb590eae8c62cbd1c2affc44d3ebce265
  - Who knew indexing could be so slow

<br>

### references

- [brainfuck optimization strategies](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html)
- [some brainfuck fluff](http://braifuck.org/)
- [Brainf***](http://www.iwriteiam.nl/Ha_BF.html)
- [Computed goto for efficient dispatch tables](https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)
