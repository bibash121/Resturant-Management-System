#include "rms.h"
#include "./ui_rms.h"
#include <QtSql>
#include<QShortcut>
#include<QKeyEvent>


RMS::RMS(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RMS)
{
    ui->setupUi(this);
    ui->MenuTable->setWordWrap(true);
    ui->OrderTable->setWordWrap(true);
    connect(ui->Calendar, &QCalendarWidget::selectionChanged, this, &RMS::onCalendarSelectionChanged);

    connect(ui->OrderTable, &QTableWidget::itemChanged, this, &RMS::calculateTotal);
    ui->OrderTable2->resizeRowsToContents();

    ui->MenuTable->hideColumn(2);
     ui->HiddenWidget->hide();
     ui->Background->setCurrentWidget(ui->LoginPage);
     adjustTableColumnWidths();
     loadMenuFromDatabase();
     Stylesheetfunction();
     customizeButton(ui->LoginButton);
     ui->EmployedDetailLabel->hide();
     //wrapTextInTable(ui->OrderTable2);

     setButtonStyle(ui->AddButton);
     setButtonStyle(ui->pushButton);
     setButtonStyle(ui->ConfirmButton);
     setButtonStyle(ui->DeleteButton);
     setButtonStyle(ui->ReturnButton);
     setButtonStyle(ui->CancelAdd);
     setButtonStyle(ui->Calculate);
     setButtonStyle(ui->ClearOrderTable);
     setButtonStyle(ui->SelectButton);
     setButtonStyle(ui->CancelSelect);
     setButtonStyle(ui->RecordsBack);
     //setButtonStyle(ui->AddDetails);
     setButtonStyle(ui->ConfirmDetails);
     setButtonStyle(ui->CancelDetails);
     setButtonStyle(ui->ClearButton);
     setButtonStyle(ui->BackHistory);
     setButtonStyle(ui->BackSales);
     setButtonStyle(ui->ClearSales);
     setButtonStyle(ui->RefreshButton);
     connect(ui->Calendar, &QCalendarWidget::selectionChanged, this, &RMS::onCalendarSelectionChanged);

     setButtonStyle2(ui->MenuB);
     addHoverAnimation(ui->MenuB);

     setButtonStyle2(ui->OrderB);
     addHoverAnimation(ui->OrderB);

     editSalesTable();
     edittableHistory();

     setButtonStyle2(ui->RecordB);
     addHoverAnimation(ui->RecordB);

     setButtonStyle2(ui->LogoutB);
     addHoverAnimation(ui->LogoutB);

     ui->UserLabel->hide();
     ui->PassLabel->hide();
     ui->Username->hide();
     ui->Password->hide();

     // Disable dates from other months
     ui->Calendar->setDateEditEnabled(false);
     ui->Calendar->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
}

RMS::~RMS()
{
    delete ui;
}


void RMS::on_LoginButton_clicked()
{

    if (ui->Username->isHidden()) {
        slideLabelToUp(ui->LoginLogo);
        fadeInTextBox2(ui->Username);
        fadeInTextBox2(ui->Password);
        fadeInWidget(ui->UserLabel);
        fadeInWidget(ui->PassLabel);
    }
    else {

        QSqlDatabase sqlitedb = QSqlDatabase::addDatabase("QSQLITE");
        sqlitedb.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");

        QString username = ui->Username->text();
        QString password = ui->Password->text();

        if (sqlitedb.open()) {
            QSqlQuery query(sqlitedb);
            query.prepare("SELECT * FROM Login WHERE Username = :username AND Password = :password");
            query.bindValue(":username", username);
            query.bindValue(":password", password);

            // Check if the login credentials are correct
            if (query.exec()) {
                if (query.next()) {

                    entry();
                    ui->NameLabel->setText(username);

                    QString loginTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

                    // Insert the login information into the database
                    QSqlQuery historyQuery(sqlitedb);
                    historyQuery.prepare("INSERT INTO History (Cashier, Login) VALUES (:Cashier, :Login)");
                    historyQuery.bindValue(":Cashier", username);
                    historyQuery.bindValue(":Login", loginTime);

                    if (!historyQuery.exec()) {
                        QMessageBox::critical(this, "Database Error", "Failed to log login history.");
                    }

                } else {

                    ui->Confirmation->setText("Invalid username or password.");
                }
            } else {

                ui->Confirmation->setText("Login failed (database error).");
            }
        } else {

            ui->Confirmation->setText("Connection failed!");
        }


        sqlitedb.close();
    }
}
void RMS:: entry()
{
    ui->Background->setCurrentWidget(ui->HomePage);
}

