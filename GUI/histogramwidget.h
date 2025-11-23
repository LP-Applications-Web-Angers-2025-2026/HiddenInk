#pragma once
#include <QWidget>
#include <QPainter>
#include <vector>
#include <algorithm>

class HistogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    void setHist(const std::vector<int> &h) { hist = h; update(); }
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter p(this);
        p.fillRect(rect(), QColor(30,30,30));
        if(hist.empty()) {
            p.setPen(Qt::gray);
            p.drawText(rect(), Qt::AlignCenter, "Histogramme vide");
            return;
        }
        int maxVal = *std::max_element(hist.begin(), hist.end());
        if(maxVal <= 0) maxVal = 1;
        int n = (int)hist.size();
        double barW = double(width()) / n;
        for(int i=0;i<n;i++) {
            double ratio = double(hist[i]) / maxVal;
            int h = int(ratio * (height()-10));
            QRectF r(i*barW, height()-h, barW*0.95, h);
            QColor col = QColor::fromHsv((i*360)/n, 160, 200);
            p.fillRect(r, col);
        }
        p.setPen(QColor("#e0e0e0"));
        p.drawLine(0, height()-1, width(), height()-1);
    }
private:
    std::vector<int> hist;
};
