#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //开始 开始就打开车辆管理的页面
    on_actionCar_triggered();

    //打开数据库,(调用我们封装好的函数)
    connectDB();

    //初始化数据（刚开始没有选择厂家前）
    initData();

    //在上一级目录下生成demo.xml文件
    DomXML::createXML("../demo.xml");

//    //测试用
//    //添加子节点调用测试
//    QStringList list;
//    list << "二汽神龙" << "毕加索" << "39" << "1" << "39";
//    DomXML::appendXML("../demo.xml", list);


    //测试histogram 柱状图(未关联数据库)
    ui->widget_H->AddItem("大众", 8);
    ui->widget_H->AddItem("小蜜", 25);
    ui->widget_H->AddItem("铃木", 11);
    //设定坐标最大值
    ui->widget_H->SetMaxValue(30);

    //测试扇形图（未关联数据库）
    QVector<SectorInfo> result;
    SectorInfo info;
    info.description = "已经出售";
    info.percent = 0.3*100;
    result.push_back(info);

    info.description = "剩余";
    info.percent = (1 - 0.3)*100;
    result.push_back(info);

    ui->widget_P->setData(result);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//车辆管理按钮
void MainWindow::on_actionCar_triggered()
{
    //切换到车辆管理的页面
    ui->stackedWidget->setCurrentWidget(ui->car);
    //更改跳到的界面的标题
    ui->label->setText("车辆管理");
}

//销售统计菜单
void MainWindow::on_actionCalc_triggered()
{
    //切换到销售统计的页面
    ui->stackedWidget->setCurrentWidget(ui->calc);
    ui->label->setText("销售统计");
}

//连接数据库
void MainWindow::connectDB()
{
    //添加数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");    //主机名或IP，用NP可查看主机名，回环也可用127.0.0.1
    db.setUserName("root");
    db.setPassword("");
    db.setDatabaseName("car");

    //打开数据库
    if(!db.open())
    {
        //打印错误
        QMessageBox::warning(this, "数据库打开失败", db.lastError().text());
        return;
    }
}

//初始化数据
void MainWindow::initData()
{
    //厂家下拉框初始化
    QSqlQueryModel *queryModel = new QSqlQueryModel(this);  //新建模型
    queryModel->setQuery("select name from factory");     //模型中执行sql语句
    //用法与可视化数据库一样的  模型与视图 将列名添加到厂家下拉框
    ui->comboBoxFactory->setModel(queryModel);


    ui->lineEdit_Last->setText("0");        //剩余数量初始化   刚开始没有选择前
    ui->lineEdit_Total->setEnabled(false);  //金额


}

//厂家下拉框 的槽函数，选择的信号为currentIndexChanged(QString)（当前显示的内容）
void MainWindow::on_comboBoxFactory_currentIndexChanged(const QString &arg1)
{
    //"请选择厂家" 的选项
    if(arg1 == "请选择厂家")
    {
        //内容清空
        ui->comboBox_brand->clear();    //品牌下拉框清空
        ui->lineEdit_price->clear();    //报价清空
        ui->lineEdit_Last->setText("0");//剩余数量清空
        ui->lineEdit_Total->clear();    //金额清空
        ui->spinBox->setValue(0);       //数量选择框
        ui->spinBox->setEnabled(false); //初始化时，无法选择销售数量
        ui->Button_sure->setEnabled(false);//没有销售数量时，不能按确定按钮
    }
    //其它的厂商选项
    else
    {
        //检索内容添加到'品牌'的下拉框,但发现更新时，旧的没数据删除，所以要在更新数据时，清空一下
        ui->comboBox_brand->clear();
        //创建执行对象
        QSqlQuery query;
        QString sql = QString("select name from brand where factory = '%1'").arg(arg1);//arg1是参数不是函数
        //select name from brand where factory = '一汽大众'
        //执行sql语句
        query.exec(sql);

        //获取内容
        while(query.next())
        {
            //获取检索内容
            QString name = query.value(0).toString();
            //检索内容添加到'品牌'的下拉框,但发现更新时，旧的没数据删除，所以要在更新数据时，清空一下
            ui->comboBox_brand->addItem(name);
        }
        //当选择了厂商与品牌之后，就能选择销售数量的框了
        ui->spinBox->setEnabled(true);
    }
}

