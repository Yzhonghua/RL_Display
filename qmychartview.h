#ifndef QMYCHARTVIEW_H
#define QMYCHARTVIEW_H
 
#include <QtCharts/QChartView>
#include <QObject>
 
QT_CHARTS_USE_NAMESPACE
class QMyChartView : public QChartView
{
    Q_OBJECT
public:
    QMyChartView(QWidget* pParent = nullptr);
    explicit QMyChartView(QChart *chart, QWidget *parent = nullptr);
    virtual ~QMyChartView();
protected:
     virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
     virtual void mousePressEvent(QMouseEvent *pEvent) override;
     virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
     virtual void wheelEvent(QWheelEvent *pEvent) override;
     virtual void enterEvent(QEvent *pEvent)override;
     virtual void leaveEvent(QEvent *pEvent)override;
private:
     bool m_bMiddleButtonPressed;
     QPoint m_oPrePos;
     QGraphicsLineItem* x_line;
     int m_oldPosOfX = 0;
signals:
     void sendXpos(int nPos);
 
};
 
#endif // QMYCHARTVIEW_H
