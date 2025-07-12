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
#include<QInputDialog>
#include <QLabel>


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
    buttonLayout->addWidget(ui->AC, 1, 3);

    // 第三行: 数字7-9
    buttonLayout->addWidget(ui->num7, 2, 0);
    buttonLayout->addWidget(ui->num8, 2, 1);
    buttonLayout->addWidget(ui->num9, 2, 2);
    buttonLayout->addWidget(ui->jia, 2, 3);

    // 第四行: 数字4-6
    buttonLayout->addWidget(ui->num4, 3, 0);
    buttonLayout->addWidget(ui->num5, 3, 1);
    buttonLayout->addWidget(ui->num6, 3, 2);
    buttonLayout->addWidget(ui->jian, 3, 3);

    // 第五行: 数字1-3
    buttonLayout->addWidget(ui->num1, 4, 0);
    buttonLayout->addWidget(ui->num2, 4, 1);
    buttonLayout->addWidget(ui->num3, 4, 2);
    buttonLayout->addWidget(ui->cheng, 4, 3);

    // 第六行: 导航和数字0
    buttonLayout->addWidget(ui->zuoyi, 5, 0);      // 左移
    buttonLayout->addWidget(ui->num0, 5, 1);       // 0
    buttonLayout->addWidget(ui->youyi, 5, 2);      // 右移
    buttonLayout->addWidget(ui->chu, 5, 3);       // =

    // 第七行: 功能按钮和括号
    leftParenBtn = new QToolButton();
    leftParenBtn->setText("(");
    rightParenBtn = new QToolButton();
    rightParenBtn->setText(")");

    buttonLayout->addWidget(ui->shanchu, 6, 0);    // 退格
    buttonLayout->addWidget(leftParenBtn, 6, 1);   // 左括号
    buttonLayout->addWidget(rightParenBtn, 6, 2);  // 右括号
    buttonLayout->addWidget(ui->deng, 6, 3);         // AC

    // 第八行：其他功能按钮
    buttonLayout->addWidget(ui->historyBtn, 7, 0, 1, 4); // 历史（跨4列）

    calcLayout->addWidget(calcButtons);

    // 2. 统计模式
    statsWidget = new QWidget();
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);

    dataTable = new QTableWidget();
    dataTable->setColumnCount(1);
    dataTable->setHorizontalHeaderLabels(QStringList() << "数据值");
    dataTable->horizontalHeader()->setStretchLastSection(true);
    statsLayout->addWidget(dataTable);

    // 添加按钮行布局
    QHBoxLayout *statsButtonsLayout = new QHBoxLayout();
    addDataButton = new QPushButton("添加数据行");
    connect(addDataButton, &QPushButton::clicked, this, &MainWindow::addDataRow);
    statsButtonsLayout->addWidget(addDataButton);

    calculateStatsButton = new QPushButton("计算统计量");
    connect(calculateStatsButton, &QPushButton::clicked, this, &MainWindow::calculateStatistics);
    statsButtonsLayout->addWidget(calculateStatsButton);

    clearDataButton = new QPushButton("清除所有数据");
    connect(clearDataButton, &QPushButton::clicked, this, &MainWindow::clearData);
    statsButtonsLayout->addWidget(clearDataButton);

    statsLayout->addLayout(statsButtonsLayout);
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

    // 3. 矩阵计算模式
    matrixWidget = new QWidget();
    QVBoxLayout *matrixLayout = new QVBoxLayout(matrixWidget);

    // 矩阵操作按钮
    QHBoxLayout *matrixControlLayout = new QHBoxLayout();

    createMatrixAButton = new QPushButton("创建矩阵A");
    connect(createMatrixAButton, &QPushButton::clicked, this, [=]() {
        createMatrix(matrixATable);
    });
    matrixControlLayout->addWidget(createMatrixAButton);

    createMatrixBButton = new QPushButton("创建矩阵B");
    connect(createMatrixBButton, &QPushButton::clicked, this, [=]() {
        createMatrix(matrixBTable);
    });
    matrixControlLayout->addWidget(createMatrixBButton);

    addMatricesButton = new QPushButton("矩阵加法 (A+B)");
    connect(addMatricesButton, &QPushButton::clicked, this, &MainWindow::matrixAddition);
    matrixControlLayout->addWidget(addMatricesButton);

    subtractMatricesButton = new QPushButton("矩阵减法 (A-B)");
    connect(subtractMatricesButton, &QPushButton::clicked, this, &MainWindow::matrixSubtraction);
    matrixControlLayout->addWidget(subtractMatricesButton);

    multiplyMatricesButton = new QPushButton("矩阵乘法 (A×B)");
    connect(multiplyMatricesButton, &QPushButton::clicked, this, &MainWindow::matrixMultiplication);
    matrixControlLayout->addWidget(multiplyMatricesButton);

    determinantButton = new QPushButton("计算A的行列式");
    connect(determinantButton, &QPushButton::clicked, this, [=]() {
        QVector<QVector<double>> data;
        if (getMatrixData(matrixATable, data) && data.size() == data[0].size()) {
            double det = calculateDeterminant(data);
            matrixResult->setText(QString("矩阵A的行列式值: %1").arg(det));
        } else {
            matrixResult->setText("错误: 请先创建一个方阵");
        }
    });
    matrixControlLayout->addWidget(determinantButton);

    clearMatrixButton = new QPushButton("清除矩阵");
    connect(clearMatrixButton, &QPushButton::clicked, this, [=]() {
        if (matrixATable) matrixATable->clear();
        if (matrixBTable) matrixBTable->clear();
        matrixResult->clear();
    });
    matrixControlLayout->addWidget(clearMatrixButton);

    matrixLayout->addLayout(matrixControlLayout);

    // 矩阵显示区域
    QHBoxLayout *matricesLayout = new QHBoxLayout();

    // 矩阵A
    QVBoxLayout *matrixALayout = new QVBoxLayout();
    matrixALayout->addWidget(new QLabel("矩阵A"));
    matrixATable = new QTableWidget();
    matrixATable->horizontalHeader()->setStretchLastSection(true);
    matrixALayout->addWidget(matrixATable);
    matricesLayout->addLayout(matrixALayout);

    // 操作符显示
    matricesLayout->addWidget(new QLabel("    "));

    // 矩阵B
    QVBoxLayout *matrixBLayout = new QVBoxLayout();
    matrixBLayout->addWidget(new QLabel("矩阵B"));
    matrixBTable = new QTableWidget();
    matrixBTable->horizontalHeader()->setStretchLastSection(true);
    matrixBLayout->addWidget(matrixBTable);
    matricesLayout->addLayout(matrixBLayout);

    matrixLayout->addLayout(matricesLayout);

    // 结果区域
    matrixLayout->addWidget(new QLabel("结果:"));
    matrixResult = new QTextEdit();
    matrixResult->setReadOnly(true);
    matrixLayout->addWidget(matrixResult);

    // 添加到堆叠窗口
    stackedWidget->addWidget(matrixWidget);

    // 在工具栏添加矩阵模式切换按钮
    modeToolBar->addAction("矩阵计算", this, &MainWindow::switchToMatrixMode);


    // 默认显示计算器
    switchToCalculator();
    addDataRow();

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
    connect(leftParenBtn, &QToolButton::clicked, this, &MainWindow::parenFunc);
    connect(rightParenBtn, &QToolButton::clicked, this, &MainWindow::parenFunc);
    connect(ui->zuoyi, &QToolButton::clicked, this, &MainWindow::moveCursorLeft);
    connect(ui->youyi, &QToolButton::clicked, this, &MainWindow::moveCursorRight);
    connect(ui->shanchu, &QToolButton::clicked, this, &MainWindow::backspace);

    // 设置样式
    this->setStyleSheet(
        "QToolButton, QPushButton { "
        "font-size: 22px; "
        "padding: 8px; "
        "min-width: 55px; "
        "min-height: 38px; "
        "border: 2px solid #ccc; "
        "border-radius: 4px; "
        "background-color: white; "
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
        "QToolButton[text='('], QToolButton[text=')'] { "
        "background-color: #f0f0f0; "
        "font-weight: bold; "
        "}"
        "QToolButton[text='('], QToolButton[text=')'] { "
            "background-color: #f0f0f0; "
            "font-weight: bold; "
            "}"
            "QToolButton[text='⌫'] { "  // 退格按钮特殊样式
            "background-color: #ffecec; "
            "}"
    );
}
// 切换到矩阵模式
void MainWindow::switchToMatrixMode() {
    stackedWidget->setCurrentWidget(matrixWidget);
    this->setWindowTitle("多功能计算器 - 矩阵模式");
}

