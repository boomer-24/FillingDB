#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->pushButton_connect->click();

    this->mapForReplacing_.insert("E", "Е");
    this->mapForReplacing_.insert("e", "е");
    this->mapForReplacing_.insert("T", "Т");
    this->mapForReplacing_.insert("Y", "У");
    this->mapForReplacing_.insert("y", "у");
    this->mapForReplacing_.insert("O", "О");
    this->mapForReplacing_.insert("o", "о");
    this->mapForReplacing_.insert("P", "Р");
    this->mapForReplacing_.insert("p", "р");
    this->mapForReplacing_.insert("A", "А");
    this->mapForReplacing_.insert("a", "а");
    this->mapForReplacing_.insert("H", "Н");
    this->mapForReplacing_.insert("K", "К");
    this->mapForReplacing_.insert("k", "к");
    this->mapForReplacing_.insert("X", "Х");
    this->mapForReplacing_.insert("x", "х");
    this->mapForReplacing_.insert("C", "С");
    this->mapForReplacing_.insert("c", "с");
    this->mapForReplacing_.insert("B", "В");
    this->mapForReplacing_.insert("M", "М");
    this->mapForReplacing_.insert("m", "м");
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
    while (name.at(i) != "_" && name.at(i) != ".")
    {
        i++;
    }
    name.remove(i, name.size() - i);
    return name;
}

const QString MainWindow::LastPrepareName(const QString &_name)
{
    QRegExp reCommaTail(",\\s*(\\w)+$");
    QString result(_name);
    if (!_name.isEmpty())
    {
        if (result.contains(reCommaTail))
        {
            result.remove(reCommaTail);
        }

        QRegExp reEnglishChars("([A-Z]|[a-z])+");
        if (result.contains(reEnglishChars))
        {
            result.remove(reEnglishChars);
        }
        int indexOfLastElement = result.size() - 1;
        while (result.at(indexOfLastElement) == " " && indexOfLastElement > 0)
        {
            result.remove(indexOfLastElement, 1);
            indexOfLastElement = result.size() - 1;
        }
    }
    return result;
}

void MainWindow::ReplaceEngToRu(QString &_str)
{
    for (int i = 0; i < _str.size(); i++)
    {
        if (this->mapForReplacing_.contains(_str.at(i)))
            _str.replace(i, 1, this->mapForReplacing_.value(_str.at(i)));
    }
}

