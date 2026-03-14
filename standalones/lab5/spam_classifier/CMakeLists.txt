cmake_minimum_required(VERSION 4.1)
project(spam_classifier)

set(CMAKE_CXX_STANDARD 23)

add_executable(spam_classifier
        src/main.cpp
        src/data/LabledSample.h
        src/transforms/ITransform.h
        src/transforms/Lowercase.h
        src/transforms/Tokenizer.h
        src/transforms/StripPunctuation.h
        src/transforms/RemoveStopWords.h
        src/transforms/Sequential.h
        src/data/IDataSet.h
        src/data/EmailDataSet.h
        src/data/IDataLoader.h
        src/data/EmailDataLoader.h
        src/data/RandomSplit.h
        src/data/Subset.h
        src/transforms/Pipe.h
        src/model/IModel.h
        src/model/IClassifier.h
        src/model/EmailBayes.h
        src/metrics/ConfusionMatrix.h
        src/metrics/ClassificationReport.h
)

target_include_directories(spam_classifier PRIVATE src/)