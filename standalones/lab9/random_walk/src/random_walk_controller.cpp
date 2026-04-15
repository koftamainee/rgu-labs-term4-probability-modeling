#include "random_walk_controller.hpp"
#include <QFileDialog>
#include <QDebug>
#include <QtConcurrent>

RandomWalkController::RandomWalkController(QObject* parent) : QObject(parent) {
    m_step_timer = new QTimer(this);
    m_step_timer->setInterval(500);
    connect(m_step_timer, &QTimer::timeout, this, &RandomWalkController::onStepTimer);
    m_step_dist = DiscreteRandomVariable({{1.0, 1.0}});
    updateSimulation();
}

void RandomWalkController::loadDistributionFromFile() {
  qDebug() << QThread::currentThread();
  qDebug() << qApp->thread();
    QString path = QFileDialog::getOpenFileName(nullptr, "Load Step Distribution", "", "JSON Files (*.json);;All Files (*)");
    if (path.isEmpty()) return;
    try {
        m_step_dist = DiscreteRandomVariable::load_from_file(path.toStdString());
        emit step_distribution_changed();
        updateSimulation();
        computeDistribution();
    } catch (const std::exception& e) {
        qWarning() << "Failed to load distribution:" << e.what();
    }
}

void RandomWalkController::startSimulation() {
    if (m_simulation_running) return;
    if (m_simulation_finished) updateSimulation();
    m_simulation_running = true;
    m_simulation_finished = false;
    emit simulation_running_changed();
    emit simulation_finished_changed();
    m_step_timer->start();
}

void RandomWalkController::stopSimulation() {
    if (!m_simulation_running) return;
    m_step_timer->stop();
    m_simulation_running = false;
    emit simulation_running_changed();
}

void RandomWalkController::resetSimulation() {
    if (m_simulation_running) stopSimulation();
    updateSimulation();
}

void RandomWalkController::computeDistribution() {
    if (m_total_steps <= 0) return;
    double startPos = m_start_position;
    DiscreteRandomVariable dist = m_step_dist;
    int steps = m_total_steps;
    QtConcurrent::run([this, dist, steps, startPos]() {
        DiscreteRandomVariable sum = dist;
        for (int i = 1; i < steps; ++i) sum = sum + dist;
        auto pmf = sum.pmf();
        std::vector<std::pair<double, double>> shifted;
        for (const auto& [val, prob] : pmf)
            shifted.emplace_back(val + startPos, prob);
        m_final_distribution = DiscreteRandomVariable(shifted);
        emit distribution_ready();
    });
}

double RandomWalkController::start_position() const { return m_start_position; }
void RandomWalkController::set_start_position(double pos) {
    if (qFuzzyCompare(m_start_position, pos)) return;
    m_start_position = pos;
    emit start_position_changed();
    updateSimulation();
    computeDistribution();
}

int RandomWalkController::total_steps() const { return m_total_steps; }
void RandomWalkController::set_total_steps(int n) {
    if (m_total_steps == n) return;
    m_total_steps = n;
    emit total_steps_changed();
    updateSimulation();
    computeDistribution();
}

double RandomWalkController::current_position() const { return m_current_position; }
int RandomWalkController::steps_done() const { return m_steps_done; }
bool RandomWalkController::simulation_running() const { return m_simulation_running; }
bool RandomWalkController::simulation_finished() const { return m_simulation_finished; }

QString RandomWalkController::step_distribution_info() const {
    auto pmf = m_step_dist.pmf();
    if (pmf.empty()) return "None";
    QString info;
    for (const auto& [val, prob] : pmf)
        info += QString("x=%1 p=%2 ").arg(val).arg(prob, 0, 'f', 3);
    return info;
}

QVariantList RandomWalkController::finalDistributionPMF() const {
    QVariantList list;
    for (const auto& [val, prob] : m_final_distribution.pmf()) {
        QVariantMap point;
        point["value"] = val;
        point["probability"] = prob;
        list.append(point);
    }
    return list;
}

void RandomWalkController::onStepTimer() {
    if (!m_sim || m_sim->is_finished()) {
        stopSimulation();
        m_simulation_finished = true;
        emit simulation_finished_changed();
        return;
    }
    double old_pos = m_sim->current_position();
    m_sim->step();
    double new_pos = m_sim->current_position();
    m_current_position = new_pos;
    m_steps_done = m_sim->steps_taken();
    emit position_changed();
    emit steps_done_changed();
    emit step_performed(new_pos, new_pos - old_pos);
    if (m_sim->is_finished()) {
        stopSimulation();
        m_simulation_finished = true;
        emit simulation_finished_changed();
    }
}

void RandomWalkController::updateSimulation() {
    m_sim = std::make_unique<RandomWalkSimulation>(m_step_dist, m_start_position, m_total_steps);
    m_current_position = m_start_position;
    m_steps_done = 0;
    m_simulation_finished = false;
    emit position_changed();
    emit steps_done_changed();
    emit simulation_finished_changed();
    emit step_performed(m_current_position, 0);
}