# Pleter
is a fast thread-safe word completion engine 

## Implementation
We are currently using a variation of range trees consisting of a compressed trie and sorted arrays

## Concurrency
Pleter features fine-grained read-write lock hierarchies which eliminates data races and deadlock while still permitting large amounts of parallelism should you need it.

Our hand in hand locking scheme allow us to update a subtrie while all other subtries are being read from or written to.

## Testing
See if it work with
```bash
make
./main combined_preprocessed.txt | tail -n 1
```
the displayed number is the average nanoseconds per word processed
