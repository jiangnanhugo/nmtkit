#ifndef NMTKIT_MULTILAYER_PERCEPTRON_H_
#define NMTKIT_MULTILAYER_PERCEPTRON_H_

#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <dynet/dynet.h>
#include <dynet/expr.h>
#include <dynet/model.h>
#include <nmtkit/serialization_utils.h>

namespace nmtkit {

// Utility class of the multilayer perceptron with ReLU hidden activation.
class MultilayerPerceptron {
public:
  // Constructs an empty perceptron.
  MultilayerPerceptron() {}

  // Constructs a perceptron object with given number of layers/units.
  // E.g. constructing 3-class classifier with 1024 inputs and two 256
  // hidden layers:
  //   MultilayerPerceptron perceptron({ 1024, 256, 256, 3 }, &model);
  //
  // Arguments:
  //   spec: List of positive numbers which specifies the network structure.
  //         The length of this argument specifies the number of layers
  //         (including input/output layers).
  //         Each number in this argument specifies the number of units in each
  //         layer.
  //   model: Model object for training.
  MultilayerPerceptron(
      const std::vector<unsigned> & spec,
      dynet::Model * model);

  // Prepares precomputed values.
  //
  // Arguments:
  //   cg: Computation graph.
  void prepare(dynet::ComputationGraph * cg);

  // Performs forward calculation.
  //
  // Arguments:
  //   input: input expression.
  //
  // Returns:
  //   Output expression.
  dynet::expr::Expression compute(const dynet::expr::Expression & input);

private:
  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {
    ar & w_ & b_;
  }

  std::vector<unsigned> spec_;
  std::vector<dynet::Parameter> w_;
  std::vector<dynet::Parameter> b_;
  std::vector<dynet::expr::Expression> i_w_;
  std::vector<dynet::expr::Expression> i_b_;
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::MultilayerPerceptron);

#endif  // NMTKIT_MULTILAYER_PERCEPTRON_H_
