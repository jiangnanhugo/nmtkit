#include <config.h>

#include <nmtkit/forward_encoder.h>

using std::vector;

namespace DE = dynet::expr;

namespace nmtkit {

ForwardEncoder::ForwardEncoder(
    const unsigned num_layers,
    const unsigned vocab_size,
    const unsigned embed_size,
    const unsigned hidden_size,
    const float dropout_rate,
    dynet::Model * model)
: num_layers_(num_layers)
, vocab_size_(vocab_size)
, embed_size_(embed_size)
, hidden_size_(hidden_size)
, dropout_rate_(dropout_rate)
, rnn_(num_layers, embed_size, hidden_size, *model) {
  p_lookup_ = model->add_lookup_parameters(vocab_size_, {embed_size_});
}

void ForwardEncoder::prepare(
    dynet::ComputationGraph * cg,
    const bool is_training) {
  rnn_.set_dropout(is_training ? dropout_rate_ : 0.0f);
  rnn_.new_graph(*cg);
  rnn_.start_new_sequence();
}

vector<DE::Expression> ForwardEncoder::compute(
    const vector<vector<unsigned>> & input_ids,
    dynet::ComputationGraph * cg,
    const bool /* is_training */) {
  vector<DE::Expression> outputs;
  for (const vector<unsigned> & ith_input : input_ids) {
    const DE::Expression embed = DE::lookup(*cg, p_lookup_, ith_input);
    outputs.emplace_back(rnn_.add_input(embed));
  }
  return outputs;
}

vector<DE::Expression> ForwardEncoder::getStates() const {
  return rnn_.final_s();
}

}  // namespace nmtkit

NMTKIT_SERIALIZATION_IMPL(nmtkit::ForwardEncoder);
