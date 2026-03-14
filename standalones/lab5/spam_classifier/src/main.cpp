#include <iostream>

#include "data/EmailDataLoader.h"
#include "data/RandomSplit.h"
#include "metrics/ClassificationReport.h"
#include "model/EmailBayes.h"
#include "model/IClassifier.h"
#include "transforms/Sequential.h"
#include "transforms/Lowercase.h"
#include "transforms/Pipe.h"
#include "transforms/RemoveStopWords.h"
#include "transforms/StripPunctuation.h"
#include "transforms/Tokenizer.h"

int main() {
  std::unordered_set<std::string> stop_words; //TODO: load stop words

  auto email_pipeline = std::make_unique<transform::Tokenizer>();
  auto words_pipeline = transform::make_sequential(
      std::make_unique<transform::Lowercase>(),
      std::make_unique<transform::StripPunctuation>(),
      std::make_unique<transform::RemoveStopWords>(stop_words)
      );
  const auto pipeline = transform::make_pipe(
      std::move(email_pipeline),
      std::move(words_pipeline)
      );

  const auto dataset = data::EmailDataLoader().load(
      "/path/to/dataset/", *pipeline);
  auto [train_data, test_data] = data::random_split(dataset, 0.7);

  auto model = model::EmailBayes(1.0);
  model.fit(train_data);

  const auto cm = metrics::make_confusion_matrix(model, test_data);
  auto report = metrics::make_classification_report(cm);

  report.print();

  const auto& spam = report.per_class[true];
  std::cout << "Spam precision : " << spam.precision << "\n"
      << "Spam recall    : " << spam.recall << "\n"
      << "Spam F1        : " << spam.f1 << "\n"
      << "Accuracy       : " << report.accuracy << "\n";

  return 0;
}