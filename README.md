# vault

*vault* is an ANSI C implementation of a multi-user secure password manager based on the command line. It spawns a simple shell that allows to issue commands to change account data, insert or delete entries, print information, amongst others. *vault* uses cryptographic algorithms to store and lookup information in a safe manner.

*vault* was developed a few years back for academic purposes. The tool is available for anyone who wishes to explore C cryptography usage or to extend the code base further. Or simply for people who just want to look at the software.

# Dependencies and Usage

*vault* was developed with a few free implementations of cryptographic algorithms, namely [RSA's MD5](people.csail.mit.edu/rivest/Md5.c) and [Fabrice Bellard's AES](https://github.com/avikivity/qemu/blob/master/aes.c). It stores all the data in a local [SQLite](http://www.sqlite.org/) database using the C amalgamation software. The script in `db/init.sql` is required to be run in `sqlite3` as follows in order to create the `vault.db` database, provided that the command line is at `db/`:

* `sqlite3 vault.db`;
* `sqlite> .read init.sql`.

A `Makefile` is provided for compilation and linkage purposes, which creates object files at `src/obj` and writes the binary to `bin/vault`. This can be achieved by issuing `make clean; make` at a command line whose current working directory is `src/`. The help message included in the software enumerates the supported commands and options.

# Author

[@dfernan__](https://twitter.com/dfernan__)