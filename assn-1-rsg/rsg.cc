/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */

#include <map>
#include <fstream>
#include "definition.h"
#include "production.h"
using namespace std;

/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream &infile, map<string, Definition> &grammar)
{
  while (true)
  {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof())
      return; // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}

/**
 * Checks if word is a terminal, i.e a string that can not
 * be expanded further into other strings.
 *
 * @param word a valid reference to a flat text file storing the grammar.
 *
 */

bool isTerminal(const string &word)
{
  return word.at(0) != '<';
}

/**
 * Takes a reference to production, a reference to grammar and a
 * reference text and appends strings to text by starting from the
 * current string in production, which is pointed to by the parameter
 * iterator current.
 *
 * @param production a reference to STL vector, which contains strings
 * @param current an iterator to production
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 * @param text an STL vector which contains the generated text from production.
 */

static void buildTextVector(Production &production,
                            Production::iterator current,
                            const map<string, Definition> &grammar,
                            vector<string> &text)
{
  // production has at least one more word(terminal or nonterminal)
  // to be expanded
  while (current != production.end())
  {
    string word = *current; // the current word from production to be expanded
    // when word is terminal, it is just added to text
    if (isTerminal(word))
    {
      text.push_back(word);
    }
    // when word is nonterminal, it should be recursively expanded
    else
    {
      Definition definition = grammar.at(word);
      Production newProduction = definition.getRandomProduction();
      buildTextVector(newProduction, newProduction.begin(), grammar, text);
    }
    ++current;
  }
}

/**
 * Prints the contents of the vector text passed in as a reference.
 *
 * @param text an STL vector of strings
 */

static void printTextVector(const vector<string> &text)
{
  for (vector<string>::const_iterator it = text.begin();
       it != text.end(); ++it)
    std::cout << *it << ' ';

  cout << endl;
}

/**
 * Takes a string and a reference to grammar and generates text
 * according to grammar where the start of the text is determined
 * by the provided string.
 *
 * @param startNonTerminal a string that specifies the start of the text
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void generateText(const string startNonTerminal,
                         const map<string, Definition> &grammar)
{
  Definition definition = grammar.at(startNonTerminal);

  Production production = definition.getRandomProduction();

  int numberOfVersions = 3;
  for (int i = 0; i < numberOfVersions; ++i)
  {
    vector<string> text;

    buildTextVector(production, production.begin(), grammar, text);

    cout << "Version #" << i + 1 << ":";
    cout << " ---------------------------" << endl;
    printTextVector(text);
    cout << endl;
  }
}

/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  You're to update and decompose the main function to print
 * three randomly generated sentences, as illustrated by the sample
 * application.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    cerr << "You need to specify the name of a grammar file." << endl;
    cerr << "Usage: rsg <path to grammar text file>" << endl;
    return 1; // non-zero return value means something bad happened
  }

  ifstream grammarFile(argv[1]);
  if (grammarFile.fail())
  {
    cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
    return 2; // each bad thing has its own bad return value
  }

  // things are looking good...
  map<string, Definition> grammar;
  readGrammar(grammarFile, grammar);
  cout << "The grammar file called \"" << argv[1] << "\" contains "
       << grammar.size() << " definitions." << endl;

  string startNonTerminal = "<start>";
  generateText(startNonTerminal, grammar);

  return 0;
}
