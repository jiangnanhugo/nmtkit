#ifndef NMTKIT_DECODER_H_
#define NMTKIT_DECODER_H_

#include <vector>
#include <boost/serialization/access.hpp>
#include <dynet/dynet.h>
#include <dynet/expr.h>
#include <dynet/rnn.h>
#include <nmtkit/attention.h>
#include <nmtkit/serialization_utils.h>

namespace nmtkit {

// Abstract class for the decoder implementations.
class Decoder {
  Decoder(const Decoder &) = delete;
  Decoder(Decoder &&) = delete;
  Decoder & operator=(const Decoder &) = delete;
  Decoder & operator=(Decoder &&) = delete;

public:
  // Structure to represent a decoder state.
  struct State {
    std::vector<dynet::RNNPointer> positions;
    std::vector<dynet::expr::Expression> params;
  };

  Decoder() {}
  virtual ~Decoder() {}

  // Prepares internal parameters.
  //
  // Arguments:
  //   seed: Seed values of initial states, e.g., final encoder states.
  //   cg: Computation graph.
  //   is_training: true when training, false otherwise.
  //
  // Returns:
  //   Initial state of the decoder.
  virtual State prepare(
      const std::vector<dynet::expr::Expression> & seed,
      dynet::ComputationGraph * cg,
      const bool is_training) = 0;

  // Proceeds one decoding step.
  //
  // Arguments:
  //   state: Previous decoder state.
  //   input_ids: List of input symbols in the current step.
  //   attention: Attention object.
  //   atten_probs: Placeholder of the attention probability vector. If the
  //                value is nullptr, this argument would be ignored.
  //   output: Placeholder of the output embedding. If the value is nullptr,
  //           this argument would be ignored.
  //   cg: Computation graph.
  //   is_training: true when training, false otherwise.
  //
  // Returns:
  //   Next state of the decoder.
  virtual State oneStep(
      const State & state,
      const std::vector<unsigned> & input_ids,
      Attention * attention,
      dynet::expr::Expression * atten_probs,
      dynet::expr::Expression * output,
      dynet::ComputationGraph * cg,
      const bool is_training) = 0;

  // Returns the number of units in the output embedding.
  virtual unsigned getOutputSize() const = 0;

private:
  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {}
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::Decoder);

#endif  // NMTKIT_DECODER_H_
