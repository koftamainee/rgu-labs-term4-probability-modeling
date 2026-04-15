#pragma once
#include <QObject>
#include <QTimer>
#include <QFutureWatcher>
#include "discrete_random_variable.h"
#include "random_walk_simulation.hpp"

class RandomWalkController : public QObject {
    Q_OBJECT
    Q_PROPERTY(double start_position READ start_position WRITE set_start_position NOTIFY start_position_changed)
    Q_PROPERTY(int total_steps READ total_steps WRITE set_total_steps NOTIFY total_steps_changed)
    Q_PROPERTY(double current_position READ current_position NOTIFY position_changed)
    Q_PROPERTY(int steps_done READ steps_done NOTIFY steps_done_changed)
    Q_PROPERTY(bool simulation_running READ simulation_running NOTIFY simulation_running_changed)
    Q_PROPERTY(bool simulation_finished READ simulation_finished NOTIFY simulation_finished_changed)
    Q_PROPERTY(QString step_distribution_info READ step_distribution_info NOTIFY step_distribution_changed)

public:
    explicit RandomWalkController(QObject* parent = nullptr);
    Q_INVOKABLE void loadDistributionFromFile();
    Q_INVOKABLE void startSimulation();
    Q_INVOKABLE void stopSimulation();
    Q_INVOKABLE void resetSimulation();
    Q_INVOKABLE void computeDistribution();
    double start_position() const;
    void set_start_position(double pos);
    int total_steps() const;
    void set_total_steps(int n);
    double current_position() const;
    int steps_done() const;
    bool simulation_running() const;
    bool simulation_finished() const;
    QString step_distribution_info() const;
    Q_INVOKABLE QVariantList finalDistributionPMF() const;

signals:
    void start_position_changed();
    void total_steps_changed();
    void position_changed();
    void steps_done_changed();
    void step_performed(double new_pos, double step_value);
    void simulation_running_changed();
    void simulation_finished_changed();
    void step_distribution_changed();
    void distribution_ready();

private slots:
    void onStepTimer();

private:
    void updateSimulation();
    double m_start_position = 0.0;
    int m_total_steps = 10;
    double m_current_position = 0.0;
    int m_steps_done = 0;
    bool m_simulation_running = false;
    bool m_simulation_finished = false;
    DiscreteRandomVariable m_step_dist;
    std::unique_ptr<RandomWalkSimulation> m_sim;
    QTimer* m_step_timer;
    DiscreteRandomVariable m_final_distribution;
};