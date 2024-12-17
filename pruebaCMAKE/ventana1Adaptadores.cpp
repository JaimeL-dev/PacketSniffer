#include "ventana1Adaptadores.h"
#include "./ui_ventana1Adaptadores.h"
#include <pcap.h>
#include <QDebug>
#include <QStringList>
#include "ventanacaptura.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->inicializarComboBox();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// VARIABLES
string devName;

void MainWindow::inicializarComboBox()
{
    char errbuf[PCAP_ERRBUF_SIZE]; // Buffer para almacenar mensajes de error
    pcap_if_t* alldevs;           // Lista de dispositivos disponibles

    // Obtener la lista de dispositivos de red
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        qDebug() << "Error al obtener los dispositivos de red:" << errbuf;
        return;
    }

    // Asegurarse de que la ComboBox esté disponible
    if (ui->comboBox) {
        // Agregar cada dispositivo encontrado a la ComboBox
        for (pcap_if_t* d = alldevs; d != nullptr; d = d->next) {
            QString deviceFullName = QString(d->name); // Nombre completo del dispositivo
            QString description = d->description ? QString(d->description) : "Sin descripción";

            // Mostrar el nombre completo junto con la descripción si está disponible
            QString displayText = description + " (" + deviceFullName + ")";
            ui->comboBox->addItem(displayText, deviceFullName);

            qDebug() << "Dispositivo encontrado:" << displayText;
        }
    } else {
        qDebug() << "ComboBox no encontrada.";
    }

    // Liberar la memoria utilizada para la lista de dispositivos
    pcap_freealldevs(alldevs);
}


void MainWindow::on_pushButton_clicked()
{
    // Obtener el nombre completo del dispositivo desde los datos asociados
    QString deviceFullName = ui->comboBox->currentData().toString();

    // Crear la nueva ventana pasando el nombre completo del dispositivo
    ventanaCaptura *ventana = new ventanaCaptura(nullptr, deviceFullName);
    ventana->show();
    this->close();
}




