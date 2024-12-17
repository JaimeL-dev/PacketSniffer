#ifndef VENTANACAPTURA_H
#define VENTANACAPTURA_H

#include <QMainWindow>
#include <QVector>
#include "hiloCaptura.h"
#include "ventana1Adaptadores.h"

namespace Ui {
class ventanaCaptura;
}

class ventanaCaptura : public QMainWindow
{
    Q_OBJECT

public:
    explicit ventanaCaptura(QWidget *parent = nullptr, QString devname = "");
    ~ventanaCaptura();

private slots:
    void mostrarPaqueteEnTabla(const QString &time,
                               const QString &source,
                               const QString &destination,
                               const QString &protocol,
                               const QString &length,
                               const QString &info,
                               const QString &rawData,
                               const QString &macSource,
                               const QString &macDestination,
                               const QString &ttl,
                               const QString &portSource,
                               const QString &portDestination,
                               const QString &flags);

    void mostrarRawData(int row, int column); // Nuevo slot para manejar clic en la tabla
    void mostrarDataEstructurada(int row, int column); //Slot para data structurada, misma funcion que raw data

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void guardarTablaCSV();

    void on_pushButton_5_clicked();
    void generarGrafica();



private:
    Ui::ventanaCaptura *ui;
    hiloCaptura *captura;
    QVector<QString> rawDataList; // Almacena la raw data de cada paquete
    QVector<QString> macSourceList;
    QVector<QString> macDestinationList;
    QVector<QString> ttlList;
    QVector<QString> portSourceList;
    QVector<QString> portDestinationList;
    QVector<QString> flagsList;

};

#endif // VENTANACAPTURA_H
