#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_select_files_btn_clicked();

    void on_convert_btn_clicked();

    void on_file_list_itemSelectionChanged();

    void on_del_btn_clicked();

    void foreachDirList(QStringList dirList);

private:
    Ui::Widget *ui;
    QStringList all_files;
    QMap<QString , int> m_fileMaps;
    void init_codec_list();
    void fill_items(QStringList files);
    void fill_result(int row_idx, bool result);
    void fill_row_count(int row_idx, int row_count);
    QString m_strTrPath;
};

#endif // WIDGET_H