// 创建矩阵
void MainWindow::createMatrix(QTableWidget *&matrixTable) {
    bool ok;
    int rows = QInputDialog::getInt(this, "矩阵行数", "请输入行数:", 2, 1, 10, 1, &ok);
    if (!ok) return;

    int cols = QInputDialog::getInt(this, "矩阵列数", "请输入列数:", 2, 1, 10, 1, &ok);
    if (!ok) return;

    if (!matrixTable) {
        matrixTable = new QTableWidget();
    }

    matrixTable->setRowCount(rows);
    matrixTable->setColumnCount(cols);

    // 设置表头
    QStringList headers;
    for (int i = 0; i < cols; ++i) {
        headers << QString("%1").arg(i + 1);
    }
    matrixTable->setHorizontalHeaderLabels(headers);

    // 初始化单元格
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrixTable->setItem(i, j, new QTableWidgetItem("0"));
        }
    }

    matrixTable->resizeColumnsToContents();
}

// 获取矩阵数据
bool MainWindow::getMatrixData(QTableWidget *matrixTable, QVector<QVector<double>> &data) {
    if (!matrixTable || matrixTable->rowCount() == 0 || matrixTable->columnCount() == 0) {
        return false;
    }

    data.clear();
    for (int i = 0; i < matrixTable->rowCount(); ++i) {
        QVector<double> rowData;
        bool valid = true;
        for (int j = 0; j < matrixTable->columnCount(); ++j) {
            QTableWidgetItem *item = matrixTable->item(i, j);
            if (!item || item->text().isEmpty()) {
                valid = false;
                break;
            }
            bool ok;
            double value = item->text().toDouble(&ok);
            if (!ok) {
                valid = false;
                break;
            }
            rowData.append(value);
        }
        if (!valid) {
            matrixResult->setText("错误: 矩阵数据无效");
            data.clear();
            return false;
        }
        data.append(rowData);
    }
    return true;
}

