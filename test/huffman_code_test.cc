#include "config.h"

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <nmtkit/huffman_code.h>
#include <nmtkit/word_vocabulary.h>

using std::ifstream;
using std::string;
using std::vector;
using boost::archive::binary_iarchive;
using nmtkit::HuffmanCode;
using nmtkit::WordVocabulary;

namespace {

const string tok_filename = "data/small.en.tok";
const string vocab_filename = "data/small.en.vocab";

template <class T>
void loadArchive(const string & filepath, T * obj) {
  ifstream ifs(filepath);
  binary_iarchive iar(ifs);
  iar >> *obj;
}

}  // namespace

BOOST_AUTO_TEST_SUITE(HuffmanCodeTest)

BOOST_AUTO_TEST_CASE(CheckNumBits) {
  const vector<unsigned> vocab_sizes {4, 8, 16, 32, 64, 128};
  const vector<unsigned> num_bits {3, 5, 7, 8, 9, 10};

  for (unsigned i = 0; i < vocab_sizes.size(); ++i) {
    WordVocabulary vocab(tok_filename, vocab_sizes[i]);
    HuffmanCode codec(vocab);
    BOOST_CHECK_EQUAL(num_bits[i], codec.getNumBits());
  }
}

BOOST_AUTO_TEST_CASE(CheckNumBits2) {
  WordVocabulary vocab;
  ::loadArchive(::vocab_filename, &vocab);
  HuffmanCode codec(vocab);
  BOOST_CHECK_EQUAL(13, codec.getNumBits());
}

BOOST_AUTO_TEST_CASE(CheckEncoding) {
  const vector<unsigned> ids {0, 1, 2, 3, 4, 10, 50, 100, 200, 400, 499};
  const vector<vector<bool>> expected {
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0},
  };

  WordVocabulary vocab;
  ::loadArchive(::vocab_filename, &vocab);
  HuffmanCode codec(vocab);

  for (unsigned i = 0; i < ids.size(); ++i) {
    const vector<bool> observed = codec.getCode(ids[i]);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        expected[i].begin(), expected[i].end(),
        observed.begin(), observed.end());
  }
}

BOOST_AUTO_TEST_CASE(CheckDecoding) {
  const vector<vector<bool>> code {
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  const vector<unsigned> expected {
    0, 1, 2, 3, 4, 10, 50, 100, 200, 400, 499, 1, 10,
  };

  WordVocabulary vocab;
  ::loadArchive(::vocab_filename, &vocab);
  HuffmanCode codec(vocab);

  for (unsigned i = 0; i < code.size(); ++i) {
    const unsigned observed = codec.getID(code[i]);
    BOOST_CHECK_EQUAL(expected[i], observed);
  }
}

BOOST_AUTO_TEST_SUITE_END()
