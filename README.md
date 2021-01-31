# C-Slim Language Project

Goal of this project: create a simple C-style language, compiler, and
interpreter. The language will basically be a copy of C but without verbose and outdated features that I find frustrating, as well as some miscellaneous new
features. 

For the sake of time, simplicity, and the fact that I just want to 
have fun while working on this project, the compiler will compile code to a 
custom bytecode, which will be run by an interpreter. So, this project won't be
anything super serious (it'll be a "free time on the weekend" thing), but it 
should still be decently efficient and work well as a language in general.

## Feature changes:

* NO header files.
* NO confusing typing (i.e. "int \*x, y"). The type is specified by the type identifier, not the name of the object (so "int\* x, y" means both x and y are integer pointers).
* NO typedef nor typedef struct. Structs are types. 
* All whitespace is equal (including new lines). Any statement can carry onto the next line and is ended by semicolon. Multi-line string literals are thus inherently allowed. A consequence of this is that comments need to be closed with a semicolon as well (an interesting quirk, and I've changed the comment identifier from "//" to "$" in order to lessen the conflict with ingrained knowledge).
