#include "drunk_walk_controller.hpp"
#include <QDebug>
#include <QtConcurrent>

DrunkWalkController::DrunkWalkController(QObject* parent) : QObject(parent) {
    rebuild_sim();
}

void DrunkWalkController::rebuild_sim() {
    m_sim = std::make_unique<DrunkWalkSimulation>(CLIFF_POS, m_cafe_pos, m_p_forward);
    m_current_pos = m_cafe_pos;
    m_steps = 0;
    m_finished = false;
    m_cliff = false;
}

void DrunkWalkController::run_batch(int n) {
    if (n <= 0 || m_batch_running) return;
    m_batch_running = true;
    emit batch_running_changed();

    double p       = m_p_forward;
    int    cafePos = m_cafe_pos;

    struct BatchResult { int n; double cliff_prob; double avg_steps; };

    auto* watcher = new QFutureWatcher<BatchResult>(this);
    connect(watcher, &QFutureWatcher<BatchResult>::finished, this, [=]() {
        BatchResult r = watcher->result();
        m_batch_n     = r.n;
        m_cliff_prob  = r.cliff_prob;
        m_cafe_prob   = 1.0 - r.cliff_prob;
        m_avg_steps   = r.avg_steps;
        m_batch_running = false;
        emit batch_running_changed();
        emit batch_done();
        qDebug() << "Batch" << r.n << "cliff=" << m_cliff_prob;
        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run([=]() -> BatchResult {
        DrunkWalkSimulation sim(CLIFF_POS, cafePos, p);
        auto results = sim.run_batch_bounded(n);
        int cliff_count = 0;
        long long total_steps = 0;
        for (const auto& r2 : results) {
            if (r2.fell_off_cliff) cliff_count++;
            total_steps += r2.steps_taken;
        }
        return { n, (double)cliff_count / n, (double)total_steps / n };
    }));
}

// Plain-old-data point for thread safety

void DrunkWalkController::build_curve(int n_runs, int b_max) {
    // Curve: P(cliff) and P(cafe) as function of B (cafe distance), for fixed p
    if (m_batch_running) return;
    m_batch_running = true;
    emit batch_running_changed();

    double p    = m_p_forward;
    int    nRuns = qBound(200, n_runs, 1000);
    int    bMax  = qBound(5, b_max, 30);

    auto* watcher = new QFutureWatcher<QVector<CurvePoint>>(this);
    connect(watcher, &QFutureWatcher<QVector<CurvePoint>>::finished, this, [=]() {
        QVector<CurvePoint> pts = watcher->result();
        QVariantList result;
        result.reserve(pts.size());
        for (const auto& pt : pts) {
            QVariantMap m;
            m["p"]          = pt.p;   // here "p" stores B value
            m["cliff_prob"] = pt.cliff_prob;
            m["cafe_prob"]  = 1.0 - pt.cliff_prob;
            result.append(m);
        }
        m_batch_running = false;
        emit batch_running_changed();
        emit curve_ready(result);
        qDebug() << "Curve done, points:" << result.size();
        watcher->deleteLater();
    });

    watcher->setFuture(QtConcurrent::run([=]() -> QVector<CurvePoint> {
        QVector<CurvePoint> pts;
        pts.reserve(bMax);
        for (int b = 1; b <= bMax; b++) {
            DrunkWalkSimulation sim(CLIFF_POS, b, p);
            auto results = sim.run_batch(nRuns);
            int cliff_count = 0;
            for (const auto& r : results)
                if (r.fell_off_cliff) cliff_count++;
            pts.append({ (double)b, (double)cliff_count / nRuns });
        }
        return pts;
    }));
}

void DrunkWalkController::reset_walk() {
    rebuild_sim();
    emit walk_updated();
}

void DrunkWalkController::step_walk() {
    if (!m_sim || m_finished) return;
    m_current_pos = m_sim->step_once();
    m_steps       = m_sim->steps_taken();
    m_finished    = m_sim->is_finished();
    m_cliff       = m_sim->fell_off_cliff();
    emit walk_updated();
}

void DrunkWalkController::set_p_forward(double p) {
    m_p_forward = qBound(0.01, p, 0.99);
    rebuild_sim();
    emit params_changed();
    emit walk_updated();
}

void DrunkWalkController::set_cafe_pos(int b) {
    if (b <= CLIFF_POS) return;
    m_cafe_pos = b;
    rebuild_sim();
    emit params_changed();
    emit walk_updated();
}

double DrunkWalkController::get_p_forward()        const { return m_p_forward; }
int    DrunkWalkController::get_cafe_pos()          const { return m_cafe_pos; }
int    DrunkWalkController::get_cliff_pos()         const { return CLIFF_POS; }
int    DrunkWalkController::get_current_position()  const { return m_current_pos; }
int    DrunkWalkController::get_steps_taken()       const { return m_steps; }
bool   DrunkWalkController::is_walk_finished()      const { return m_finished; }
bool   DrunkWalkController::get_fell_off_cliff()    const { return m_cliff; }
double DrunkWalkController::get_cliff_probability() const { return m_cliff_prob; }
double DrunkWalkController::get_cafe_probability()  const { return m_cafe_prob; }
double DrunkWalkController::get_avg_steps()         const { return m_avg_steps; }
int    DrunkWalkController::get_batch_n()           const { return m_batch_n; }
bool   DrunkWalkController::is_batch_running()      const { return m_batch_running; }
