#pragma once
#include <QFutureWatcher>
#include <QObject>
#include <QVariantList>
#include <QVector>
#include <memory>
#include "drunk_walk.hpp"

struct CurvePoint { double p; double cliff_prob; };

class DrunkWalkController : public QObject {
    Q_OBJECT

    Q_PROPERTY(double p_forward        READ get_p_forward        NOTIFY params_changed)
    Q_PROPERTY(int    cafe_pos         READ get_cafe_pos          NOTIFY params_changed)
    Q_PROPERTY(int    cliff_pos        READ get_cliff_pos         NOTIFY params_changed)

    Q_PROPERTY(int    current_position READ get_current_position  NOTIFY walk_updated)
    Q_PROPERTY(int    steps_taken      READ get_steps_taken       NOTIFY walk_updated)
    Q_PROPERTY(bool   walk_finished    READ is_walk_finished      NOTIFY walk_updated)
    Q_PROPERTY(bool   fell_off_cliff   READ get_fell_off_cliff    NOTIFY walk_updated)

    Q_PROPERTY(double cliff_probability READ get_cliff_probability NOTIFY batch_done)
    Q_PROPERTY(double cafe_probability  READ get_cafe_probability  NOTIFY batch_done)
    Q_PROPERTY(double avg_steps         READ get_avg_steps         NOTIFY batch_done)
    Q_PROPERTY(int    batch_n           READ get_batch_n           NOTIFY batch_done)
    Q_PROPERTY(bool   batch_running     READ is_batch_running      NOTIFY batch_running_changed)

public:
    explicit DrunkWalkController(QObject* parent = nullptr);

    Q_INVOKABLE void run_batch(int n);
    Q_INVOKABLE void build_curve(int n_runs, int steps);

    Q_INVOKABLE void reset_walk();
    Q_INVOKABLE void step_walk();

    Q_INVOKABLE void set_p_forward(double p);
    Q_INVOKABLE void set_cafe_pos(int b);

    double get_p_forward()        const;
    int    get_cafe_pos()         const;
    int    get_cliff_pos()        const;
    int    get_current_position() const;
    int    get_steps_taken()      const;
    bool   is_walk_finished()     const;
    bool   get_fell_off_cliff()   const;
    double get_cliff_probability()const;
    double get_cafe_probability() const;
    double get_avg_steps()        const;
    int    get_batch_n()          const;
    bool   is_batch_running()     const;

signals:
    void params_changed();
    void walk_updated();
    void batch_done();
    void batch_running_changed();
    void curve_ready(QVariantList data);

private:
    void rebuild_sim();

    double m_p_forward  = 0.4;
    int    m_cafe_pos   = 5;
    static constexpr int CLIFF_POS = 0;

    std::unique_ptr<DrunkWalkSimulation> m_sim;

    int    m_current_pos   = 5;
    int    m_steps         = 0;
    bool   m_finished      = false;
    bool   m_cliff         = false;

    double m_cliff_prob    = 0.0;
    double m_cafe_prob     = 0.0;
    double m_avg_steps     = 0.0;
    int    m_batch_n       = 0;
    bool   m_batch_running = false;
};
