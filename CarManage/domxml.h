#ifndef DOMXML_H
#define DOMXML_H
#include <QString>
#include <QFile>
#include <QDomDocument> //xml文件指针要用的      文件
#include <QDomProcessingInstruction>//格式要用  格式头部
#include <QDomElement>  //元素                 一个个的元素
#include <QTextStream>  //文件流
#include <QDebug>
#include <QStringList>
#include <QDateTime>    //<日期>子节点 用到的 获取时间
//包含头文件才能用
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"
class DomXML
{
public:
    DomXML();

    //创建XML空文件
    static void createXML(QString filePath);
    //list.at(0) == 二汽神龙  这种思路来添加xml文件子节点
    static void appendXML(QString filePath, QStringList list);
    //写有效数据（添加时间节点与其后的节点元素）
    static void writeXML(QDomDocument &doc, QDomElement &root, QStringList &list);
    //
    static void readXML(QString filePath,
                        QStringList &fList,
                        QStringList &bList,
                        QStringList &pList,
                        QStringList &nList,
                        QStringList &tList
                        );
    //静态好处：方便调用
};

#endif // DOMXML_H
