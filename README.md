# Differ

This program is used to compare two files and store the information about changes in additional diff file and efficiently create a new file based on the old one and the diff.

## How to build

To build the program you need [gcc](https://gcc.gnu.org/) installed. Simply type the following command:

```
make
```

## How to use

To create a diff file based on the old and new files, run the program with following command:

```
"Calculate diff" [old file name] [new file name] [diff file name]
```

To create new file based on old and new files, run with following command:

```
"Restore" [old file name] [diff file name] [new file name]
```