void RMS::Exit()
{
    ui->Background->setCurrentIndex(1);
}

void RMS::on_MenuB_clicked()
{
    ui->Background->setCurrentWidget(ui->Menu);
}

void RMS::on_OrderB_clicked()
{
    ui->Background->setCurrentWidget(ui->Order);

    QDate currentDate = QDate::currentDate();
    QString formattedDate = currentDate.toString("dd-MM-yyyy");
    ui->Date->setText(formattedDate);

    int billID = rand() % 100000;
    QString billid = QString::number(billID);
    ui->BillIdDisplay->setText(billid);
    editOrderTable();
    editOrderTable2();
    loadMenuForOrder();
}

void RMS::on_BillB_clicked()
{
    ui->Background->setCurrentWidget(ui->Bills);
}

void RMS::on_RecordB_clicked()
{
    ui->Background->setCurrentWidget(ui->Records);
}


void RMS::on_LogoutB_clicked()
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to proceed?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            // Get the current date and time as logout time
            QString logoutTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

            // Get the username of the cashier (assuming it's stored in the 'username' variable)
            QString username = ui->NameLabel->text();  // Or wherever the username is stored

            // Open the database to insert the logout time
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");

            if (db.open()) {
                QSqlQuery query(db);

                // Update the History table to insert logout time
                query.prepare("UPDATE History SET Logout = :logoutTime WHERE Cashier = :username AND Logout IS NULL");
                query.bindValue(":logoutTime", logoutTime);  // Use the current time for logout
                query.bindValue(":username", username);  // Use the username of the cashier

                if (!query.exec()) {
                    QMessageBox::critical(this, "Database Error", "Failed to log out.");
                }
            } else {
                QMessageBox::critical(this, "Database Error", "Unable to connect to the database.");
            }

            db.close();

            // Switch to the login page
            ui->Background->setCurrentWidget(ui->LoginPage);
        }
    }


void RMS::on_ReturnButton_clicked()
{
    UncheckCheckbox();
    Exit();
}

void RMS::on_AddButton_clicked()
{
   ui->HiddenWidget->setCurrentIndex(0);
   ui->HiddenWidget->show();
    UncheckCheckbox();
     ui->MenuTable->hideColumn(2);

}

void RMS::on_ConfirmButton_clicked()
{
    QString itemName = ui->ItemName->text();
    double price = ui->PriceBox->text().toDouble();

    // Validate input
    if (itemName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Item name cannot be empty!");
        return;
    }

    if (price <= 0) {
        QMessageBox::warning(this, "Input Error", "Price must be greater than 0!");
        return;
    }


    QSqlDatabase orderDb = QSqlDatabase::addDatabase("QSQLITE");
    orderDb.setDatabaseName("C:/BIB/QT/ResturantManagement/ordertable.db"); // Path to your database

    if (!orderDb.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database: " + orderDb.lastError().text());
        return;
    }


    QSqlQuery queryInsert(orderDb);
    queryInsert.prepare("INSERT INTO Order_table (item, price) VALUES (:item, :price)");
    queryInsert.bindValue(":item", itemName);
    queryInsert.bindValue(":price", price);

    if (!queryInsert.exec()) {
        QMessageBox::critical(this, "Insertion Error", "Failed to insert the item: " + queryInsert.lastError().text());
        return;
    }

    ui->ItemName->clear();
    ui->PriceBox->clear();
    loadMenuFromDatabase();
}

void RMS::loadMenuFromDatabase()
{

    QSqlDatabase orderDb = QSqlDatabase::addDatabase("QSQLITE");
    orderDb.setDatabaseName("C:/BIB/QT/ResturantManagement/ordertable.db");

    if (!orderDb.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database: " + orderDb.lastError().text());
        return;
    }

    // Clear the existing rows
    ui->MenuTable->setRowCount(0);


    QSqlQuery query(orderDb);
    if (!query.exec("SELECT item, price FROM Order_table ORDER BY item ASC"))
    {
        QMessageBox::critical(this, "Query Error", "Failed to execute query: " + query.lastError().text());
        return;
    }


    int row = 0;
    while (query.next()) {
        ui->MenuTable->insertRow(row);
        ui->MenuTable->setWordWrap(true);

        QString itemName = query.value(0).toString();
        double price = query.value(1).toDouble();

        ui->MenuTable->setItem(row, 0, new QTableWidgetItem(itemName));
        ui->MenuTable->setItem(row, 1, new QTableWidgetItem(QString::number(price, 'f', 2)));

        QCheckBox* checkbox = new QCheckBox();

        ui->MenuTable->setCellWidget(row, 2, checkbox);
        checkbox->setStyleSheet(
            "QCheckBox {"

           " margin: 4px;"
            "   background:grey;"

            "}"
            );

        ui->MenuTable->setRowHeight(row, 35);
        row++;
    }
}

