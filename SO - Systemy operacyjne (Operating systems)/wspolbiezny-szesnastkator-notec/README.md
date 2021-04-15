# Współbieżny Szesnastkator Noteć

An assembly implementation of the following function:
```
uint64_t notec(uint32_t n, char const *calc);
```
that can be run from C code, from up to N threads (where N is an environment variable)

It returns the result of the Reverse Polish Notation calculation given in 'calc'.
'n' is the id of the notec instance being commenced.