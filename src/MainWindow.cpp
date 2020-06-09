//
// Created by chudonghao on 2020/6/6.
//

#include "MainWindow.h"

#include <random>
#include <QDebug>
#include <QTimer>
#include <QtMath>
#include <QMouseEvent>

#include "PointTableWidget.h"
#include "ui_MainWindow.h"

#include <kalman/ExtendedKalmanFilter.hpp>
#include <kalman/SquareRootExtendedKalmanFilter.hpp>
#include "Model.hpp"

static State state;
static SystemModel model;
static PosMeasurementModel posMeasurementModel;
static VMeasurementModel vMeasurementModel;
static Kalman::ExtendedKalmanFilter<State> ekf;
static Kalman::SquareRootExtendedKalmanFilter<State> srekf;

const static int TRUE_SEQUENCE = 0;
const static int DETECT_SEQUENCE = 1;
const static int EKF_SEQUENCE = 2;
const static int SREKF_SEQUENCE = 3;

static int UPDATE_T = 500;
static int PREDICT_T = 100;

MainWindow::MainWindow() {
  ui = new Ui;
  ui->setupUi(this);
  ui->point_table->createSequence(TRUE_SEQUENCE, Qt::green);
  ui->point_table->createSequence(DETECT_SEQUENCE, Qt::blue);
  ui->point_table->createSequence(EKF_SEQUENCE, Qt::red);
  ui->point_table->createSequence(SREKF_SEQUENCE, Qt::darkRed);

  connect(ui->start_button, &QPushButton::clicked, this, &Self::start);
  connect(ui->stop_button, &QPushButton::clicked, this, &Self::stop);
  connect(ui->reset_button, &QPushButton::clicked, this, &Self::reset);

  connect(ui->auto_move_check_box, &QCheckBox::toggled, this, &Self::setAutoMove);
  connect(ui->update_t_spin_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [](int v) {
    UPDATE_T = v;
  });
  connect(ui->predict_t_spin_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [](int v) {
    PREDICT_T = v;
  });
  UPDATE_T = ui->update_t_spin_box->value();
  PREDICT_T = ui->predict_t_spin_box->value();

  m_simulateTimer = new QTimer(this);
  m_simulateTimer->setInterval(0);
  connect(m_simulateTimer, &QTimer::timeout, this, &Self::updateTimeEdit);

  ui->point_table->installEventFilter(this);
}
void MainWindow::timerEvent(QTimerEvent *event) {
  if (m_moveObjectTimer == event->timerId()) {
    moveObject();
  } else if (m_detectTimer == event->timerId()) {
    std::random_device rd;
    std::default_random_engine re(rd());
    std::normal_distribution<float> nd(0, 10);
    QPointF pos = ui->point_table->objectPos();
    QPointF d = QPointF(nd(re), nd(re));
    pos += d;
    ui->point_table->addSequencePoint(DETECT_SEQUENCE, {pos, m_simulateStartTime.msecsTo(QTime::currentTime())});

    // kf
    {
      QPointF v = ui->point_table->objectV();
      VMeasurement m;
      m.vx() = v.x() * (float) PREDICT_T / 1000.f;
      m.vy() = v.y() * (float) PREDICT_T / 1000.f;
      ekf.update(vMeasurementModel, m);
      srekf.update(vMeasurementModel, m);
    }
    {
      PosMeasurement m;
      m.x() = pos.x();
      m.y() = pos.y();
      ekf.update(posMeasurementModel, m);
      srekf.update(posMeasurementModel, m);
    }
  } else if (m_recordTruePosTimer == event->timerId()) {
    ui->point_table->addSequencePoint(TRUE_SEQUENCE, {ui->point_table->objectPos(), m_simulateStartTime.msecsTo(QTime::currentTime())});
  } else if (m_predictTimer == event->timerId()) {

    //kf
    //state = model.f(state);
    State ekf_pre = ekf.predict(model);
    State srekf_pre = srekf.predict(model);
    {
      QPointF pos(ekf_pre.x(), ekf_pre.y());
      ui->point_table->addSequencePoint(EKF_SEQUENCE, {pos, m_simulateStartTime.msecsTo(QTime::currentTime())});
    }
    {
      QPointF pos(srekf_pre.x(), srekf_pre.y());
      ui->point_table->addSequencePoint(SREKF_SEQUENCE, {pos, m_simulateStartTime.msecsTo(QTime::currentTime())});
    }
  }
}
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (watched == ui->point_table) {
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) {
      QMouseEvent *e = static_cast<QMouseEvent *>(event);
      auto pos = e->localPos();
      pos.setY(pos.y() - 10);
      ui->point_table->moveObject(pos);
    }
  }
  return QObject::eventFilter(watched, event);
}
void MainWindow::start() {
  reset();
  m_simulateStartTime = QTime::currentTime();
  m_simulateTimer->start();
  m_recordTruePosTimer = startTimer(30);
  m_predictTimer = startTimer(PREDICT_T);
  if (m_autoMove) {
    m_moveObjectTimer = startTimer(30);
  }
  if (m_autoDetect) {
    m_detectTimer = startTimer(UPDATE_T);
  }
  // manual move
  if (m_autoMove) {
    ui->point_table->removeEventFilter(this);
  }
  resetButtonState(true);
  if(m_autoMove)
  {
    moveObject();
  }
  // kf
  state.x() = ui->point_table->objectPos().x();
  state.y() = ui->point_table->objectPos().y();
  state.vx() = 0;
  state.vy() = 0;
  state.ax() = 0;
  state.ay() = 0;
  // init kf
  ekf.init(state);
  srekf.init(state);
}
void MainWindow::stop() {
  m_simulateTimer->stop();
  if (m_moveObjectTimer) {
    killTimer(m_moveObjectTimer);
    m_moveObjectTimer = 0;
  }
  if (m_detectTimer) {
    killTimer(m_detectTimer);
    m_detectTimer = 0;
  }
  if (m_recordTruePosTimer) {
    killTimer(m_recordTruePosTimer);
    m_recordTruePosTimer = 0;
  }
  if (m_predictTimer) {
    killTimer(m_predictTimer);
    m_predictTimer = 0;
  }
  ui->point_table->installEventFilter(this);
  resetButtonState(false);
}
void MainWindow::reset() {
  stop();
  ui->point_table->clearSequences();
}
void MainWindow::setAutoMove(bool autoMove) {
  m_autoMove = autoMove;
}
void MainWindow::resetButtonState(bool running) {
  ui->variable_container->setEnabled(!running);
  ui->start_button->setEnabled(!running);
  ui->stop_button->setEnabled(running);
}
void MainWindow::updateTimeEdit() {
  int ms = m_simulateStartTime.msecsTo(QTime::currentTime());
  ui->time_edit->setTime(QTime::fromMSecsSinceStartOfDay(ms));
}
void MainWindow::moveObject() const {
  QPointF pos;
  const float T = 20;

  QPointF center = ui->point_table->rect().center();
  float hr = ui->point_table->size().height() / 2.1f;
  float wr = ui->point_table->size().width() / 2.5f;
  float t = m_simulateStartTime.msecsTo(QTime::currentTime()) / 1000.f;

  QLineF line(center, center + QPointF(hr, 0));
  line.setAngle(360.f / T * t);
  pos.setY(line.p2().y());

  line.setLength(wr);
  line.setAngle(360.f / (T / 3) * t);
  pos.setX(line.p2().x());

  ui->point_table->moveObject(pos);
}
