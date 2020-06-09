//
// Created by chudonghao on 2020/6/6.
//

#ifndef STUDYKALMAN_SRC_POINTTABLEWIDGET_H_
#define STUDYKALMAN_SRC_POINTTABLEWIDGET_H_

#include <QWidget>
#include <QTime>

struct PointInfo {
  QPointF pos;
  // ms; time from first point
  int time;
};

class PointTableWidget : public QWidget {
 Q_OBJECT;
 public:
  explicit PointTableWidget(QWidget *parent);

  void clearSequences();
  void createSequence(int id, QColor color);
  void destroySequence(int id);
  void addSequencePoint(int id, PointInfo);

  void moveObject(QPointF pos);
  QPointF objectPos() const;
  QPointF objectV() const;
 private:
  struct Sequence {
    int id;
    QColor color;
    std::vector<PointInfo> pointList;
  };
  void paintEvent(QPaintEvent *event) override;
  void timerEvent(QTimerEvent *event) override;

  void drawPointList(QPainter &p, QPen pathPen, QPen pointPen, const std::vector<PointInfo> &pointList);
 private:
  std::map<int, Sequence> m_sequences;
  /*for cal v*/
  QPointF m_lastObjectPos;
  QPointF m_objectPos;
  QPointF m_objectV;
};

#endif //STUDYKALMAN_SRC_POINTTABLEWIDGET_H_
