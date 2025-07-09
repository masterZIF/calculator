#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QVector>
#include <cmath>
#include <algorithm>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/2.png"));
    this->setWindowTitle("多功能计算器");
     setBackgroundImage();

    // 初始化成员变量
    Add = Sub = Mul = Div = Pow = false;
    parenLevel = 0;
    memory = 0.0;

    // 初始化堆叠窗口
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // 1. 计算器模式
    calculatorWidget = new QWidget();
    QVBoxLayout *calcLayout = new QVBoxLayout(calculatorWidget);
    calcLayout->addWidget(ui->plainTextEdit);

    // 添加计算器按钮布局
    QWidget *calcButtons = new QWidget();
    QGridLayout *buttonLayout = new QGridLayout(calcButtons);

    // 第一行: 高级功能按钮
    buttonLayout->addWidget(ui->sqrtBtn, 0, 0);      // √
    buttonLayout->addWidget(ui->percentBtn, 0, 1);   // %
    buttonLayout->addWidget(ui->piBtn, 0, 2);        // π
    buttonLayout->addWidget(ui->eBtn, 0, 3);         // e

    // 第二行: 三角函数
    buttonLayout->addWidget(ui->sinBtn, 1, 0);
    buttonLayout->addWidget(ui->cosBtn, 1, 1);
    buttonLayout->addWidget(ui->tanBtn, 1, 2);
    buttonLayout->addWidget(ui->cheng, 1, 3);  // ×

    // 第三行: 数字7-9
    buttonLayout->addWidget(ui->num7, 2, 0);
    buttonLayout->addWidget(ui->num8, 2, 1);
    buttonLayout->addWidget(ui->num9, 2, 2);
    buttonLayout->addWidget(ui->chu, 2, 3);    // ÷

    // 第四行: 数字4-6
    buttonLayout->addWidget(ui->num4, 3, 0);
    buttonLayout->addWidget(ui->num5, 3, 1);
    buttonLayout->addWidget(ui->num6, 3, 2);
    buttonLayout->addWidget(ui->jian, 3, 3);     // -

    // 第五行: 数字1-3
    buttonLayout->addWidget(ui->num1, 4, 0);
    buttonLayout->addWidget(ui->num2, 4, 1);
    buttonLayout->addWidget(ui->num3, 4, 2);
    buttonLayout->addWidget(ui->jia, 4, 3);      // +

    // 第六行: 导航和数字0
    buttonLayout->addWidget(ui->zuoyi, 5, 0);      // 左移
    buttonLayout->addWidget(ui->num0, 5, 1);         // 0
    buttonLayout->addWidget(ui->youyi, 5, 2);     // 右移
    buttonLayout->addWidget(ui->deng, 5, 3);     // =

    // 第七行: 功能按钮
    buttonLayout->addWidget(ui->shanchu, 6, 0); // 退格
    buttonLayout->addWidget(ui->historyBtn, 6, 1);   // 历史
    buttonLayout->addWidget(ui->AC, 6, 2, 1, 2);     // AC，跨2列

    calcLayout->addWidget(calcButtons);

    // 2. 统计模式
    statsWidget = new QWidget();
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);

    dataTable = new QTableWidget();
    dataTable->setColumnCount(1);
    dataTable->setHorizontalHeaderLabels(QStringList() << "数据值");
    dataTable->horizontalHeader()->setStretchLastSection(true);
    statsLayout->addWidget(dataTable);

    addDataButton = new QPushButton("添加数据行");
    connect(addDataButton, &QPushButton::clicked, this, &MainWindow::addDataRow);
    statsLayout->addWidget(addDataButton);

    calculateStatsButton = new QPushButton("计算统计量");
    connect(calculateStatsButton, &QPushButton::clicked, this, &MainWindow::calculateStatistics);
    statsLayout->addWidget(calculateStatsButton);

    statsResults = new QTextEdit();
    statsResults->setReadOnly(true);
    statsLayout->addWidget(statsResults);

    // 添加到堆叠窗口
    stackedWidget->addWidget(calculatorWidget);
    stackedWidget->addWidget(statsWidget);

    // 创建工具栏
    QToolBar *modeToolBar = addToolBar("模式选择");
    modeToolBar->addAction("计算器", this, &MainWindow::switchToCalculator);
    modeToolBar->addAction("统计分析", this, &MainWindow::switchToStatistics);

    // 默认显示计算器
    switchToCalculator();
    addDataRow(); // 添加初始数据行

    // 连接数字按钮信号槽
    QToolButton *numButtons[10];
    for(int k=0; k<10; k++) {
        QString numName = "num"+QString::number(k);
        numButtons[k] = findChild<QToolButton *>(numName);
        connect(numButtons[k], &QToolButton::clicked, this, &MainWindow::numOnClick);
    }

    // 连接运算符按钮
    connect(ui->jia, &QToolButton::clicked, this, &MainWindow::fuHao);
    connect(ui->jian, &QToolButton::clicked, this, &MainWindow::fuHao);
    connect(ui->cheng, &QToolButton::clicked, this, &MainWindow::fuHao);
    connect(ui->chu, &QToolButton::clicked, this, &MainWindow::fuHao);
    connect(ui->AC, &QToolButton::clicked, this, &MainWindow::fuHao);
    connect(ui->deng, &QToolButton::clicked, this, &MainWindow::equalNum);

    // 连接函数按钮
    connect(ui->sinBtn, &QToolButton::clicked, this, &MainWindow::sciFunc);
    connect(ui->cosBtn, &QToolButton::clicked, this, &MainWindow::sciFunc);
    connect(ui->tanBtn, &QToolButton::clicked, this, &MainWindow::sciFunc);
    connect(ui->sqrtBtn, &QToolButton::clicked, this, &MainWindow::sciFunc);

    // 连接其他功能按钮
    connect(ui->historyBtn, &QToolButton::clicked, this, &MainWindow::showHistory);
    connect(ui->percentBtn, &QToolButton::clicked, this, &MainWindow::percentFunc);
    connect(ui->piBtn, &QToolButton::clicked, this, &MainWindow::constantFunc);
    connect(ui->eBtn, &QToolButton::clicked, this, &MainWindow::constantFunc);
    connect(ui->zuoyi, &QToolButton::clicked, this, &MainWindow::moveCursorLeft);
    connect(ui->youyi, &QToolButton::clicked, this, &MainWindow::moveCursorRight);
    connect(ui->shanchu, &QToolButton::clicked, this, &MainWindow::backspace);

    // 设置样式
    this->setStyleSheet(

        "QToolButton, QPushButton { "
        "font-size: 16px; "
        "padding: 8px; "
        "min-width: 40px; "
        "min-height: 40px; "
        "border: 1px solid #ccc; "
        "border-radius: 4px; "
        "background-color: rgba(255, 255, 255, 200); "
        "}"
        "QToolButton:hover, QPushButton:hover { "
        "background-color: #e0e0e0; "
        "}"
        "QPlainTextEdit, QTextEdit, QTableWidget { "
        "font-size: 16px; "
        "border: 1px solid #ccc; "
        "border-radius: 4px; "
        "padding: 4px; "
        "}"
        "QPlainTextEdit { "
        "min-height: 60px; "
        "}"
    );
}
void MainWindow::setBackgroundImage()
{
    QPixmap bkgnd(":/3.png"); // 确保在资源文件中有这个图片
    if(!bkgnd.isNull()) {
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
        QPalette palette;
        palette.setBrush(QPalette::Window, bkgnd);
        this->setPalette(palette);
    }
}
// 模式切换功能
void MainWindow::switchToCalculator() {
    stackedWidget->setCurrentWidget(calculatorWidget);
    this->setWindowTitle("多功能计算器 - 计算模式");
}