void RMS::loadMenuForOrder()
{
    // Open the database
    QSqlDatabase orderDb = QSqlDatabase::addDatabase("QSQLITE");
    orderDb.setDatabaseName("C:/BIB/QT/ResturantManagement/ordertable.db");

    if (!orderDb.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database: " + orderDb.lastError().text());
        return;
    }

    // Clear the existing rows
    ui->OrderTable2->setRowCount(0);

    QSqlQuery query(orderDb);
    if (!query.exec("SELECT item, price FROM Order_table ORDER BY item ASC"))
    {
        QMessageBox::critical(this, "Query Error", "Failed to execute query: " + query.lastError().text());
        return;
    }

    // Populate the table
    int row = 0;
    while (query.next()) {
        ui->OrderTable2->insertRow(row);
        QString itemName = query.value(0).toString();
        double price = query.value(1).toDouble();
        QPushButton* incrementButton = new QPushButton("+");
        incrementButton->setFixedSize(25, 25);
        incrementButton->setStyleSheet("padding: 5px;background-color: BLACK;font-size:20px;");
        ui->OrderTable2->setCellWidget(row, 2, incrementButton);


        QPushButton* decrementButton = new QPushButton("-");
        decrementButton->setFixedSize(25, 25);
        decrementButton->setStyleSheet("padding: 5px;background-color: BLACK;font-size:20px;");

        ui->OrderTable2->setCellWidget(row, 3, decrementButton);


        connect(incrementButton, &QPushButton::clicked, this, [=]()
                {
            handleIncrement(row);

        });

        connect(decrementButton, &QPushButton::clicked, this, [=]()
                {
            handleDecrement(row);

        });

        ui->OrderTable2->setItem(row, 0, new QTableWidgetItem(itemName));
        ui->OrderTable2->setItem(row, 1, new QTableWidgetItem(QString::number(price, 'f', 2)));
        ui->OrderTable2->setRowHeight(row, 35);
        row++;
    }
}

void RMS::on_CancelAdd_clicked()
{
    ui->ItemName->clear();
    ui->PriceBox->clear();
    ui->HiddenWidget->hide();
}

void RMS::on_CancelSelect_clicked()
{
    ui->HiddenWidget->hide();
    ui->MenuTable->hideColumn(2);
    UncheckCheckbox();


}

void RMS::adjustTableColumnWidths() {

    ui->MenuTable->setColumnWidth(0, 405);

    ui->MenuTable->setColumnWidth(1, 150);

    ui->MenuTable->setColumnWidth(2, 50);


    QHeaderView *header = ui->MenuTable->horizontalHeader();
    header->setStyleSheet("QHeaderView::section { padding: 10px; }");

}

void RMS::on_SelectButton_clicked()
{

    ui->MenuTable->showColumn(2);
    ui->HiddenWidget->setCurrentIndex(1);
    ui->HiddenWidget->show();
}

void RMS::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Up) {

        if (ui->PriceBox->hasFocus()) {
            ui->ItemName->setFocus();

        }
    }
    else if (event->key() == Qt::Key_Down) {
        if (ui->ItemName->hasFocus()) {
            ui->PriceBox->setFocus();

        }

    }
    else {
        QWidget::keyPressEvent(event);
    }
}


void RMS::on_DeleteButton_clicked()
{
    QSqlDatabase orderDb = QSqlDatabase::addDatabase("QSQLITE");
    orderDb.setDatabaseName("C:/BIB/QT/ResturantManagement/ordertable.db");

    if (!orderDb.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open the database: " + orderDb.lastError().text());
        return;
    }

    for (int row = 0; row < ui->MenuTable->rowCount(); ++row) {
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(ui->MenuTable->cellWidget(row, 2));
        if (checkbox && checkbox->isChecked()) {
            QTableWidgetItem* item = ui->MenuTable->item(row, 0);
            if (item) {
                QString itemName = item->text();

                // Delete the item from the database
                QSqlQuery query(orderDb);
                query.prepare("DELETE FROM Order_table WHERE item = :item");
                query.bindValue(":item", itemName);

                if (!query.exec()) {
                    QMessageBox::critical(this, "Deletion Error", "Failed to delete item: " + query.lastError().text());
                    return;
                }   
                ui->MenuTable->removeRow(row);
                --row;
            }
        }
    }

}

