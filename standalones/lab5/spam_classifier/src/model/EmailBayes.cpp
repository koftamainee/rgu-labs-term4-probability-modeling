//
// Created by koftamainee on 3/16/26.
//

#include "EmailBayes.h"

#include <cmath>

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace model {
EmailBayes::EmailBayes(double alpha) : m_alpha(alpha) {}

static void write_str(std::ofstream& out, const std::string& s) {
    size_t len = s.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.data(), static_cast<long>(len));
}

static std::string read_str(std::ifstream& in) {
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string s(len, '\0');
    in.read(s.data(), static_cast<long>(len));
    return s;
}

void EmailBayes::save(const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Cannot open file to save model");

    out.write(reinterpret_cast<const char*>(&m_alpha),            sizeof(m_alpha));
    out.write(reinterpret_cast<const char*>(&m_total_spam_words), sizeof(m_total_spam_words));
    out.write(reinterpret_cast<const char*>(&m_total_ham_words),  sizeof(m_total_ham_words));
    out.write(reinterpret_cast<const char*>(&m_prior_spam),       sizeof(m_prior_spam));
    out.write(reinterpret_cast<const char*>(&m_prior_ham),        sizeof(m_prior_ham));

    size_t V = m_vocabulary.size();
    out.write(reinterpret_cast<const char*>(&V), sizeof(V));

    for (const auto& w : m_vocabulary) {
        write_str(out, w);
        double l_spam = m_likelihood_spam.at(w);
        double l_ham  = m_likelihood_ham.at(w);
        out.write(reinterpret_cast<const char*>(&l_spam), sizeof(l_spam));
        out.write(reinterpret_cast<const char*>(&l_ham),  sizeof(l_ham));
    }
}

void EmailBayes::load(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Cannot open file to load model");

    m_vocabulary.clear();
    m_likelihood_spam.clear();
    m_likelihood_ham.clear();

    in.read(reinterpret_cast<char*>(&m_alpha),            sizeof(m_alpha));
    in.read(reinterpret_cast<char*>(&m_total_spam_words), sizeof(m_total_spam_words));
    in.read(reinterpret_cast<char*>(&m_total_ham_words),  sizeof(m_total_ham_words));
    in.read(reinterpret_cast<char*>(&m_prior_spam),       sizeof(m_prior_spam));
    in.read(reinterpret_cast<char*>(&m_prior_ham),        sizeof(m_prior_ham));

    size_t V;
    in.read(reinterpret_cast<char*>(&V), sizeof(V));

    for (size_t i = 0; i < V; ++i) {
        std::string word = read_str(in);
        double l_spam, l_ham;
        in.read(reinterpret_cast<char*>(&l_spam), sizeof(l_spam));
        in.read(reinterpret_cast<char*>(&l_ham),  sizeof(l_ham));
        m_vocabulary.insert(word);
        m_likelihood_spam[word] = l_spam;
        m_likelihood_ham[word]  = l_ham;
    }
}

void EmailBayes::fit(
    const data::IDataSet<std::vector<std::string>, bool>& dataset) {
  size_t spam_docs_count = 0;
  size_t ham_docs_count = 0;
  std::unordered_map<std::string, size_t> spam_count;
  std::unordered_map<std::string, size_t> ham_count;

  for (size_t i = 0; i < dataset.size(); i++) {
    auto [words, is_spam] = dataset[i];
    if (is_spam) {
      spam_docs_count++;
    } else {
      ham_docs_count++;
    }

    for (const auto& word : words) {
      m_vocabulary.insert(word);
      if (is_spam) {
        m_total_spam_words++;
        spam_count[word]++;
      } else {
        m_total_ham_words++;
        ham_count[word]++;
      }
    }
  }

  m_prior_spam = static_cast<double>(spam_docs_count) / static_cast<double>(
                   dataset.size());
  m_prior_ham = static_cast<double>(ham_docs_count) / static_cast<double>(
                  dataset.size());

  const auto v = static_cast<double>(m_vocabulary.size());

  for (const auto& word : m_vocabulary) {
    m_likelihood_spam[word] = (static_cast<double>(spam_count[word]) + m_alpha)
                              / (static_cast<double>(m_total_spam_words) +
                                 m_alpha
                                 * v);
    m_likelihood_ham[word] = (static_cast<double>(ham_count[word]) + m_alpha)
                             / (static_cast<double>(m_total_ham_words) + m_alpha
                                *
                                v);
  }
}

bool EmailBayes::predict(const std::vector<std::string>& input) const {
  auto result = predict_log_probabilities(input);
  const double is_spam = result[true];
  const double is_ham = result[false];

  return is_spam > is_ham;
}

std::unordered_map<bool, double> EmailBayes::predict_log_probabilities(
    const std::vector<std::string>& input) const {
  double log_spam = std::log(m_prior_spam);
  double log_ham = std::log(m_prior_ham);
  const auto v = static_cast<double>(m_vocabulary.size());

  for (const auto& word : input) {
    const double p_spam = m_likelihood_spam.contains(word)
                            ? m_likelihood_spam.at(word)
                            : m_alpha / (
                                static_cast<double>(m_total_spam_words) +
                                m_alpha * v);
    log_spam += std::log(p_spam);
    const double p_ham = m_likelihood_ham.contains(word)
                           ? m_likelihood_ham.at(word)
                           : m_alpha / (
                               static_cast<double>(m_total_ham_words) + m_alpha
                               * v);
    log_ham += std::log(p_ham);
  }

  return {{true, log_spam}, {false, log_ham}};
}

std::unordered_map<bool, double> EmailBayes::predict_probabilities(
    const std::vector<std::string>& input) const {
  const auto log_scores = predict_log_probabilities(input);

  const double log_spam = log_scores.at(true);
  const double log_ham = log_scores.at(false);

  const double max_log = std::max(log_spam, log_ham);

  double spam = std::exp(log_spam - max_log);
  double ham = std::exp(log_ham - max_log);

  const double sum = spam + ham;

  spam /= sum;
  ham /= sum;

  return {{true, spam}, {false, ham}};
}
}