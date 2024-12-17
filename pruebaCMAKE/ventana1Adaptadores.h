#ifndef VENTANA1ADAPTADORES_H
#define VENTANA1ADAPTADORES_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    // Nueva función para inicializar la ComboBox
    void inicializarComboBox();
};

#endif // VENTANA1ADAPTADORES_H
