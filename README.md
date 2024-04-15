# PONCH_GRAPH

A program for implementing algorithms on arbitrary graphs, including 2 libraries: matrix, graph.
#Interaction occurs via the console#

## Files in repository:

## source:
  1) app.h — header file with declarations of classes and functions, the main ones:
     
     **load_matrix()** — reading a matrix from a table (the matrix must be written in the format |0 0 0 ... 1|...|1 1 1 ... 0|
     
     **compute_components** and **compute_components_parallel** — functions that highlight strongly coupled components (for sequential and parallel execution modes), using dfc algorithm
     
     **dijkstra** — a function that implements Dejikstra's algorithm on a graph (returns the shortest route for two arbitrary vertices)
     
     **spfa** — a function that implements SPFA algorithm on a graph (returns the shortest route for two arbitrary vertices)

  3) **app.hpp** — implementation of functions from the app.
  4) **executor.hpp** — selection of execution mode
  5) **pseudonyms.h** — pseudonyms for some types
     
## workdir:

  1) generate_matrix.hpp — generating a random square matrix of a given size
     
## matrix:

header-only library with tools for working with matrices

## graph:

header-only library with tools for working with graphs

### Find more in repository !!!

## Team members:

Mikolaenko Vadim (Миколаенко Вадим) — team lead, author of the idea 
Yurakov Nazar (Юраков Назар) — developer 
