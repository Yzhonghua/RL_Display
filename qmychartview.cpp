#include "qmychartview.h"
#include <QDebug>
#include <math.h>
#include <QtGlobal>
#include <QLineSeries>
#include <QToolTip>
QMyChartView::QMyChartView(QWidget* pParent /*= nullptr*/):
    QChartView(pParent),
    m_bMiddleButtonPressed(false) ,
    m_oPrePos(0, 0)
{
    // 创建线
    x_line = new QGraphicsLineItem();
    // 设置颜色
    x_line->setPen(QPen(QColor( 100, 100, 100 )));
    x_line->setZValue(2);
    // 添加到scene中。
    this->scene()->addItem(x_line);

}

QMyChartView::QMyChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
    , m_bMiddleButtonPressed(false)
    , m_oPrePos(0, 0)
{
    // 创建线
    x_line = new QGraphicsLineItem();
    // 设置颜色
    x_line->setPen(QPen(QColor( 100, 100, 100 )));
    x_line->setZValue(2);
    // 添加到scene中。
    this->scene()->addItem(x_line);

}
QMyChartView::~QMyChartView()
{

}
// 鼠标移动
void QMyChartView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (m_bMiddleButtonPressed)
    {
     QPoint oDeltaPos = pEvent->pos() - m_oPrePos;
     this->chart()->scroll(-oDeltaPos.x(), oDeltaPos.y());
     m_oPrePos = pEvent->pos();

    }
    QChartView::mouseMoveEvent(pEvent);

    // 把x坐标发出去
    int nX = this->chart()->mapToValue(pEvent->pos()).x();
    if(nX != m_oldPosOfX){
        m_oldPosOfX = nX;
        emit sendXpos(m_oldPosOfX);
    }
    // 绘制线
    QRectF plotArea = this->chart()->plotArea();
    x_line->setLine(pEvent->x(),plotArea.y(),pEvent->x(),plotArea.y() + plotArea.height());

    // 显示数据
    int vx = qRound(this->chart()->mapToValue(pEvent->pos()).x());
    QList<QAbstractSeries*> series = this->chart()->series();
    QString label;
    for (int i = 0; i < series.size(); ++i) {
        QLineSeries* line = dynamic_cast<QLineSeries*>(series[i]);
        if (line) {
            auto pnts = line->pointsVector();
            if (0 <= vx && vx < pnts.size()) {
                QPointF value = pnts[vx];
                label += line->name() + " : " + QString("%1 , %2").arg(vx).arg(QString::number(value.y(), 'f', 4)) + "\n";
            }
        }
    }
    if (!label.isEmpty()) {
        label = label.left(label.size()-1);
        QToolTip::showText(cursor().pos(), label, this);
    }
}

// 鼠标按键
void QMyChartView::mousePressEvent(QMouseEvent *pEvent)
{
     if (pEvent->button() == Qt::MiddleButton)
     {
         m_bMiddleButtonPressed = true;
         m_oPrePos = pEvent->pos();
         this->setCursor(Qt::OpenHandCursor);
     }
     QChartView::mousePressEvent(pEvent);
}
// 鼠标抬起
void QMyChartView::mouseReleaseEvent(QMouseEvent *pEvent)
{
     if (pEvent->button() == Qt::MiddleButton)
     {
         m_bMiddleButtonPressed = false;
         this->setCursor(Qt::ArrowCursor);
     }
     QChartView::mouseReleaseEvent(pEvent);
}

void QMyChartView::wheelEvent(QWheelEvent *pEvent)
{
     qreal rVal = std::pow(0.999, pEvent->delta()); // 设置比例
     // 1. 读取视图基本信息
     QRectF oPlotAreaRect = this->chart()->plotArea();
     QPointF oCenterPoint = oPlotAreaRect.center();
     // 2. 水平调整
     oPlotAreaRect.setWidth(oPlotAreaRect.width() * rVal);
     // 3. 竖直调整
     oPlotAreaRect.setHeight(oPlotAreaRect.height() * rVal);
     // 4.1 计算视点，视点不变，围绕中心缩放
     //QPointF oNewCenterPoint(oCenterPoint);
     // 4.2 计算视点，让鼠标点击的位置移动到窗口中心
     //QPointF oNewCenterPoint(pEvent->pos());
     // 4.3 计算视点，让鼠标点击的位置尽量保持不动(等比换算，存在一点误差)
     QPointF oNewCenterPoint(2 * oCenterPoint - pEvent->pos() - (oCenterPoint - pEvent->pos()) / rVal);
     // 5. 设置视点
     oPlotAreaRect.moveCenter(oNewCenterPoint);
     // 6. 提交缩放调整
     this->chart()->zoomIn(oPlotAreaRect);
     QChartView::wheelEvent(pEvent);
}

void QMyChartView::enterEvent(QEvent *pEvent)
{
    x_line->setVisible(true);
    QChartView::enterEvent(pEvent);
}

void QMyChartView::leaveEvent(QEvent *pEvent)
{
    x_line->setVisible(false);
    QChartView::leaveEvent(pEvent);
}