void MainWindow::switchToStatistics() {
    stackedWidget->setCurrentWidget(statsWidget);
    this->setWindowTitle("多功能计算器 - 统计模式");
}

// 统计功能实现
void MainWindow::addDataRow() {
    int row = dataTable->rowCount();
    dataTable->insertRow(row);
    QTableWidgetItem *item = new QTableWidgetItem();
    dataTable->setItem(row, 0, item);
    dataTable->setCurrentItem(item);
}

void MainWindow::calculateStatistics() {
    QVector<double> data;

    // 收集有效数据
    for (int i = 0; i < dataTable->rowCount(); ++i) {
        QTableWidgetItem *item = dataTable->item(i, 0);
        if (item && !item->text().isEmpty()) {
            bool ok;
            double value = item->text().toDouble(&ok);
            if (ok) data.append(value);
        }
    }

    if (data.isEmpty()) {
        statsResults->setText("没有有效数据");
        return;
    }

    // 计算基本统计量
    double sum = 0, min = data[0], max = data[0];
    for (double value : data) {
        sum += value;
        if (value < min) min = value;
        if (value > max) max = value;
    }
    double mean = sum / data.size();

    // 计算标准差
    double variance = 0;
    for (double value : data) {
        variance += pow(value - mean, 2);
    }
    variance /= data.size();
    double stddev = sqrt(variance);

    // 排序计算中位数
    std::sort(data.begin(), data.end());
    double median = data.size() % 2 == 1 ?
                   data[data.size()/2] :
                   (data[data.size()/2 - 1] + data[data.size()/2]) / 2;

    // 显示结果
    QString resultText;
    resultText += QString("数据个数: %1\n").arg(data.size());
    resultText += QString("平均值: %1\n").arg(mean, 0, 'f', 4);
    resultText += QString("中位数: %1\n").arg(median, 0, 'f', 4);
    resultText += QString("最小值: %1\n").arg(min, 0, 'f', 4);
    resultText += QString("最大值: %1\n").arg(max, 0, 'f', 4);
    resultText += QString("标准差: %1\n").arg(stddev, 0, 'f', 4);

    statsResults->setText(resultText);
}

