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
#include <QFileSystemWatcher>
#include <QDir>

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
    const QString LastPrepareName(const QString& _name);
    void ReplaceEngToRu(QString& _str);
    void Parse();
    bool Parse(QString _csvPath);

    bool InsertInto(const QString &_table, const QString &_column, const QString &_value);
    bool InsertInto(const QString &_table, QStringList _slColumns, QStringList _slValues);
    const int GetId(const QString &_table, const QString &_column, const QString &_value);
    const int GetId(const QString &_table, QStringList _slColumns, QStringList _slValues);
    const QPair<int, int> GetIdsPrefixAndUnit(const QString &_prefixAndUnit);
    const int GetPreviousImsNumber();
    const QVector<QString> GetAllSomething(const QString &_table, const QString &_smthColumn) const;
    const QString GetSomething(const QString &_table, const QString &_smth, const QStringList &_slColumns, const QStringList &_slValues);
    const QString ExtractOnlyEnglishCharacters(const QString& _str);
    const QString ExtractPrefixAndUnitPair(const QString& _testName);
    QString WrapQuotes(const QString _strForWrap);

private slots:
    void slotDirChanged(QString _pathChangedDir);
    void on_pb_SelectDir_clicked();
    void on_pb_Exec_clicked();
    void on_pb_Parse_clicked();
    void on_pb_imsNameInsert_clicked();
    void on_pushButton_connect_clicked();

    void on_pushButton_ParseOneFile_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemWatcher watcher_;
    QString dirPath_;
    QStringList slCSVFiles_;
    QSqlDatabase db_;
    int colomnNumber_;
    QMap<QString, QString> mapForReplacing_;
};

#endif // MAINWINDOW_H
