#ifndef DATABASE_H
#define DATABASE_H
#include<QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QVector>

class QWidget;
class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QGridLayout;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QTreeWidget;
class QTreeWidgetItem;
class QGroupBox;
class QToolButton;
class QTableWidget;
class QCheckBox;
class QTableWidgetItem;
class QTimer;

namespace Ui {
class database;
}

class database : public QObject
{
    Q_OBJECT

//public:
//    explicit database(QWidget *parent = 0);
//    ~database();

private:
    enum{MANAGER,USER,VISITOR,USER_LOGIN,USER_REGISTER};
    QString current_user;

    int user_type;
    int current_page;
    int max_page;

    int user_current_page;
    int user_max_page;
    int user_num;

    int current_row_number;

    QTimer *timer;

    QTableWidget *tablewidget;
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout0;
    QVBoxLayout *vboxLayout1;
    QVBoxLayout *vboxLayout;
    QWidget *titleImage;

    QVBoxLayout *vboxLayout3;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout1;
    QHBoxLayout *hboxLayout;
    QHBoxLayout *hboxLayout2;
    QHBoxLayout *hboxLayout3;

    QLabel *spacelabel2;
    QLabel *spacelabel3;
    QGroupBox *groupbox;
    QGroupBox *groupbox1;
    QPushButton *clearbutton;
    QPushButton *searchbutton;
    QPushButton *registerbutton;
    QPushButton *loginbutton;
    QPushButton *quitloginbutton;
    QLabel *label;

    QTreeWidget *tree;
    QTreeWidgetItem *root;

    QLineEdit *nameLine;
    QLineEdit *publishLine;
    QLineEdit *priceLine;
    QLineEdit *priceLine1;
    QLineEdit *authorLine;
    QComboBox *dateBox;
    QComboBox *dateBox1;
    QComboBox *isLent;
    QWidget *window;

    QPushButton *nextbutton;
    QPushButton *lastbutton;

    QSqlDatabase db;
    QSqlQuery query;
    QTabWidget *mainTabWidget;
    QTabWidget *loginTabWidget;
    QWidget *user_loginWidget;
    QWidget *manager_loginWidget;

    QLineEdit *username1;
    QLineEdit *password1;
    QLineEdit *username2;
    QLineEdit *password2;

    QPushButton *okbutton;
    QVBoxLayout *vboxLayout4;
    QVBoxLayout *vboxLayout5;
    QVBoxLayout *vboxLayout6;
    QHBoxLayout *hboxLayout4;
    QHBoxLayout *hboxLayout5;
    QHBoxLayout *hboxLayout6;
    QGridLayout *gridLayout2;
    QTabWidget *manageTabWidget;
    QWidget *addBookWidget;
    QLabel *label2;

    QLineEdit *nameLine2;
    QComboBox *typeBox2;
    QLineEdit *priceLine2;
    QLineEdit *authorLine2;
    QLineEdit *numLine2;
    QLineEdit *publishLine2;
    QComboBox *dateBox2;

    QWidget *window1;
    QWidget *window2;
    QWidget *window3;
    QWidget *window4;
    QWidget *window5;

    QWidget *registerWindow;
    QLabel *label3;
    QLineEdit *nameLine3;
    QLineEdit *passwordLine1;
    QLineEdit *passwordLine2;
    QPushButton *registerOkButton;
    QHBoxLayout *hboxLayout7;
    QHBoxLayout *hboxLayout8;
    QVBoxLayout *vboxLayout7;
    QGridLayout *gridLayout3;
    QLabel *label4;
    QVector<QString>result;

    QLabel *label5;
    QLineEdit *idLine;
    QVBoxLayout *vboxLayout9;
    QHBoxLayout *hboxLayout9;
    QWidget *deletebookWidget;
    QPushButton *deleteOkButton;

    QTableWidget *tablewidget1;
    QVBoxLayout *vboxLayout10;
    QHBoxLayout *hboxLayout10;
    QPushButton *deleteUserButton;
    QPushButton *grantLentButton;
    QPushButton *lastPageButton;
    QPushButton *nextPageButton;
    QTableWidgetItem *item0[13];
    QTableWidgetItem *item1[13];
    QVector<QString*>userMessage;

    QTableWidgetItem *item2[10];
    QTableWidgetItem *item3[10];
    QPushButton *lendBookButton;

    QTableWidget *tablewidget2;
    QVector<QString*>loanMessage;
    QHBoxLayout *hboxLayout11;
    QVBoxLayout *vboxLayout11;
    QTableWidgetItem *item4[3];
    QPushButton *returnBookButton;

    QTableWidget *tablewidget3;
    QVBoxLayout *vboxLayout12;
    QHBoxLayout *hboxLayout12;
    QPushButton *addBookOkButton;
    QPushButton *addBookClearButton;

    QComboBox *bookType[13];
    QComboBox *publishDate[13];
    QString bookMessage[7];

    QLabel *tipLabel;
    QWidget *fileWindow;
    QHBoxLayout *hboxLayout13;
    QPushButton *loadFileButton;
    QVector<QString*>multiBookMessage;

    void setLayout();
    bool createConnection();

    void setWindowShowBook(int type);
    void setWindowSearchBook(int type);
    void setWindowAddBook();
    void setWindowUserManage();
    void setWindowLoanBook();
    void setWindowMutiAddBook();
    void setWindowTitle();

    void updateManageWindow();
    void updateShowBookWindow();
    void updateLoanBookWindow();
    void updateTitle(int type);

    bool addBook();

    void loadUserMessage();
    void loadLoanMessage();

    QWidget* createLoginWindow(int type);

public:

    database(QObject *parent = 0);
    ~database();

private slots:

    void searchBook();
    void updateTime();
    void clear();
    void quitLogin();
    void returnBook();
    void managerLogin();
    void deleteLogin();
    void bookLastPage();
    void bookNextPage();
    void deleteBook();
    void searchBookByType(QTreeWidgetItem*,int);
    void userLogin();
    void setWindowRegister();
    void setWindowLogin();
    void Register();
    void deleteUser();
    void grantLent();
    void userNextPage();
    void userLastPage();
    void lendBook();
    void oneAddBook();
    void multiAddBook();
    void clearAddBookMessage();
    void openFile();
};

#endif // DATABASE_H
