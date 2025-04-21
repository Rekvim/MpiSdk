#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPointF>
#include <QThread>

#include "FileSaver.h"
#include "MyChart.h"
#include "Program.h"
#include "Registry.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SetRegistry(Registry *registry);
    void setReport(const FileSaver::Report &report);

private slots:
    void SetText(const TextObjects object, const QString &text);
    void SetTask(qreal task);
    void SetTextColor(const TextObjects object, const QColor color);
    void SetStepTestResults(QVector<StepTest::TestResult> results, quint32 T_value);
    void SetSensorsNumber(quint8 num);
    void SetButtonInitEnabled(bool enable);
    void AddPoints(Charts chart, QVector<Point> points);
    void ClearPoints(Charts chart);
    void SetChartVisible(Charts chart, quint16 series, bool visible);
    void ShowDots(bool visible);
    void DublSeries();
    void EnableSetTask(bool enable);
    void GetPoints(QVector<QVector<QPointF>> &points, Charts chart);
    void SetRegressionEnable(bool enable);
    void GetMainTestParameters(MainTestSettings::TestParameters &parameters);
    void GetStepTestParameters(StepTestSettings::TestParameters &parameters);
    void GetResolutionTestParameters(OtherTestSettings::TestParameters &parameters);
    void GetResponseTestParameters(OtherTestSettings::TestParameters &parameters);
    void Question(QString title, QString text, bool &result);
    void GetDirectory(QString current_path, QString &result);
    void StartTest();
    void EndTest();
    void ButtonStartMain();
    void ButtonStartStroke();
    void ButtonStartOptional();
    void SetButtonsDOChecked(quint8 status);
    void SetCheckboxDIChecked(quint8 status);

private:
    FileSaver::Report currentReport_;
    FileSaver::Report report_;
    bool testing_;
    Ui::MainWindow *ui;
    Program *program_;
    QThread *program_thread_;
    Registry *registry_;
    QHash<TextObjects, QLabel *> labels_;
    QHash<TextObjects, QLineEdit *> line_edits_;
    QHash<Charts, MyChart *> charts_;
    MainTestSettings *main_test_settings_;
    StepTestSettings *step_test_settings_;
    OtherTestSettings *response_test_settings_;
    OtherTestSettings *resolution_test_settings_;
    FileSaver *file_saver_;
    void InitCharts();
    void SaveChart(Charts chart);
    void GetImage(QLabel *label, QImage *image);
    void InitReport();
signals:
    void SetDAC(qreal value);
    void StartMainTest();
    void StartStrokeTest();
    void StartOptionalTest(quint8 test_num);
    void StopTest();
    void SetDO(quint8 DO_num, bool state);
};
#endif // MAINWINDOW_H
