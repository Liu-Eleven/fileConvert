#include "widget.h"
#include "ui_widget.h"


#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QDebug>
#include <QDirIterator>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(tr("文件格式转换"));

    init_codec_list();

    ui->src_code_combox->setCurrentIndex(24);

}

Widget::~Widget()
{
    delete ui;
}

 void Widget::foreachDirList(QStringList dirList)
 {

     for(int i=0 ; i<dirList.size() ; i++)
     {
         QString strPath = dirList.at(i);
         //qDebug()<<strPath;

         QDirIterator dir_iterator(strPath,QDir::Files | QDir::NoDotAndDotDot);
         QStringList files;


            //遍历数据
            while (dir_iterator.hasNext())
            {
                  QString file =dir_iterator.next();
                  QFileInfo fileInfo(file);

                  if ( (fileInfo.suffix() == "cpp") || (fileInfo.suffix() == "h") ||  (fileInfo.suffix() == "c") )
                      files<<file;

             }

            int row_idx = ui->file_list->rowCount();
            ui->file_list->insertRow(row_idx);
            QTableWidgetItem *newItem = new QTableWidgetItem(strPath);
            newItem->setBackgroundColor(QColor(255,251,166));
            ui->file_list->setItem(row_idx, 0, newItem);


            if(!files.isEmpty())
                fill_items(files);

            //将列的大小设置为和内容相匹配
            ui->file_list->resizeColumnsToContents();
     }
 }

void Widget::on_select_files_btn_clicked()
{
    static QString strDir = "d:/";
    strDir = QFileDialog::getExistingDirectory(this, tr("选择工程"),
                                               strDir,
                                               QFileDialog::ShowDirsOnly |
                                               QFileDialog::DontResolveSymlinks);
    // 选择工程目录
    if (!strDir.isEmpty()) {
        m_strTrPath = strDir;
    }
    else {
        m_strTrPath = "d:/";
    }


    QStringList dirList;

    QDirIterator dirIterator(m_strTrPath,
                                 QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                 QDirIterator::Subdirectories);


    qDebug()<<dirIterator.hasNext();
    dirList<<m_strTrPath;

    while(dirIterator.hasNext())
    {
        QString fileDir = dirIterator.next();
        dirList<<fileDir;
    }

    foreachDirList(dirList);

#if 0

    QDirIterator dir_iterator(m_strTrPath,QDir::Files | QDir::NoDotAndDotDot);
    QStringList files;


       //遍历数据
       while (dir_iterator.hasNext())
       {
             QString file =dir_iterator.next();
             QFileInfo fileInfo(file);

             if ( (fileInfo.suffix() == "cpp") || (fileInfo.suffix() == "h") ||  (fileInfo.suffix() == "c") )
                 files<<file;

        }

       fill_items(files);

       //将列的大小设置为和内容相匹配
       ui->file_list->resizeColumnsToContents();

#endif
#if 0
    QStringList files = QFileDialog::getOpenFileNames(
                              this,
                              tr("选择文件"),
                              ".",
                              "CPP Files (*.h *.cpp ) | Text Files (*.txt) | All Files (*.*)");
    if(files.isEmpty())
    {
        return;
    }
    fill_items(files);
    //将列的大小设置为和内容相匹配
    ui->file_list->resizeColumnsToContents();
#endif
}


//转换
void Widget::on_convert_btn_clicked()
{
    int src_mib = ui->src_code_combox->itemData(ui->src_code_combox->currentIndex()).toInt();
    QTextCodec *src_codec = QTextCodec::codecForMib(src_mib);
    int dest_mib = ui->dest_code_combox->itemData(ui->dest_code_combox->currentIndex()).toInt();
    QTextCodec *dest_codec = QTextCodec::codecForMib(dest_mib);
    for(int idx=0; idx<all_files.size(); ++idx)
    {
        QString file_name = all_files[idx];
        QFile in_file(file_name);

        //根据value重新找到key,即下标
        int new_idx = m_fileMaps.value(file_name);
        //qDebug()<<new_idx;
        if(!in_file.exists())
        {
            fill_result(new_idx, false);
            continue;
        }

        QFile out_file("tmp");
        QFileInfo file_info(file_name);
        if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text) || !out_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            fill_result(new_idx, false);
            continue;
        }
        //文件转码
        QByteArray encoded_data = in_file.readAll();
        //qDebug()<<encoded_data;
        QTextStream in(&encoded_data);
        in.setAutoDetectUnicode(false);
        in.setCodec(src_codec);
        QString decoded_str = in.readAll();
        QTextStream out(&out_file);
        out.setCodec(dest_codec);
        out << decoded_str;

        fill_result(new_idx, true);
