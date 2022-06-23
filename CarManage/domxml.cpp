#include "domxml.h"

DomXML::DomXML()
{
}

//创建XML空文件
void DomXML::createXML(QString filePath)
{
    QFile file(filePath);       //关联文件名字
    if(true == file.exists() )  //如果存在则不创建
    {
        cout << "文件已存在";
        return;
    }
    else    //不存在才创建
    {
        //只写方式打开文件
        bool isOK = file.open(QIODevice::WriteOnly);
        if(true == isOK)    //打开成功
        {
            //创建xml文档对象
            QDomDocument doc;
            //创建xml头部格式 <?xml version="1.0" encoding="UTF-8"?>
            QDomProcessingInstruction ins;
            ins = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""); //参1自动生成<?xml ?> 参2则是补充剩下的 注意需要加\来转义
            //追加元素
            doc.appendChild(ins);

            //创建根节点元素
            QDomElement root = doc.createElement("日销售清单");
            doc.appendChild(root);  //追加元素

            //保存，文本流
            QTextStream stream(&file); //文本流关联文件
            doc.save(stream, 4);  //4代表缩进字符，缩进多少

            //关闭文件
            file.close();
        }
        else
        {
            //编程习惯：打印错误提示
            cout << "WriteOnly error";
            return;
        }
    }
}

//添加子节点
void DomXML::appendXML(QString filePath, QStringList list)
{
    //打开文件
    QFile file(filePath);
    bool isOK = file.open(QIODevice::ReadOnly);
    if(true == isOK)    //打开成功
    {
        //创建xml文档对象
        QDomDocument doc;
        //file和xml文档对象关联
        isOK = doc.setContent(&file);
        if(isOK)    //如果关联成功
        {
            //文件已经读到文档doc里，此时可以关闭了
            file.close();   //关闭文件
            //获取根节点元素
            QDomElement root = doc.documentElement();

            //获取当前日期,并转换成字符串格式
            QDateTime date = QDateTime::currentDateTime();
            QString dateStr = date.toString("yyyy-MM-dd");

            //判断根节点下有无子节点
            if(root.hasChildNodes())    //如果有子节点
            {
                //查找最后一个子结点
                QDomElement lastEmt = root.lastChildElement();
                if(lastEmt.attribute("date") == dateStr)
                {   //判断有没有当天日期
                    //有当天的话，则在当天的日期子节点后添加时间节点写有效数据 即可
                    writeXML(doc, lastEmt, list);

                }
                else//没有当天日期(则添加新的日期子节点)
                {
                    //创建<日期>子节点
                    QDomElement dateEmt = doc.createElement("日期");
                    //给<日期>子节点设置属性
                    QDomAttr dateAttr = doc.createAttribute("date");
                    //给属性设置一个值
                    dateAttr.setNodeValue(dateStr);
                    //把节点与属性关联起来
                    dateEmt.setAttributeNode(dateAttr);
                    //把日期节点追加到根节点上
                    root.appendChild(dateEmt);

                    //写有效数据(封装到下面的函数中了，包含创建时间节点等)
                    writeXML(doc, dateEmt, list);
                }
            }
            else    //没有子节点
            {
                //创建<日期>子节点
                QDomElement dateEmt = doc.createElement("日期");
                //给<日期>子节点设置属性
                QDomAttr dateAttr = doc.createAttribute("date");
                //给属性设置一个值
                dateAttr.setNodeValue(dateStr);
                //把节点与属性关联起来
                dateEmt.setAttributeNode(dateAttr);
                //把日期节点追加到根节点上
                root.appendChild(dateEmt);

                //写有效数据(封装到下面的函数中了)
                writeXML(doc, dateEmt, list);
            }
        }
        else
        {
            cout << "setContent error";
            return;
        }

        //保存文件
        isOK = file.open(QIODevice::WriteOnly);
        if(isOK)
        {
            QTextStream stream(&file);
            doc.save(stream, 4);    //4为格式
            file.close();
        }
    }
    else
    {
        cout << "ReadOnly error";
    }
}

