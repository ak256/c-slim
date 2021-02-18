# C-Slim Language Project

Goal of this project: create a simple C-style language, compiler, and interpreter from scratch. The language will be heavily based on C but without verbose and outdated features that I find frustrating, as well as some miscellaneous new features. 

For the sake of time, simplicity, and the fact that I just want to have fun while working on this project, the compiler will compile to custom bytecode that will be run by the interpreter. So, this project won't be anything super serious, but it should still be efficient, highly readable, and usable as a language in general.

## Features:

(Note: for as long as I continue to work on this project, features are subject to change. Also, not all will be implemented currently)

### Because I think it's an improvement:

* NO header files. Definitions are not separate from declarations by default.
* NO confusing typing (i.e. "int \*x, y" means x is a pointer but y is not). The type is specified by the type identifier, not the name of the object (so "int\* x, y" means both x and y are integer pointers).
* Structs are types. No need to use typedef.
* NO "->" for dereferencing and then accessing members of a pointer to a struct. Just use the standard "." operator.
* Namespaces by file; each file is its own scope and declarations from other files can be accessed like so: "file:object". This should be better for organization and avoids the need for ridiculously long names to avoid conflicts (e.g. "mymodule\_function\_name"). As in C, to make definitions inaccessible from other files use the "static" keyword. However, it is no longer required to make them static to prevent multiple definition errors, due to namespaces.
* NO forward declarations. You can use things as long as they have been defined somewhere in the file. The compiler can put two-and-two together. This also really shouldn't have an impact on compiler performance, if done properly.
* Create and initialize structs wherever (ex: return MyStruct {.id = 0, .name = "andrew"};)
* Added a "defer" statement that will allow you to do schedule statements to be executed upon return, such as closing a file.

### Because I'm lazy and don't plan on adding it:

* NO typedef
* NO preprocessor directives other than #include and #define