void RMS:: editOrderTable()
{
    for (int row = 0; row < ui->OrderTable->rowCount(); ++row)
        for (int col = 0; col < ui->OrderTable->columnCount(); ++col)
            ui->OrderTable->item(row, col)->setTextAlignment(Qt::AlignTop | Qt::AlignLeft); ui->OrderTable->resizeColumnsToContents();

    ui->OrderTable->setColumnCount(3);
    QStringList headers = { "Item", "Qty", "Price"};
    ui->OrderTable->setHorizontalHeaderLabels(headers);
     ui->OrderTable->setColumnWidth(0, 320);
     ui->OrderTable->setColumnWidth(1, 100);
     ui->OrderTable->setColumnWidth(2, 120);

   // ui->OrderTable->horizontalHeader()->setVisible(true);
   // ui->OrderTable->horizontalHeader()->setDefaultSectionSize(8800);



}

void RMS::on_pushButton_clicked()
{
    Exit();
}

void RMS:: editOrderTable2()
{
    ui->OrderTable2->setColumnCount(4);
    ui->OrderTable2->setColumnWidth(0, 280);
    ui->OrderTable2->setColumnWidth(1, 120);
    ui->OrderTable2->setColumnWidth(2, 50);
    ui->OrderTable2->setColumnWidth(3, 50);
    QStringList headers = { "Item", "Price"};
    ui->OrderTable2->setHorizontalHeaderLabels(headers);
}



void RMS:: handleIncrement(int row) {

    QString itemName = ui->OrderTable2->item(row, 0)->text();
    double price = ui->OrderTable2->item(row, 1)->text().toDouble();

    // Check if item already exists
    for (int i = 0; i < ui->OrderTable->rowCount(); ++i) {
        if (ui->OrderTable->item(i, 0)->text() == itemName) {
            // Update Quantity and Total
            int quantity = ui->OrderTable->item(i, 1)->text().toInt() + 1;
            ui->OrderTable->item(i, 1)->setText(QString::number(quantity));
            ui->OrderTable->item(i, 2)->setText(QString::number(quantity * price));
            return;
        }
    }


    int newRow = ui->OrderTable->rowCount();
    ui->OrderTable->insertRow(newRow);
    ui->OrderTable->setItem(newRow, 0, new QTableWidgetItem(itemName));
    ui->OrderTable->setItem(newRow, 1, new QTableWidgetItem("1"));
    ui->OrderTable->setItem(newRow, 2, new QTableWidgetItem(QString::number(price)));
}

void RMS:: handleDecrement(int row) {
    QString itemName = ui->OrderTable2->item(row, 0)->text();

    // Check if item exists
    for (int i = 0; i < ui->OrderTable->rowCount(); ++i) {
        if (ui->OrderTable->item(i, 0)->text() == itemName) {
            int quantity = ui->OrderTable->item(i, 1)->text().toInt() - 1;

            if (quantity > 0) {
                // Update Quantity and Total
                double price = ui->OrderTable2->item(row, 1)->text().toDouble();
                ui->OrderTable->item(i, 1)->setText(QString::number(quantity));
                ui->OrderTable->item(i, 2)->setText(QString::number(quantity * price));
            } else {

                ui->OrderTable->removeRow(i);
            }
            return;
        }
    }
}
//LOGIN BUTTON FUNCTION
void RMS::customizeButton(QPushButton *button) {

    button->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 #AF40FF, stop:0.5 #5B42F3, stop:1 #00DDEB);"
        "    color: white;"
        "    border-radius: 20px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    padding: 10px 20px;"
        "    border: 2px solid transparent;"
        "}"
        "QPushButton:hover {"
        "    border: 2px solid #FFFFFF;"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "        stop:0 #5B42F3, stop:0.5 #00DDEB, stop:1 #AF40FF);"
        "}"
        );

    auto *shadowEffect = new QGraphicsDropShadowEffect(button);
    shadowEffect->setBlurRadius(12);
    shadowEffect->setOffset(0, 12);
    shadowEffect->setColor(QColor(160, 160, 160, 160));
    button->setGraphicsEffect(shadowEffect);

    // Create an animation for the shadow's blur radius
    auto *animation = new QPropertyAnimation(shadowEffect, "blurRadius", button);
    animation->setStartValue(12);
    animation->setEndValue(24);
    animation->setDuration(1500);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setLoopCount(-1); // Infinite loop
    animation->start();

}