// 矩阵加法
void MainWindow::matrixAddition() {
    QVector<QVector<double>> a, b;
    if (!getMatrixData(matrixATable, a) || !getMatrixData(matrixBTable, b)) {
        matrixResult->setText("错误: 请先创建有效的矩阵");
        return;
    }

    if (a.size() != b.size() || a[0].size() != b[0].size()) {
        matrixResult->setText("错误: 矩阵维度不匹配，无法相加");
        return;
    }

    QString result;
    result += "矩阵A + 矩阵B = \n\n";
    for (int i = 0; i < a.size(); ++i) {
        result += "[";
        for (int j = 0; j < a[i].size(); ++j) {
            result += QString::number(a[i][j] + b[i][j], 'f', 4);
            if (j < a[i].size() - 1) result += ", ";
        }
        result += "]\n";
    }
    matrixResult->setText(result);
}

// 矩阵减法
void MainWindow::matrixSubtraction() {
    QVector<QVector<double>> a, b;
    if (!getMatrixData(matrixATable, a) || !getMatrixData(matrixBTable, b)) {
        matrixResult->setText("错误: 请先创建有效的矩阵");
        return;
    }

    if (a.size() != b.size() || a[0].size() != b[0].size()) {
        matrixResult->setText("错误: 矩阵维度不匹配，无法相减");
        return;
    }

    QString result;
    result += "矩阵A - 矩阵B = \n\n";
    for (int i = 0; i < a.size(); ++i) {
        result += "[";
        for (int j = 0; j < a[i].size(); ++j) {
            result += QString::number(a[i][j] - b[i][j], 'f', 4);
            if (j < a[i].size() - 1) result += ", ";
        }
        result += "]\n";
    }
    matrixResult->setText(result);
}

