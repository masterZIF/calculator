#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include <QAction>
#include <QTableWidget>
#include <QTextEdit>
#include <QStack>
#include <QPushButton>
#include <QToolButton>

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
    // 模式切换
    void switchToCalculator();
    void switchToStatistics();

    // 统计功能
    void addDataRow();
    void calculateStatistics();
    void clearData();

    // 计算器功能
    void numOnClick();
    void fuHao();
    void equalNum();
    void sciFunc();
    void percentFunc();
    void constantFunc();
    void parenFunc();  // 新增括号功能

    // 其他功能
    void moveCursorLeft();
    void moveCursorRight();
    void backspace();
    void showHistory();
    void setBackgroundImage();
private:
    // 矩阵计算模式相关成员变量
       QWidget *matrixWidget;
       QTableWidget *matrixATable;
       QTableWidget *matrixBTable;
       QTextEdit *matrixResult;
       QPushButton *createMatrixAButton;
       QPushButton *createMatrixBButton;
       QPushButton *addMatricesButton;
       QPushButton *subtractMatricesButton;
       QPushButton *multiplyMatricesButton;
       QPushButton *determinantButton;
       QPushButton *clearMatrixButton;

       // 矩阵运算相关函数声明
       void createMatrix(QTableWidget *&matrixTable);
       bool getMatrixData(QTableWidget *matrixTable, QVector<QVector<double>> &data);
       void matrixAddition();
       void matrixSubtraction();
       void matrixMultiplication();
       double calculateDeterminant(QVector<QVector<double>> matrix);
       void switchToMatrixMode();
private:
    Ui::MainWindow *ui;
    QString text;
    QString a, b;
    QStack<QString> parenStack;

    // 堆叠窗口
    QStackedWidget *stackedWidget;
    QWidget *calculatorWidget;
    QWidget *statsWidget;

    // 统计模式组件
    QTableWidget *dataTable;
    QPushButton *addDataButton;
    QPushButton *calculateStatsButton;
    QPushButton *clearDataButton;
    QTextEdit *statsResults;

    // 计算器状态
    bool Add, Sub, Mul, Div, Pow;
    int parenLevel;  // 括号层级计数
    double memory;
    QStringList history;
    QToolButton *leftParenBtn;
    QToolButton *rightParenBtn;

    // 辅助函数
    void matchFh();
    double evaluateExpression(QString expr);
};

#endif // MAINWINDOW_H