void RMS::on_Calculate_clicked()
{
    onInsertData();//Store Details

    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "Confirmation",
                                  "Click OK to Print Bill",
                                  QMessageBox::Yes| QMessageBox::No,
                                  QMessageBox::Yes);

    if (reply == QMessageBox::Yes)
    {
        printWidget(ui->OrderFrame);
    } else {

        qDebug() << "User declined.";

    }



}

//TABLE TEXT/WIDTH MANAGE
void RMS:: wrapTextInTable(QTableWidget *table) {
    int rowCount = table->rowCount();
    int colCount = table->columnCount();

    table->setWordWrap(true);

    for (int col = 0; col < colCount; ++col) {
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < colCount; ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item) {
                item->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);


                QTextDocument *doc = new QTextDocument();
                doc->setPlainText(item->text());
                item->setText(doc->toPlainText());

                table->resizeRowToContents(row);
            }
        }
    }


    for (int col = 0; col < colCount; ++col) {
        table->setColumnWidth(col, 200);
    }
}

//BUTTONS STYLESHEET
void RMS::setButtonStyle(QPushButton *button) {
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #fff;"
        "   border: 0px solid #e2e8f0;"
        "   border-radius: 20px;"
        "   box-sizing: border-box;"
        "   color: #0d172a;"
        "   cursor: pointer;"
        "   font-family: Serifa BT;"
        "   font-size: 25px;"
        "   font-weight: 600;"
        "   line-height: 1;"
        "   padding: 1rem 1.6rem;"
        "   text-align: center;"
        "   text-decoration: none;"
        "   transition: all 0.1s cubic-bezier(0.4, 0, 0.2, 1);"
        "   box-shadow: 0px 1px 2px rgba(166, 175, 195, 0.25);"
        "   user-select: none;"
        "   -webkit-user-select: none;"
        "   touch-action: manipulation;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1e293b;"
        "   color: #fff;"
        "}"
        "@media (min-width: 768px) {"
        "   QPushButton {"
        "       font-size: 1.125rem;"
        "       padding: 1rem 2rem;"
        "   }"
        "}"
        );
}


void RMS::Stylesheetfunction()
{
    ui->Background->setStyleSheet("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
                                  "stop:0 rgba(91, 66, 243, 180), stop:0.4 rgba(91, 66, 243, 180), "
                                  "stop:0.6 #E0E0E0, stop:1 #A0A0A0);");

}

