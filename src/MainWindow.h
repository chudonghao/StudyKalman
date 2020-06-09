//
// Created by chudonghao on 2020/6/6.
//

#ifndef STUDYKALMAN_SRC_MAINWINDOW_H_
#define STUDYKALMAN_SRC_MAINWINDOW_H_

#include <QMainWindow>
#include <QTime>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow {
 Q_OBJECT;
 public:
  MainWindow();
 private:
  using Self = MainWindow;
  using Ui = ::Ui::MainWindow;

  void timerEvent(QTimerEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;

  void start();
  void stop();
  void reset();
  void setAutoMove(bool autoMove);
  void resetButtonState(bool running);
  void updateTimeEdit();
  void moveObject() const;
 private:
  Ui *ui;
  QTime m_simulateStartTime;
  int m_moveObjectTimer{};
  int m_recordTruePosTimer{};
  int m_detectTimer{};
  int m_predictTimer{};
  QTimer *m_simulateTimer;
  bool m_autoMove{true};
  bool m_autoDetect{true};
};

#endif //STUDYKALMAN_SRC_MAINWINDOW_H_