// 计算器功能实现
void MainWindow::numOnClick()
{
    QToolButton *numName = (QToolButton*)sender();
    ui->plainTextEdit->textCursor().insertText(numName->text());
    texT = ui->plainTextEdit->toPlainText();

    if(Add) {
        int i = texT.indexOf("+");
        texT = texT.mid(i+1);
        b = texT;
    }
    else if(Sub) {
        int i = texT.indexOf("-");
        texT = texT.mid(i+1);
        b = texT;
    }
    else if(Mul) {
        int i = texT.indexOf("×");
        texT = texT.mid(i+1);
        b = texT;
    }
    else if(Div) {
        int i = texT.indexOf("÷");
        texT = texT.mid(i+1);
        b = texT;
    }
    else if(Pow) {
        int i = texT.indexOf("^");
        texT = texT.mid(i+1);
        b = texT;
    }
    else {
        a = texT;
    }
}

void MainWindow::matchFh()
{
    if(texT.contains("+", Qt::CaseSensitive)) {
        QStringList t = texT.split("+");
        a = t[0];
        b = t[1];
        Add = true;
    }
    else if(texT.contains("-", Qt::CaseSensitive)) {
        QStringList t = texT.split("-");
        a = t[0];
        b = t[1];
        Sub = true;
    }
    else if(texT.contains("×", Qt::CaseSensitive)) {
        QStringList t = texT.split("×");
        a = t[0];
        b = t[1];
        Mul = true;
    }
    else if(texT.contains("÷", Qt::CaseSensitive)) {
        QStringList t = texT.split("÷");
        a = t[0];
        b = t[1];
        Div = true;
    }
    else if(texT.contains("^", Qt::CaseSensitive)) {
        QStringList t = texT.split("^");
        a = t[0];
        b = t[1];
        Pow = true;
    }
    else {
        a = texT;
    }
}

void MainWindow::fuHao()
{
    QToolButton *fh = (QToolButton*)sender();
    QString f = fh->text();

    if(!(Add || Sub || Mul || Div || Pow) || parenLevel > 0) {
        if(f == "+") {
            Add = true;
            ui->plainTextEdit->textCursor().insertText("+");
        }
        if(f == "-") {
            Sub = true;
            ui->plainTextEdit->textCursor().insertText("-");
        }
        if(f == "×") {
            Mul = true;
            ui->plainTextEdit->textCursor().insertText("×");
        }
        if(f == "÷") {
            Div = true;
            ui->plainTextEdit->textCursor().insertText("÷");
        }
        if(f == "^") {
            Pow = true;
            ui->plainTextEdit->textCursor().insertText("^");
        }
    }

    if(f == "←") {
        texT = ui->plainTextEdit->toPlainText();
        texT.chop(1);
        Add = Sub = Mul = Div = Pow = false;
        matchFh();
        ui->plainTextEdit->setPlainText(texT);
        ui->plainTextEdit->moveCursor(QTextCursor::End);
    }
    if(f == "AC") {
        Add = Sub = Mul = Div = Pow = false;
        parenLevel = 0;
        parenStack.clear();
        ui->plainTextEdit->setPlainText("");
    }
}

