#include <iostream>
#include <vector>
#include <fstream>
#include "synapse.h"
//#include "mpi.h"  # I get an error?

/*
 * when (neuron A, neuron B, weight)
 * when weight is positive, A is on then B is on
 * when weight is negative, A is off when B is on
 *
 */

using namespace std;

// global variables - vanessa's direct file paths

//string connectome_file = "connectome.csv";
//string synaptic_file = "postsynaptic.csv";

// mac file strings
string connectome_file = "/Users/vanessaulloa/ClionProjects/connectome/connectome.csv";
string synaptic_file = "/Users/vanessaulloa/ClionProjects/connectome/postsynaptic.csv";

// windows - pc
//string connectome_file = "K:\\School\\Summer 2016\\connectome\\connectome.csv";
//string synaptic_file = "K:\\School\\Summer 2016\\connectome\\postsynaptic.csv";

/*
 * threshold value
 * maximum accumulated value that must be exceeded before the neurite will fire
*/
int threshold = 15;

//   function prototypes
void read_connectome(vector<synapse> &x);
void read_postsynaptic(vector<synapse> &x );
void dendriteAccumulate(vector<synapse> &, vector<synapse> &,synapse);
void fireNeuron(vector<synapse> &, vector<synapse> &,synapse);
void runconnectome(vector<synapse> &, vector<synapse> &,synapse);
void testFiles(vector<synapse> &, vector<synapse> &);

///

int main(int argc, char **argv) {

    /*
        connectome_vector:
            full C Elegans Connectome
        postsynaptic_vector: 
            maintains accumulated values for each neuron and muscle.
    */
    vector<synapse> postsynaptic_vector;
    vector<synapse> connectome_vector;

    //  function that reads the connectome.csv file and values
    //  uses overloaded synapse constructor for 2 neurons and 1 int value
    read_connectome(connectome_vector);

    //  function that reads the postsynaptic.csv file and values
    //  uses overloaded synapse constructor for 1 neuron and 1 int value
    read_postsynaptic(postsynaptic_vector);

    //  runs the connectome, user input in the python code
    //  the user input was substituted with the first neurite in the
    //  connectome_vector

    int x = 825;

    cout << "\n----------" << endl;
    cout << "Running Connectome with : x " << x << " , neuron: " << connectome_vector[x].get_neuronA() << endl;
    cout << "----------\n" << endl;

    runconnectome(connectome_vector,postsynaptic_vector,connectome_vector[x]);

    return 0;

}// end main

/*      FUNCTIONS       */

/*
 * testFiles()
 * tests that the vectors were filled with data from .csv
 * by displayed some of the values
 *
 */

void testFiles(vector<synapse> &c, vector<synapse> &p)  {

    ///
    /*      testing for file reads and pushed values onto vectors   */
    ///

    //   to test that values were stored in the vector
    //   by checking the size and the first value connectome elements
    cout << "connectome vector size: " << c.size() << endl;
    for(int i = 0; i < 5; i++) {
        cout << "\t" << i << " : " << c[i].get_neuronA() << endl;
        cout << "\t" << i << " : " << c[i].get_neuronB() << endl;
        cout << "\t" << i << " : " << c[i].get_weight() << endl;
        cout << endl;
    }



    //  to test the values stored in the vector
    //  by checking the size and values of the postsynaptic elements
    cout << "postsynaptic vector size: " << p.size() << endl;
    for(int i = 0; i < 5; i++) {
        cout << "\t" << i << " : " << p[i].get_neuronA() << endl;
        cout << "\t" << i << " : " << p[i].get_weight() << endl;
        cout << endl;
    }

    ///
    /*  end test    */
    ///

}

/*
 * read_connectome()
 *
 */
