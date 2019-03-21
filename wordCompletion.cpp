#include "wordCompletion.h"

using namespace std;

FixedSizeAllocator<Trie::Node> Trie::Node::pool;

wordCompletion::wordCompletion():
    wordIdxMap(INT16_MAX),
    trie{},
    dicSize{0}
{
    wordIdxMap.max_load_factor(0.5);
}


int wordCompletion::access(string w) {
    // pre: w is a non-empty word with characters in {a..z}
    // post: w is added to structure if not previous in; its frequency is increased by 1
    // 	returns ID of word w
    auto it = wordIdxMap.find(w);
    if(it == wordIdxMap.end()) {
        trie.insert(w, dicSize);
        wordIdxMap.emplace(std::move(w), dicSize);
         
        return dicSize++;
    }

    trie.access(w, it->second);
    return it->second;
}	

vector<vector<int>> wordCompletion::getCompletions(string w, int k) {
    // pre: Dictionary is non-empty. w is non-empty. k>=1.
    // post: see assignment for what to return

    return trie.getCompletionIdx(w, k);
}

// Please leave the following "#ifndef" lines in place; this is needed
// for the cs240 test scripts to run correctly. Anything that you put
// between these lines will NOT be read by our marking scripts.

#ifdef TESTING
int main() {
    wordCompletion * h = new wordCompletion();

    // The following implements the test case from the assignment.

    h->access("break");
    h->access("break");
    h->access("break");
    h->access("break");
    h->access("break");
    h->access("break");
    h->access("crazy");
    h->access("crazy");
    h->access("crazy");
    h->access("crazy");
    h->access("crazy");
    h->access("crazy");
    h->access("crazy");
    h->access("taut");
    h->access("taut");
    h->access("taut");
    h->access("taut");
    h->access("taut");
    h->access("teatowel");
    h->access("teatowel");
    h->access("teatowel");
    h->access("teatowel");
    h->access("teamster");
    h->access("teamster");
    h->access("teamster");
    h->access("total");
    h->access("total");
    h->access("total");
    h->access("total");
    h->access("total");
    h->access("total");

    string w = "team";
    int k = 2;
    vector<vector<int> > A = h->getCompletions(w, k);

    for (int i = 0; i <= static_cast<int>(w.size()); i++) {
        cout << "<";
        for (int j=0; j < k-1; j++) cout << A[i][j] << ",";
        cout << A[i][k-1] << "> ";
    }
    cout << endl;

    // The output should look much like what you got on the assignment

    delete h;

}
#endif
