# Concurrent Hash Table

## Overview

This project implements a **Concurrent Hash Table**, a data structure designed for efficient thread-safe operations on a shared collection of key-value pairs. The implementation supports concurrent `insert`, `delete`, and `search` operations by multiple threads without causing data corruption or inconsistency. The key feature of this hash table is its ability to handle concurrency using reader-writer locks to ensure safe access to shared resources.

### Features
- **Concurrency**: Multiple threads can operate on the hash table simultaneously.
- **Thread-Safe Operations**:
  - **Insert**: Adds or updates key-value pairs.
  - **Delete**: Removes key-value pairs.
  - **Search**: Retrieves the value associated with a key.
- **Locking Mechanism**:
  - Reader-writer locks to ensure data integrity during concurrent operations.
  - Condition variables to manage operation dependencies, particularly for DELETE operations.
- **Jenkins's one_at_a_time Hash Function**: Used for computing hash values, which ensures a very low collision rate.

## Hash Table Structure

The hash table is implemented as a linked list of nodes, each containing the following fields:

```c
typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;
```

- **hash**: 32-bit unsigned integer representing the hash value of the key (name).
- **name**: A string (up to 50 characters) used as the key.
- **salary**: A 32-bit unsigned integer representing the associated value (salary).
- **next**: Pointer to the next node in the linked list.

## Supported Operations

### 1. `insert(key, values)`
- **Functionality**: Adds a new key-value pair or updates the value of an existing key.
- **Process**:
  1. Compute the hash value of the key.
  2. Acquire a write lock.
  3. Search the linked list for the key.
  4. If found, update the value; otherwise, insert a new node.
  5. Release the write lock.

### 2. `delete(key)`
- **Functionality**: Removes a key-value pair from the hash table.
- **Process**:
  1. Compute the hash value of the key.
  2. Acquire a write lock.
  3. Search the linked list for the key.
  4. If found, remove the node; otherwise, do nothing.
  5. Release the write lock.

### 3. `search(key)`
- **Functionality**: Retrieves the value associated with a key.
- **Process**:
  1. Compute the hash value of the key.
  2. Acquire a read lock.
  3. Search the linked list for the key.
  4. If found, return the value; otherwise, return NULL.
  5. Release the read lock.

### 4. `print()`
- **Functionality**: Prints the entire contents of the hash table to the output file.

## Command File

The program reads commands from a file named `commands.txt`. The commands are processed by multiple threads as specified in the file. The format of the command file is as follows:

| Command | Parameters | Description |
|---------|------------|-------------|
| `threads` | `<number of threads>,0` | Specifies the number of threads to be used. |
| `insert` | `<name>,<salary>` | Inserts or updates a record. |
| `delete` | `<name>,0` | Deletes a record. |
| `search` | `<name>,0` | Searches for a record. |
| `print` | `0,0` | Prints the entire hash table. |

### Sample Command File

```text
threads,10,0
insert,Richard Garriot,40000
insert,Sid Meier,50000
insert,Shigeru Miyamoto,51000
delete,Sid Meier,0
insert,Hideo Kojima,45000
insert,Gabe Newell,49000
insert,Roberta Williams,45900
delete,Richard Garriot,0
insert,Carol Shaw,41000
search,Sid Meier,0
```

### Output

The program writes the output to a file named `output.txt`, which includes:
- Command execution details with timestamps.
- Lock acquisition and release logs.
- Final contents of the hash table, sorted by hash value.

### Example Output

```text
1720453920,INSERT,Richard Garriot,40000
1720453925,DELETE,Sid Meier
...
1721439714611162: WAITING ON INSERTS
1721439714611918: DELETE AWAKENED
Number of lock acquisitions:  11
Number of lock releases:  11
448054155,Gabe Newell,49000
660427450,Carol Shaw,41000
909366975,Roberta Williams,45900
1874280167,Shigeru Miyamoto,51000
2569965317,Hideo Kojima,45000
```

## Compilation and Execution

1. **To compile:**
    ```bash
    make
    ```

2. **To compile and execute:**
    ```bash
    make run
    ```

Ensure that `commands.txt` is in the same directory as the compiled binary.

## Authors

- Henrique Lacerda Delgado De Sa
- Diego Lobo
- Liam Kilkenny
- Jace Adamski

## License

This project is licensed under the MIT License. See the LICENSE file for more details.
