#include <boost/mpi.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <boost/serialization/string.hpp>
#include <string>
#include <map>

namespace mpi = boost::mpi;
using namespace std;

class token
{
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // The word which appeared in the text
        ar & word;
        // The number of occurences of this word
        ar & count;
    }

public:
    std::string word;
    int count;

    token() {};
    token(string w, int c) :
            word(w), count(c)
    {}
};

struct less_than_key
{
    inline bool operator() (const token& token1, const token& token2)
    {
        return token1.word < token2.word;
    }
};

bool IsFileEmpty(ifstream& file) {
    return file.peek() == ifstream::traits_type::eof();
}

// Reads speech_tokenized.txt and creates token for each of word with zero repetition
vector<token> ReadSpeechFile(){
    vector<token> tokens;
    ifstream in_file("./speech_tokenized.txt");
    if(IsFileEmpty(in_file)) {
        ofstream output;
        output.open ("output.txt");
        output << "speech_tokenized.txt is empty." << endl;
        output.close();
        exit(3);
    }
    if(!in_file) {
        ofstream output;
        output.open ("output.txt");
        output << "Cannot open speech_tokenized.txt." << endl;
        output.close();
        exit(2);
    }
    if(in_file.is_open()) {
        string word;
        while (in_file >> word) {
            tokens.push_back(token(word, 0));
        }
    }
    in_file.close();
    return tokens;
}

// Calculates the number of tokens that will be sent to each slave
int CalculateNumTokens (vector<token> tokens, int num_processors){
    int input_size = tokens.size() / (num_processors - 1);
    // If all tokens are not distributed to slaves evenly, then the last slave will receive less token
    // This rule is implemented in main function
    if (tokens.size() % (num_processors - 1) != 0) {
        input_size++;
    }
    return input_size;
}

// Returns reduced map
map<string, int> Reduce(vector<token> subsorted_tokens) {
    std::map<std::string, int> reduced_map;
    for(int i = 0; i < subsorted_tokens.size(); i++) {
        if(reduced_map.count(subsorted_tokens[i].word) > 0) {
            reduced_map[subsorted_tokens[i].word] = reduced_map[subsorted_tokens[i].word] + 1;
        }
        else {
            reduced_map[subsorted_tokens[i].word] = 1;
        }
    }
    return reduced_map;
}

// Writes reduced map to output.txt
void WriteOutputFile(std::map<std::string, int> reduced_map) {
    ofstream output;
    output.open ("output.txt");
    map<std::string, int>::iterator it;
    for (it = reduced_map.begin(); reduced_map.end() != it; it++) {
        output << (*it).first << " " << (*it).second << endl;
    }
    output.close();
}

int main(int argc, char *argv[])   {
    mpi::environment env(argc, argv);
    mpi::communicator world;
    int num_processors = world.size();
    if(world.rank() == 0) {
        /* MASTER */
        // Step 1: Split the input and send them to slaves
        std::vector<token> tokens = ReadSpeechFile();
        int input_size = CalculateNumTokens(tokens, num_processors);

        if(input_size < num_processors - 1) {
            num_processors = input_size + 1;
            input_size = CalculateNumTokens(tokens, num_processors);
        }

        for (int i = 1; i < num_processors; i++){
            int start_index = (i - 1) * input_size;
            int end_index = i * input_size;
            // If input size is larger than the last slave can receive
            if(tokens.size() < end_index) {
                end_index = tokens.size();
            }
            vector<token> newTok(tokens.begin() + start_index, tokens.begin() + end_index);
            world.send(i, 0, newTok);
        }

        // Step 3: Split the input again and send them to slaves to be sorted
        for (int i = 1; i < num_processors; i++) {
            vector<token> newTok;
            world.recv(i, 0, newTok);
            world.send(i, 0, newTok);
        }
        // Receive sorted vectors and append them to subsorted_tokens
        vector<token> subsorted_tokens;
        for (int i = 1; i < num_processors; i++) {
            vector<token> newTok;
            world.recv(i, 0, newTok);
            for (int j = 0; j < newTok.size(); j++) {
                subsorted_tokens.push_back(newTok[j]);
            }
        }
        // Sort subsorted_tokens. Now it is fully sorted
        sort(subsorted_tokens.begin(), subsorted_tokens.end(), less_than_key());
        
        // Step 4: The master process reduces the list
        map<string, int> reduced_map = Reduce(subsorted_tokens);

        WriteOutputFile(reduced_map);
    }
    else {
        /* SLAVE */
        std::vector<token> tokens;
        // Step 2: Slaves map the words and send it back to master
        world.recv(0, 0, tokens);
        for (int i = 0; i < tokens.size(); i++) {
            tokens[i].count = 1;
        }
        world.send(0, 0, tokens);
        world.recv(0, 0, tokens);
        // Sort tokens
        std::sort(tokens.begin(), tokens.end(), less_than_key());
        world.send(0, 0, tokens);
    }
    return 0;
}