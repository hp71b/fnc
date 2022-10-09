# AREUH function compiler (FNC)

This repository contains an extension to the
[AREUH](https://github.com/hp71b/areuh)
development system for the Saturn processor.

## Presentation

This program, **fnc**, is a toy compiler for a simplified language vaguely
inspired from the C language. It is more a _proof of concept_ rather
than a complete compiler. In particular, some operators are not
implemented, even if they are specified in the Yacc grammar.

## Usage

The **fnc** compiler has no argument: source file is read from standard
input, output is generated in the **a.as** file.

Example:

```
    ./fnc < examples/fact.fn
    cat a.as
```

## History

Fnc was developped for the HP-71. It was an attempt to introduce a
subset of the C language to the Saturn processor.

Original source code dates back to september 1987. Conversion to
modern C has been done in october 2022.


## Authors and license

Pierre DAVID & Janick TAILLANDIER

This work is licensed under the [Cecill-B license](https://cecill.info/licences/Licence_CeCILL-B_V1-en.html).
In a nutshell, this license is similar to the BSD license:
you are free to use, modify or distribute these programs.
Please, keep this notice.

These programs are provided as is.  They are in no way
supported by us.

Have fun!


Pierre DAVID & Janick TAILLANDIER (September, 1987 - October 8th, 2022)
