#ifndef EQCHART_H
#define EQCHART_H

#include <QQuickPaintedItem>
#include <QPolygonF>

#include "common/Types.h"

class EqChart : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor plotColor READ plotColor WRITE setPlotColor)
    Q_PROPERTY(QColor currentPlotColor READ currentPlotColor WRITE setCurrentPlotColor)
    Q_PROPERTY(QColor sumPlotColor READ sumPlotColor WRITE setSumPlotColor)

    Q_PROPERTY(int currentPlot READ currentPlot WRITE setCurrentPlot)

public:
    EqChart(QQuickItem *parent = 0);

    Q_INVOKABLE void setFilterCount(int i);
    Q_INVOKABLE void setFilter(int i, const Filter& filter);

    QColor plotColor() const;
    void setPlotColor(const QColor &color);
    QColor currentPlotColor() const;
    void setCurrentPlotColor(const QColor &color);
    QColor sumPlotColor() const;
    void setSumPlotColor(const QColor &color);

    int currentPlot() const;
    void setCurrentPlot(int i);

    void paint(QPainter *painter) override;

private:
    void computeResponse(const Filter& f, QPolygonF* mags);

    QColor m_plotColor;         // background plots
    QColor m_currentPlotColor;  // current plot
    QColor m_sumPlotColor;      // sum plot

    int    m_currentPlot = -1;

    QList<QPolygonF>    m_plots;
};

#endif // EQCHART_H
