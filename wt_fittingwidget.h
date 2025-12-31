/*
 * 文件名: wt_fittingwidget.h
 * 文件作用: 试井拟合分析主界面类的头文件
 * 功能描述:
 * 1. 定义拟合分析界面的主要控件成员变量和布局逻辑。
 * 2. 声明用于Levenberg-Marquardt非线性回归拟合的核心算法函数。
 * 3. 声明观测数据（时间、压差、导数）的管理函数。
 * 4. 提供与外部模块（如主窗口、模型管理器）的交互接口。
 * 注意：已更新适配 ModelSolver 分离后的架构。
 */

#ifndef WT_FITTINGWIDGET_H
#define WT_FITTINGWIDGET_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QFutureWatcher>
#include <QJsonObject>
#include <QStandardItemModel>
#include "modelmanager.h" // 包含 ModelManager 的 ModelType 定义
#include "mousezoom.h"
#include "chartsetting1.h"
#include "fittingparameterchart.h"
#include "paramselectdialog.h"

namespace Ui { class FittingWidget; }

class FittingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FittingWidget(QWidget *parent = nullptr);
    ~FittingWidget();

    void setModelManager(ModelManager* m);
    void setProjectDataModel(QStandardItemModel* model);

    void setObservedData(const QVector<double>& t, const QVector<double>& deltaP, const QVector<double>& deriv);
    void updateBasicParameters();

    void loadFittingState(const QJsonObject& data = QJsonObject());
    QJsonObject getJsonState() const;

signals:
    void fittingCompleted(ModelManager::ModelType modelType, const QMap<QString, double>& parameters);
    void sigIterationUpdated(double error, QMap<QString, double> currentParams, QVector<double> t, QVector<double> p, QVector<double> d);
    void sigProgress(int progress);
    void sigRequestSave();

private slots:
    void on_btnLoadData_clicked();
    void on_btnRunFit_clicked();
    void on_btnStop_clicked();
    void on_btnImportModel_clicked();
    void on_btnExportData_clicked();
    void on_btnExportChart_clicked();
    void on_btnResetParams_clicked();
    void on_btnResetView_clicked();
    void on_btnChartSettings_clicked();
    void on_btn_modelSelect_clicked();
    void on_btnSelectParams_clicked();
    void on_btnSaveFit_clicked();
    void on_btnExportReport_clicked();

    void onIterationUpdate(double err, const QMap<QString,double>& p, const QVector<double>& t, const QVector<double>& p_curve, const QVector<double>& d_curve);
    void onFitFinished();
    void onSliderWeightChanged(int value);

private:
    Ui::FittingWidget *ui;
    ModelManager* m_modelManager;
    QStandardItemModel* m_projectModel;

    MouseZoom* m_plot;
    QCPTextElement* m_plotTitle;

    // 使用 ModelManager 中定义的 ModelType (即 ModelSolver01_06::ModelType)
    ModelManager::ModelType m_currentModelType;

    FittingParameterChart* m_paramChart;

    QVector<double> m_obsTime;
    QVector<double> m_obsDeltaP;
    QVector<double> m_obsDerivative;

    bool m_isFitting;
    bool m_stopRequested;
    QFutureWatcher<void> m_watcher;

    void setupPlot();
    void initializeDefaultModel();
    void updateModelCurve();

    // 核心算法函数
    void runOptimizationTask(ModelManager::ModelType modelType, QList<FitParameter> fitParams, double weight);
    void runLevenbergMarquardtOptimization(ModelManager::ModelType modelType, QList<FitParameter> params, double weight);
    QVector<double> calculateResiduals(const QMap<QString, double>& params, ModelManager::ModelType modelType, double weight);
    QVector<QVector<double>> computeJacobian(const QMap<QString, double>& params, const QVector<double>& residuals, const QVector<int>& fitIndices, ModelManager::ModelType modelType, const QList<FitParameter>& currentFitParams, double weight);
    QVector<double> solveLinearSystem(const QVector<QVector<double>>& A, const QVector<double>& b);
    double calculateSumSquaredError(const QVector<double>& residuals);

    QString getPlotImageBase64();
    void plotCurves(const QVector<double>& t, const QVector<double>& p, const QVector<double>& d, bool isModel);
};

#endif // WT_FITTINGWIDGET_H