// 矩阵乘法
void MainWindow::matrixMultiplication() {
    QVector<QVector<double>> a, b;
    if (!getMatrixData(matrixATable, a) || !getMatrixData(matrixBTable, b)) {
        matrixResult->setText("错误: 请先创建有效的矩阵");
        return;
    }

    if (a[0].size() != b.size()) {
        matrixResult->setText("错误: 矩阵维度不匹配，无法相乘");
        return;
    }

    // 初始化结果矩阵
    QVector<QVector<double>> result(a.size(), QVector<double>(b[0].size(), 0));

    // 计算矩阵乘积
    for (int i = 0; i < a.size(); ++i) {
        for (int j = 0; j < b[0].size(); ++j) {
            for (int k = 0; k < a[0].size(); ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    // 显示结果
    QString resultText;
    resultText += "矩阵A × 矩阵B = \n\n";
    for (int i = 0; i < result.size(); ++i) {
        resultText += "[";
        for (int j = 0; j < result[i].size(); ++j) {
            resultText += QString::number(result[i][j], 'f', 4);
            if (j < result[i].size() - 1) resultText += ", ";
        }
        resultText += "]\n";
    }
    matrixResult->setText(resultText);
}

// 计算行列式（仅适用于方阵）
double MainWindow::calculateDeterminant(QVector<QVector<double>> matrix) {
    int n = matrix.size();
    if (n == 1) return matrix[0][0];

    double det = 0;
    int sign = 1;

    for (int i = 0; i < n; ++i) {
        // 创建余子式矩阵
        QVector<QVector<double>> minor;
        for (int j = 1; j < n; ++j) {
            QVector<double> row;
            for (int k = 0; k < n; ++k) {
                if (k != i) {
                    row.append(matrix[j][k]);
                }
            }
            minor.append(row);
        }

        det += sign * matrix[0][i] * calculateDeterminant(minor);
        sign *= -1;
    }

    return det;
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
// 清除数据的函数
void MainWindow::clearData()
{
    dataTable->setRowCount(0);  // 清除所有行
    statsResults->clear();      // 清除统计结果
    addDataRow();               // 添加一个空行方便输入
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

    // 计算方差和标准差
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
    resultText += QString("方差: %1\n").arg(variance, 0, 'f', 4);  // 新增方差
    resultText += QString("标准差: %1\n").arg(stddev, 0, 'f', 4);

    statsResults->setText(resultText);
}

// 计算器功能实现
void MainWindow::numOnClick()
{
    QToolButton *numName = (QToolButton*)sender();
    ui->plainTextEdit->textCursor().insertText(numName->text());
}


void MainWindow::matchFh()
{
    if(text.contains("+", Qt::CaseSensitive)) {
        QStringList t = text.split("+");
        a = t[0];
        b = t[1];
        Add = true;
    }
    else if(text.contains("-", Qt::CaseSensitive)) {
        QStringList t = text.split("-");
        a = t[0];
        b = t[1];
        Sub = true;
    }
    else if(text.contains("×", Qt::CaseSensitive)) {
        QStringList t = text.split("×");
        a = t[0];
        b = t[1];
        Mul = true;
    }
    else if(text.contains("÷", Qt::CaseSensitive)) {
        QStringList t = text.split("÷");
        a = t[0];
        b = t[1];
        Div = true;
    }
    else if(text.contains("^", Qt::CaseSensitive)) {
        QStringList t = text.split("^");
        a = t[0];
        b = t[1];
        Pow = true;
    }
    else {
        a = text;
    }
}

void MainWindow::fuHao()
{
    QToolButton *fh = (QToolButton*)sender();
    QString f = fh->text();

    if(f == "←") {
        ui->plainTextEdit->textCursor().deletePreviousChar();
    }
    else if(f == "AC") {
        ui->plainTextEdit->setPlainText("");
    }
    else {
        ui->plainTextEdit->textCursor().insertText(f);
    }
}


double MainWindow::evaluateExpression(QString expr)
{
    // 处理空格
    expr = expr.replace(" ", "");

    // 处理百分比 - 新改进的逻辑
    // 先处理带%的数字，将其转换为除以100的形式
    QRegularExpression percentRx("(-?\\d+\\.?\\d*)%");
    QRegularExpressionMatchIterator it = percentRx.globalMatch(expr);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString numStr = match.captured(1);
        double num = numStr.toDouble();
        expr.replace(match.captured(0), QString("(%1÷100)").arg(num));
    }

    // 处理π和e
    expr = expr.replace("π", QString::number(M_PI));
    expr = expr.replace("e", QString::number(M_E));

    // 处理括号
    QRegularExpression rx("\\(([^()]+)\\)");
    QRegularExpressionMatch match;
    while((match = rx.match(expr)).hasMatch()) {
        QString inner = match.captured(1);
        double val = evaluateExpression(inner);
        expr.replace(match.captured(0), QString::number(val));
    }

    // 处理函数
    rx.setPattern("(sin|cos|tan|√)(-?\\d+\\.?\\d*)");
    while((match = rx.match(expr)).hasMatch()) {
        QString func = match.captured(1);
        double num = match.captured(2).toDouble();
        double res = 0;

        if(func == "sin") res = sin(num * M_PI / 180);
        else if(func == "cos") res = cos(num * M_PI / 180);
        else if(func == "tan") res = tan(num * M_PI / 180);
        else if(func == "√") res = sqrt(num);

        expr.replace(match.captured(0), QString::number(res));
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
    QString expr = ui->plainTextEdit->toPlainText();

    if(expr.isEmpty()) return;

    try {
        double result = evaluateExpression(expr);
        history << QString("%1 = %2").arg(expr).arg(result);
        ui->plainTextEdit->setPlainText(QString::number(result));
    } catch(...) {
        ui->plainTextEdit->setPlainText("错误: 表达式无效");
    }
}

void MainWindow::sciFunc()
{
    QToolButton *btn = (QToolButton*)sender();
    QString func = btn->text();
    ui->plainTextEdit->textCursor().insertText(func);
}

void MainWindow::percentFunc()
{
    // 简单插入%符号，不立即计算
    ui->plainTextEdit->textCursor().insertText("%");
}


void MainWindow::constantFunc()
{
    QToolButton *btn = (QToolButton*)sender();
    QString constant = btn->text();
    ui->plainTextEdit->textCursor().insertText(constant);
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
// 新增括号功能实现
void MainWindow::parenFunc()
{
    QToolButton *btn = (QToolButton*)sender();
    QString paren = btn->text();

    // 插入括号并更新括号层级
    ui->plainTextEdit->textCursor().insertText(paren);
    if (paren == "(") {
        parenLevel++;
    } else if (paren == ")") {
        if (parenLevel > 0) {
            parenLevel--;
        } else {
            // 右括号多于左括号时给出提示
            ui->plainTextEdit->setPlainText(ui->plainTextEdit->toPlainText() + " 括号不匹配!");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    parenStack.clear();
}
