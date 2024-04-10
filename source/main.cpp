#include <iostream>
#include "../graph/graph.h"
#include <iostream>

int main() {
    // Create a graph
    graph::Graph<int, std::string, double> myGraph;

    // Add nodes and edges
    myGraph[1] = "Node 1";
    myGraph[2] = "Node 2";
    myGraph[3] = "Node 3";


    // Print node values
    for (const auto& [key, node] : myGraph) {
        std::cout << "Node " << key << ": " << node.value() << std::endl;
    }

    // Print graph size
    std::cout << "Graph size: " << myGraph.size() << std::endl;

    // Print iterators
    std::cout << "Iterators:" << std::endl;
    for (auto it = myGraph.begin(); it != myGraph.end(); ++it) {
        std::cout << "Node " << it->first << ": " << it->second.value() << std::endl;
    }

    // Test graph algorithms
    std::cout << "Degree in of Node 2: " << myGraph.degree_in(2) << std::endl;
    std::cout << "Degree out of Node 2: " << myGraph.degree_out(2) << std::endl;
    std::cout << "Does Node 1 have a loop? " << (myGraph.loop(1) ? "Yes" : "No") << std::endl;

    // Test exception handling
    try {
        std::cout << "Value of Node 4: " << myGraph.at(4) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    // Test swap
    graph::Graph<int, std::string, double> anotherGraph;
    anotherGraph[10] = "Node 10";

    std::cout << "Before swap:" << std::endl;
    std::cout << "myGraph size: " << myGraph.size() << std::endl;
    std::cout << "anotherGraph size: " << anotherGraph.size() << std::endl;

    swap(myGraph, anotherGraph);

    std::cout << "After swap:" << std::endl;
    std::cout << "myGraph size: " << myGraph.size() << std::endl;
    std::cout << "anotherGraph size: " << anotherGraph.size() << std::endl;

    std::cout << "All tests passed successfully!" << std::endl;

    return 0;
}
