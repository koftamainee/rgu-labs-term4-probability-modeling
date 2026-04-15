#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "random_walk_controller.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  RandomWalkController controller;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("walk_controller", &controller);
  engine.load(QUrl(QStringLiteral("../qml/Main.qml")));
  if (engine.rootObjects().isEmpty()) return -1;
  return app.exec();
}