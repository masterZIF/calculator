#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include <QAction>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 计算器功能
    void numOnClick();
    void fuHao();
    void equalNum();
    void sciFunc();
    void percentFunc();
    void showHistory();
    void constantFunc();
    void parenClicked();
    void toggleSign();
    void memoryFunc();
    void moveCursorLeft();
    void moveCursorRight();
    void backspace();

    // 模式切换
    void switchToCalculator();
    void switchToStatistics();

    // 统计功能
    void addDataRow();
    void calculateStatistics();

private:
    Ui::MainWindow *ui;

    // 计算器相关
    QString texT, a, b;
    bool Add = false, Sub = false, Mul = false, Div = false, Pow = false;
    int parenLevel = 0;
    QStringList parenStack;
    QStringList history;
    double memory = 0;

    // 多模式界面
    QStackedWidget *stackedWidget;
    QWidget *calculatorWidget;
    QWidget *statsWidget;

    // 统计相关
    QTableWidget *dataTable;
    QPushButton *addDataButton;
    QPushButton *calculateStatsButton;
    QTextEdit *statsResults;

    void setBackgroundImage();
    void matchFh();
    double evaluateExpression(QString expr);
};

#endif // MAINWINDOW_H
