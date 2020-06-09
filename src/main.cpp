//
// Created by chudonghao on 2020/6/6.
//

#include <kalman/LinearizedSystemModel.hpp>
#include <QtWidgets/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MainWindow m;
  m.show();
  return QApplication::exec();
}
