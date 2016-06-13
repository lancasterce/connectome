#include <iostream>
#include <vector>
#include <fstream>
#include "synapse.h"
#include "mpi.h"

/*
 * when (neuron A, neuron B, weight)
 * when weight is positive, A is on then B is on
 * when weight is negative, A is off when B is on
 *
 */

using namespace std;

// global variables - vanessa's direct file paths

string connectome_file = "connectome.csv";
string synaptic_file = "postsynaptic.csv";

//string connectome_file = "/Users/vanessaulloa/ClionProjects/connectome/connectome.csv";
//string synaptic_file = "/Users/vanessaulloa/ClionProjects/connectome/postsynaptic.csv";

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

int main(int argc, char *argv[]) {

    /*
        connectome_vector:
            full C Elegans Connectome
        postsynaptic_vector: 
            maintains accumulated values for each neuron and muscle.
    */

    /*  MPI START   */

    /*
     * Node (master branch):
     *      1. read the csv files
     *      2. initiate MPI
     *      3. processor rank, comm rank, comm size
     *
     * Node 1: 0 to 800
     * Node 2: 801 to 1600
     * Node 3: 1601 to 2400
     * Node 4: 2401 to 3200
     * Node 5: 3201 to 3954 (connectome_vector size)
     *
     */

    //  NODE [MASTER]

    int world_rank, world_size, name_len, chunk_size, tag1, tag2, source, dest, offset;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    string neuron;
    MPI_Status status;

    //  initiate MPI
    MPI::Init(argc, argv);

    //  checking
    cout << "\nprocessor " << processor_name <<" : I am " << world_rank << " of " << world_size << "\n\n";

    vector<synapse> postsynaptic_vector;
    vector<synapse> connectome_vector;

    //  function that reads the connectome.csv and postsynaptic.csv file and values
    //  uses overloaded synapse constructor for 2 neurons and 1 int value
    //  uses overloaded synapse constructor for 1 neuron and 1 int value
    read_connectome(connectome_vector);
    read_postsynaptic(postsynaptic_vector);

    //  get values for name_len, world_rank, world_size
    MPI_Get_processor_name(processor_name, &name_len);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    tag2 = 1;
    tag1 = 2;

    //  user input for neuron
    cout << "\nPlease enter Neuron: ";
    cin >> neuron;

    //  set chunk size
    chunk_size = connectome_vector.size() / (world_size - 1);

    //  Send each node its portion of connectome_vector
    offset = chunk_size;

    for(int i = 1; i < world_size ; i++) {

        MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
        MPI_Send(&connectome_vector[offset], chunk_size, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD);

        cout << "Sent " << chunk_size << " elements to task " << dest << " offset = " << offset << endl;
        offset += chunk_size;

    }

    //  node does its part of the work
    offset = 0;

    for (int i = 0; i < (offset + chunk_size); i++) {

        if (connectome_vector[i].get_neuronA() == neuron) {

            cout << "----------" << endl;
            cout << "Running Connectome with : x " << i << " , neuron: " << connectome_vector[i].get_neuronA() <<
            " on " << processor_name << endl;
            cout << "----------" << endl;

            //  receive from node
            //  MPI_Recv

            runconnectome(connectome_vector, postsynaptic_vector, connectome_vector[i]);

        }
    }

    //  receive from other nodes
    for(int i = 1; i < world_size; i++) {

        source = i;
        MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&connectome_vector[offset], chunk_size, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);

    }

    //  pass through Node 1 to Node 5
    //  world_rank or node = 0
    if (world_rank > 0) {

        //  receive instructions from node (master)
        source = 0;
        MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&connectome_vector[offset], chunk_size, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);

        for (int i = 0; i < (offset + chunk_size); i++) {

            if (connectome_vector[i].get_neuronA() == neuron) {

                cout << "----------" << endl;
                cout << "Running Connectome with : x " << i << " , neuron: " << connectome_vector[i].get_neuronA() <<
                " on " << processor_name << endl;
                cout << "----------" << endl;

                //  receive from node
                //  MPI_Recv

                runconnectome(connectome_vector, postsynaptic_vector, connectome_vector[i]);

                // send back to master
                dest = 0;
                MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
                MPI_Send(&connectome_vector[offset], chunk_size, MPI_FLOAT, 0, tag2, MPI_COMM_WORLD);

            }

        }
    }

    MPI_Finalize();

    /*  MPI END    */

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

    //for(synapse allneurons : x) {
    int allneurons,postsyn;

    for(allneurons = 0 ; allneurons < x.size() ; allneurons++) {

        if(x[allneurons].get_neuronA() == a.get_neuronA())    {

            //cout << "allneurons: " << allneurons.get_neuronA() << endl;
            //cout << "a          :" << a.get_neuronA() << endl;
            //cout << "allneurons.get_neuronA() == a.get_neuronA() : " << (allneurons.get_neuronA() == a.get_neuronA()) << "\n\n";

            //for (synapse postsyn : y)   {
            for(postsyn = 0; postsyn < y.size() ; postsyn++)    {

                if(y[postsyn].get_neuronA() == x[allneurons].get_neuronB())   {

                    //cout << "\tpostsyn: " << postsyn.get_neuronA() << endl;
                    //cout << "\tallneurons: " << allneurons.get_neuronA() << endl;
                    //cout << "postsyn.get_neuronA() == allneurons.get_neuronB() : " << (postsyn.get_neuronA() == allneurons.get_neuronB()) << "\n\n";

                    //  POSTSYNAPTIC VECTOR is altered here.
                    y[postsyn].set_weight(x[allneurons].get_weight());
                    //cout << "\tpostsynaptic vector altered at: ";
                    //cout << "\t" << y[postsyn].get_neuronA() << ", " << y[postsyn].get_weight() << endl;

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

    int postsynaccum;
     dendriteAccumulate(x,y,a);

    //for(synapse postsynaccum: y)    {
    for(postsynaccum = 0 ; postsynaccum < y.size() ; postsynaccum++ )   {

        //cout << "postsynaccum: " << y[postsynaccum].get_neuronA() << endl;

        if(y[postsynaccum].get_weight() > threshold)  {

            //  this is the output and where you put in ###
            //  connections to other applications
            //  note after firing, the accumulator is set to 0 -- important

            //cout << "postsynaccum : " << y[postsynaccum].get_neuronA().substr(0,2) << endl;

            if(y[postsynaccum].get_neuronA().substr(0,2) == "MV" || y[postsynaccum].get_neuronA().substr(0,2) == "MD") {

                cout << "Fire Muscle " + y[postsynaccum].get_neuronA() << y[postsynaccum].get_weight() << endl;
                y[postsynaccum].set_weight(0);

            } else {

                cout << "Fire Neuron " + y[postsynaccum].get_neuronA() << endl;
                dendriteAccumulate(x,y,y[postsynaccum]);
                //y[postsynaccum].set_weight(0); Redundent, not needed.

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

    int postsynaccum;

    dendriteAccumulate(x,y,a);

    //for( synapse postsynaccum : y)  {
    for(postsynaccum = 0 ; postsynaccum < y.size() ; postsynaccum++)    {

        //cout << "postsynaccum: " << y[postsynaccum].get_neuronA()  << " , " << y[postsynaccum].get_weight() << endl;

        if(y[postsynaccum].get_weight() > threshold)  {

            //cout << "postsynaccum: " << postsynaccum.get_neuronA()  << " , " << postsynaccum.get_weight() << endl;

            fireNeuron(x,y,y[postsynaccum]);
            y[postsynaccum].set_weight(0);

        }// end if

    }// end for

}// end runconnectome()

