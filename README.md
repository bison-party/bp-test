# Bison Party Testing Library

The goal for this testing library is to be portable and easily extensible.
`bp-test` will run well in bare-metal environments while also remaining powerful
in more featureful environemnts (i.e. posix).

## Project Structure

For the sake of portably, src code of this project is organized based on
its dependencies.

__IMPORTANT:__ By design, dependencies for different modules within this repo
should never exceed that which is available via libc and posix headers.
This is to avoid cyclic testing situations where a piece of code
from another repo is both used in `bs-test` and tested with `bs-test`.
For this reason, `bs-test` will __NOT__ depend on any other Bison Party repos.
Whereas, all other Bison Party repos will likely depend on `bs-test`!

__ALSO NOTE__: Each module will have a `test` directory which *tests* the testing logic itself.
There are no real rules in the `test` directories. For example, `common`'s logic is meant to 
be portable, but the `common` tests will likely assume posix.

### `common`

This will be the most abstract and portable layer of `bs-test`.

It will define core testing interfaces and logic. 

It should be a self contained as possible. `common` will __NOT__ use any heap management or posix
functions. 
