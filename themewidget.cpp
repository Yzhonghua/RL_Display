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

#include "themewidget.h"

#include <QStackedBarSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QLineSeries>
#include <QRandomGenerator>
#include <QValueAxis>
#include <QKeySequence>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

QMediaPlayer *player = new QMediaPlayer();

ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_listCount(3),
    m_valueMax(10),
    m_valueCount(5),
    m_dataTable(generateRandomData(m_listCount, m_valueMax, m_valueCount)),
    m_dataTable1(generateRandomData(m_listCount, m_valueMax, m_valueCount)),
    m_dataTable2(generateRandomData(m_listCount, m_valueMax, m_valueCount)),
    m_dataTable3(generateRandomData(m_listCount, m_valueMax, m_valueCount)),
    m_ui(new Ui_ThemeWidgetForm)
{
    m_ui->setupUi(this);

    //调用python脚本
    m_exe();
}

ThemeWidget::~ThemeWidget()
{
    process->terminate();
    process->waitForFinished();
    delete m_ui;
}

void ThemeWidget::m_exe()
{
    m_ui->lineEdit->setText(tr("time"));
    m_ui->textEdit->setLineWrapMode(QTextEdit::NoWrap);

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    m_ui->m_pushButton_Enter->setFocus();
    m_ui->m_pushButton_Enter->setShortcut(QKeySequence::InsertParagraphSeparator);
    m_ui->m_pushButton_Enter->setShortcut(Qt::Key_Enter);
    m_ui->m_pushButton_Enter->setShortcut(Qt::Key_Return);

    connect(m_ui->m_pushButton,SIGNAL(clicked()),m_ui->lineEdit,SLOT(clear()));                           //清除命令行
    connect(m_ui->m_pushButton_Enter,SIGNAL(clicked()),this,SLOT(write_cmd()));                         //回车按钮
    connect(m_ui->m_pushButton_Clear,SIGNAL(clicked()),m_ui->textEdit,SLOT(clear()));                     //清除文本框按钮

    connect(process,SIGNAL(readyRead()),this,SLOT(read_data()));                                    //读命令行数据
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(read_data()));                      //读命令行标准数据（兼容）
    connect(process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(error_process()));      //命令行错误处理
    connect(process,SIGNAL(finished(int)),this,SLOT(finished_process()));                           //命令行结束处理

    connect(m_ui->m_pushButton,SIGNAL(clicked()),m_ui->lineEdit,SLOT(setFocus()));
    connect(m_ui->m_pushButton_Enter,SIGNAL(clicked()),m_ui->lineEdit,SLOT(setFocus()));
    connect(m_ui->m_pushButton_Clear,SIGNAL(clicked()),m_ui->lineEdit,SLOT(setFocus()));

    connect(m_ui->valueCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        UpdateStyle(index);
    });

    process->start("cmd.exe");
}

void ThemeWidget::write_cmd()
{
    QString cmd = m_ui->lineEdit->text() + "\r\n"; //
    m_ui->lineEdit->clear();
    m_ui->lineEdit->setFocus();

    QByteArray qbarr = cmd.toLatin1();
    char *ch = qbarr.data();
    qint64 len = cmd.length();
    process->write(ch, len);

    //output
    qDebug() << "write_cmd() successfully" << cmd << endl;
}

void ThemeWidget::read_data()
{
    //receive message
    QByteArray bytes = process->readAll();

    //display
    QString msg = QString::fromLocal8Bit(bytes);
    m_ui->textEdit->append(msg);
    m_ui->textEdit->update();

    //output
    qDebug() << "read_data() successfully" << msg << endl;
}

void ThemeWidget::finished_process()
{
    int flag = process->exitCode();
    qDebug() << "finished_process successfully" << flag << endl;
}

void ThemeWidget::error_process()
{
    int err_code = process->exitCode();
    QString err = process->errorString();

    m_ui->textEdit->append(QString("error code:%1").arg(err_code));
    m_ui->textEdit->append(err);

    qDebug() << "error_process() successfully" << err << endl;
}