//        fill_row_count(idx, row_count);

        //备份文件
        if(ui->back_src_ckb->isChecked())
        {
            QString back_file = QString("%1_bak").arg(file_info.fileName());
            in_file.copy(back_file);
        }
        in_file.close();
        in_file.remove();
        out_file.close();
        out_file.rename(file_name);
    }
}


//初始化字符集列表
void Widget::init_codec_list()
{
    QMap<QString, QTextCodec *> codecMap;
    QRegExp iso8859RegExp("ISO[- ]8859-([0-9]+).*");

    foreach (int mib, QTextCodec::availableMibs()) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);

        QString sortKey = codec->name().toUpper();
        int rank;

        if (sortKey.startsWith("UTF-8")) {
            rank = 1;
        } else if (sortKey.startsWith("UTF-16")) {
            rank = 2;
        } else if (iso8859RegExp.exactMatch(sortKey)) {
            if (iso8859RegExp.cap(1).size() == 1)
                rank = 3;
            else
                rank = 4;
        } else {
            rank = 5;
        }
        sortKey.prepend(QChar('0' + rank));

        codecMap.insert(sortKey, codec);
    }
    QList<QTextCodec *> codec_list = codecMap.values();
    ui->src_code_combox->clear();
    ui->dest_code_combox->clear();
    foreach (QTextCodec* codec, codec_list) {
        ui->src_code_combox->addItem(codec->name(), codec->mibEnum());
        ui->dest_code_combox->addItem(codec->name(), codec->mibEnum());
    }
}


//填充打开的文件列表
void Widget::fill_items(QStringList files)
{
    int row_idx = ui->file_list->rowCount();
    foreach(QString file_name, files)
    {
        if(all_files.contains(file_name))
        {
            continue;
        }
        ui->file_list->insertRow(row_idx);
        QTableWidgetItem *newItem = new QTableWidgetItem(file_name);
        ui->file_list->setItem(row_idx, 0, newItem);
        all_files.append(file_name);

        //重新获取一次下标
        int id_row = ui->file_list->rowCount();

        //下标自减一次
        id_row--;
        //qDebug()<<file_name<<id_row;
        //下标插入集合
        m_fileMaps.insert(file_name , id_row);
    }

   // qDebug()<<m_fileMaps;
}
//填充转换结果
void Widget::fill_result(int row_idx, bool result)
{
    QString result_str = result?QString("%1").arg("Y"):QString("%1").arg("N");
    QTableWidgetItem *result_item = new QTableWidgetItem(result_str);
    ui->file_list->setItem(row_idx, 1, result_item);
}

void Widget::fill_row_count(int row_idx, int row_count)
{
    QTableWidgetItem *row_count_item = new QTableWidgetItem(QString("%1").arg(row_count));
    ui->file_list->setItem(row_idx, 2, row_count_item);
}

/*Item选择改变时，让删除键可用*/
void Widget::on_file_list_itemSelectionChanged()
{
    QList<QTableWidgetItem *> items = ui->file_list->selectedItems();
    ui->del_btn->setEnabled(!items.isEmpty());
}

void Widget::on_del_btn_clicked()
{
    QList<int> del_rows;
    QList<QTableWidgetItem *> items = ui->file_list->selectedItems();
    foreach(QTableWidgetItem* cur_item, items)
    {
        int row_idx = cur_item->row();
        if(del_rows.contains(row_idx))
        {
            continue;
        }
        del_rows.append(row_idx);
    }

    //删除选中项
    foreach(int row_idx, del_rows)
    {
        QTableWidgetItem* file_name_item = ui->file_list->item(row_idx, 0);
        QString file_name = file_name_item->text();
        ui->file_list->removeRow(row_idx);
        all_files.removeOne(file_name);
    }
}
