//
// Created by Vanessa Ulloa on 5/11/16.
//

#include "synapse.h"

using namespace std;

synapse::synapse(void) { }

synapse::synapse(string a, int c) {
    neuronA = a;
    weight = c;
}

synapse::synapse(string a, string b, int c) {
    neuronA = a;
    neuronB = b;
    weight = c;
}

int synapse::get_weight() {
    return weight;
}

void synapse::set_weight(int x) {
    //weight += atoi(x.c_str());
    weight += x;
}

string synapse::get_neuronA() {
    return neuronA;
}

void synapse::set_neuronA(string x) {
    neuronA = x;
}

string synapse::get_neuronB() {
    return neuronB;
}

void synapse::set_neuronB(string x) {
    neuronB = x;
}