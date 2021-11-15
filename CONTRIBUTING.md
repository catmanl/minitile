# Contributing
## Fork the repository
```
$ git clone https://github.com/catmanl/minitile.git minitile
$ cd minitile
```

## Create a branch
```
$ git branch -M my_branch
```

## Contribute your change
Make sure to follow these conventions:
- variable and function names: `lower_case` (`my_var`, `my_func()`)
- type names: `PascalCase` (`MyInt`, `Vec2`). EXCEPTIONS: prefixed structures
- indentation: 2 tabs
- operations like `+` and `-` have spaces between them (`a + b`, `a - b`)
- operations like `*` and `/` have no spaces (`a*b`, `a/b`, `a + b/2`, `(a + b)/2`)
- braces on functions go under the function names
```c
void my_func(void)
{}
```

- control flow, etc. have braces by their side. There's ALWAYS braces.
```
if (cond) {

} else {

}
```
- floats go with `0.0f` style
- try to make the code self-explanatory and don't fill it with redundant comments.

Please check you've followed the conventions and send a PR!
