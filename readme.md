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

## micro-optimizations :heart:

genuinely my favourite part of C++ programming.

- [5% performance gains by removing instruction check and adding an exit instruction](https://github.com/Jumbub/bf-cpp/commit/b722a046532dd8e3c8c86855452dd39ffc56522c?diff=unified&w=1)
  - branchless looping
- [2% performance gains by referencing the pointer to the instruction rather than a copy](https://github.com/Jumbub/bf-cpp/commit/68e0ee97465877154d13e3cc133b67f566f379b8)
- [12% performance gains by moving pointer around rather than recomputing every loop iteration](https://github.com/Jumbub/bf-cpp/commit/113145cbb590eae8c62cbd1c2affc44d3ebce265)
- [2.5% performance gains by copying offset_data_pointer around to only evaluate offset_data_pointer where necessary](https://github.com/Jumbub/bf-cpp/commit/bb45703201c53c94e2f6d5eb659c5c6f38b6a356)
- [3% performance gains by telling the compiler that it will never reach the default switch case](https://github.com/jumbub/bf-cpp/commit/cafb46b4e5239c24966859327d6d6369cf06201a)
- [2% performance gains by using sequential enum values](https://github.com/Jumbub/bf-cpp/commit/eb9b1714bd0e1bed281d94bde227510293d466cd?diff=unified&w=1)
  - better switch jump table generation
- [3% performance gains by changing Type enum from uint8_t to uint64_t](https://github.com/Jumbub/bf-cpp/commit/0e52ab9c875a291a1e3ba4bf2d8b520ede0da11b)
- [2% performance gains by changing Value type from int32_t to int64_t](https://github.com/Jumbub/bf-cpp/commit/039f096668d2f592e41f8033bb16ecd579074cbe)
- [2% performance gains by changing Offset type from int32_t to int64_t](https://github.com/Jumbub/bf-cpp/commit/53e31164d85d45b4e94c394c210fa60a00ca58c5)
- [0.7% performance gains by using goto rather than while true](https://github.com/Jumbub/bf-cpp/commit/e35b7e3ac1efacba1df80229dff052b222a94f9a?diff=unified&w=1) ([godbolt link](https://compiler-explorer.com/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahUAUgBMAIUtXyKxqiIEh1ZpgDC6egFc2TEFl3ABkCJmwAOT8AI2xSKQBWC3IAB3QlYhcmL19/QNT0zKFQ8Ki2WPieJPtsR2chESIWUiIcvwDZB2wnLMbmohLImLjE5KUmlra8zomBsKHykaqLAEp7dB9SVE4uSwBmMNRfHABqMz2PbHUU7qJsTHPcMy0AQX3D4%2Bwzi8FSNhYiI9nm8LAcmEcfKdzh4XONSNh2EDXu9wZ9vh44WFgEiQWCIVCLgA3W5kHEo/FfaGocaEYRk0EfSGUiwWOKkMgAOiQlgswL5r1YHCUKRY2xO0VILDCNB8qAA1mcAOw2ZGvE4nZh%2BE5HFhKJQnXCkdmkJVWE4RADyEVw3wAIictOQTqoAEoWgDiLpeAFkAPqWgAqvoAYhaFBFbU6XbgXh4ABK4W2%2BgCSEREAZdCg8AeTVt9wWT3uTAad4e9LwD8cTvqsXo8NrMitt5xVb1emrYJwDAE8bicQCcfGEiAA2aS%2Boim4Hqy0W1TkacnW1W3ALtVLisvX0vW2R9X7g%2BHo/H9UAelPZ2sJ3PJwAtIvbZvfSJcCWT%2B%2BP2eL2YElZ7wlm3XR8Ay3DMXjTYNcBdJ1P1gm8fz/IFrGhH9AJedVgK3b0FGCHNVGCABNGDYI/eDf3vPYnmQ5CLlQh8n2w3Dk3wgjtwjX1bWTAA1ZNbVXEiyL/CiqJsaibFogD6JA59XxDN0/VTVQFDfEiDxvNd0I3aTXVTFTVM/G8OSkrdVAtXSoO3XdiP0/d4Mo69v1ooCn1M8yXUspMAHU42TYJcH9C0gwALSgi1rK/M5fyBACHMi8SPDo9dU3TTNs1zCJfVciIAwsl8g2TYNfRCt1wtihDF2SjMsxzPMspy9y8pTQrAyK0KYLItDG2bPZW2BHwMhME4uIkHxKT2e1hzHCcWz6gbgBOC0aBoFRJ3OCbhCmwEev5NsXjhWVJ2TJh9p6IQp3XHs%2ByIXsxvtWdVBm9dht8L5CRG26HUezTFuW7BJ3QJaVrtT7ttVTSjpO%2BomAgS6vmum4Vi%2B9UIaIUgDqyGGbpOeHsCdZ7RpON6XsR0HweO1H0aETGrpuvH3sJ96nR%2BoGAd%2BogSd6psvtmrETisbs7i8Al7RpEAQGJJxSQuFG0dOpggVJ/reaFj7RfFklSCpJBmgV3r12BQ1jQ1dRuh8O4IH5wWsC%2BMJIayJQVnOzTJvHSdMABFhfTSYc4mBrQke1bWTXdpoEL2LQI/9mK1tNf2udJ5HydlqGACoTltim5eBywRwz5P7YQqPur1zSbyHDbXfTu5JShv2A5vNWJZITWLnLog9gsab7LzymmF9NG%2B4wHxhAgHu5d9IfhCdLQOcXBuiEwMX/hSaFYadNvHnTpPe4nG6J42OlScXAB3JAGC%2BCAKewR3G1bA8lGP4hUCQE5R%2B3uXhJxm/lUXdVxBULSJkzLZQsjuW0IBf77hDp7b2whfa2BjmPKGwlWYrQDgeCUCI5ToO1LqL4lVUo1QynVXKskCoBWCqFAct9IF/yEOME4LAzboBOKgv6vpoFe3QD7E0MdOGwOrpeM0SCsgoMBn9HB6oCA0FftAhCbCiAcI9lwnhqEThgDADHGeTsTwiLOjHHOejjoIXGACAgqAJ66i2hiAgAAvbAXdcBvztkIYSRNRqI0kgnY8GBhBhFGpIpUaEjyYJYNg7x6ouqQP/l8TCvowIQSgtQn%2B64Dy%2BIYUwkgrDxGKP4dwuBvDxonDyTwoRW8XFMDEWzQJcjfwKKUU0FRBSylGLce9NRCCim1KsPUkpBS6IRKgR7eROSGkwPydXDpRT/aDPVKE8Jd99xRNSbggBcSwHJMWQebpfTBG2HKZnZBjwFEdJoiLJozgLH/2sc/HWlFnGHNEY8dx19AnzJwcszSf88GAN9IxPChFNm0O1PQycmSWG9OUQI%2BBXSoUTPgVeVpxycmBPSWC5hrCzYpDNvqRB78jmUReYE34r9wXpzrj1cl0JMVEGxUQJQLZ04IOsN/LZh4jHMvEmy7Zwy6mjN2Qi4R%2BKnmUROdHM5wKjymMuZY8YWs7lOKRYS%2BmadumQsadCzWAFZ4rMiVzXVxTeU9P5XC1R0dplvPhGEj5%2BqvmrNiU%2BPKQKDVosYRi9V4zSl8NNc0/ZSrcAnNmYa0OfK2ZjKaZM815yzFXKsfKluirhWuOee9HVdq5lWoWZAz5aSfkEOqulTKwD6oyXyoVYqFpnXppBeTN1WSPURphfaAVvDEVJsqci6pQbpGyKNQ2zVUzur2m0TQg1Uj23ZwsLndtDLfzSvMbK6xGR7GOIefnZNyriYDO5fuXxzgmABKDTmw87zBnHu%2BWslyxbQG7l9N5Xy/kWoVogQa0%2B59e0hqsC2lp7aqloJihorRrLJXBs9b6s5Bz10ds3Yend579ynrZTE35OlspVqPK6sl/b4WFObT6vZbaKl/okUGklb9JwEApcI9EkHe5tJeoyggnLgNjpOGrIeq1aIXDYxchd1z42PAgGqk1GqcOoTTUeeDGasE2uCbun5cTGrBnkimCISkAzocPJh91ImwN7NhaJr1hHHkboDSi0jZBX7DipRayllHqX%2BqJXZ5jOifFBy3nSwJ6p2NhG%2BE8eyasmDpDlAQFIx9cWUU3mETzQapEyIgBonzTAOTYABhAFYLHq08s/dhs19o8UxZ3XquTB4pPikzbJ6JCmGI4QBaxcCSZOI8T4pptJoK60Qt0423DoHus/qI52tBQbtP1qxTi4Gjn2ndvbZywJ%2BMbbVzMfQqjwK32MA/SwEZYaW1qMA9MzLJ5hPbfw3EU54lo0yv4xcW5Ca110ZTVu2LVc4hLeMWJQJnzgVkbJfZ2z1HqUbFpTixjLnR1Zdo3LWbT2jsrXDZq/rJnoNmeqeK87PGY2LoE/cybL0Thp2IC9qGDsPu2pCRVs9pP5NrJXC%2B6t8IiCbCYAaI0ZAxaWmtDg5D91adk5k4MnANAmH0CIDzw8vpfTRCHMLsIvph5WtudERg6XKtgwPBy973jPvrnp4z5nxo2crm5vHEuXUIqCmwMKUUXwJRSiYDKeUwIuBrHoNwBI/AAi8H4OgbgCUxJsY2FsZkew%2BDkBF1wHQGXyByhAHsPYHIACcioRxJASMnvYioEg8AsAADkMNwaQ/A2AgASI6D3OhyDe64PwJQIBHRh4j%2BQOAsAUAYBwPgYgZBKDUDoIwc33AQ%2BCGEGICQnAZByGEMoNQmhw96BAMkIwJg0CcpqHULIbgmCeG8O0KQ8fyAhHmGUCoiQeAFAyFDKYAQeC77SGfrIgxD9LEzyv24vRZgX538/uWfQWj3%2BGJUJ/pikwW%2BeQV%2B9gswv%2Biw/%2BPAawSgAe2w3A5IaI0IVwNwTg9w9IeISBPwZA/wW0TwyIDIqITI6IsIqMCIbAGBjIBIGIqMWIlBRB1BTcUs%2BBuIVBY0Hg1IC8w49BFIl4rILOpAXIPIO0wI5uluYoNu0osoCoYOi4HY2oRgeoeulmt85oK4fsToroHoXofoLUoY4Ye40YsYCYSY%2BaaUeYBYRYb4ZYFYVYSYtYsYDYxuO08hsM/Yg4LsE4rm90GkGEK4vhvyYCpUNkx48EV4N494zk0keUwRIRh4gk/4cmcSCSIgkE0EcRR4CRSE8UiUmkcS/yzEhEsRNkWRkWGuCUkkURWEtWhR9W7ETWvE/EcE345Ewks2NEFRSRjqskymFoCkamykxR746kxkmULoukQxIRhkoxJC7kQRGREUm8dkCUVRRabkHkd6PkfkFCrUJUmRLR4kTwMUgkKElR4MaYVU5hxC16DUZC5abU%2BxcUFUFxhChasxpaTUOxFa7ULRnUzhYMSsg082E2FcXcrYgJ80zMf0IJo4rs3MYMkMJwMsvcrmbhOMwM90AcwJLyK264UJ/0OSuJu0icFS1McMN0EmSJ7aZJ2MtMQ09MLylJyJcsNJOMdMuOLyTMhJCiaaUSR8rwEJfMAs2AKswMjcGs0IzJUMusPMg0opMc4pksLcHBQcMpquZwrwhslmVwps5slsIp1sEOROB2W8sJXh36WiAcN2oGYckcRc2cyoccxcFU7a%2BOE6BiU6Ris6VgRc8JpcF4bcm0z2NcWQRJEUipzc0IbcHcjiRpWQ/cw8%2B8w8RAd248k8RA08lJ88i8IAy8q8dJG83c7au8NwSZh8Jc6o4Q6gIukCD8T8L8qZBKuAX8rmF6Dq2kNxHkou2WemgqeKA2oq5mO6iGVWACZhRCaxICtxZaXxVCrZu67WWGXW8O3qhm4GQqA5yOQ2O6PaQmfay5YmAGmi%2B285au7pRShiM6JivGsacqFwy6DieBjZIquAjJ26IGwA6AWSVZW0cGlOGC5ObK8GyGyRXoiSLorWC5taS5x2a5%2BmeGcFgqcZpmgaO6MO7CFpxmUG9GASqOq5m2oasOO2XiaF%2B5sFvZWq%2BWf2wKI5KywF1W0kGyp5Npv4mFG5iOxGgIeFRS86t5Nyqp2Ov6D2HilqfOQF/59qvyBRLEkFdC0FOm5FfW%2BFSlWF92g5XaO6I2EKY29KIJm5TmO632GKv2w6jGNGgOdKDKzmYkJpR46uokXKIG6FuSJ2ra7F2Fg2Ei3FJW74vFmO12Alia%2BlKqLFxqilA62qJOPl6ozlcOh5VFplQatFdq9Fl60Rr4slNaGSClRFrlwMbFyFSOqFwKsVxFCV6Ol2caAVCqz5pmb5SVgF2aElyG45bxnZjU5Cz6zFWl2S4VOG%2BVeVfqQl6l25wKu5pVrlg6QGzF46FSk606FS3pflV2NiK6T5OOIlJFH5X5LCP5UVNFjVdFzVDFQC6xYCmxD6s5bo3Z%2B4a2F8Oyg1qlH8nlXF9oe2w6tlh4bF/ZHFwl2A%2B1BqyVuaaVJk4xaG3Vi5OVGFg1BmFFCOHlI1JGhllm5GNmiV/23GG1/11lDln198C8YsHG6I1Ky1VVKpNVE1iFlFlJpWEl0m1qFOPlIF3RQYvR/R6mmVPVuWvqsNKl7lalW5SNX2KN1mJlIMGNHghVOF2NwioOKS4O1p0WZsXmPGOZqAvmCsNogWwWoW4WfmUWTAhWIGu5iWBNaAYQKWaWGWM1QyOWB5eWIJRtrGZWZWQNSyx1INfyNRLEbEjW3EjRnNkN9a9tPNCFcNQ1m5xVLqQd2lQOulP1CNr5U2O5M2GuO6wJBOIZy2lpQad1G2W2uVVNu2x5H1NtlNFFZ2CpN5/l5Nt2WNNN7Ki2ROUOf5nUBqRlWSYtMyEtNKllIONlNt9lnSRWoV3NBG/Nz1iNr1QiVdGOK1N2gmWNeOwZr2xOR6dN5WYlTVTNCmNOwKOupATOWp8QIA7O/mSGPy3OB1W9KyAuQuNZBq4ukuDA%2B6suTA8u2siu2AyujNO0Z5FSLdO0n535Vwv5fI/xu0pusUYhIoEhkoUhDurwTueeXAbu5AZeXuPunK/umwYo%2BwFg/A9eTuawSACIOA8Q6WKDBe5AReJe6D2gmDVe9gteoeDDke0eweHI6eMgI4ewCQ0gY4io2eewu%2BLuXAew7uDDFe3AhDbDjeiATeyAaA6AbAKQ58FAVAEAGAqj6jIAwAMgJ%2BPe1cNeEA0QUj0QYQzQ3Y/e/AFjrApA3YFo0QugtwNj5A2jHAwgFoTA9A1jM%2B5AOAkuwAHgEg9ANenuAT2A/wJgkg/jhA8IPQxI4T5eOpqAZsOwIePsYj5e9ABANuDjKsUjtBReETxIpA0Q6Q2AtoUTxgwAuTi%2BM%2BawNARgwASgXEBA2Ax8FoNwHuA%2B8gw%2BkgY%2Bg%2BigKgGgUj%2BgJ%2BC%2Bpgy%2BuT0QNekAaw6AKQRO3At4Foewd4toEYuAVgCg7od4NIa0wAB6nKoIXuZTpABAOA8zlDXQcs6%2Bm%2BuQl%2Be%2BG%2BEBR%2BJ%2BN%2BRQ2QwBzznzUMbzSwn%2BUM3%2BrQPzBgdzwL4BB%2Bf%2B4Lb%2BYLJ%2BgBcwpQML0B6wuDo%2Bzurukj/jleJw6g2eI4t4Y4JwwAqAqAJwMgHIPAr87ezcl4weKwsjjTJDZDIwlD0eFg0gHISeio8e8eI48eCQ8e0gWgio6eioVDhexepeUjle1eLDRDawijEALeKjajjAGj1A2j6rIw%2Bj0ghjL9cQJjZj/jdjVjbjZrDjTjLjTgbjHjzARA3jvjUjgTPgwToT4TIeOA0TdTOw5e8TL%2BSTUjqT6TbjWTUjszkoBT1sRTlzJTIeZTFTKg1TPr9ToAjTAgLTbTHTXTPTbjwzAzo%2Bsgwzk%2BYz/j%2Bg8%2BtTS%2BYkhgeTNzizyz9sqz6zmz2zuz%2Bzt4hz40xzPgpzEjFeFzVzFu8AMBtQL%2BrgEA7g7%2BJ%2B%2B%2ByLkBBgfzWQ07p%2BXzALlQQLr%2B/Qy7ELm7P%2B0L87CLcLTzsL/Qa7UgMBcBnAqwKDaDGD0jXAeLBLRL0gJLZLFLXL1LEAtLKhoIqwTLEeLLLA5D1AmLXA1DtDMrOLMjzDdebDawHLXLPLfLArQrIrYrioErYj/bd7crrDGbyrKAhAS0XetA/T4ggzxb8gpb0%2BOTSANekzdHRHNAOMLD9AdHoDkoSzUM0HpAdHc%2B5AvHSgTHLHjognHHLAXHa%2BVeoHt7sr3Ato0iMi7TnTvs%2BLhLxLpL5LlL1LhI%2Boanz7r7WnH7/7xDUegQceBLWgCQIj0gwrPAMgoIkr9DUHTDNesHzLKDBDzn5euHirawZTGQrg0gQAA))
- [4% performance gains by changing data storage from char to int64, to reduce casting](https://github.com/Jumbub/bf-cpp/commit/1bc35a79799e14713de64ab7d2dec016060afd1c)
  - casting up is expensive
- [5% performance gains by using computed gotos](https://github.com/Jumbub/bf-cpp/commit/c26dec0dc0ff1661c6a151fca66b733d65ea789f?diff=unified&w=1) ([godbolt link](https://compiler-explorer.com/#z:OYLghAFBqd5TKALEBjA9gEwKYFFMCWALugE4A0BIEAZgQDbYB2AhgLbYgDkAjF%2BTXRMiAZVQtGIHgBYBQogFUAztgAKAD24AGfgCsp5eiyahUAUgBMAIUtXyKxqiIEh1ZpgDC6egFc2TA3cAGQImbAA5PwAjbFIpAGYATnIAB3QlYhcmL19/AzSM5yEQsMi2GLieJPtsRyKmESIWUiIcvwCeGrqsxuaiEojo2ITkpSaWtrzOsb6BsoqRgEp7dB9SVE4uS3jQ1F8cAGozeI9sdRTsJ2xMY9wzLQBBbd397COTwVI2FiJb%2B6eLDsmHsfIdjh4XGNSNh2H9Hs9ga93h4oaFgHCAUCQWCTgA3S4kUgYhHYt7g1BjQjCYmAl6gskWCyxUhkAB0SEsFn%2B3MerA4ShSLA2ByipBYoRoPlQAGsjgB2Gzwx4HA7MPwHPYsJRKA64Ugs0jyqwHcIAeXCuHeABEDlpyAdVAAlU0AcUdDwAsgB9M0AFS9ADFTQpwlb7Y7cA8PAAJXBWr0ASXCIl9joUHl9CfNXqCCY9Cd99pDHoevpjca9VndHktZjlVuOiqejzVbAOvoAnhcDiADj5QkQAGzSL1EI3/FVm02qcgTg5W824WfK%2Belh5eh5WsMqne7vf7g8qgD0R6O1gOJ4OAFo51a116RLhC4eX6/j6ezABWKw3z8Nld3r666pg8yYBrgjr2m%2B0GXl%2BP5/NY4Jfv%2BDwqoB64egoQSZqoQQAJpQdBr6wd%2BN7xHciGIScyG3vemHYQmuF4RuoZelaCYAGoJlaS5ESRP5kRRNiUTY1F/rRQEPk%2BgbOt6SaqAoz5Ebul7Lqhq6SU6SZKcpb6XqyEnrqopraRBG5boRuk7rB5EXh%2B1EAfexmmY65nxgA6tGCZBLgPqmv6ABaEGmpZ75HN%2Bfx/nZ4WiR4NErkmKZphmWbhF6znhL6ZmPv6CYBl6QXOqF0VwXOiWpummbZhlWWuTlib5X6BXBVBJEoXWDbxE2/w%2BBkJgHBxEg%2BGS8Q2gOw6jo2PV9cABymjQNAqGOxxjcIE2/F1PLNg8UJSmOCZMLtThZOOK6dt2RBdiNNpTqoU0roNvhvLiQ3Xba93qfNi3YGO6ALUt1rvZtSrqQdR31BA51vJdFyLB9Kpg0QpB7VkkNXQcMPYPaj3DQcL1PXDwOg4dSMo0IaMXVd2OvXjr32l9AN/d9RCE919YfdNaIHFYHZENgXg4jalIgCA%2BJOGQ4KI8jx1CH8RO9VzAtvcLosEhLJyoEgzRy91K7/HqBqquolw%2BHzEA83zSsHKE4NZEoiynep40jmOmA/CwXppAOsSA1o8Malrhpu00cHxFo4d%2B1FK1Gn77NEwjJPS/UABU1uJ2TTCA5Yg426TMuHXBkedbr6mXv2a0u9bfNivUvv%2B5eKti4S4Ll0Q8QWJNtm50nWResjTBehgPjCBA3cZ4PqzCPaWis3ODdEJgIvfCk4JQ/are3GnttCKOV0T8PG0lzMzioHj6AEJgqe6H4KS%2BiwUSMKVUejY7%2B7Z9nt1qW/FiDtnC4Wl/Pc78f7oTcoA3cwDBygJyuAnckDQEgTAhBWBKp4F0SwjhfCKCzy/xAeghiTEWLxnYlxHi2C0GSXqgGWSiZwgKULHOCBP8/5OUdNpchzC8GaRMplMym4wyMLgZwqBTkeG1Tcl6Ty3lfJNUKiFQRqDhHlWSlVNKNVsrSTys1IqCicHZ2UZVVK6UxEaNyo1fy2j5F6zjiXFUwB0AkCvjfO%2BD9sBwTHvnQSmMaJE3%2BKA9Rrl%2BEgDnMHD2XthA%2B1sNHDx9RBJMyWv7exJADgPADLVDmjwDEpWqiYuqmjzGBWCr2OsTYVQYBJgcFgpt0AHHiT9L0oTPbnwiYaaOjTwnVzPMaGJWQ4n/R%2Bv7AgNADgQFCXBOpRAGnuyad7IkUcLCfgOAswcBwwBgGjjPV%2BO4elCCzj/HZBdvzHwIKgQeWoNoogIAAL2wJ3XAo906eNuPjYacNxLxx3Ek9ATi2C33vo/b8ByvFXR8aU%2BUKE7EOJqak9Jvj2YgwQe6JBjpikKjnOUsYlTqm1P6ZM9pzTOltOmR0yJ54gW3Amf7MZ34JlTKaDMlpXSt551ic816yEulEpDjS3FdKwkEtiKCkJ7txm8vxbMjlGzElQpSWkiCHN4XbVAUErZ1KrDisZbYZlPchB9OZpKqiq1t5MEEi8txHyvmythWzdqjxQH0UwXhVFYKMVjiqck2lGrCUvy9aS7pjzWXkUpR811WKPWmxSKbHU0SA29LZU9f2nwRnupqQQOuXVrbIlqRGqNjZrZROsA7EpgiDkFtEmClUarPXEoFa0slsbdUUtxQa0SRqWVxvImag4qcq1iprRK95YKOrCu5VYat9KSVzJtFKi1MqYXyrhbazc94crOvRUITFKacXMz5Qy71NpfV1v9cavVCSPm9p3YeyVL9yWdvZbO5J87HQKqXVk1RxiXJSTMZYtdK5Q1bvHfy2ZgND1MtvbgYNYKhkjIvUtXdk6DWLOWdaDZRa0UrhVAcvZOcG2HKsMc054gxjggyDcu5DyT3xteUKjDBwvk/L%2Ba49xuHgUXBo%2BpYdK5LVPpfVtfxuSJFSJ8n5QpzpgkrgAO5IAYG8UZIrvyga1eByl8ykOfhWWs1DWzK39s1Ya7VGdTX3qHYqyFj65XPt8Xa1h2lf3qX/diwDe7Ik%2Bt04S%2BtlGg3No%2BUmh5Y400zu6Vm8DZq80EDLWhitBwVZD2WtRE40WmgnzOcRjWgdbhydHU5hDqmlnqdntYiFdG50Wd4wild0lqGmjknQxSdmykbrdY5vtE7a0gbc36gzTyvP6p82QEZA5M2BaGwlkLxn80iUi4IzWzQ06RsPoImLoR3h3FsirJg6RpQEBSBJ6N5FN6hHm/7TDwyIBrKW0wVk2A/oQEWFN2jOmsstaAy069q0jsfNQaZ8Fc5uOlcXXx/BjqiFsU4txXA9WNSNbDTU7LbWuUvfc8e9tjaetnpddDgDOaiB7bbTqk1VHzVQdw2W/2OM3jEFiD8O26awVSZkzB%2BTY7nvOanWeNTGn1kv02cW2jsH6lXvma2rrgbcChc%2B1XKn9QlCk4%2BZx7aKpfNboC9zsLWbVhEHmzLjN4XJvaZF1kWXUX%2Bd4o60eg3qOIPNqFzGzzYuaap0pzXO2/t5dmehf9m1fHFzidLqeHAUQfDAFZJgIQ2Bbv%2B2hEQNYmcDZkBFmaC0GTQKmmnL793VqF0lyfen6KAeg%2BsiRkKaUUsM4QAIyli5pHbkbXuVhq8FvDoh/drdu7gySciWlY4g419fkuIBVYcD3jB3chsVtDqYU%2BTYAFEKN4opxRMElDKf4XBlj0G4J%2BfgAReD8HQNwOKIlourHWAyeIfByBEG0Kv5Y0oQDxHiKyRIcpByfmWZ%2BeIcpPw8AsAADkMNwaQfgNgEAT8O0bfHQcgPfLgfgJQEAO0S/LgHQZYOAWAFADAHAfAYgMgSgagOgRgKfbgc/QQYQMQCQTgGQOQYQZQNQTQRAvQEACwQwYwUwMtLoAkLINwJgTwbwdoQILguYIYSoT8OUVIdITIIQSYDoUQwoLIAQ8oYYHgYQtg/OXoCYHgqYZQ%2BoVQ/oUIQYeQyoewcYVodQqQmYFoOQhYHgZYJQY/DYbgEkJEcEM4C4K4G4fbeEWkREekZET4b4WvbkTw0kZESEJGGENgGkLERwk4VEEwCIukHEDwJudWO4DwyI7w8kSkAcOIrwsERkZkNkDkRkcfXkdgafQUYUefCUKUWUXnBXVUJgdUTUbUXUfUfrEpE0RcX2e0J0V0d0b0JqIMEMbcCMKMWMeMN9IxXMfMZ8YsUscseMKsKMWsMfEGVsdsdGXsVudaLZT%2BW8RcQBZVCyKyPiD8c8S8G8RyShJ8YqY4/cfiX8IrRFUCEQcCSCW4u4j8UiBCWKeKdSe1DBRiLBd4vce474qJMSR4oHQE5iUCYhMHMhE4mKK8QSUnKiOKcSS49cKhGheSRSG4vSU8A4mzTKfE94/SQyD9XhQJI44EmyS0GyOKTEyk8RfhSRLyYTWRFqD4mKSKG0fiJCDE0GZMCqbJNRATeqLRORYqfkhKYUlRIxAJL9BqETSxVqT49qFY7aBWfqcnQGZ2TuJsbU2aBmH6PUiuA04onaFlA4UvfOLZKGDGdGaOW6MnGmLtQLOcE036XFWnLaBOY1CmaGK6ArYmAMh0zGamJ6WmAmf2W0iGcMqmAaN0umOaH0iZEM4dKzB4I07mXmfmLAZWBeEWJIokE4OMw3fbeWGaA4K2aORuNWUsjwGbUs1bWxI4R4OPQ0M4E2M2C2fMw4A5e2LZfU12M3X0v9QOA4NVMOCON7GOTMsFcsoQR3XDbDQcwudjOeU8LYyuJ3anXZD0lceeReVWcWRs1uduO5RvPuYefeEeA5O8ogaeEMsKFWZeVeRMjeLuXDXeC4R85PFUMIdQS/RbCTYgTWAbFjW4TGe7dSMpLUN4fjT9IJXRRTfTZTbzKLFUUUGEaUY7DUBCm0uUwxHJT9CUgpH9fXBrCpADFnSddrVrYDJTKCtHAZCXE7RnJ7S9M3DlTTbnWCw8LDaObOdco5JLE5SvEja5GvDLMbGMkfB7T5GVIChbRSt3PcHClgPCiXdS%2BClQDSYCJFF4iCSHXcBzD1Oi%2BHVzRivTZHfHU9NirCqcpnOHJijzFHAnO9BNG3aylgUVbimy6udjfcE3eDWtOcv2dikUaELS/C3Sgi/Sw4q0UyncNVNCuywzJtfVHyoWcSwjc5ckdLciCjDyozGMqKzS7SqLeKojRCqEpiFK6izdZrAKxHFzA9Mc5iu3SDXRcy2HbHXHRvMq4afCxXfrCAZXWnEbDwbNTXXNHXCLKi7ZDvYSctXRR7PynlVq1nMDFiq3bK6dQ1davcCvIjC5Zs2SvartHtFyyygdK0F8oBb7XcUKwXadVXCqmKqqwRGqwi6BJ8RqqHGilquDRTXynarq0qrK9HVC267anLd63K6nfK1LJsoquvK616R63cSquK56hKt4CY0iqkpUyUopJavq7dUGzq8G%2BiyG%2By6Gxy3RaDTLTa5neG8K5%2BTqG0HndDOCvcISl%2BES3DGXMS5GyS6I6S8jOS6jBS/m3cS1FS0an7RS3GnS/G2qgy5kvhLcNk6RFUuRXPXcenRgTitm9Koaxm34KKPinmgSg8dK23KGrykaqK%2BK7Cr6/CzW/xNhTKQGym1yzVWm%2BHdyhm1i1S%2BWg4XzQbFXHmtXcES2l2onCbVa%2B2/cGLDXZEBO06gqtLbWYq16niv8bGuBfGj23CvGorPSuqq4/0KrGrehf2zHEGgXGmjqwKzrDC3rJy6O4QEbOOnXYLTGhNBavXWow8ZsubU2ZWlUC7FbTedbTbbbXbee78j7Jyjis7MAC7K7G7O7Jal6uG6mjutnGNde4692lWyOtW6qjWv6%2Bq/CEHEhcHJu4Giyjmpi4OtyjK7rfamGxSgOgas0u3cXJy0tTvKK3Uvc6XccyOk22TQuk%2B3irnO2g%2B5yri4%2Btq0%2B/THO1Gi64qmW7AEu3caBl3CBpy%2BXXRJXbFWOoGILBOjXLXMLRa8eg8cB1ame9BtmwOpHROv%2BgZHKxLcWs6wq/OjGkBh3SXZ3DdYhq%2B/cG%2Bn6u%2BxKn3XRKPGPFog0BPRcL2wi26I2ncBRvnbAGgKpegECxSr0L0QPBgZwAeYeGKmbVxCPdWpdEhla8EkuRWs4COhc8fesSfUomfCosUKo5fR4Vff/LgTfcgcA3fffMtI/NYYUbYCwfgBApA5YJAGEHAOIW7SJwA8gYA0AmJq/SA7gGAuAi/K/O7cgW/CwLQR/OULQT8H/dTHgRIH/LQaQT8dTSJ%2BILfUpqAtJ6p8gFAhAeACANA9AX5GTCgKgCADAGZxgOIYAGQToPA6uWAiAKIUpqIUIZoDsQg/gPZ1gUgDsU0KIXQAkI58gRZjgYQU0JgegQ5ug8gfPYADwCQegWAnfN57Ab4EwSQV5wgaEY6fEH5iA7s1AU2TYc/b2dfV5%2BgAgefM5pWUppGAgYA35/EUgKIdIbAK0f55gpFkwEZmgIwYAJQDiAgbACTU0C4bfIg%2BQUgyQCg4gxQFQDQUp/QToIwUl8wESQwZF2AyAZYdAFIaXbgK8U0eIa8K0UMXAKwBQF0a8SkFaYABostQEXfHF0gC%2BafeAaw2odg1wCAdwSQhgu0YIXQ%2BYYYe/aQ8Q7IEwy1h1%2BoCwu1%2BITQnoIwi1%2Bpr1oQbQ91uIe1sw4w3IAIP10NoNu/eIaw2w8gtfDfAZ15qAg4dQH/QcK8YcOjVAU%2BGQVkHgEZTAwkM8M/RYYZugmprJlgHJ6gG/Bg6QVkZ/OURIRIQcRIT8RIaQLQOUD/EQhFgpopsAwZ8p%2BwSp9JiJsZyZtAaZlIWZnAhZ2d2ZkAVZ6QdZmx2ILZnZ15k5g5m53ds5i5q5pwG5u55gIgR5550p95z5%2Bgb5m5nAAF4AIFiAkF9g8F0pqFmFm5%2BF0ppFlFjsNF15jFrF8/HFvFlQQlp9kl0AStgQClqlmlulhlm59lll8g2Qdl6grl15/QRgvllgwV/9kVvJ8VyVrgaV2Vq8eVniJVlVq8NV0aDVnwLV/pyA3V/Vkjo17oU18151iwToa10oQQl1goR131zoMTt1m1kTyN41lQn1/j6YeTrQow6NyNxT8Nhg6YNTmT/Qy1uNpJzgCwRNqJ5NiA1N9NzN7N4AXNg4fNwtiAYttowEEzitjJ8gat2tvJgdoAkA4dlN0d2A%2BA6p%2BthZVkJICwJ/dpxIM/HtighFtj2Jsp6AqpuDqdlAQgBaBdtD8QVlzD%2BQbD2giA%2BgJAWA3lsrrLmgTGSp0rlQYCsUMjjg1L0gMrhg8gVrpQKrmru0Tr7xxriV5riJhF6J5LqAq0IZYZal2ln2KzrN6QHNvNxtwt3EHUObmzuzhz9z6/WpkAh/QceIYcaQQEH/Jpn/H/KL2QXzkpwL1L4LtLjzhF1Jm7iz0didmpnFjIVwaQIAA%3D%3D))
  - single jump table
- [4% performance gains by changing the order of the cpp files in the CMakelists.txt](https://github.com/Jumbub/bf-cpp/commit/44017042f7515248d655477616b7ac0e224861bd)
  - [2016 LLVM Developers’ Meeting: Z. Ansari "Causes of Performance Instability due to Code ..."](https://www.youtube.com/watch?v=IX16gcX4vDQ)
  - ["Performance Matters" by Emery Berger ](https://www.youtube.com/watch?v=r-TLSBdHe1A)
  - Counter example: [Revert "Remove legacy debug operations", because it decreases performance by 8%, even though it's literally unreferenced](https://github.com/Jumbub/bf-cpp/commit/bfdc321af2a041127a87a647e358e7e97e987b6d)


> Measured against `samples/mandelbrot.b`, with a standard deviation of 0.3%.

## References

- [brainfuck optimization strategies](http://calmerthanyouare.org/2015/01/07/optimizing-brainfuck.html)
- [some brainfuck fluff](http://braifuck.org/)
- [Brainf***](http://www.iwriteiam.nl/Ha_BF.html)
- [Computed goto for efficient dispatch tables](https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)

### Notable Competitors

- [Tritium "This is the fastest brainfuck interpreter around"](https://github.com/rdebath/Brainfuck/tree/master/tritium)

### Random References

- Neovim & CCLS config (with latest C++ features)
  - `lspconfig.ccls.setup({init_options={clang={extraArgs={"-stdlib=libc++","-std=c++2b"}}}})
  - Requires installing `libc++`
