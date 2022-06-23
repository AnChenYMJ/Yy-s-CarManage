#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase> //数据库
#include <QSqlError>    //数据库错误提示
#include <QMessageBox>  //对话框
#include <QSqlQueryModel>   //解决下拉框的
#include <QSqlQuery>    //数据库操作
#include "domxml.h"     //添加我们写的头文件

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void connectDB();   //连接数据库
    void initData();    //初始化

private slots:
    void on_actionCar_triggered();

    void on_actionCalc_triggered();

    void on_comboBoxFactory_currentIndexChanged(const QString &arg1);

    void on_comboBox_brand_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_Button_cancel_clicked();

    void on_Button_sure_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
