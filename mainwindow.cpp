#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->db_ = QSqlDatabase::addDatabase("QMYSQL");
    this->db_.setHostName("10.0.0.11");
    this->db_.setDatabaseName("my_project");
    this->db_.setUserName("depressa");
    this->db_.setPassword("d4m27244n");
    if (!this->db_.open())
    {
        this->ui->lineEdit_DB->setText(this->db_.lastError().text());
    } else this->ui->lineEdit_DB->setText("success");
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QString MainWindow::RemoveDigit(const QString &_str) const
{
    QString s(_str);
    while (!s.at(0).isLetter())
    {
        if (s.size() < 2)
            return QString("");
        s.remove(0, 1);
    }
    return s;
}

bool MainWindow::isExistInDB_testName(const QString &_str)
{
    QString q("SELECT EXISTS (SELECT * FROM test_name WHERE name = \"");
    q.append(_str);
    q.append("\")");

    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(q))
    {
        qDebug() << "Ошибка в запросе_ ///return true;///";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return true;
    }
    while (query.next())
    {
        int qu = query.value(0).toInt();
        qDebug() << "\nDEBUG:::::" << query.record();
        qDebug() << "\n _____qu: " << qu;
        if (qu == 1)
            return true;
        if (qu == 0)
            return false;
    }
}

bool MainWindow::isExistInDB(const QString &_table, const QString &_column, const QString &_value)
{
    QString q("SELECT EXISTS (SELECT * FROM ");
    q.append(_table);
    q.append(" WHERE ");
    q.append(_column);
    q.append(" = ");
    q.append(_value);
    q.append(")");

    qDebug() << q;
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(q))
    {
        qDebug() << "Ошибка в запросе_ ///return true;///";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().text();
        qDebug() << query.lastError().number();
        return true;
    }
    while (query.next())
    {
        int qu = query.value(0).toInt();
        qDebug() << "\nDEBUG:::::" << query.record();
        qDebug() << "\n _____qu: " << qu;
        if (qu)
            return true;
        else if (!qu)
            return false;
    }
    return true;
}

bool MainWindow::isExistInDB(const QString &_table, QStringList _slColumns, QStringList _slValues)
{
    QString q("SELECT EXISTS (SELECT * FROM ");
    q.append(_table);
    q.append(" WHERE ");
    for (int i = 0; i < _slColumns.size(); i++)
    {
        q.append(_slColumns.at(i));
        q.append(" = ");
        q.append(_slValues.at(i));
        if (i != _slColumns.size() - 1)
        {q.append(" ");
            q.append(" AND ");}
        else q.append(")");
    }
    qDebug() << q;
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(q))
    {
        qDebug() << "Ошибка в запросе_ ///return true;///";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        qDebug() << query.lastError().text();
        qDebug() << query.lastError().number();
        return true;
    }
    while (query.next())
    {
        int qu = query.value(0).toInt();
        qDebug() << "\nDEBUG:::::" << query.record();
        qDebug() << "\n _____qu: " << qu;
        if (qu == 1)
            return true;
        if (qu == 0)
            return false;
    }
    return true;
}

const QString MainWindow::PrepareName(const QString &_name)
{
    QString name(_name);
    int i = 0;
    while (name.at(i) != "_")
    {
        i++;
    }
    name.remove(i, name.size() - i);
    return name;
}

