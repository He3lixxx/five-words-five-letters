### Five Five-Letter Words With Twenty-Five Unique Letters

In his 2022 video ["Can you find: five five-letter words with twenty-five
unique letters?"](https://youtu.be/_-AfhLQfb6w), Matt Parker proposes the
Wordle-inspired problem of finding all combinations of five english five-letter
words that do not contain duplicate characters, and thus span 25 letters in
total.  In his video, he demonstrates an approach to compute these combinations
in a runtime of ~32 days = 2.8e6 seconds.

At the end of this video, he shows an approach to solving this problem that [a
viewer proposes](https://gitlab.com/bpaassen/five_clique).  They model a graph,
with each word being a node and two nodes being connected by an edge if they do
not share a letter.  In this graph, a 5-clique models a valid combination from
the original problem.  Using a generic clique-finding algorithm, their approach
can solve the problem in ~20 minutes = 9e2 seconds.  This is three to four
orders of magnitude faster than Parker's original approach.

However, it still appeared to the author as optimizable ;) (and a fun
optimization / competitive programming challenge). So, we present our solution
to the problem. On a i5-6200U running Ubuntu 22.04, our implementation computes
all combinations from the initial word list in 1.8s, improving by
approximately three orders of magnitude compared to the state-of-the-art
solution. Our approach is not very complex, and certainly simpler than the
graph approach.  The C++ implementation is ~100 lines long.  We describe the
approach in detail below.


### Building and Running
1. Clone the repository and switch into the directory
   ```bash
   git clone https://github.com/He3lixxx/five-words-five-letters
   cd five-words-five-letters
   ```

2. Build the source code.  We use the C++20 function `std::popcount`
   internally, so your compiler requires minimal C++20 support (GCC 9.2 (2019)
   or Clang 9 (2019) and above). With older compilers, you should be able to
   replace the call with `__popcnt()` or whatever intrinsic your compiler
   provides.
   ```bash
   # using the makefile:
   make
   # or, with clang:
   clang++ -std=c++20 -O3 -march=native -o five_words five_words.cpp
   # or, with gcc:
   g++ -std=c++20 -O3 -march=native -o five_words five_words.cpp
   ```

3. Download a wordlist used as input. Matt Parker used `words_alpha.txt` from
   https://github.com/dwyl/english-words
   ```bash
   wget https://github.com/dwyl/english-words/raw/master/words_alpha.txt
   ```

3. Run the program.
   ```bash
   ./five_words < words_alpha.txt
   # or, to hide the output spam and measure execution time:
   time ./five_words < words_alpha.txt > /dev/null
   ```

   on our test platform, we get the following output:
   ```bash
   $ time ./five_words < words_alpha.txt > /dev/null
   Read 15920 words with 5 characters.
   of those, 5977 unique words remain when removing anagrams.
   Done. 538 results found (equals 831 results when including anagrams).
   ./five_words < words_alpha.txt > /dev/null  1,77s user 0,00s system 99% cpu 1,774 total
   ```

### Approach
The approach is simple: We see words as 5-sets of (lower-case) letters.  The
english alphabet has 26 letters, so we can store one such set in a single 32bit
number: The n-th bit is 1 if the set contains the n-th character of the
alphabet.  This uses 26 of the available 32 bits.  For example, the word "ac"
would be the number 0b101 (leading zeros omitted) Now, we can easily compute
set intersection and unions by using bitwise and/or instructions.

For building the actual combinations, we follow a simple recursive approach:
* Start with zero selected words.
* Go through all words, and see if the word can be added without duplicating
  any characters.
* If an additional word is found, recurse (to add a second, third, fourth, and
  fifth word).
* Keep track of the words chosen so far in a stack (which always contains
  between zero and five 32bit numbers).
* If the recursive function is called with five words selected, we have found a
  valid combination.  Find out what the character order in the input was (maybe
  there are multiple input words, in cases of anagrams) and output the
  combination.

#### Performance Considerations:
* For each combination, all 5! = 120 permutations of the five words would be
  valid. To find only one permutation, we simply use the order of the
  internally stored word list, and only look at combinations where elements are
  ordered the same as in the word list.  In technical terms: During recursion,
  we always keep track of where we are in the word list in the outer recursion
  levels. Inner recursion levels then start at that location instead of the
  start.
* To prevent recursing into branches that we already tested earlier, we use a
  bitset. It has 2^26 = 6.7e7 entries, one for every possible 26-letter-subset
  we could have possibly encountered. This requires 8MB of memory to store.
  Initially, all bits are 0. If we encounter a subbranch, say for the character
  set "asdfgqwertz", and we find that it is not possible to combine this set
  with three more words to form a valid result combination, we store a 1 in the
  bitset at the location of this set. This allows us to remember that we
  already tried to come up with a solution for this subset and failed, and thus
  do not have to recurse into this branch again.
  * Since 8MB is too big to fit into L1 or L2 cache in most processors, we try
    to make our access pattern into the bitset less random by sorting all input
    words by their set-representation first. This way, we will likely access
    bits in closer proximity to the last accesses, and increase the chance of
    cache hits.
