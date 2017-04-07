#ifndef NMTKIT_MLP_ATTENTION_H_
#define NMTKIT_MLP_ATTENTION_H_

#include <boost/serialization/base_object.hpp>
#include <dynet/model.h>
#include <nmtkit/attention.h>
#include <nmtkit/serialization_utils.h>

namespace nmtkit {

// Multilayer perceptron-based attention.
// score = b^T * tanh(W * [mem; ctrl])
class MLPAttention : public Attention {
  MLPAttention(const MLPAttention &) = delete;
  MLPAttention(MLPAttention &&) = delete;
  MLPAttention & operator=(const MLPAttention &) = delete;
  MLPAttention & operator=(MLPAttention &&) = delete;

public:
  // Initializes an empty attention object.
  MLPAttention() {}

  // Initializes attention object.
  //
  // Arguments:
  //   memory_size: Number of units in each memory input.
  //   controller_size: Number of units in the controller input.
  //   hidden_size: Number of units in the internal hidden layer.
  //   model: Model object for training.
  MLPAttention(
      unsigned memory_size,
      unsigned controller_size,
      unsigned hidden_size,
      dynet::Model * model);

  ~MLPAttention() override {}

  void prepare(
      const std::vector<dynet::expr::Expression> & memories,
      dynet::ComputationGraph * cg) override;

  std::vector<dynet::expr::Expression> compute(
      const dynet::expr::Expression & controller) override;

private:
  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {
    ar & boost::serialization::base_object<Attention>(*this);
    ar & p_mem2h_;
    ar & p_ctrl2h_;
    ar & p_h2logit_;
  }

  dynet::Parameter p_mem2h_;
  dynet::Parameter p_ctrl2h_;
  dynet::Parameter p_h2logit_;
  dynet::expr::Expression i_concat_mem_;
  dynet::expr::Expression i_h_mem_;
  dynet::expr::Expression i_broadcast_;
  dynet::expr::Expression i_ctrl2h_;
  dynet::expr::Expression i_h2logit_;
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::MLPAttention);

#endif  // NMTKIT_MLP_ATTENTION_H_
