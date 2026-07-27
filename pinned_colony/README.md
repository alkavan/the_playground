# `pinned_colony`

A class that looks like `std::hive` but it's not. It allocate virtual memory
for both Windows and other Unix/Linux systems using `mmap`.

## What this new example does

* Spawns 12 000 entities (well within the 32 k capacity).
* Runs a 300-frame simulation with four systems:
    - movement
    - soft attraction and damping (makes entities orbit / collapse interestingly)
    - health decay and periodic regen
    - target system that repeatedly dereferences stable raw pointers stored inside components
* Measures wall-clock time for spawn and for each system.
* Prints a clear benchmark summary and verifies that every stored Health* remains valid after the entire run.