void read_connectome(vector<synapse> & x) {

    // create file reading object and open the file
    /*  NEED WAY TO DYNAMICALLY REFERENCE file  */
    ifstream file;
    //string connectome_file = "/Users/vanessaulloa/Google Drive/Capstone Project/SimpleConnectome/connectome/connectome.csv";
    file.open(connectome_file);

    // substring variables
    string line;
    size_t pos1, pos2,length;
    string neuronA,neuronB,weight,temp;

    //  continue while file is open

    if(!file.is_open()) {

        cout << " Connectome.csv file could not be opened. " << endl;
        exit(0);

    }   else    {

        while (getline(file, line)) {

            /*
             * int pos1 = first position of ',' in value
             * string neuronA = from 0 to pos1 in value
             * string temp = pos1 to length
             * int pos2 = fiirst position of ',' in temp
             * string neuronB = from pos1 to pos2 in value
             * string weight = from pos2 to string_length in value
             *
             */

            length = line.length();
            pos1 = line.find(',') + 1;

            neuronA = line.substr(0,pos1 - 1);
            temp = line.substr(pos1,length);
            pos2 = temp.find(',');

            neuronB = temp.substr(0,pos2);
            weight = temp.substr(pos2 + 1,length);
            int w = atoi(weight.c_str());

            //  push a new synapse object onto the vector
            x.push_back(synapse(neuronA,neuronB,w));

        }   //end while loop

    }// end if statement

    //  close the file
    file.close();

}// end function

/*
 * read_postsynaptic()
 *
 */
void read_postsynaptic(vector<synapse> & x)    {

    // create file reading object and open the file
    /*  NEED WAY TO DYNAMICALLY REFERENCE file  */
    ifstream file;
    //string synaptic_file = "/Users/vanessaulloa/Google Drive/Capstone Project/SimpleConnectome/postsynaptic.csv";
    file.open(synaptic_file);

    // substring variables
    string line;
    size_t pos1;
    string neuronA;

    if(!file.is_open()) {

        cout << "synaptic.csv file could not be opened. " << endl;
        exit(0);

    }   else    {

        while(getline(file,line)) {

            //length = line.length();
            pos1 = line.find(',') + 1;

            neuronA = line.substr(0, pos1 - 1);

            //  not needed since default value is 0
            //weight = line.substr(pos1, length);
            //int w = atoi(weight.c_str());

            //  push a new synapse object onto the vector
            x.push_back(synapse(neuronA, 0));

        }// end while

    }//  end if statement

    file.close();

}// end function

/*
 * dendriteAccumulate()
 *
 */
void dendriteAccumulate(vector<synapse> &x, vector<synapse> &y, synapse a)  {

    for(synapse allneurons : x) {

        if(allneurons.get_neuronA() == a.get_neuronA())    {

            for (synapse postsyn : y)   {

                if(postsyn.get_neuronA() == allneurons.get_neuronB())   {

                    //  POSTSYNAPTIC VECTOR is altered here.
                    postsyn.set_weight(allneurons.get_weight());
                    cout << "postsynaptic vector altered at: ";
                    cout << postsyn.get_neuronA() << ", " << postsyn.get_weight() << endl;

                }// end if

            }// end for

        }// end if

    }// end for

}// end dendriteAccumulate()

/*
 * fireNeuron()
 * when the threshold has been exceeded, fire the neurite
 * 1st we accumulate the postsynaptic weights
 * then we check everywhere the accumulator is > threshold
 *
 */
void fireNeuron(vector<synapse> &x, vector<synapse> &y,synapse a)   {

    dendriteAccumulate(x,y,a);

    for(synapse postsynaccum: y)    {

        if((postsynaccum.get_weight() > threshold))    {

            //  this is the output and where you put in ###
            //  connections to other applications
            //  note after firing, the accumulator is set to 0 -- important

            if(postsynaccum.get_neuronA().substr(0,2) == "MV" || postsynaccum.get_neuronA().substr(0,2) == "MD") {

                string msg = postsynaccum.get_neuronA() + " " + to_string(postsynaccum.get_weight());
                cout << "msg: " << msg << endl;
                cout << "Fire Muscle " + postsynaccum.get_neuronA() + to_string(postsynaccum.get_weight());
                postsynaccum.set_weight(0);

            } else {

                cout << "Fire Neuron " + postsynaccum.get_neuronA() << endl;
                dendriteAccumulate(x,y,postsynaccum);
                postsynaccum.set_weight(0);

            }// end if/else

        }// end if

    }// end for

}// end fireNeuron()

/*
 * runconnectome()
 *
 *
 */
void runconnectome(vector<synapse> &x, vector<synapse> &y, synapse a)   {

    dendriteAccumulate(x,y,a);

    for( synapse postsynaccum : y)  {

        if(abs(postsynaccum.get_weight()) > threshold)  {

            fireNeuron(x,y,postsynaccum);
            postsynaccum.set_weight(0);

        }// end if

    }// end for

}// end runconnectome()

