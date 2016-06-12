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

    string neuron;
    //int i = 0;

    cout << "\nPlease enter Neuron: ";
    cin >> neuron;
    //neuron = connectome_vector[3954].get_neuronA();

    for(int i = 0; i < connectome_vector.size() ; i++) {

        if (connectome_vector[i].get_neuronA() == neuron) {

            cout << "----------" << endl;
            cout << "Running Connectome with : x " << i << " , neuron: " << connectome_vector[i].get_neuronA() << endl;
            cout << "----------" << endl;

            runconnectome(connectome_vector, postsynaptic_vector, connectome_vector[i]);
        }

    }

    /*  MPI START   */

    cout << "\n----------" << endl;
    cout << "MPI START: " << endl;
    cout << "----------" << endl;
    cout << endl;

    int world_rank,world_size,name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI::Init(argc,argv);

    // find c++ implementation
    MPI_Get_processor_name(processor_name,&name_len);

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    cout << "processor " << processor_name <<" : I am " << world_rank << " of " << world_size << endl;

    MPI_Finalize();

    cout << endl;
    cout << "----------" << endl;
    cout << "MPI END: " << endl;
    cout << "----------" << endl;

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

                //string msg = y[postsynaccum].get_neuronA() + " " + to_string(y[postsynaccum].get_weight());
                //cout << "msg: " << msg << endl;
                cout << "Fire Muscle " + y[postsynaccum].get_neuronA() << endl;
                y[postsynaccum].set_weight(0);

            } else {

                cout << "Fire Neuron " + y[postsynaccum].get_neuronA() << endl;
                dendriteAccumulate(x,y,y[postsynaccum]);
                y[postsynaccum].set_weight(0);

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