//品牌下拉框的槽函数，信号选择与厂商下拉框一样
void MainWindow::on_comboBox_brand_currentIndexChanged(const QString &arg1)
{
    //由于要在数据库检索对应内容，需要厂商、品牌两个数据确定了才能精确找到对应的行
    //创建执行对象
    QSqlQuery query;
    QString sql = QString("select price, last from brand where factory = '%1' and name = '%2'")
            .arg(ui->comboBoxFactory->currentText())//厂家下拉框当前选中的内容
            .arg(arg1);
    //类似于执行了：select price, last from brand where factory = '一汽大众' and name = '奥迪';
    //执行sql语句
    query.exec(sql);

    //获取内容  (其实只获取了一行，但用while也没啥)
    while(query.next())
    {
        //获取报价
        int price = query.value("price").toInt();
        //剩余数
        int last = query.value("last").toInt();

        //更新到对应的空间
        ui->lineEdit_price->setText(QString::number(price));    //QString::number 整形转字符串
        ui->lineEdit_Last->setText(QString::number(last));      //剩余数量
    }
}

//数量选择框的槽函数，信号用valueChanged
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    //1)获取厂家
    QString factoryStr = ui->comboBoxFactory->currentText();
    //2)品牌
    QString brandStr = ui->comboBox_brand->currentText();

    //数量为0时，确认按钮不可按
    if(0 == arg1)
        ui->Button_sure->setEnabled(false);
    else
        ui->Button_sure->setEnabled(true);

    //获取数据库剩余数量
    QSqlQuery query;
    QString sql = QString("select last from brand where factory = '%1' and name = '%2'")
            .arg(factoryStr)
            .arg(brandStr);
    //执行数据库
    query.exec(sql);
    //获取数据库剩余数量
    int last;
    while(query.next())
    {
        last = query.value("last").toInt(); //剩余
    }

    //若选择数目大于了 实际剩余数目，中断程序
    if(arg1 > last)
    {
        ui->spinBox->setValue(last);    //给选择框设置最大值
        return;
    }
    //还不够，剩余数量显示不对，当arg1 == 0 时显示不对
    if(arg1 == 0)
    {

        return;
    }

    //更新剩余数量
    int tempNum = last - arg1;
    ui->lineEdit_Last->setText(QString::number(tempNum));

    //更新金额
    int price = ui->lineEdit_price->text().toInt(); //获取报价
    int sum = price * arg1;                         //报价 * 销售的数量
    ui->lineEdit_Total->setText(QString::number(sum));//总额
}

//取消按钮
void MainWindow::on_Button_cancel_clicked()
{
    //直接调用槽函数，传参保证它是执行初始化 即可
    on_comboBoxFactory_currentIndexChanged("请选择厂家");
    //要把厂家下拉框也设置为"请选择厂家"才行
    ui->comboBoxFactory->setCurrentIndex(0);
    //将剩余数量初始化成0
    ui->lineEdit_Last->setText("0");
}

//确定按钮
void MainWindow::on_Button_sure_clicked()
{
    //获取信息
    //1）销售数量
    int num = ui->spinBox->value();
    //2)剩余数量
    int last = ui->lineEdit_Last->text().toInt();

    //获取数据库的销量
    QSqlQuery query;
    QString sql = QString("select sell from brand where factory = '%1' and name = '%2'")
            .arg(ui->comboBoxFactory->currentText())
            .arg(ui->comboBox_brand->currentText());
    //执行数据库
    query.exec(sql);
    //获取数据库销量
    int sell;
    while(query.next())
    {
        sell = query.value("sell").toInt();
    }

    //更新数据库，剩余数量，销售总量
    //注意：当销售数量为0的时候，不能按确定按钮，所以在初始化时，将确定按钮改为不可按
    sell += num;

    sql = QString("update brand set sell = %1, last = %2 where factory = '%3' and name = '%4'")
                .arg(sell)
                .arg(last)
                .arg(ui->comboBoxFactory->currentText())
                .arg(ui->comboBox_brand->currentText());
    //执行sql语句
    query.exec(sql);

    //把确认修改后的数据，更新到xml文件中
    QStringList list;
    list << ui->comboBoxFactory->currentText()
         << ui->comboBox_brand->currentText()
         << ui->lineEdit_price->text()
         << QString::number(num)
         << ui->lineEdit_Total->text();
    DomXML::appendXML("../demo.xml", list);

    //将xml文件中的当天信息给取出来打印
    QStringList fList;
    QStringList bList;
    QStringList pList;
    QStringList nList;
    QStringList tList;
    DomXML::readXML("../demo.xml", fList, bList, pList, nList, tList);
    for(int i = 0; i < fList.size(); i++)
    {
        QString str = QString("%1:%2:卖出了%3, 单价：%4, 总价：%5")
                .arg(fList.at(i))
                .arg(bList.at(i))
                .arg(pList.at(i))
                .arg(nList.at(i))
                .arg(tList.at(i));
        //编辑区打印
        ui->textEdit->append(str);
        qDebug() << str.toUtf8().data();
    }

    //按完以后初始化
    ui->Button_sure->setEnabled(false);
    on_Button_cancel_clicked();
}
