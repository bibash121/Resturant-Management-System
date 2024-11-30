#ifndef RMS_H
#define RMS_H
#include <QtSql>
#include <QSqlDatabase>
#include <QMainWindow>
#include<QStackedWidget>
#include<QMessageBox>
#include<QFile>
#include <QTableWidgetItem>
#include "QTextEdit"
#include<QRadioButton>
#include <QCheckBox>
#include<QTableView>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include<QPrinter>
#include<QPainter>
#include <QPrintDialog>
#include <QPageSize>
#include <QMarginsF>
#include <QPageLayout>
#include <QGraphicsOpacityEffect>
#include <QTableWidgetItem>
#include<QTableWidget>
#include <QCalendarWidget>
#include<QCloseEvent>
QT_BEGIN_NAMESPACE
namespace Ui {
class RMS;
}
QT_END_NAMESPACE

class RMS : public QMainWindow
{
    Q_OBJECT

public:
    RMS(QWidget *parent = nullptr);
    ~RMS();
    void entry();
    void Exit();
    void adjustTableColumnWidths();
    void keyPressEvent(QKeyEvent *event);
    void loadMenuFromDatabase();
    void editOrderTable();
    void editOrderTable2();
    void loadMenuForOrder();
    void handleIncrement(int row);
    void handleDecrement(int row);
    void  setui(int row);
    void Stylesheetfunction();
    void customizeButton(QPushButton *button);
    void  calculateTotal();
    void wrapTextInTable(QTableWidget *table);
    void setButtonStyle(QPushButton *button);
    //For Main Page Buttons
    void setButtonStyle2(QPushButton *button);
    //for animation
    void addHoverAnimation(QPushButton *button) ;
    void printWidget(QWidget *widgetToPrint);
    void slideLabelToUp(QLabel *label);
    void fadeInWidget(QLabel *label);
    void fadeInTextBox(QTextEdit *textBox);
    void  UncheckCheckbox();
    void  displayDetails(QString st);
    void editSalesTable();
    void fadeInTextBox2(QLineEdit *textBox);
    void onCalendarSelectionChanged();
    void onInsertData();
    void  edittableHistory();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_MenuB_clicked();

    void on_LoginButton_clicked();

    void on_OrderB_clicked();

    void on_BillB_clicked();

    void on_RecordB_clicked();

    void on_LogoutB_clicked();

    void on_ReturnButton_clicked();


    void on_AddButton_clicked();

    void on_ConfirmButton_clicked();

    void on_CancelAdd_clicked();


    void on_CancelSelect_clicked();

    void on_SelectButton_clicked();

    void on_DeleteButton_clicked();

    void on_pushButton_clicked();

    void on_Calculate_clicked();

    void on_ClearOrderTable_clicked();


    void on_RecordsBack_clicked();

    void on_ConfirmDetails_clicked();

    void on_CancelDetails_clicked();

    void on_ClearButton_clicked();

    void on_SearchDetailButton_clicked();





    void on_BackHistory_clicked();

    void on_ClearSales_clicked();

    void on_BackSales_clicked();

    void on_RefreshButton_clicked();

private:
    Ui::RMS *ui;
};
bool setupDatabaseConnection();
void setTableMultiline(QTableWidget* tableWidget);
#endif // RMS_H