double MainWindow::evaluateExpression(QString expr)
{
    // 处理括号
    QRegularExpression rx("\\(([^()]+)\\)");
    QRegularExpressionMatch match;
    while((match = rx.match(expr)).hasMatch()) {
        QString inner = match.captured(1);
        double val = evaluateExpression(inner);
        expr.replace(match.captured(0), QString::number(val));
    }

    // 处理次方
    rx.setPattern("(-?\\d+\\.?\\d*)\\^(-?\\d+\\.?\\d*)");
    while((match = rx.match(expr)).hasMatch()) {
        double base = match.captured(1).toDouble();
        double exp = match.captured(2).toDouble();
        expr.replace(match.captured(0), QString::number(pow(base, exp)));
    }

    // 处理乘除
    rx.setPattern("(-?\\d+\\.?\\d*)([×÷])(-?\\d+\\.?\\d*)");
    while((match = rx.match(expr)).hasMatch()) {
        double left = match.captured(1).toDouble();
        QString op = match.captured(2);
        double right = match.captured(3).toDouble();
        double res = (op == "×") ? left * right : left / right;
        expr.replace(match.captured(0), QString::number(res));
    }

    // 处理加减
    rx.setPattern("(-?\\d+\\.?\\d*)([+-])(-?\\d+\\.?\\d*)");
    while((match = rx.match(expr)).hasMatch()) {
        double left = match.captured(1).toDouble();
        QString op = match.captured(2);
        double right = match.captured(3).toDouble();
        double res = (op == "+") ? left + right : left - right;
        expr.replace(match.captured(0), QString::number(res));
    }

    return expr.toDouble();
}

void MainWindow::equalNum()
{
    if(parenLevel != 0) {
        ui->plainTextEdit->setPlainText("错误: 括号不匹配");
        parenLevel = 0;
        parenStack.clear();
        return;
    }

    texT = ui->plainTextEdit->toPlainText();
    try {
        double result = evaluateExpression(texT);
        history << QString("%1 = %2").arg(texT).arg(result);
        ui->plainTextEdit->setPlainText(QString::number(result));
        a = QString::number(result);
        Add = Sub = Mul = Div = Pow = false;
    } catch(...) {
        ui->plainTextEdit->setPlainText("错误: 表达式无效");
    }

    ui->plainTextEdit->moveCursor(QTextCursor::End);
}

void MainWindow::sciFunc()
{
    QToolButton *btn = (QToolButton*)sender();
    QString func = btn->text();

    QString text = ui->plainTextEdit->toPlainText();
    bool ok;
    double num = text.toDouble(&ok);

    if(!ok) {
        ui->plainTextEdit->setPlainText("Error");
        return;
    }

    if(func == "√") {
        if(num >= 0) {
            double result = sqrt(num);
            ui->plainTextEdit->setPlainText(QString::number(result));
            history << QString("√%1 = %2").arg(num).arg(result);
        } else {
            ui->plainTextEdit->setPlainText("Error");
        }
    }
    else if(func == "sin") {
        double result = sin(num * M_PI / 180);
        ui->plainTextEdit->setPlainText(QString::number(result));
        history << QString("sin(%1°) = %2").arg(num).arg(result);
    }
    else if(func == "cos") {
        double result = cos(num * M_PI / 180);
        ui->plainTextEdit->setPlainText(QString::number(result));
        history << QString("cos(%1°) = %2").arg(num).arg(result);
    }
    else if(func == "tan") {
        if(cos(num * M_PI / 180) != 0) {
            double result = tan(num * M_PI / 180);
            ui->plainTextEdit->setPlainText(QString::number(result));
            history << QString("tan(%1°) = %2").arg(num).arg(result);
        } else {
            ui->plainTextEdit->setPlainText("Error");
        }
    }
}

void MainWindow::percentFunc()
{
    QString text = ui->plainTextEdit->toPlainText();
    bool ok;
    double value = text.toDouble(&ok);

    if(ok) {
        double result = value / 100.0;
        ui->plainTextEdit->setPlainText(QString::number(result));
        history << QString("%1% = %2").arg(value).arg(result);
    } else {
        ui->plainTextEdit->setPlainText("Error");
    }
}

void MainWindow::constantFunc()
{
    QToolButton *btn = (QToolButton*)sender();
    QString constant = btn->text();

    if(constant == "π") {
        ui->plainTextEdit->textCursor().insertText(QString::number(M_PI));
    }
    else if(constant == "e") {
        ui->plainTextEdit->textCursor().insertText(QString::number(M_E));
    }
}

void MainWindow::moveCursorLeft()
{
    ui->plainTextEdit->moveCursor(QTextCursor::Left);
}

void MainWindow::moveCursorRight()
{
    ui->plainTextEdit->moveCursor(QTextCursor::Right);
}

void MainWindow::backspace()
{
    ui->plainTextEdit->textCursor().deletePreviousChar();
}

void MainWindow::showHistory()
{
    QDialog *historyDialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout;
    QTextEdit *historyDisplay = new QTextEdit;

    historyDisplay->setReadOnly(true);
    historyDisplay->setPlainText(history.join("\n"));

    layout->addWidget(historyDisplay);
    historyDialog->setLayout(layout);
    historyDialog->resize(400, 300);
    historyDialog->setWindowTitle("计算历史");
    historyDialog->exec();
}

MainWindow::~MainWindow()
{
    delete ui;
    parenStack.clear();
}
