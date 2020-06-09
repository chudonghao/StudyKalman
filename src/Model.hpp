//
// Created by chudonghao on 2020/6/6.
//

#include <kalman/SystemModel.hpp>
#include <kalman/MeasurementModel.hpp>

class State : public Kalman::Vector<double, 6> {
 public:
  KALMAN_VECTOR(State, double, 6)
  static constexpr size_t X = 0;
  static constexpr size_t Y = 1;
  static constexpr size_t VX = 2;
  static constexpr size_t VY = 3;
  static constexpr size_t AX = 4;
  static constexpr size_t AY = 5;

  Scalar &vx() { return (*this)[VX]; }
  Scalar &vy() { return (*this)[VY]; }
  Scalar &ax() { return (*this)[AX]; }
  Scalar &ay() { return (*this)[AY]; }

  Scalar vx() const { return (*this)[VX]; }
  Scalar vy() const { return (*this)[VY]; }
  Scalar ax() const { return (*this)[AX]; }
  Scalar ay() const { return (*this)[AY]; }
};

class PosMeasurement : public Kalman::Vector<double, 2> {
 public:
  KALMAN_VECTOR(PosMeasurement, double, 2)
  static constexpr size_t X = 0;
  static constexpr size_t Y = 1;
};

class PosMeasurementModel : public Kalman::LinearizedMeasurementModel<State, PosMeasurement> {
 public:
  typedef State S;
  typedef Measurement M;

  PosMeasurementModel() {
    this->H.setZero();
    this->H(M::X, S::X) = 1;
    this->H(M::Y, S::Y) = 1;

    this->V.setZero();
    this->V(M::X, M::X) = 100;
    this->V(M::Y, M::Y) = 100;
  }

  M h(const S &x) const override {
    M m;
    m.x() = x.x();
    m.y() = x.y();
    return m;
  }
};

class VMeasurement : public Kalman::Vector<double, 2> {
 public:
  KALMAN_VECTOR(VMeasurement, double, 2)
  static constexpr size_t VX = 0;
  static constexpr size_t VY = 1;

  Scalar &vx() { return (*this)[VX]; }
  Scalar &vy() { return (*this)[VY]; }
  Scalar vx() const { return (*this)[VX]; }
  Scalar vy() const { return (*this)[VY]; }
};

class VMeasurementModel : public Kalman::LinearizedMeasurementModel<State, VMeasurement> {
 public:
  typedef State S;
  typedef Measurement M;

  VMeasurementModel() {
    this->H.setZero();
    this->H(M::VX, S::VX) = 1;
    this->H(M::VY, S::VY) = 1;

    this->V.setZero();
    this->V(M::VX, M::VX) = 10;
    this->V(M::VY, M::VY) = 10;
  }

  M h(const S &x) const override {
    M m;
    m.vx() = x.vx();
    m.vy() = x.vy();
    return m;
  }
};

class SystemModel : public Kalman::LinearizedSystemModel<State> {
 public:
  //! State type shortcut definition
  typedef State S;
  typedef Control C;

  SystemModel() {

    this->F.setZero();
    this->F(S::X, S::X) = 1;
    this->F(S::Y, S::Y) = 1;

    this->F(S::X, S::VX) = 1;
    this->F(S::Y, S::VY) = 1;

    this->F(S::VX, S::VX) = 1;
    this->F(S::VY, S::VY) = 1;

    this->F(S::VX, S::AX) = 1;
    this->F(S::VY, S::AY) = 1;

    this->F(S::AX, S::AX) = 1;
    this->F(S::AY, S::AY) = 1;

    this->P.setIdentity();

    /*x vx*/
    this->P(S::X, S::VX) = 1;
    this->P(S::VX, S::X) = 1;
    /*y vy*/
    this->P(S::Y, S::VY) = 1;
    this->P(S::VY, S::Y) = 1;

    /*ax vx*/
    this->P(S::AX, S::VX) = 1;
    this->P(S::VX, S::AX) = 1;
    /*ay vy*/
    this->P(S::AY, S::VY) = 1;
    this->P(S::VY, S::AY) = 1;

    this->W.setIdentity();
  }

  S f(const S &x, const C &u) const override {
    S x_;
    x_.x() = x.x() + x.vx();
    x_.y() = x.y() + x.vy();
    x_.vx() = x.vx() + x.ax();
    x_.vy() = x.vy() + x.ay();
    x_.ax() = x.ax();
    x_.ay() = x.ay();
    return x_;
  }
};