//Home page button style
void RMS::setButtonStyle2(QPushButton *button) {
    button->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient("
        "       spread: pad, "
        "       x1: 0, y1: 0, x2: 1, y2: 1, "
        "       stop: 0 #AF40FF, stop: 0.5 #5B42F3, stop: 1 #00DDEB"
        "   );"
        "   border: none;"
        "   border-radius: 20px;"
        "   color: white;"
        "   font-family: Serifa BT;"
        "   font-size: 30px;"
        "   padding: 3px;"
        "   text-align: center;"
        "   cursor: pointer;"
        "   box-shadow: rgba(151, 65, 252, 0.2) 0px 15px 30px -5px;"
        "   transition: all 0.3s ease;"
        "   min-width: 180px;"
        "   display: inline-block;"
        "}"
        "QPushButton:active {"
        "   transform: scale(1);"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient("
        "       spread: pad, "
        "       x1: 1, y1: 1, x2: 0, y2: 0, "
        "       stop: 0 #AF40FF, stop: 0.5 #5B42F3, stop: 1 #00DDEB"
        "   );"
        "}"
        );
}

void RMS::addHoverAnimation(QPushButton *button) {
    QPropertyAnimation *animation = new QPropertyAnimation(button, "geometry");
    animation->setDuration(400);
    animation->setStartValue(button->geometry());
    animation->setEndValue(button->geometry().adjusted(-5, -5, 5, 5));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(button, &QPushButton::pressed, [animation]()
    {
        animation->stop();
        animation->start();
    });
}

void RMS::on_ClearOrderTable_clicked()
{
    int billID = rand() % 100000;
    QString billid = QString::number(billID);
    ui->BillIdDisplay->setText(billid);
    int rowCount = ui->OrderTable->rowCount();
    qDebug() << "Row Count: " << rowCount;
    if (ui->OrderTable->rowCount() > 0)
    {
        ui->OrderTable->setRowCount(0);
        ui->TotalBox->clear();
        ui->NetTotalBox->clear();
        ui->VatAmtBox->clear();
    }
    else {
        qDebug() << "Table is already empty!";
    }
}


void RMS::printWidget(QWidget *widgetToPrint) {

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog printDialog(&printer);
    if (printDialog.exec() != QDialog::Accepted) {
        return;
    }

    QPainter painter;
    if (!painter.begin(&printer)) {
        qDebug() << "Failed to begin painting on printer.";
        return;
    }
    widgetToPrint->render(&painter);
    painter.end();
}

void RMS::slideLabelToUp(QLabel *label) {
    int startPos = label->y();


    int endPos = startPos - 150;

    QPropertyAnimation *animation = new QPropertyAnimation(label, "pos");
    animation->setDuration(900);
    animation->setStartValue(QPoint(label->x(), startPos));
    animation->setEndValue(QPoint(label->x(), endPos));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void RMS::fadeInWidget(QLabel *label) {

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect();
    label->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);

    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(900);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);


    animation->start(QAbstractAnimation::DeleteWhenStopped);
    label->show();
}

void RMS::fadeInTextBox(QTextEdit *textBox)
{

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect();
    textBox->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);


    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(900);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    textBox->show();
}
void RMS::fadeInTextBox2(QLineEdit *textBox)
{

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect();
    textBox->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0);


    QPropertyAnimation *animation = new QPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(900);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    textBox->show();
}

void RMS:: UncheckCheckbox()
{
    int rowCount =ui->MenuTable->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QWidget* widget = ui->MenuTable->cellWidget(row, 2);

        if (widget) {
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
            if (checkBox) {
                checkBox->setChecked(false);

            }
        }
    }
}


void RMS::on_RecordsBack_clicked()
{
    ui->ErrorLabel->clear();
    Exit();
}

void RMS::on_ConfirmDetails_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to proceed?",QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {

    QString name = ui->EnterName->text();
    QString phone = ui->EnterPhone->text();
    QString address = ui->EnterAddress->text();
    QString email = ui->EnterEmail->text();
    QString password = ui->EnterPass->text();
 if (name.isEmpty() || phone.isEmpty() || address.isEmpty() || email.isEmpty() || password.isEmpty()) {
        ui->ErrorLabel->setText("Error: All fields must be filled!");
        return;
    }


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");
    if (!db.open()) {
        ui->ErrorLabel->setText("Error: Failed to connect to database.");
        return;
    }


    QString user=name;
    QSqlQuery queryCheck(db);
    queryCheck.prepare("SELECT COUNT(*) FROM Login WHERE Username LIKE :usernamePattern");
    queryCheck.bindValue(":usernamePattern", name );
    queryCheck.exec();

    if (queryCheck.next()) {
        int count = queryCheck.value(0).toInt();
        user += QString::number(count + 1);
    }


    QSqlQuery query(db);
    query.prepare("INSERT INTO Login (Username, Name, Phone, Address, Email, Password) "
                  "VALUES (:username, :name, :phone, :address, :email, :password)");


    query.bindValue(":username", user);
    query.bindValue(":name", name);
    query.bindValue(":phone", phone);
    query.bindValue(":address", address);
    query.bindValue(":email", email);
    query.bindValue(":password", password);


    if (!query.exec()) {
        ui->ErrorLabel->setText("Failed to register employee.");
    } else {
        ui->ErrorLabel->setText("Employee registered successfully!");
         displayDetails(user);
    }

    db.close();
    }
}



void RMS::on_CancelDetails_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to proceed?",QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        ui->EnterAddress->clear();
        ui->EnterPhone->clear();
        ui->EnterPass->clear();
        ui->EnterName->clear();
        ui->EnterEmail->clear();
    }

}


void RMS::on_ClearButton_clicked()
{
    ui->EmployedDetailLabel->hide();
    ui->DisplayAddress->clear();
    ui->DisplayName->clear();
    ui->DisplayEmail->clear();
    ui->DisplayUsername->clear();
    ui->DisplayPhone->clear();
    ui->SearchBar->clear();
}



