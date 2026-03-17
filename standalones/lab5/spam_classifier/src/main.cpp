#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include "Application.h"
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

namespace {
std::atomic<bool> spinner_running{false};

void spin(const std::string& msg) {
  spinner_running = true;
  std::thread([msg]() {
    int i = 0;
    while (spinner_running) {
      const char* frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
      std::cout << "\r" << frames[i % 10] << "  " << msg << "   " << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(80));
      ++i;
    }
  }).detach();
}

void stop_spin(const std::string& done_msg) {
  spinner_running = false;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << "\r" << done_msg << "\n" << std::flush;
}
}

int main(int argc, char* argv[]) {
  try {
    const auto config = Configuration::load(argc, argv);
    if (!config.is_complete()) {
      print_help();
      return 1;
    }

    spin("Loading stop words...");
    std::unordered_set<std::string> stop_words = load_stop_words_from_csv(
        config.stop_words_path);
    stop_spin("Stop words loaded");

    spin("Building pipeline...");
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
    stop_spin("Transform pipeline ready");

    if (config.mode == Configuration::Mode::Train) {
      spin("Loading dataset...");
      const auto dataset = data::EmailDataLoader().load(
          config.data_path, *pipeline);
      stop_spin(
          "Dataset loaded (" + std::to_string(dataset->size()) + " samples)");

      spin("Splitting into train/test sets...");
      auto [train_data, test_data] = data::random_split(dataset, 0.7);
      stop_spin(
          "Split done -- train: " + std::to_string(train_data.size()) +
          ", test: " + std::to_string(test_data.size()));

        spin("Training model...");
        auto model = model::EmailBayes(config.alpha);
        model.fit(train_data);
        stop_spin("Model trained       ");

        spin("Evaluating on test set...");
        const auto cm = metrics::make_confusion_matrix(model, test_data);
        auto report = metrics::make_classification_report(cm);
        stop_spin("Evaluation complete");

        std::cout << "\n";
        report.print();
        std::cout << "\n";

        spin("Saving model...");
        model.save(config.model_path);
        stop_spin(
            "Model saved to " + config.model_path);
    } else {
      spin("Loading model...");
      auto model = model::EmailBayes(config.alpha);
      model.load(config.model_path);
      stop_spin("Model loaded from " + config.model_path);

      spin("Loading emails...");
      auto emails = load_emails_from_csv(config.data_path);
      stop_spin("Loaded " + std::to_string(emails.size()) + " emails");

      spin("Running predictions...");
      std::vector<bool> is_spam(emails.size());
      for (int i = 0; i < emails.size(); i++) {
        auto transformed_email = (*pipeline)(emails[i]);
        is_spam[i] = model.predict(transformed_email);
      }
      stop_spin("Predictions done");

      spin("Writing results...");
      write_results_to_csv(config.output_path, is_spam);
      stop_spin("Results written to " + config.output_path);
    }
  } catch (std::exception& e) {
    spinner_running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cerr << "\nError: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}