# Adding support for big int

This version extends TinyScheme to use big integers thanks to the help of [libtommath](https://github.com/libtom/libtommath). I only did the minimal work, as a result (unfortunately), the numbers must be represented by strings.

Five functions are added to manipulate big integer numbers. For now, it is only possible to operate on 2 arguments, but it is possible to extend to infinite number of args.
```
(big-add "x1" "x2")
(big-sub "x1" "x2")
(big-mul "x1" "x2")
(big-div "x1" "x2")
(big-mod "x1" "x2")
```
