# C-Slim Language Project

Goal of this project: create a simple C-style language, compiler, and
interpreter. The language will be heavily based on C but without verbose and outdated features that I find frustrating, as well as some miscellaneous new
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
* NO "->" for dereferencing and then accessing members of a pointer to a struct. The "." operator will be used here instead also.
* Namespaces by file; each file is its own scope and declarations from other files can be accessed like so: "otherfile:declaration". This is for better organization and no need for ridiculously long names to avoid conflicts (e.g. "mymodule\_function\_name").
