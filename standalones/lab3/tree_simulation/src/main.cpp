#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <iostream>

#include "TreeController.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    TreeController tree_controller;
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("treeCtrl", &tree_controller);

    engine.load(QUrl(QStringLiteral("../../qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        std::cout << "rootObjects is empty!" << std::endl;
        return -1;
    }

    return app.exec();
}