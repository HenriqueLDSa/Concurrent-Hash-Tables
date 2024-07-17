#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Define the name of the file to be read HERE
static const char FILENAME[] = "commands.txt";

// Function Prototypes
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length);
void insert(char* key_name, uint32_t salary);
void delete(char* key_name);
uint32_t search(char* key_name);

// hashRecord struct
typedef struct hash_struct {
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct* next;
} hashRecord;

int main()
{
  // Open File
  FILE *fp;
  fp = fopen(FILENAME, "r");

  // Variable Declaration
  char buffer[2][30]; // This just consumes 2 of the strings in the first line
                      // that are not useful
  char command[30];
  char name[30];
  int salary = 0;
  int numOfThreads = 0;

  // Read first line of the file to get the numOfThreads
  fscanf(fp, "%[^,],%d,%s", buffer[0], &numOfThreads, buffer[1]);

  // Loop each line of the file
  for(int i = 0; i < numOfThreads; i++)
  {
    fscanf(fp, "%[^,],%[^,],%d", command, name, &salary);
  }

  // Close file
  fclose(fp);

  return 0;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
  size_t i = 0;
  uint32_t hash = 0;

  while (i != length)
  {
      hash += key[i++];
      hash += hash << 10;
      hash ^= hash >> 6;
  }

  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;

  return hash;
}

// Inserts a new key-value pair node or updates an existing one
void insert(char* key_name, uint32_t salary) {
  // Compute the hash value of the key
  uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

  //acquire the writer-lock that protects the list and searches the linked list for the hash

  //if found, update it; if not, add it to the LL

  //release writer-lock and return
}

// Deletes key-value pair node if it exists
void delete(char* key_name) {
  //compute the hash value of the key
  uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

  //acquire the writer-lock

  //search LL for the key

  //if key is found, remove node and free memory; otherwise, do nothing and return

  //release writer-lock and return
}

// Searches for key-value pair node and if found, returns the value; if not found, returns NULL
// If found, the caller prints the record; otherwise, the caller prints "No Record Found"
uint32_t search(char* key_name) {
  //compute the hash value of the key
  uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key_name, sizeof(key_name) - 1);

  // acquire reader-lock

  //search LL for the key

  //if found, return value; otherwise, return null
}
