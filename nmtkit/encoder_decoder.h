#ifndef NMTKIT_ENCODER_DECODER_H_
#define NMTKIT_ENCODER_DECODER_H_

#include <memory>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <dynet/dynet.h>
#include <dynet/expr.h>
#include <dynet/lstm.h>
#include <dynet/model.h>
#include <nmtkit/attention.h>
#include <nmtkit/basic_types.h>
#include <nmtkit/decoder.h>
#include <nmtkit/encoder.h>
#include <nmtkit/inference_graph.h>
#include <nmtkit/serialization_utils.h>
#include <nmtkit/predictor.h>

namespace nmtkit {

class EncoderDecoder {
  EncoderDecoder(const EncoderDecoder &) = delete;
  EncoderDecoder(EncoderDecoder &&) = delete;
  EncoderDecoder & operator=(const EncoderDecoder &) = delete;
  EncoderDecoder & operator=(EncoderDecoder &&) = delete;

public:
  // Constructs an empty encoder-decoder model.
  EncoderDecoder() {}

  // Constructs a new encoder-decoder model.
  //
  // Arguments:
  //   encoder: Pointer to the Encoder object.
  //   decoder: Pointer to the Decoder object.
  //   attention: Pointer to the Attention object.
  //   predictor: Pointer to the Predictor object.
  //   loss_integration_type:
  //     Name of the strategy to integrate loss values.
  //     Available values:
  //       'sum': Sum all losses.
  //       'mean': Sum all losses, then divides by batch size.
  EncoderDecoder(
      std::shared_ptr<Encoder> & encoder,
      std::shared_ptr<Decoder> & decoder,
      std::shared_ptr<Attention> & attention,
      std::shared_ptr<Predictor> & predictor,
      const std::string & loss_integration_type);

  // Constructs computation graph for the batch data.
  //
  // Arguments:
  //   batch: Batch data to be trained.
  //   cg: Computation graph.
  //   is_training: true when training, false otherwise.
  //
  // Returns:
  //   dynet::Expression object representing total loss value.
  dynet::expr::Expression buildTrainGraph(
      const Batch & batch,
      dynet::ComputationGraph * cg,
      const bool is_training);

  // Generates output sentence using given input sentence.
  //
  // Arguments:
  //   source_ids: List of source word IDs.
  //   source_bos_id: "<s>" ID in the source language.
  //   source_eos_id: "</s>" ID in the source language.
  //   target_bos_id: "<s>" ID in the target language.
  //   target_eos_id: "</s>" ID in the target language.
  //   max_length: Maximum number of words (except "<s>") to be generated.
  //   beam_width: Beam width.
  //   word_penalty: Word penalty.
  //
  // Returns:
  //   Inference graph representing the decoder results.
  InferenceGraph infer(
      const std::vector<unsigned> & source_ids,
      const unsigned source_bos_id,
      const unsigned source_eos_id,
      const unsigned target_bos_id,
      const unsigned target_eos_id,
      const unsigned max_length,
      const unsigned beam_width,
      const float word_penalty);

  // Forcely decode given sentence.
  //
  // Arguments:
  //   source_ids: List of source word IDs.
  //   target_ids: List of target word IDs.
  //   source_bos_id: "<s>" ID in the source language.
  //   source_eos_id: "</s>" ID in the source language.
  //   target_bos_id: "<s>" ID in the target language.
  //   target_eos_id: "</s>" ID in the target language.
  //
  // Returns:
  //   Inference graph representing the decoder results.
  InferenceGraph forceDecode(
      const std::vector<unsigned> & source_ids,
      const std::vector<unsigned> & target_ids,
      const unsigned source_bos_id,
      const unsigned source_eos_id,
      const unsigned target_bos_id,
      const unsigned target_eos_id);

  // Sample a target sentence.
  //
  // Arguments:
  //   source_ids: List of source word IDs.
  //   source_bos_id: "<s>" ID in the source language.
  //   source_eos_id: "</s>" ID in the source language.
  //   target_bos_id: "<s>" ID in the target language.
  //   target_eos_id: "</s>" ID in the target language.
  //
  // Returns:
  //   Inference graph representing the decoder results.
  InferenceGraph sample(
      const std::vector<unsigned> & source_ids,
      const unsigned source_bos_id,
      const unsigned source_eos_id,
      const unsigned target_bos_id,
      const unsigned target_eos_id);

private:
  // Generates output sequence using encoder results.
  //
  // Arguments:
  //   bos_id: "<s>" ID in the target language.
  //   eos_id: "</s>" ID in the target language.
  //   max_length: Maximum number of words (except "<s>") to be generated.
  //   beam_width: Beam width.
  //   word_penalty: Word penalty.
  //   cg: Computation graph.
  //
  // Returns:
  //   Inference graph representing the decoder results.
  InferenceGraph beamSearch(
      const unsigned bos_id,
      const unsigned eos_id,
      const unsigned max_length,
      const unsigned beam_width,
      const float word_penalty,
      dynet::ComputationGraph * cg);

  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {
    ar & mean_by_samples_;
    ar & encoder_;
    ar & decoder_;
    ar & attention_;
    ar & predictor_;
  }

  bool mean_by_samples_;
  std::shared_ptr<Encoder> encoder_;
  std::shared_ptr<Decoder> decoder_;
  std::shared_ptr<Attention> attention_;
  std::shared_ptr<Predictor> predictor_;
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::EncoderDecoder);

#endif  // NMTKIT_ENCODER_DECODER_H_
