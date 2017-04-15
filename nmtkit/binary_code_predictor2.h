#ifndef NMTKIT_BINARY_CODE_PREDICTOR2_H_
#define NMTKIT_BINARY_CODE_PREDICTOR2_H_

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <nmtkit/binary_code.h>
#include <nmtkit/error_correcting_code.h>
#include <nmtkit/multilayer_perceptron.h>
#include <nmtkit/predictor.h>
#include <nmtkit/serialization_utils.h>

namespace nmtkit {

// Word predictor using binary code outputs.
class BinaryCodePredictor2 : public Predictor {
  BinaryCodePredictor2(const BinaryCodePredictor2 &) = delete;
  BinaryCodePredictor2(BinaryCodePredictor2 &&) = delete;
  BinaryCodePredictor2 & operator=(const BinaryCodePredictor2 &) = delete;
  BinaryCodePredictor2 & operator=(BinaryCodePredictor2 &&) = delete;

public:
  // Initializes an empty predictor.
  BinaryCodePredictor2() {}

  // Initializes the predictor.
  //
  // Arguments:
  //   input_size: Number of units in the input vector.
  //   bc: Pointer to a BinaryCode object.
  //   ecc: Pointer to a ErrorCorrectingCode object.
  //   model: Model object for training.
  BinaryCodePredictor2(
      const unsigned input_size,
      boost::shared_ptr<BinaryCode> & bc,
      boost::shared_ptr<ErrorCorrectingCode> & ecc,
      dynet::Model * model);

  ~BinaryCodePredictor2() override {}

  void prepare(dynet::ComputationGraph * cg) override;

  dynet::expr::Expression computeLoss(
      const dynet::expr::Expression & input,
      const std::vector<unsigned> & target_ids,
      dynet::ComputationGraph * cg) override;

  std::vector<Predictor::Result> predictKBest(
      const dynet::expr::Expression & input,
      const unsigned num_results,
      dynet::ComputationGraph * cg) override;

  std::vector<Predictor::Result> predictByIDs(
      const dynet::expr::Expression & input,
      const std::vector<unsigned> word_ids,
      dynet::ComputationGraph * cg) override;

private:
  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {
    ar & boost::serialization::base_object<Predictor>(*this);
    ar & num_original_bits_;
    ar & num_encoded_bits_;
    ar & bc_;
    ar & ecc_;
    ar & converter_;
  }

  unsigned num_original_bits_;
  unsigned num_encoded_bits_;
  boost::shared_ptr<BinaryCode> bc_;
  boost::shared_ptr<ErrorCorrectingCode> ecc_;
  MultilayerPerceptron converter_;
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::BinaryCodePredictor2);

#endif  // NMTKIT_BINARY_CODE_PREDICTOR2_H_