#include <config.h>

#include <nmtkit/default_decoder.h>
#include <nmtkit/exception.h>

using std::vector;

namespace DE = dynet::expr;

namespace nmtkit {

DefaultDecoder::DefaultDecoder(
    const unsigned num_layers,
    const unsigned vocab_size,
    const unsigned embed_size,
    const unsigned hidden_size,
    const unsigned seed_size,
    const unsigned context_size,
    const float dropout_rate,
    dynet::Model * model)
: num_layers_(num_layers)
, vocab_size_(vocab_size)
, embed_size_(embed_size)
, hidden_size_(hidden_size)
, seed_size_(seed_size)
, context_size_(context_size)
, dropout_rate_(dropout_rate)
, rnn_(num_layers, embed_size + context_size, hidden_size, *model)
, p_lookup_(model->add_lookup_parameters(vocab_size, {embed_size}))
{
  for (unsigned i = 0; i < num_layers; ++i) {
    enc2dec_.emplace_back(
        MultilayerPerceptron({seed_size, hidden_size}, model));
  }
}

Decoder::State DefaultDecoder::prepare(
    const vector<DE::Expression> & seed,
    dynet::ComputationGraph * cg,
    const bool is_training) {
  NMTKIT_CHECK_EQ(2 * num_layers_, seed.size(), "Invalid number of initial states.");
  vector<DE::Expression> states;
  for (unsigned i = 0; i < num_layers_; ++i) {
    enc2dec_[i].prepare(cg);
    states.emplace_back(enc2dec_[i].compute(seed[i]));
  }
  for (unsigned i = 0; i < num_layers_; ++i) {
    states.emplace_back(DE::tanh(states[i]));
  }
  rnn_.set_dropout(is_training ? dropout_rate_ : 0.0f);
  rnn_.new_graph(*cg);
  rnn_.start_new_sequence(states);
  return {{rnn_.state()}, {rnn_.back()}};
}

Decoder::State DefaultDecoder::oneStep(
    const Decoder::State & state,
    const vector<unsigned> & input_ids,
    Attention * attention,
    dynet::expr::Expression * atten_probs,
    dynet::expr::Expression * output,
    dynet::ComputationGraph * cg,
    const bool is_training) {
  NMTKIT_CHECK_EQ(
      1, state.positions.size(), "Invalid number of RNN positions.");
  NMTKIT_CHECK_EQ(
      1, state.params.size(), "Invalid number of state parameters.");

  // Aliases
  const dynet::RNNPointer & prev_pos = state.positions[0];
  const DE::Expression & prev_h = state.params[0];

  // Calculation
  const DE::Expression embed = DE::lookup(*cg, p_lookup_, input_ids);
  const vector<DE::Expression> atten_info = attention->compute(
      prev_h, is_training);
  const DE::Expression next_h = rnn_.add_input(
      prev_pos, DE::concatenate({embed, atten_info[1]}));

  // Store outputs.
  if (atten_probs != nullptr) {
    *atten_probs = atten_info[0];
  }
  if (output != nullptr) {
    *output = next_h;
  }

  return {{rnn_.state()}, {next_h}};
}

}  // namespace nmtkit

NMTKIT_SERIALIZATION_IMPL(nmtkit::DefaultDecoder);