void MainWindow::Parse()
{
    if (!this->slCSVFiles_.empty())
    {
        for (auto path : this->slCSVFiles_)
        {
            qDebug() << "HERE    " << path;
            QString imsName = this->PrepareName(path);
            if (!isExistInDB("list_ims", "name", this->WrapQuotes(imsName)))
                qDebug() << this->InsertInto("list_ims", "name", this->WrapQuotes(imsName));
            int idIms = this->GetId("list_ims", "name", this->WrapQuotes(imsName));
            if (!isExistInDB("party_ims", "id_ims", QString::number(idIms)))
                qDebug() << this->InsertInto("party_ims", "id_ims", QString::number(idIms));
            int idParty = this->GetId("party_ims", "id_ims", QString::number(idIms));

            bool checkColumnNames(false);
            QString p(this->dirPath_);
            p += "/";
            p += path;
            QFile csvFile(p);
            if (!csvFile.open(QIODevice::ReadOnly))
                qDebug() << p << " not open";
            else
            {
                QTextStream stream(&csvFile);
                stream.setCodec("CP-1251");
                int columnTestNumber = 0, columnTestName = 0, columnPinName = 0,
                        columnMeasValue = 0, columnLowLimit = 0, columnHighLimit = 0,
                        columnSuit = 0, columnPower = 0, columnPower_2 = 0, columnOther = 0;
                int rows = 0;
                QString dateTime("2000-01-01 01:01:01");
                while (!stream.atEnd())
                {
                    QString line = stream.readLine();
                    rows++;
                    qDebug() << "____ROW:____ " << rows;
                    if (!line.isEmpty())
                    {
                        QStringList slist = line.split(";");
                        while (slist.size() < 10)
                            (slist.push_back(" "));
                        for (int i = 0; i < slist.size(); i++)
                        {
                            QString tmp(slist.at(1));
                            tmp = tmp.toLower();
                            if (tmp == "false") // МБ косяк когда строка пустая / DA POX
                            {
                                dateTime.clear();
                                dateTime = "20";
                                QString rowDate = slist.at(2);
                                QString time = slist.at(3);
                                QStringList dateSl = rowDate.split(".");
                                dateTime.append(dateSl.at(2)).append("-").append(dateSl.at(1)).
                                        append("-").append(dateSl.at(0)).append(" ");
                                dateTime.append(time);
                                break;
                            }
                            if (!checkColumnNames)
                            {
                                if (slist.at(i).contains("Номер теста"))
                                {columnTestNumber = i; continue;}
                                if (slist.at(i).contains("Название теста"))
                                {columnTestName = i; continue;}
                                if (slist.at(i).contains("Номер вывода"))
                                {columnPinName = i; continue;}
                                if (slist.at(i).contains("Измеренное значение"))
                                {columnMeasValue = i; continue;}
                                if (slist.at(i).contains("Нижняя граница"))
                                {columnLowLimit = i; continue;}
                                if (slist.at(i).contains("Верхняя граница"))
                                {columnHighLimit = i; continue;}
                                if (slist.at(i).contains("Годность"))
                                {columnSuit = i; continue;}
                                if (slist.at(i).contains("Напряжение питания"))
                                {columnPower = i; continue;}
                                if (slist.at(i).contains("Напряжение питания 2"))
                                {columnPower_2 = i; continue;}
                                if (slist.at(i).contains("Задержка"))
                                {columnOther = i; checkColumnNames = true;}
                            }
                            else
                            {
                                QString colTestN = slist.at(columnTestNumber);
                                if (colTestN.isEmpty())
                                    break;
                                if (colTestN.at(0).isDigit())
                                {
                                    QString tmpName = slist.at(columnTestName);
                                    QString testName = this->RemoveDigit(tmpName);
                                    QString shortTestName = this->ExtractOnlyEnglishCharacters(testName);
                                    QString pinName = slist.at(columnPinName);
                                    QString measValue = slist.at(columnMeasValue);
                                    QString lowLimit = slist.at(columnLowLimit);
                                    QString highLimit = slist.at(columnHighLimit);
                                    QString suit = slist.at(columnSuit);
                                    QString power = slist.at(columnPower);
                                    QString power_2;
                                    if (columnPower_2)
                                        power_2 = slist.at(columnPower_2);
                                    QString other = slist.at(columnOther);

                                    if (!isExistInDB("test_short_name", "short_name", this->WrapQuotes(shortTestName)))
                                    {
                                        bool result = InsertInto("test_short_name", "short_name", this->WrapQuotes(shortTestName));
                                        qDebug() << result;
                                    }
                                    int idShortTestName = this->GetId("test_short_name", "short_name", this->WrapQuotes(shortTestName));

                                    if (!isExistInDB("test_name", "name", this->WrapQuotes(testName)))
                                    {
                                        QString tn("");
                                        tn.append(QString::number(idShortTestName));
                                        tn.append(", ");
                                        tn.append(this->WrapQuotes(testName));
                                        bool result = InsertInto("test_name", "id_short_name, name", tn);
                                        qDebug() << result;
                                    }

                                    int idTestName = this->GetId("test_name", "name", this->WrapQuotes(testName));
                                    int idSuit = this->GetId("measuring_status", "status", this->WrapQuotes(suit.toLower()));

                                    QStringList slCols{"id_test_name", "low_limit", "high_limit", "power"};
                                    QStringList slVals{QString::number(idTestName), lowLimit, highLimit, power};
                                    if (!power_2.isNull())
                                    {
                                        slCols.push_back("power_2");
                                        slVals.push_back(power_2);
                                    }

                                    if (!isExistInDB("test_variant", slCols, slVals))
                                    {
                                        qDebug() << this->InsertInto("test_variant", slCols, slVals);
                                    }
                                    int idTestVariant = this->GetId("test_variant", slCols, slVals);

                                    QStringList slColsForMeasuring{"date", "id_session", "id_party", "id_test_variant",
                                                                   "pin_name", "meas", "id_measuring_status"};
                                    QStringList slValsForMeasuring {this->WrapQuotes(dateTime), QString::number(1),
                                                QString::number(idParty), QString::number(idTestVariant),
                                                this->WrapQuotes(pinName), measValue, QString::number(idSuit)};
                                    qDebug() << this->InsertInto("measuring", slColsForMeasuring, slValsForMeasuring);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool MainWindow::InsertInto(const QString &_table, const QString &_column, const QString &_value)
{
    QSqlQuery query = QSqlQuery(this->db_);
    QString base("INSERT INTO ");
    QString queryStr(base);
    queryStr.append(_table);
    queryStr.append("(");
    queryStr.append(_column);
    queryStr.append(")");
    queryStr.append(" values(");
    //    queryStr.append(" value(");
    queryStr.append(_value);
    queryStr.append(")");
    if (!query.exec(queryStr))
    {
        qDebug() << " ne polu4ilos\\n";
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

bool MainWindow::InsertInto(const QString &_table, QStringList _slColumns, QStringList _slValues)
{
    QSqlQuery query = QSqlQuery(this->db_);
    QString base("INSERT INTO ");
    QString queryStr(base);
    queryStr.append(_table);
    queryStr.append("(");
    for (int i = 0; i < _slColumns.size(); i++)
    {
        queryStr.append(_slColumns.at(i));
        if (i != _slColumns.size() - 1)
            queryStr.append(", ");
    }

    queryStr.append(") values(");
    for (int i = 0; i < _slValues.size(); i ++)
    {
        queryStr.append(_slValues.at(i));
        if (i != _slValues.size() - 1)
            queryStr.append(", ");
    }
    queryStr.append(")");
    qDebug() << queryStr;
    if (!query.exec(queryStr))
    {
        qDebug() << " ne polu4ilos\\n";
        qDebug() << query.lastError();
        return false;
    }
    return true;
}

const int MainWindow::GetId(const QString &_table, const QString &_column, const QString &_value)
{
    QString queryStr("SELECT id FROM ");
    queryStr.append(_table);
    queryStr.append(" WHERE ");
    queryStr.append(_column);
//    queryStr.append(" = \"");
    queryStr.append(" = ");
    queryStr.append(_value);
//    queryStr.append("\"");

    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(queryStr))
    {
        qDebug() << "NE DAL ID";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return 0;
    }
    while (query.next())
    {
        int qu = query.value(0).toInt();
        return qu;
    }
}

const int MainWindow::GetId(const QString &_table, QStringList _slColumns, QStringList _slValues) //ЕСЛИ НЕ ДАЛ ID ВОЗВРАЩАТЬ "-1"
{
    QString queryStr("SELECT id FROM ");
    queryStr.append(_table);
    queryStr.append(" WHERE ");
    for (int i = 0; i < _slColumns.size(); i++)
    {
        queryStr.append(_slColumns.at(i));
        queryStr.append(" = ");
        queryStr.append(_slValues.at(i));
        if (i != _slColumns.size() - 1)
        {queryStr.append(" ");
            queryStr.append(" AND ");}
    }
    qDebug() << queryStr;
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(queryStr))
    {
        qDebug() << "NE DAL ID";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return -1;
    }
//    while (query.next())
//    {
        int qu = query.value(0).toInt();
        return qu;
//    }
}

const QString MainWindow::ExtractOnlyEnglishCharacters(const QString &_str)
{
    QRegExp re("([A-Z]|[a-z])+");
    if (_str.contains(re))
    {
        QString resultEng = _str;
        int i = re.indexIn(resultEng);
        qDebug() << re.pos(0);
        resultEng = re.pos(0);
        return resultEng;
    } else return QString("");
}

QString MainWindow::WrapQuotes(const QString _strForWrap)
{
    QString s("\"");
    s.append(_strForWrap);
    s.append("\"");
    return s;
}

void MainWindow::on_pb_SelectDir_clicked()
{
    this->dirPath_ = QFileDialog::getExistingDirectory(this, "Select directory", "C:/");
    this->ui->lineEdi_dir->setText(this->dirPath_);
    QDir dir(this->dirPath_);
    this->slCSVFiles_ = dir.entryList(QStringList() << "*.csv");
}

void MainWindow::on_pb_Exec_clicked()
{
    QString Squery = this->ui->lineEdit_Exec->text();
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(Squery))
    {
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return;
    }
    while (query.next())
    {
        //        int qu = query.value(0).toInt();
        QString qu = query.value(1).toString();
        qDebug() << query.record();
        qDebug() << "\n _____qu: " << qu;
    }
    qDebug() << Squery;
}

void MainWindow::on_pb_Parse_clicked()
{
    this->Parse();
    //    this->LetParse();
    //    this->Parse140ud();
}

void MainWindow::on_pb_imsNameInsert_clicked()
{
    QDir dir(this->dirPath_);
    QStringList names = dir.entryList(QStringList() << "*.csv");
    QSqlQuery query = QSqlQuery(this->db_);
    QString quer("INSERT INTO list_ims(name) value(\"");

    for (auto str : names)
    {
        QString s = this->PrepareName(str);
        if (!this->isExistInDB("list_ims", "name", s))
        {
            QString q(quer);
            q.append(s);
            q.append("\")");
            if (!query.exec(q))
            {
                qDebug() << str << " ne polu4ilos";
            }
        }
    }
}
