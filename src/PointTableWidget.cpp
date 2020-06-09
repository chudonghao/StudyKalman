//
// Created by chudonghao on 2020/6/6.
//

#include "PointTableWidget.h"
#include <utility>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QDebug>

PointTableWidget::PointTableWidget(QWidget *parent) : QWidget(parent) {
  setAutoFillBackground(true);
  setBackgroundRole(QPalette::ColorRole::Dark);

  startTimer(100);
}
void PointTableWidget::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  QStyleOption opt;
  opt.init(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

  // setup painter
  p.setRenderHint(QPainter::RenderHint::Antialiasing);

  // draw object
  p.setPen(Qt::white);
  p.setBrush(Qt::green);
  QRectF o;
  o.setSize({10, 10});
  o.moveCenter(m_objectPos);
  p.drawEllipse(o);

  // draw sequences
  for (const auto &pair : m_sequences) {
    auto &color = pair.second.color;
    auto &pointList = pair.second.pointList;
    if (pointList.empty()) {
      continue;
    }

    QPen pathPen(color);
    QPen pointPen(color);
    pointPen.setCapStyle(Qt::PenCapStyle::RoundCap);
    pointPen.setWidth(5);
    drawPointList(p, pathPen, pointPen, pointList);
  }

  // draw object pos & v
  p.setPen(Qt::white);
  p.setBrush(Qt::white);
  p.drawText(rect(), Qt::AlignLeft | Qt::AlignBottom,
             QString("P: %1,%2 V: %3,%4").arg(m_objectPos.x()).arg(m_objectPos.y()).arg(m_objectV.x()).arg(m_objectV.y()));
}
void PointTableWidget::timerEvent(QTimerEvent *event) {
  QPointF newV;
  if (m_lastObjectPos == m_objectPos) {
    newV = {};
  } else {
    auto dp = m_objectPos - m_lastObjectPos;
    float dt = 100.f / 1000;
    newV = QPointF(dp.x() / dt, dp.y() / dt);
  }
  if (m_objectV != newV) { update(); }
  m_objectV = newV;
  m_lastObjectPos = m_objectPos;
}
void PointTableWidget::drawPointList(QPainter &p, QPen pathPen, QPen pointPen, const std::vector<PointInfo> &pointList) {
  if (pointList.empty()) { return; }
  p.save();
  p.setBrush(Qt::NoBrush);
  // draw path
  QPainterPath pp;
  pp.moveTo(pointList.front().pos);
  for (const auto &pointInfo : pointList) {
    pp.lineTo(pointInfo.pos);
  }
  p.setPen(pathPen);
  p.drawPath(pp);

  // draw key points and time
  p.setPen(pointPen);
  QPointF lastTextPos = {-100, -100};
  for (const auto &pointInfo : pointList) {
    auto pos = pointInfo.pos;
    auto time = pointInfo.time;
    QLineF line(lastTextPos, pos);
    auto length = line.length();
    if (length > 20) {
      p.drawText(pos, QString::number(time));
      lastTextPos = pos;
    }
    p.drawPoint(pos);
  }
  p.restore();
}
void PointTableWidget::createSequence(int id, QColor color) {
  m_sequences[id].color = std::move(color);
  update();
}
void PointTableWidget::destroySequence(int id) {
  m_sequences.erase(id);
  update();
}
void PointTableWidget::addSequencePoint(int id, PointInfo pointInfo) {
  auto i = m_sequences.find(id);
  if (i != m_sequences.end()) {
    auto &sequence = i->second;
    auto &pointList = sequence.pointList;
    if (!pointList.empty()) {
      QLineF line(pointList.back().pos, pointInfo.pos);
      if (line.length() <= 1) {
        return;
      }
    }
    pointList.push_back(pointInfo);
    update();
  }
}
void PointTableWidget::clearSequences() {
  for (auto &pair : m_sequences) {
    auto &s = pair.second;
    s.pointList.clear();
  }
  update();
}
void PointTableWidget::moveObject(QPointF pos) {
  QPointF topLeft = rect().topLeft();
  QPointF bottomRight = rect().bottomRight();
  pos.setX(std::min(bottomRight.x(), pos.x()));
  pos.setX(std::max(topLeft.x(), pos.x()));
  pos.setY(std::min(bottomRight.y(), pos.y()));
  pos.setY(std::max(topLeft.y(), pos.y()));
  m_objectPos = pos;
  update();
}
QPointF PointTableWidget::objectPos() const {
  return m_objectPos;
}
QPointF PointTableWidget::objectV() const {
  return m_objectV;
}
