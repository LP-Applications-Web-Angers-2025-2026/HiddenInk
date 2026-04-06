#pragma once
#include <QWidget>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <vector>
#include <algorithm>

/**
 * Widget de visualisation d'entropie de Shannon par blocs (heatmap).
 * Vert = entropie basse (normal), Jaune = moyenne, Rouge = haute (suspect).
 */
class EntropyWidget : public QWidget {
    Q_OBJECT
public:
    explicit EntropyWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumHeight(160);
    }

    /**
     * Met à jour la carte d'entropie.
     * @param blocks    Valeurs d'entropie par bloc (row-major)
     * @param gW, gH    Dimensions de la grille
     * @param minE/maxE Valeurs min et max pour normalisation
     * @param globalE   Entropie globale (affiché dans la légende)
     */
    void setEntropyMap(const std::vector<double>& blocks, int gW, int gH,
                       double minE, double maxE, double globalE) {
        m_blocks  = blocks;
        m_gW      = gW;
        m_gH      = gH;
        m_minE    = minE;
        m_maxE    = maxE;
        m_globalE = globalE;
        update();
    }

    void clear() {
        m_blocks.clear();
        m_gW = m_gH = 0;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, false);
        p.fillRect(rect(), QColor(28, 28, 28));

        if (m_blocks.empty() || m_gW <= 0 || m_gH <= 0) {
            p.setPen(QColor(100, 100, 100));
            p.setFont(QFont("Segoe UI", 9));
            p.drawText(rect(), Qt::AlignCenter, "Carte d'entropie — lancez une analyse");
            return;
        }

        const int legendH = 22;
        const int mapH    = height() - legendH;
        double cellW = double(width())  / m_gW;
        double cellH = double(mapH)     / m_gH;
        double range = m_maxE - m_minE;
        if (range < 0.001) range = 0.001;

        // ── Dessin de la grille ──────────────────────────────────
        for (int by = 0; by < m_gH; ++by) {
            for (int bx = 0; bx < m_gW; ++bx) {
                double e = m_blocks[size_t(by) * m_gW + bx];
                double t = std::max(0.0, std::min(1.0, (e - m_minE) / range));

                // Palette : vert → jaune → rouge
                int r, g, b = 0;
                if (t < 0.5) {
                    r = int(t * 2.0 * 255);
                    g = 200;
                } else {
                    r = 255;
                    g = int((1.0 - (t - 0.5) * 2.0) * 200);
                }

                QRectF cell(bx * cellW, by * cellH, cellW + 0.5, cellH + 0.5);
                p.fillRect(cell, QColor(r, g, b));
            }
        }

        // ── Légende dégradée ─────────────────────────────────────
        QLinearGradient grad(0, mapH, width(), mapH);
        grad.setColorAt(0.0, QColor(0,   200, 0));
        grad.setColorAt(0.5, QColor(255, 200, 0));
        grad.setColorAt(1.0, QColor(255, 0,   0));
        p.fillRect(QRect(0, mapH, width(), legendH), grad);

        p.setPen(Qt::white);
        p.setFont(QFont("Segoe UI", 7));

        // Min (gauche)
        p.drawText(QRect(3, mapH, 50, legendH), Qt::AlignVCenter | Qt::AlignLeft,
                   QString::number(m_minE, 'f', 2));
        // Global (centre)
        p.drawText(QRect(0, mapH, width(), legendH), Qt::AlignVCenter | Qt::AlignHCenter,
                   QString("global: %1 bits").arg(m_globalE, 0, 'f', 2));
        // Max (droite)
        p.drawText(QRect(0, mapH, width() - 3, legendH), Qt::AlignVCenter | Qt::AlignRight,
                   QString::number(m_maxE, 'f', 2));
    }

private:
    std::vector<double> m_blocks;
    int    m_gW = 0, m_gH = 0;
    double m_minE = 0.0, m_maxE = 0.0, m_globalE = 0.0;
};