void MainWindow::Parse()
{
    if (!this->slCSVFiles_.empty())
    {
        for (auto fileName : this->slCSVFiles_)
        {
            QString  imsName = this->PrepareName(fileName);
            this->ReplaceEngToRu(imsName);
            if (!isExistInDB("list_ims", "name", this->WrapQuotes(imsName)))
                qDebug() << this->InsertInto("list_ims", "name", this->WrapQuotes(imsName));
            int idIms = this->GetId("list_ims", "name", this->WrapQuotes(imsName));
            if (!isExistInDB("party_ims", "id_ims", QString::number(idIms)))
                qDebug() << this->InsertInto("party_ims", "id_ims", QString::number(idIms));
            int idParty = this->GetId("party_ims", "id_ims", QString::number(idIms));

            bool checkColumnNames(false);
            QString path(this->dirPath_);
            path += "/";
            path += fileName;
            QFile csvFile(path);
            if (!csvFile.open(QIODevice::ReadOnly))
                qDebug() << path << " not open";
            else
            {
                QTextStream stream(&csvFile);
                stream.setCodec("CP-1251");
                int columnTestNumber = 0, columnTestName = 0, columnPinName = 0,
                        columnMeasValue = 0, columnLowLimit = 0, columnHighLimit = 0,
                        columnSuit = 0, columnPower = 0, columnPower_2 = 0, columnOther = 0;
                int rows = 0;
                QString dateTime("2000-01-01 01:01:01");
                int imsNumber(0);
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
                            if (tmp == "false")
                            {
                                dateTime.clear();
                                dateTime = "20";
                                QString rowDate = slist.at(2);
                                QString time = slist.at(3);
                                QStringList dateSl = rowDate.split(".");
                                dateTime.append(dateSl.at(2)).append("-").append(dateSl.at(1)).
                                        append("-").append(dateSl.at(0)).append(" ");
                                dateTime.append(time);
                                imsNumber = this->GetPreviousImsNumber() + 1;
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
                                    if (tmpName.isEmpty())
                                        break;
                                    if (tmpName.contains("онтактирован"))
                                        break;

                                    if (tmpName.contains("c"))
                                        tmpName.replace("c", "с");
                                    if (tmpName.contains("A"))
                                        tmpName.replace("A", "А");
                                    if (tmpName.contains("B"))
                                        tmpName.replace("B", "В");
                                    if (tmpName.contains("Om"))
                                        tmpName.replace("Om", "Ом");

                                    QString testName = this->RemoveDigit(tmpName);
                                    QString shortTestName = this->ExtractOnlyEnglishCharacters(testName);
                                    QString testNameFinal = this->LastPrepareName(testName);
                                    if (testNameFinal.isEmpty())
                                        break;
                                    this->ReplaceEngToRu(testNameFinal);
                                    QString prefixUnit = this->ExtractPrefixAndUnitPair(testName);
                                    this->ReplaceEngToRu(prefixUnit);
                                    QPair<int, int> prefixUnitPair(this->GetIdsPrefixAndUnit(prefixUnit));
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
                                        bool result = InsertInto("test_short_name", "short_name",
                                                                 this->WrapQuotes(shortTestName));

                                    int idShortTestName = this->GetId("test_short_name", "short_name",
                                                                      this->WrapQuotes(shortTestName));

                                    if (!isExistInDB("test_name", "name", this->WrapQuotes(testNameFinal)))
                                    {
                                        QString tn("");
                                        tn.append(QString::number(prefixUnitPair.second));
                                        tn.append(", ");
                                        tn.append(QString::number(idShortTestName));
                                        tn.append(", ");
                                        tn.append(this->WrapQuotes(testNameFinal));
                                        bool result = InsertInto("test_name", "id_category, id_short_name, name", tn);
                                        qDebug() << result;
                                    }

                                    //                                    int idTestName = this->GetId("test_name", "name", this->WrapQuotes(testNameFinal));
                                    //                                    int idSuit = this->GetId("measuring_status", "status", this->WrapQuotes(suit.toLower()));

                                    //                                    QStringList slCols{"id_test_name", "id_prefix", "low_limit", "high_limit", "power"};
                                    //                                    QStringList slVals{QString::number(idTestName), QString::number(prefixUnitPair.first),
                                    //                                                lowLimit, highLimit, power};
                                    //                                    if (!power_2.isNull())
                                    //                                    {
                                    //                                        slCols.push_back("power_2");
                                    //                                        slVals.push_back(power_2);
                                    //                                    }

                                    //                                    if (!isExistInDB("test_variant", slCols, slVals))
                                    //                                    {
                                    //                                        qDebug() << this->InsertInto("test_variant", slCols, slVals);
                                    //                                    }
                                    //                                    int idTestVariant = this->GetId("test_variant", slCols, slVals);

                                    //                                    QStringList slColsForMeasuring{"date", "id_session", "id_party", "number", "id_test_variant",
                                    //                                                                   "pin_name", "meas", "id_measuring_status"};
                                    //                                    QStringList slValsForMeasuring {this->WrapQuotes(dateTime), QString::number(1),
                                    //                                                QString::number(idParty), QString::number(imsNumber), QString::number(idTestVariant),
                                    //                                                this->WrapQuotes(pinName), measValue, QString::number(idSuit)};
                                    //                                    qDebug() << this->InsertInto("measuring", slColsForMeasuring, slValsForMeasuring);
                                    break;
                                } else break;
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
    queryStr.append(" = ");
    queryStr.append(_value);

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
    queryStr.append(" WHERE (");
    for (int i = 0; i < _slColumns.size(); i++)
    {
        queryStr.append(_slColumns.at(i));
        queryStr.append(" = ");
        queryStr.append(_slValues.at(i));
        if (i != _slColumns.size() - 1)
        {
            queryStr.append(" ");
            queryStr.append(" AND ");
        } else queryStr.append(")");
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
    QSqlRecord rec = query.record();
    qDebug() << "Number of columns: " << rec.count();
    int numberOfColumn = rec.indexOf("id");
    int qu = 0;
    while (query.next())
        qu = query.value(numberOfColumn).toInt();
    return qu;
}

const QPair<int, int> MainWindow::GetIdsPrefixAndUnit(const QString &_prefixAndUnit)
{
    if (!_prefixAndUnit.isEmpty())
    {
        QVector<QString> vUnits(this->GetEverySomething("test_category", "isu_name"));
        int index(-1);
        for (QString& unit : vUnits)
        {
            if (_prefixAndUnit.contains(unit))
            {
                index = _prefixAndUnit.indexOf(unit);
                break;
            }
        }
        if (index != -1)
        {
            QString prefix;
            QString unit;
            for (int i = 0; i < _prefixAndUnit.size(); i++)
            {
                if (i < index)
                    prefix.push_back(_prefixAndUnit.at(i));
                else
                    unit.push_back(_prefixAndUnit.at(i));
            }
            int idPrefix = this->GetId("prefix", "name", this->WrapQuotes(prefix));
            int idUnit = this->GetId("test_category", "isu_name", this->WrapQuotes(unit));

            return QPair<int, int>(idPrefix, idUnit);
        } else return QPair<int, int>(-1, -1);
    } else return QPair<int, int>(-1, -1);
}

const int MainWindow::GetPreviousImsNumber()
{
    QString queryStr("SELECT number FROM measuring ORDER BY number DESC LIMIT 1");
    QSqlQuery query = QSqlQuery(this->db_);

    if (!query.exec(queryStr))
    {
        qDebug() << "NE DAL MAX Number";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return -1;
    }
    QSqlRecord rec = query.record();
    qDebug() << "Number of columns: " << rec.count();
    //    int numberOfColumn = rec.indexOf(_smth);
    //    QString qu("0");
    int qu(-1);
    while (query.next())
        qu = query.value(0).toInt();
    return qu;
}

const QVector<QString> MainWindow::GetEverySomething(const QString &_table, const QString &_smthColumn) const
{
    QString queryStr("SELECT ");
    queryStr.append(_smthColumn).append(" FROM ");
    queryStr.append(_table);
    //    queryStr.append(")");
    qDebug() << queryStr;
    QVector<QString> vResultValues;
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(queryStr))
    {
        qDebug() << "NE DAL QVector<SMTH>";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return vResultValues;
    }
    QSqlRecord rec = query.record();
    qDebug() << "Number of columns: " << rec.count();
    int numberOfColumn = rec.indexOf(_smthColumn);
    QString qu("0");
    while (query.next())
        vResultValues.push_back(query.value(numberOfColumn).toString());
    return vResultValues;
}

const QString MainWindow::GetSomething(const QString &_table, const QString &_smth,
                                       const QStringList& _slColumns, const QStringList& _slValues)
{
    QString queryStr("SELECT ");
    queryStr.append(_smth).append(" FROM ");
    queryStr.append(_table);
    queryStr.append(" WHERE (");
    for (int i = 0; i < _slColumns.size(); i++)
    {
        queryStr.append(_slColumns.at(i));
        queryStr.append(" = ");
        queryStr.append(_slValues.at(i));
        if (i != _slColumns.size() - 1)
        {
            queryStr.append(" ");
            queryStr.append(" AND ");
        } else queryStr.append(")");
    }
    qDebug() << queryStr;
    QSqlQuery query = QSqlQuery(this->db_);
    if (!query.exec(queryStr))
    {
        qDebug() << "NE DAL SMTH";
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
        return QString("-1");
    }
    QSqlRecord rec = query.record();
    qDebug() << "Number of columns: " << rec.count();
    int numberOfColumn = rec.indexOf(_smth);
    QString qu("0");
    while (query.next())
        qu = query.value(numberOfColumn).toString();
    return qu;
}

const QString MainWindow::ExtractOnlyEnglishCharacters(const QString &_str)
{
    QRegExp re("([A-Z]|[a-z])+");
    if (_str.contains(re))
    {
        QString resultEng = _str;
        int i = re.indexIn(resultEng);
        resultEng = re.cap(0);
        return resultEng;
    } else return QString("-");
}

const QString MainWindow::ExtractPrefixAndUnitPair(const QString &_testName)
{
    if (!_testName.isEmpty() && _testName.contains(","))
    {
        QString last = _testName.split(",").last();
        last.remove(" ");
        return last;
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
    qDebug() << this->GetPreviousImsNumber();
    //    QString Squery = this->ui->lineEdit_Exec->text();
    //    QSqlQuery query = QSqlQuery(this->db_);
    //    if (!query.exec(Squery))
    //    {
    //        qDebug() << query.lastError().databaseText();
    //        qDebug() << query.lastError().driverText();
    //        return;
    //    }
    //    while (query.next())
    //    {
    //        //        int qu = query.value(0).toInt();
    //        QString qu = query.value(1).toString();
    //        qDebug() << query.record();
    //        qDebug() << "\n _____qu: " << qu;
    //    }
    //    qDebug() << Squery;
}

void MainWindow::on_pb_Parse_clicked()
{
    this->Parse();
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

void MainWindow::on_pushButton_connect_clicked()
{
    this->db_ = QSqlDatabase::addDatabase("QMYSQL");
    //    this->db_.setHostName(this->ui->lineEdit_ipPath->text());
    this->db_.setHostName("10.0.0.11");
    this->db_.setDatabaseName("my_project");
    this->db_.setUserName("depressa");
    this->db_.setPassword("d4m27244n");
    if (!this->db_.open())
    {
        this->ui->lineEdit_DB->setText(this->db_.lastError().text());
    } else this->ui->lineEdit_DB->setText("success");
}