void RMS::on_SearchDetailButton_clicked()
{
    ui->EmployedDetailLabel->show();
    QString searchText = ui->SearchBar->text();
    // Check if the username is empty
    if (searchText.isEmpty()) {
        ui->ErrorLabel2->setText("Please enter a username.");
        return;
    }

    // Open the database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");

    if (!db.open()) {
        ui->ErrorLabel2->setText("Failed to connect to the database.");
        return;
    }


    QSqlQuery query(db);
    query.prepare("SELECT Name, Address, Phone, Email FROM Login WHERE Username = :username");
    query.bindValue(":username", searchText);  // Bind the input username to the query

    if (query.exec()) {
        if (query.next()) {
            qDebug() << "Query executed successfully";

            ui->DisplayName->setText("Name: " + query.value("Name").toString());
            ui->DisplayAddress->setText("Address: " + query.value("Address").toString());
            ui->DisplayPhone->setText("Phone: " + query.value("Phone").toString());
            ui->DisplayEmail->setText("Email: " + query.value("Email").toString());
            ui->DisplayUsername->setText("Login Username: " + searchText);

        } else {
            qDebug() << "No employee found";
            ui->ErrorLabel2->setText("No employee found with the given username.");
        }
    } else {
        ui->ErrorLabel2->setText("Failed to execute the query.");
    }

    db.close();
}

void RMS:: displayDetails(QString st)
{
    QString username=st;
    ui->SearchBar->setText(username);
    on_SearchDetailButton_clicked();
}


void RMS:: editSalesTable()
{
    ui->SalesTable->setColumnCount(5);
    ui->SalesTable->setColumnWidth(0, 200);
    ui->SalesTable->setColumnWidth(1, 120);
    ui->SalesTable->setColumnWidth(2, 120);
    ui->SalesTable->setColumnWidth(3, 120);
    ui->SalesTable->setColumnWidth(4, 120);
    ui->SalesTable->setRowCount(2);
    QStringList headers = { "Cashier", "BillID","Total","VAT","Net Total"};
    ui->SalesTable->setHorizontalHeaderLabels(headers);

}

void RMS::onCalendarSelectionChanged()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/BIB/QT/ResturantManagement/saal.db");

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QDate selectedDate = ui->Calendar->selectedDate();
    QString formattedDate = selectedDate.toString("dd-MM-yyyy");

    // Debug: Output the selected date
    qDebug() << "Selected Date:" << formattedDate;

    QSqlQuery query;
    // Update SQL query to match the correct column for the date (assuming it's the 6th column)
    query.prepare("SELECT cashier, bill_id, total, VAT, net_total FROM Sales WHERE date = :date");
    query.bindValue(":date", formattedDate);

    // Debug: Output the query being executed
    qDebug() << "Executing query:" << query.executedQuery();

    // Execute the query
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        return;
    }

    // Clear previous data in the table
    ui->SalesTable->clearContents();
    ui->SalesTable->setRowCount(0);

    // Populate the table with data from the query
    int row = 0;
    while (query.next()) {
        ui->SalesTable->insertRow(row);
        ui->SalesTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // cashier
        ui->SalesTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // bill_id
        ui->SalesTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // total
        ui->SalesTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString())); // VAT
        ui->SalesTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString())); // net_total
        row++;
    }
}

void RMS::calculateTotal()
{
    double netTotal=0.0;

    for (int row = 0; row < ui->OrderTable->rowCount(); ++row) {
        QTableWidgetItem *item = ui->OrderTable->item(row, 2);
        if (item) {
            qDebug() << "Row:" << row << "Price:" << item->text();
            bool ok;
            double value = item->text().toDouble(&ok);
            if (ok) {
                netTotal += value;
            } else {
                qDebug() << "Row:" << row << "Invalid value in Price column:" << item->text();
            }
        } else {
            qDebug() << "Row:" << row << "Price column is empty.";
        }
    }

    ui->TotalBox->setText(QString::number(netTotal, 'f', 2));
    double VATamt=0.13*netTotal;
    netTotal=netTotal+VATamt;
    qDebug() << "Net Total:" << netTotal;

    ui->VatAmtBox->setText(QString::number(VATamt, 'f', 2));
    ui->NetTotalBox->setText(QString::number(netTotal, 'f', 2));
}

