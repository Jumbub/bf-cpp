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

### Notable Gains

- Impact of control flow
  - [3% performance gains by telling the compiler that it will never reach the default switch case](https://github.com/jumbub/bf-cpp/commit/cafb46b4e5239c24966859327d6d6369cf06201a)
  - [0.7% performance gains by using goto rather than while true](https://github.com/Jumbub/bf-cpp/commit/e35b7e3ac1efacba1df80229dff052b222a94f9a?diff=unified&w=1) ([godbolt link](https://compiler-explorer.com/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahUAUgBMAIUtXyKxqiIEh1ZpgDC6egFc2TEFl3ABkCJmwAOT8AI2xSKQBWC3IAB3QlYhcmL19/QNT0zKFQ8Ki2WPieJPtsR2chESIWUiIcvwDZB2wnLMbmohLImLjE5KUmlra8zomBsKHykaqLAEp7dB9SVE4uSwBmMNRfHABqMz2PbHUU7qJsTHPcMy0AQX3D4%2Bwzi8FSNhYiI9nm8LAcmEcfKdzh4XONSNh2EDXu9wZ9vh44WFgEiQWCIVCLgA3W5kHEo/FfaGocaEYRk0EfSGUiwWOKkMgAOiQlgswL5r1YHCUKRY2xO0VILDCNB8qAA1mcAOw2ZGvE4nZh%2BE5HFhKJQnXCkdmkJVWE4RADyEVw3wAIictOQTqoAEoWgDiLpeAFkAPqWgAqvoAYhaFBFbU6XbgXh4ABK4W2%2BgCSEREAZdCg8AeTVt9wWT3uTAad4e9LwD8cTvqsXo8NrMitt5xVb1emrYJwDAE8bicQCcfGEiAA2aS%2Boim4Hqy0W1TkacnW1W3ALtVLisvX0vW2R9X7g%2BHo/H9UAelPZ2sJ3PJwAtIvbZvfSJcCWT%2B%2BP2eL2YElZ7wlm3XR8Ay3DMXjTYNcBdJ1P1gm8fz/IFrGhH9AJedVgK3b0FGCHNVGCABNGDYI/eDf3vPYnmQ5CLlQh8n2w3Dk3wgjtwjX1bWTAA1ZNbVXEiyL/CiqJsaibFogD6JA59XxDN0/VTVQFDfEiDxvNd0I3aTXVTFTVM/G8OSkrdVAtXSoO3XdiP0/d4Mo69v1ooCn1M8yXUspMAHU42TYJcH9C0gwALSgi1rK/M5fyBACHMi8SPDo9dU3TTNs1zCJfVciIAwsl8g2TYNfRCt1wtihDF2SjMsxzPMspy9y8pTQrAyK0KYLItDG2bPZW2BHwMhME4uIkHxKT2e1hzHCcWz6gbgBOC0aBoFRJ3OCbhCmwEev5NsXjhWVJ2TJh9p6IQp3XHs%2ByIXsxvtWdVBm9dht8L5CRG26HUezTFuW7BJ3QJaVrtT7ttVTSjpO%2BomAgS6vmum4Vi%2B9UIaIUgDqyGGbpOeHsCdZ7RpON6XsR0HweO1H0aETGrpuvH3sJ96nR%2BoGAd%2BogSd6psvtmrETisbs7i8Al7RpEAQGJJxSQuFG0dOpggVJ/reaFj7RfFklSCpJBmgV3r12BQ1jQ1dRuh8O4IH5wWsC%2BMJIayJQVnOzTJvHSdMABFhfTSYc4mBrQke1bWTXdpoEL2LQI/9mK1tNf2udJ5HydlqGACoTltim5eBywRwz5P7YQqPur1zSbyHDbXfTu5JShv2A5vNWJZITWLnLog9gsab7LzymmF9NG%2B4wHxhAgHu5d9IfhCdLQOcXBuiEwMX/hSaFYadNvHnTpPe4nG6J42OlScXAB3JAGC%2BCAKewR3G1bA8lGP4hUCQE5R%2B3uXhJxm/lUXdVxBULSJkzLZQsjuW0IBf77hDp7b2whfa2BjmPKGwlWYrQDgeCUCI5ToO1LqL4lVUo1QynVXKskCoBWCqFAct9IF/yEOME4LAzboBOKgv6vpoFe3QD7E0MdOGwOrpeM0SCsgoMBn9HB6oCA0FftAhCbCiAcI9lwnhqEThgDADHGeTsTwiLOjHHOejjoIXGACAgqAJ66i2hiAgAAvbAXdcBvztkIYSRNRqI0kgnY8GBhBhFGpIpUaEjyYJYNg7x6ouqQP/l8TCvowIQSgtQn%2B64Dy%2BIYUwkgrDxGKP4dwuBvDxonDyTwoRW8XFMDEWzQJcjfwKKUU0FRBSylGLce9NRCCim1KsPUkpBS6IRKgR7eROSGkwPydXDpRT/aDPVKE8Jd99xRNSbggBcSwHJMWQebpfTBG2HKZnZBjwFEdJoiLJozgLH/2sc/HWlFnGHNEY8dx19AnzJwcszSf88GAN9IxPChFNm0O1PQycmSWG9OUQI%2BBXSoUTPgVeVpxycmBPSWC5hrCzYpDNvqRB78jmUReYE34r9wXpzrj1cl0JMVEGxUQJQLZ04IOsN/LZh4jHMvEmy7Zwy6mjN2Qi4R%2BKnmUROdHM5wKjymMuZY8YWs7lOKRYS%2BmadumQsadCzWAFZ4rMiVzXVxTeU9P5XC1R0dplvPhGEj5%2BqvmrNiU%2BPKQKDVosYRi9V4zSl8NNc0/ZSrcAnNmYa0OfK2ZjKaZM815yzFXKsfKluirhWuOee9HVdq5lWoWZAz5aSfkEOqulTKwD6oyXyoVYqFpnXppBeTN1WSPURphfaAVvDEVJsqci6pQbpGyKNQ2zVUzur2m0TQg1Uj23ZwsLndtDLfzSvMbK6xGR7GOIefnZNyriYDO5fuXxzgmABKDTmw87zBnHu%2BWslyxbQG7l9N5Xy/kWoVogQa0%2B59e0hqsC2lp7aqloJihorRrLJXBs9b6s5Bz10ds3Yend579ynrZTE35OlspVqPK6sl/b4WFObT6vZbaKl/okUGklb9JwEApcI9EkHe5tJeoyggnLgNjpOGrIeq1aIXDYxchd1z42PAgGqk1GqcOoTTUeeDGasE2uCbun5cTGrBnkimCISkAzocPJh91ImwN7NhaJr1hHHkboDSi0jZBX7DipRayllHqX%2BqJXZ5jOifFBy3nSwJ6p2NhG%2BE8eyasmDpDlAQFIx9cWUU3mETzQapEyIgBonzTAOTYABhAFYLHq08s/dhs19o8UxZ3XquTB4pPikzbJ6JCmGI4QBaxcCSZOI8T4pptJoK60Qt0423DoHus/qI52tBQbtP1qxTi4Gjn2ndvbZywJ%2BMbbVzMfQqjwK32MA/SwEZYaW1qMA9MzLJ5hPbfw3EU54lo0yv4xcW5Ca110ZTVu2LVc4hLeMWJQJnzgVkbJfZ2z1HqUbFpTixjLnR1Zdo3LWbT2jsrXDZq/rJnoNmeqeK87PGY2LoE/cybL0Thp2IC9qGDsPu2pCRVs9pP5NrJXC%2B6t8IiCbCYAaI0ZAxaWmtDg5D91adk5k4MnANAmH0CIDzw8vpfTRCHMLsIvph5WtudERg6XKtgwPBy973jPvrnp4z5nxo2crm5vHEuXUIqCmwMKUUXwJRSiYDKeUwIuBrHoNwBI/AAi8H4OgbgCUxJsY2FsZkew%2BDkBF1wHQGXyByhAHsPYHIACcioRxJASMnvYioEg8AsAADkMNwaQ/A2AgASI6D3OhyDe64PwJQIBHRh4j%2BQOAsAUAYBwPgYgZBKDUDoIwc33AQ%2BCGEGICQnAZByGEMoNQmhw96BAMkIwJg0CcpqHULIbgmCeG8O0KQ8fyAhHmGUCoiQeAFAyFDKYAQeC77SGfrIgxD9LEzyv24vRZgX538/uWfQWj3%2BGJUJ/pikwW%2BeQV%2B9gswv%2Biw/%2BPAawSgAe2w3A5IaI0IVwNwTg9w9IeISBPwZA/wW0TwyIDIqITI6IsIqMCIbAGBjIBIGIqMWIlBRB1BTcUs%2BBuIVBY0Hg1IC8w49BFIl4rILOpAXIPIO0wI5uluYoNu0osoCoYOi4HY2oRgeoeulmt85oK4fsToroHoXofoLUoY4Ye40YsYCYSY%2BaaUeYBYRYb4ZYFYVYSYtYsYDYxuO08hsM/Yg4LsE4rm90GkGEK4vhvyYCpUNkx48EV4N494zk0keUwRIRh4gk/4cmcSCSIgkE0EcRR4CRSE8UiUmkcS/yzEhEsRNkWRkWGuCUkkURWEtWhR9W7ETWvE/EcE345Ewks2NEFRSRjqskymFoCkamykxR746kxkmULoukQxIRhkoxJC7kQRGREUm8dkCUVRRabkHkd6PkfkFCrUJUmRLR4kTwMUgkKElR4MaYVU5hxC16DUZC5abU%2BxcUFUFxhChasxpaTUOxFa7ULRnUzhYMSsg082E2FcXcrYgJ80zMf0IJo4rs3MYMkMJwMsvcrmbhOMwM90AcwJLyK264UJ/0OSuJu0icFS1McMN0EmSJ7aZJ2MtMQ09MLylJyJcsNJOMdMuOLyTMhJCiaaUSR8rwEJfMAs2AKswMjcGs0IzJUMusPMg0opMc4pksLcHBQcMpquZwrwhslmVwps5slsIp1sEOROB2W8sJXh36WiAcN2oGYckcRc2cyoccxcFU7a%2BOE6BiU6Ris6VgRc8JpcF4bcm0z2NcWQRJEUipzc0IbcHcjiRpWQ/cw8%2B8w8RAd248k8RA08lJ88i8IAy8q8dJG83c7au8NwSZh8Jc6o4Q6gIukCD8T8L8qZBKuAX8rmF6Dq2kNxHkou2WemgqeKA2oq5mO6iGVWACZhRCaxICtxZaXxVCrZu67WWGXW8O3qhm4GQqA5yOQ2O6PaQmfay5YmAGmi%2B285au7pRShiM6JivGsacqFwy6DieBjZIquAjJ26IGwA6AWSVZW0cGlOGC5ObK8GyGyRXoiSLorWC5taS5x2a5%2BmeGcFgqcZpmgaO6MO7CFpxmUG9GASqOq5m2oasOO2XiaF%2B5sFvZWq%2BWf2wKI5KywF1W0kGyp5Npv4mFG5iOxGgIeFRS86t5Nyqp2Ov6D2HilqfOQF/59qvyBRLEkFdC0FOm5FfW%2BFSlWF92g5XaO6I2EKY29KIJm5TmO632GKv2w6jGNGgOdKDKzmYkJpR46uokXKIG6FuSJ2ra7F2Fg2Ei3FJW74vFmO12Alia%2BlKqLFxqilA62qJOPl6ozlcOh5VFplQatFdq9Fl60Rr4slNaGSClRFrlwMbFyFSOqFwKsVxFCV6Ol2caAVCqz5pmb5SVgF2aElyG45bxnZjU5Cz6zFWl2S4VOG%2BVeVfqQl6l25wKu5pVrlg6QGzF46FSk606FS3pflV2NiK6T5OOIlJFH5X5LCP5UVNFjVdFzVDFQC6xYCmxD6s5bo3Z%2B4a2F8Oyg1qlH8nlXF9oe2w6tlh4bF/ZHFwl2A%2B1BqyVuaaVJk4xaG3Vi5OVGFg1BmFFCOHlI1JGhllm5GNmiV/23GG1/11lDln198C8YsHG6I1Ky1VVKpNVE1iFlFlJpWEl0m1qFOPlIF3RQYvR/R6mmVPVuWvqsNKl7lalW5SNX2KN1mJlIMGNHghVOF2NwioOKS4O1p0WZsXmPGOZqAvmCsNogWwWoW4WfmUWTAhWIGu5iWBNaAYQKWaWGWM1QyOWB5eWIJRtrGZWZWQNSyx1INfyNRLEbEjW3EjRnNkN9a9tPNCFcNQ1m5xVLqQd2lQOulP1CNr5U2O5M2GuO6wJBOIZy2lpQad1G2W2uVVNu2x5H1NtlNFFZ2CpN5/l5Nt2WNNN7Ki2ROUOf5nUBqRlWSYtMyEtNKllIONlNt9lnSRWoV3NBG/Nz1iNr1QiVdGOK1N2gmWNeOwZr2xOR6dN5WYlTVTNCmNOwKOupATOWp8QIA7O/mSGPy3OB1W9KyAuQuNZBq4ukuDA%2B6suTA8u2siu2AyujNO0Z5FSLdO0n535Vwv5fI/xu0pusUYhIoEhkoUhDurwTueeXAbu5AZeXuPunK/umwYo%2BwFg/A9eTuawSACIOA8Q6WKDBe5AReJe6D2gmDVe9gteoeDDke0eweHI6eMgI4ewCQ0gY4io2eewu%2BLuXAew7uDDFe3AhDbDjeiATeyAaA6AbAKQ58FAVAEAGAqj6jIAwAMgJ%2BPe1cNeEA0QUj0QYQzQ3Y/e/AFjrApA3YFo0QugtwNj5A2jHAwgFoTA9A1jM%2B5AOAkuwAHgEg9ANenuAT2A/wJgkg/jhA8IPQxI4T5eOpqAZsOwIePsYj5e9ABANuDjKsUjtBReETxIpA0Q6Q2AtoUTxgwAuTi%2BM%2BawNARgwASgXEBA2Ax8FoNwHuA%2B8gw%2BkgY%2Bg%2BigKgGgUj%2BgJ%2BC%2Bpgy%2BuT0QNekAaw6AKQRO3At4Foewd4toEYuAVgCg7od4NIa0wAB6nKoIXuZTpABAOA8zlDXQcs6%2Bm%2BuQl%2Be%2BG%2BEBR%2BJ%2BN%2BRQ2QwBzznzUMbzSwn%2BUM3%2BrQPzBgdzwL4BB%2Bf%2B4Lb%2BYLJ%2BgBcwpQML0B6wuDo%2Bzurukj/jleJw6g2eI4t4Y4JwwAqAqAJwMgHIPAr87ezcl4weKwsjjTJDZDIwlD0eFg0gHISeio8e8eI48eCQ8e0gWgio6eioVDhexepeUjle1eLDRDawijEALeKjajjAGj1A2j6rIw%2Bj0ghjL9cQJjZj/jdjVjbjZrDjTjLjTgbjHjzARA3jvjUjgTPgwToT4TIeOA0TdTOw5e8TL%2BSTUjqT6TbjWTUjszkoBT1sRTlzJTIeZTFTKg1TPr9ToAjTAgLTbTHTXTPTbjwzAzo%2Bsgwzk%2BYz/j%2Bg8%2BtTS%2BYkhgeTNzizyz9sqz6zmz2zuz%2Bzt4hz40xzPgpzEjFeFzVzFu8AMBtQL%2BrgEA7g7%2BJ%2B%2B%2ByLkBBgfzWQ07p%2BXzALlQQLr%2B/Qy7ELm7P%2B0L87CLcLTzsL/Qa7UgMBcBnAqwKDaDGD0jXAeLBLRL0gJLZLFLXL1LEAtLKhoIqwTLEeLLLA5D1AmLXA1DtDMrOLMjzDdebDawHLXLPLfLArQrIrYrioErYj/bd7crrDGbyrKAhAS0XetA/T4ggzxb8gpb0%2BOTSANekzdHRHNAOMLD9AdHoDkoSzUM0HpAdHc%2B5AvHSgTHLHjognHHLAXHa%2BVeoHt7sr3Ato0iMi7TnTvs%2BLhLxLpL5LlL1LhI%2Boanz7r7WnH7/7xDUegQceBLWgCQIj0gwrPAMgoIkr9DUHTDNesHzLKDBDzn5euHirawZTGQrg0gQAA))
- Impact of number casting:
  - [4% performance gains by changing data storage from char to int64, to reduce casting](https://github.com/Jumbub/bf-cpp/commit/1bc35a79799e14713de64ab7d2dec016060afd1c)
- Impact of number sizedness:
  - [2% performance gains by changing Offset type from int32_t to int64_t](https://github.com/Jumbub/bf-cpp/commit/53e31164d85d45b4e94c394c210fa60a00ca58c5)
  - [2% performance gains by changing Value type from int32_t to int64_t](https://github.com/Jumbub/bf-cpp/commit/039f096668d2f592e41f8033bb16ecd579074cbe)
  - [3% performance gains by changing Type enum from uint8_t to uint64_t](https://github.com/Jumbub/bf-cpp/commit/0e52ab9c875a291a1e3ba4bf2d8b520ede0da11b)

## References

- [brainfuck optimization strategies](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html)
- [some brainfuck fluff](http://braifuck.org/)
- [Brainf***](http://www.iwriteiam.nl/Ha_BF.html)

### Notable Competitors

- [Tritium "This is the fastest brainfuck interpreter around"](https://github.com/rdebath/Brainfuck/tree/master/tritium)

### Random References

- [Lambda + shared_ptr<> = memory leak](https://floating.io/2017/07/lambda-shared_ptr-memory-leak/)
- Neovim & CCLS config (with latest C++ features)
  - `lspconfig.ccls.setup({init_options={clang={extraArgs={"-stdlib=libc++","-std=c++2b"}}}})
  - Requires installing `libc++`
