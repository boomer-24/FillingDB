#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <qDebug>
#include <QTextCodec>
#include <QRegExp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    const QString RemoveDigit(const QString &_str) const;
    bool isExistInDB_testName(const QString &_str);
    bool isExistInDB(const QString &_table, const QString &_column, const QString &_value);
    bool isExistInDB(const QString &_table, QStringList _slColumns, QStringList _slValues);

    const QString PrepareName(const QString& _name);
    void Parse();

    bool InsertInto(const QString &_table, const QString &_column, const QString &_value);
    bool InsertInto(const QString &_table, QStringList _slColumns, QStringList _slValues);
    const int GetId(const QString &_table, const QString &_column, const QString &_value);
    const int GetId(const QString &_table, QStringList _slColumns, QStringList _slValues);
    const QString ExtractOnlyEnglishCharacters(const QString& _str);
    QString WrapQuotes(const QString _strForWrap);

private slots:
    void on_pb_SelectDir_clicked();
    void on_pb_Exec_clicked();
    void on_pb_Parse_clicked();
    void on_pb_imsNameInsert_clicked();

private:
    Ui::MainWindow *ui;
    QString dirPath_;
    QStringList slCSVFiles_;
    QSqlDatabase db_;
    int colomnNumber_;
};

#endif // MAINWINDOW_H