DataTable ThemeWidget::generateRandomData(int listCount, int valueMax, int valueCount) const
{
    DataTable dataTable;

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + QRandomGenerator::global()->bounded(valueMax / (qreal) valueCount);
            QPointF value((j + QRandomGenerator::global()->generateDouble()) * ((qreal) m_valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

DataTable ThemeWidget::generateCsvData(int listCount, int valueMax, int valueCount, QString fileName)
{
    DataTable dataTable;
    //open file
    QFile file(fileName);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.close();

    QStringList fields;
    DataList dataList;
    int i = 1;
    QString label = "Slice " + QString::number(1) + ":" + QString::number(1);
    for (const QString &line : qAsConst(lines)) {
        fields = line.split(',');
        // 您可以在这里访问fields[i]以获取第i列的内容
        //step
        //min_dists_std
        QString str = fields[6];
        QPointF temp(str.toDouble(), i++);
        dataList << Data(temp, label);
    }
    return dataTable;
}

QChart *ThemeWidget::createBarChart(int valueCount) const
{
    Q_UNUSED(valueCount);
    QChart *chart = new QChart();
    chart->setTitle("训练成功率&训练成功步数");

    QStackedBarSeries *series = new QStackedBarSeries(chart);
    for (int i(0); i < m_dataTable.count(); i++) {
        QBarSet *set = new QBarSet("算法 " + QString::number(i));
        for (const Data &data : m_dataTable[i])
            *set << data.first.y();
        series->append(set);
    }
    chart->addSeries(series);

    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, m_valueMax * 2);
    // Add space to label to add space between labels and axis
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}

QChart *ThemeWidget::createLineChartFromNamedData(const QString &title, const QVector<QPair<QString, QVector<double>>> &data) const
{
    QChart *chart = new QChart();
    chart->setTitle(title);
    if (data.isEmpty())
        return chart;

    for (const QPair<QString, QVector<double>> &list : data) {
        QLineSeries *series = new QLineSeries(chart);
        series->setName(list.first);
        for (int i = 0; i < list.second.size(); ++i) {
            series->append(QPointF(i, list.second[i]));
        }

        chart->addSeries(series);
    }

    chart->createDefaultAxes();

    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    Q_ASSERT(axisY);
    axisY->setLabelFormat("%.1f  ");

    return chart;
}

void ThemeWidget::UpdateChart()
{
    // 删除已经存在的图表
    QList<QMyChartView*> items = m_ui->container_train->findChildren<QMyChartView*>();
    foreach(QMyChartView*item, items)
        delete item;

    // 根据数据创建图表
    QHash<QString, QPair<int, int>> pos_train; // 位置
    pos_train["rew_mean"] = {0,0}; pos_train["rew_std"] = {0,1};
    pos_train["min_dists_mean"] = {1,0}; pos_train["min_dists_std"] = {1,1};
    QHash<QString, QPair<int, int>> pos_test;
    pos_test["occ_land_mean"] = {0,0}; pos_test["occ_land_std"] = {0,1};
    pos_test["return_mean"] = {1,0}; pos_test["return_std"] = {1,1};

    for (auto ite = named_data.begin(); ite != named_data.end(); ++ite) {
        QChart* single = createLineChartFromNamedData(ite.key(), ite.value());
        QMyChartView* view = new QMyChartView(single, this);
        QChart::ChartTheme theme = GetTheme(m_ui->valueCombo->currentIndex());
        single->setTheme(theme);
        if (pos_train.count(ite.key())) {
            view->setMaximumSize(0.5 * m_ui->container_train->size());
            m_ui->gridLayout_train->addWidget(view, pos_train[ite.key()].first, pos_train[ite.key()].second);
        }
        else if (pos_test.count(ite.key())) {
            view->setMaximumSize(0.5 * m_ui->container_test->size());
            m_ui->gridLayout_test->addWidget(view, pos_test[ite.key()].first, pos_test[ite.key()].second);
        }
    }
}

QChart::ChartTheme ThemeWidget::GetTheme(int index)
{
    switch (index) {
    default:
    case 0: return QChart::ChartThemeLight;
    case 1: return QChart::ChartThemeBlueCerulean;
    case 2: return QChart::ChartThemeDark;
    case 3: return QChart::ChartThemeBrownSand;
    case 4: return QChart::ChartThemeBlueNcs;
    case 5: return QChart::ChartThemeHighContrast;
    case 6: return QChart::ChartThemeBlueIcy;
    case 7: return QChart::ChartThemeQt;
    }
}

void ThemeWidget::UpdateStyle(int index)
{
    QPalette pal = window()->palette();
    switch (index) {
    default:
    case 0: {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        break;
    }
    case 1: {
        pal.setColor(QPalette::Window, QRgb(0x40434a));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        break;
    }
    case 2: {
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        break;
    }
    case 3: {
        pal.setColor(QPalette::Window, QRgb(0x9e8965));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        break;
    }
    case 4: {
        pal.setColor(QPalette::Window, QRgb(0x018bba));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        break;
    }
    case 5: {
        pal.setColor(QPalette::Window, QRgb(0xffab03));
        pal.setColor(QPalette::WindowText, QRgb(0x181818));
        break;
    }
    case 6: {
        pal.setColor(QPalette::Window, QRgb(0xcee7f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        break;
    }
    case 7: {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        break;
    }
    }
    window()->setPalette(pal);

    QChart::ChartTheme theme = GetTheme(index);
    QList<QMyChartView*> charts = m_ui->container_train->findChildren<QMyChartView*>();
    charts.append(m_ui->container_test->findChildren<QMyChartView*>());
    for (QMyChartView *chartView : charts) {
        chartView->chart()->setTheme(theme);
    }
}

void ThemeWidget::paintEvent(QPaintEvent *event)
{
    // 绘制事件需要调整图尺寸
    QList<QMyChartView*> items = m_ui->container_train->findChildren<QMyChartView*>();
    foreach(QMyChartView*item, items)
        item->setMaximumSize(0.5 * m_ui->container_train->size());

     items = m_ui->container_test->findChildren<QMyChartView*>();
    foreach(QMyChartView*item, items)
        item->setMaximumSize(0.5 * m_ui->container_test->size());
    QWidget::paintEvent(event);
}

void ThemeWidget::on_pushButton_set_clicked()
{
    player->setMedia(QMediaContent(QUrl::fromLocalFile("O:/code/Qt/chartthemes/Python/Video/demo1.mp4")));
    player->setVideoOutput(m_ui->widget_video);
}

void ThemeWidget::on_pushButton_start_clicked()
{
    player->play();
}

void ThemeWidget::on_pushButton_loadCSV_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择csv", "", "*.csv");
    if (fileName.isEmpty())
        return;

    m_ui->lineEdit_csvPath->setText(fileName);

    //////csv文件命名需要以"算法名_***.csv"为格式，例如"algo1_out.csv"!!!
    QFileInfo info(fileName);
    QString algoName = info.fileName().split('_').at(0);
    if (algoName.isEmpty())
        return;

    QFile file(fileName);
    QTextStream stream(&file);
    if (file.open(QIODevice::ReadOnly)) {
        // 先读取头
        QString head_s = stream.readLine();
        QStringList headers = head_s.split(',');
        QVector<QPair<QString, QVector<double>>> table(headers.size());
        for (int i = 0; i < headers.size(); ++i) {
            table[i].first = headers[i];
        }

        // 读数据，第一列是索引值，不处理
        while(!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList datas = line.split(',');
            for (int i = 1; i < datas.size(); ++i) {
                table[i].second.append(datas[i].toDouble());
            }
        }

        // 读完之后传递给成员
        for (int i = 0; i < table.size(); ++i) {
            QString value_name = table[i].first;
            if (value_name.isEmpty())
                continue;
            if (named_data.count(value_name) == 0) {
                named_data.insert(value_name, {});
            }
            named_data[value_name].append({algoName, table[i].second});
        }

        // 刷新表格
        UpdateChart();
    }
}