//写有效数据，子节点后添加时间等节点元素
void DomXML::writeXML(QDomDocument &doc, QDomElement &root, QStringList &list)
{
    //获取当前时间
    QDateTime time = QDateTime::currentDateTime();
    QString timeStr = time.toString("hh-mm-ss");    //时间是时分秒，日期是年月日

    //创建时间节点元素
    QDomElement timeEmt = doc.createElement("时间");
    //创建属性
    QDomAttr timeAttr = doc.createAttribute("time");
    //给属性设置值
    timeAttr.setNodeValue(timeStr);
    //时间节点元素和属性关联
    timeEmt.setAttributeNode(timeAttr);
    //把时间节点追加到日期节点后面
    root.appendChild(timeEmt);

    //创建节点元素(无属性，有文本内容)
    QDomElement factory = doc.createElement("厂家");
    QDomElement brand = doc.createElement("品牌");
    QDomElement price = doc.createElement("报价");
    QDomElement num = doc.createElement("数量");
    QDomElement total = doc.createElement("金额");
    //给元素添加文本内容空间
    QDomText text = doc.createTextNode(list.at(0));
    factory.appendChild(text);

    text = doc.createTextNode(list.at(1));
    brand.appendChild(text);

    text = doc.createTextNode(list.at(2));
    price.appendChild(text);

    text = doc.createTextNode(list.at(3));
    num.appendChild(text);

    text = doc.createTextNode(list.at(4));
    total.appendChild(text);

    //元素追加到时间节点后
    timeEmt.appendChild(factory);
    timeEmt.appendChild(brand);
    timeEmt.appendChild(price);
    timeEmt.appendChild(num);
    timeEmt.appendChild(total);
}

//将xml中的文件信息读取出来
void DomXML::readXML(QString filePath,
                    QStringList &fList,
                    QStringList &bList,
                    QStringList &pList,
                    QStringList &nList,
                    QStringList &tList
                    )
{
    //打开文件
    QFile file(filePath);
    bool isOK = file.open(QIODevice::ReadOnly);
    if(true == isOK)    //打开成功
    {
        //创建xml文档对象
        QDomDocument doc;
        //file和xml文档对象关联
        isOK = doc.setContent(&file);
        if(isOK)    //如果关联成功
        {
            //获取根节点
            QDomElement root = doc.documentElement();
            file.close();

            //获取当前日期
            QDateTime date = QDateTime::currentDateTime();
            QString dateStr = date.toString("yyyy-MM-dd");

            if(root.hasChildNodes())    //有无子节点
            {
                //找最后一个节点
                QDomElement lastEmt = root.lastChildElement();
                if(lastEmt.attribute("date") == dateStr)    //判断有无当天日期
                {
                    //找出当前日期下所有时间子节点
                    QDomNodeList list = lastEmt.childNodes();
                    for(int i = 0; i < list.size(); i++)
                    {
                        //list.at(0).toElement();
                        //转换成元素，找到时间节点下的所有子节点(那五个节点元素，厂家等)
                        QDomNodeList subList = list.at(i).toElement().childNodes();
                        //厂家
                        QString factory = subList.at(0).toElement().text();
                        fList.append(factory);

                        QString brand = subList.at(1).toElement().text();
                        bList.append(brand);

                        QString price = subList.at(2).toElement().text();
                        pList.append(price);

                        QString num = subList.at(3).toElement().text();
                        nList.append(num);

                        QString total = subList.at(4).toElement().text();
                        tList.append(total);
                    }
                }
                else
                {
                    cout << "没有当天日期";
                    return;
                }
            }
            else
            {
                cout << "没有子节点";
                return;
            }
        }
        else
        {
            cout << "setContent error";
            return;
        }
    }
    else
    {
        //错误提示是习惯，方便查找错误
        cout << "ReadOnly error";
        return;
    }
}
