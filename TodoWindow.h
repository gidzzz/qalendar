#include "ui_TodoWindow.h"

#include <CTodo.h>

#include "ChangeClient.h"

class TodoWindow : public QMainWindow, public ChangeClient
{
    Q_OBJECT

public:
    TodoWindow(CTodo *todo, QWidget *parent);
    ~TodoWindow();

    void onChange();

private:
    Ui::TodoWindow *ui;

    CTodo *todo;

    void reload();

    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private slots:
    void editTodo();
    void deleteTodo();
};