void RMS::onInsertData()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:/BIB/QT/ResturantManagement/saal.db");

    if (!db.open()) {
        qDebug() << "Error: Could not open database";
        return;
    }

    QSqlQuery query;

    // Start the transaction
    db.transaction();

    QString username = ui->Username->text();
    QString billId = ui->BillIdDisplay->text();
    QString date = ui->Date->text();

    // Convert total, vat, and netTotal to double
    double total = ui->TotalBox->text().toDouble();
    double vat = ui->VatAmtBox->text().toDouble();
    double netTotal = ui->NetTotalBox->text().toDouble();

    query.prepare("INSERT INTO Sales (cashier, bill_id, total, VAT, net_total, date) "
                  "VALUES (:cashier, :bill_id, :total, :VAT, :net_total, :date)");

    query.bindValue(":cashier", username);
    query.bindValue(":bill_id", billId);
    query.bindValue(":total", total);
    query.bindValue(":VAT", vat);
    query.bindValue(":net_total", netTotal);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "Insert failed: " << query.lastError();
        // Rollback the transaction if the insert fails
        db.rollback();
    } else {
        qDebug() << "Data inserted successfully!";
        // Commit the transaction if the insert is successful
        db.commit();
    }

    db.close();
}

void RMS:: edittableHistory()
{
    ui->tableHistory->setColumnCount(3);
    ui->tableHistory->setColumnWidth(0,200);
    ui->tableHistory->setColumnWidth(1,330);
    ui->tableHistory->setColumnWidth(2,330);

}

void RMS::closeEvent(QCloseEvent *event)
{

    if (ui->NameLabel->text() != "") {
        QString username = ui->NameLabel->text();
        QString logoutTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");


        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");

        if (db.open()) {
            QSqlQuery query(db);


            query.prepare("UPDATE History SET Logout = :logoutTime WHERE Cashier = :username AND Logout IS NULL");
            query.bindValue(":logoutTime", logoutTime);
            query.bindValue(":username", username);
            if (!query.exec()) {
                QMessageBox::critical(this, "Database Error", "Failed to update logout time.");
            }
        } else {
            QMessageBox::critical(this, "Database Error", "Unable to connect to database.");
        }

        db.close();
    }

    event->accept();  // Accept the close event to close the application
}






void RMS::on_BackHistory_clicked()
    {

        Exit();
    }


void RMS::on_ClearSales_clicked()
    {
   ui->SalesTable->clear();
    }


void RMS::on_BackSales_clicked()
    {

        Exit(); }


void RMS::on_RefreshButton_clicked()
    {
    ui->tableHistory->clearContents();
    ui->tableHistory->setRowCount(0);

    // Use a unique connection name
    QString connectionName = "history_connection";

    // Check if the connection already exists and remove it if necessary
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Add the database connection with a unique name
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("C:/BIB/QT/ResturantManagement/login.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Unable to connect to the database.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT Cashier, Login, Logout FROM History");

    if (query.exec()) {
        int row = 0;

        while (query.next()) {
            ui->tableHistory->insertRow(row);

            QString cashier = query.value("Cashier").toString();
            QString loginTime = query.value("Login").toString();
            QString logoutTime = query.value("Logout").toString();

            ui->tableHistory->setItem(row, 0, new QTableWidgetItem(cashier));
            ui->tableHistory->setItem(row, 1, new QTableWidgetItem(loginTime));
            ui->tableHistory->setItem(row, 2, new QTableWidgetItem(logoutTime));

            if (!logoutTime.isEmpty()) {
                QDateTime login = QDateTime::fromString(loginTime, "yyyy-MM-dd HH:mm:ss");
                QDateTime logout = QDateTime::fromString(logoutTime, "yyyy-MM-dd HH:mm:ss");
                qint64 durationInSeconds = login.secsTo(logout);
                int hours = durationInSeconds / 3600;
                int minutes = (durationInSeconds % 3600) / 60;
                int seconds = durationInSeconds % 60;

                QString duration = QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
                                       .arg(minutes, 2, 10, QChar('0'))
                                       .arg(seconds, 2, 10, QChar('0'));
                ui->tableHistory->setItem(row, 3, new QTableWidgetItem(duration));
            } else {
                ui->tableHistory->setItem(row, 3, new QTableWidgetItem("Not Logged Out"));
            }

            row++;
        }
    } else {
        QMessageBox::critical(this, "Database Error", "Failed to retrieve history data.");
    }

    db.close();
    }

