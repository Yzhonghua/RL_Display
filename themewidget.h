/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

//#include "D:\Anaconda3\envs\gym\Lib\site-packages\tensorflow\include\external\local_config_python\python_include\Python.h"
#include <QWidget>
#include <QChartView>
#include <QDebug>
#include <QProcess>
#include <QMediaPlayer>
#include <qmychartview.h>
#include "ui_themewidget.h"

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;
using namespace QtCharts;

class ThemeWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = 0);
    ~ThemeWidget();

private Q_SLOTS:
    void on_pushButton_set_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_loadCSV_clicked();

private:
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;
    DataTable generateCsvData(int listCount, int valueMax, int valueCount, QString fileName);
    QChart *createBarChart(int valueCount) const;
    QChart *createLineChartFromNamedData(const QString& title, const QVector<QPair<QString, QVector<double>>>& data) const;

    void UpdateChart();
    QChart::ChartTheme GetTheme(int index);
    void UpdateStyle(int index);

protected:
    void paintEvent(QPaintEvent* event);

private:
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    DataTable m_dataTable;
    DataTable m_dataTable1;
    DataTable m_dataTable2;
    DataTable m_dataTable3;

    Ui_ThemeWidgetForm *m_ui;

private:
    QProcess *process;
    // 数据名称，及当前读进来的不同算法的该组数据，例如<min_dists_std, {<"algo1", {0.1,0.1}>, <"algo2",{0.1,0.1}>}>
    QHash<QString, QVector<QPair<QString, QVector<double>>>> named_data;

public slots:
    void m_exe();
    void write_cmd();
    void read_data();
    void finished_process();
    void error_process();
};

#endif /* THEMEWIDGET_H */
