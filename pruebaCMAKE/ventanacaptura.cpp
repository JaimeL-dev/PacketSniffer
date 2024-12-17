#include "ventanacaptura.h"
#include "ui_ventanacaptura.h"
#include <QDebug>
#include <QFile>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPixmap>
#include <QTextStream>
#include <QFileDialog>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QMap>


ventanaCaptura::ventanaCaptura(QWidget *parent, QString devname)
    : QMainWindow(parent)
    , ui(new Ui::ventanaCaptura)
{
    ui->setupUi(this);

    qDebug() << "Nombre del dispositivo:" << devname;

    // Configurar QTableWidget
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels({"No.", "Hora", "IPFuente", "IPDestino", "Protocolo", "Longitud", "Información"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    connect(ui->pushButton_7, &QPushButton::clicked, this, &ventanaCaptura::generarGrafica);


    // Conectar el clic en la tabla al slot mostrarRawData
    connect(ui->tableWidget, &QTableWidget::cellClicked,
            this, &ventanaCaptura::mostrarRawData);

    // Inicializar y configurar el hilo de captura
    captura = new hiloCaptura(devname, this);

    connect(captura, &hiloCaptura::packetCaptured,
            this, &ventanaCaptura::mostrarPaqueteEnTabla);

    captura->start();
}

ventanaCaptura::~ventanaCaptura()
{
    if (captura) {
        captura->stop();
        captura->wait();
    }
    delete ui;
}

boolean pasaFiltro,
    filtroProtocolo=false,
    filtroIpFuente=false,
    filtroIpDestino=false,
    filtroTamMax=false,
    filtroTamMin=false;
void ventanaCaptura::mostrarPaqueteEnTabla(const QString &time,
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
                                           const QString &flags)
{
    pasaFiltro=true;
    if(filtroProtocolo){
        if(protocol!=ui->textEdit->toPlainText()){
            pasaFiltro=false;
        }
    }
    if(filtroIpFuente){
        if(source!=ui->textEdit->toPlainText()){
            pasaFiltro=false;
        }
    }
    if(filtroIpDestino){
        if(destination!=ui->textEdit->toPlainText()){
            pasaFiltro=false;
        }
    }
    if(filtroTamMax){
        if(length.toInt()>ui->textEdit->toPlainText().toInt()){
            pasaFiltro=false;
        }
    }
    if(filtroTamMin){
        if(length.toInt()<ui->textEdit->toPlainText().toInt()){
            pasaFiltro=false;
        }
    }
    if(pasaFiltro){
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(time));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(source));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(destination));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(protocol));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(length));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(info));

        // Almacenar la raw data
        rawDataList.append(rawData);
        macSourceList.append(macSource);
        macDestinationList.append(macDestination);
        ttlList.append(ttl);
        portSourceList.append(portSource);
        portDestinationList.append(portDestination);
        flagsList.append(flags);
    }
}

void ventanaCaptura::mostrarRawData(int row, int column)
{
    Q_UNUSED(column);

    // Obtener la raw data correspondiente a la fila seleccionada
    if (row >= 0 && row < rawDataList.size()) {
        ui->plainTextEdit->setPlainText(rawDataList[row]);
    } else {
        qDebug() << "Fila fuera de rango";
    }
    mostrarDataEstructurada(row,column);
}


void ventanaCaptura::on_pushButton_4_clicked()
{
    if(captura->isRunning())
        captura->stop();
}


void ventanaCaptura::on_pushButton_6_clicked()
{
    if (!captura->isRunning()) {
        captura->restart();
    } else {
        qDebug() << "El hilo ya está activo.";
    }
}
void ventanaCaptura::on_pushButton_2_clicked()
{
    if (captura && captura->isRunning()) {
        captura->stop();
        captura->wait();
    }

    ui->tableWidget->setRowCount(0);
    rawDataList.clear();
    macSourceList.clear();
    macDestinationList.clear();
    ttlList.clear();
    portSourceList.clear();
    portDestinationList.clear();
    flagsList.clear();

    MainWindow *ventanaPrincipal = new MainWindow();
    ventanaPrincipal->show();

    this->close();
}
void ventanaCaptura::mostrarDataEstructurada(int row, int column)
{
    Q_UNUSED(column); // No necesitamos la columna seleccionada

    if (row >= 0 && row < rawDataList.size()) {
        QString structuredData;

        // Estructura del paquete
        structuredData += "Paquete Capturado:\n";

        // Direcciones MAC
        structuredData += "   MAC Fuente: " + macSourceList[row] + "\n";
        structuredData += "   MAC Destino: " + macDestinationList[row] + "\n\n";

        // Detalles de IPv4
        structuredData += "Internet Protocol Version 4 (IPv4):\n";
        structuredData += "   TTL: " + ttlList[row] + "\n";
        structuredData += "   IP Fuente: " + ui->tableWidget->item(row, 2)->text() + "\n";
        structuredData += "   IP Destino: " + ui->tableWidget->item(row, 3)->text() + "\n\n";

        // Detalles del protocolo de transporte
        structuredData += "Transporte:\n";
        structuredData += "   Puerto Fuente: " + portSourceList[row] + "\n";
        structuredData += "   Puerto Destino: " + portDestinationList[row] + "\n";
        structuredData += "   Banderas: " + flagsList[row] + "\n";

        // Mostrar la data estructurada en el QPlainTextEdit
        ui->plainTextEdit_->setPlainText(structuredData);
    } else {
        qDebug() << "Fila fuera de rango";
    }
}



void ventanaCaptura::on_pushButton_3_clicked()
{
    ui->textEdit->setText("");
    filtroProtocolo=false;
    filtroIpFuente=false;
    filtroIpDestino=false;
    filtroTamMax=false;
    filtroTamMin=false;
}


void ventanaCaptura::on_pushButton_clicked()
{
    if(!ui->textEdit->toPlainText().isEmpty()){
        switch(ui->comboBox->currentIndex()){
            case 0: filtroProtocolo=true;break;
            case 1: filtroIpFuente=true;break;
            case 2: filtroIpDestino=true;break;
            case 3: filtroTamMax=true;break;
            case 4: filtroTamMin=true;break;
            default:break;
        }
    }
}

void ventanaCaptura::guardarTablaCSV()
{
    if(captura->isRunning())
        captura->stop();
    // Seleccionar la ruta y nombre del archivo usando un diálogo
    QString fileName = QFileDialog::getSaveFileName(this, "Guardar como CSV", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        qDebug() << "No se seleccionó ningún archivo.";
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "No se pudo abrir el archivo para escritura:" << fileName;
        return;
    }

    QTextStream stream(&file);

    // Guardar las cabeceras de la tabla
    QStringList headers;
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QString headerText = ui->tableWidget->horizontalHeaderItem(col)
        ? ui->tableWidget->horizontalHeaderItem(col)->text()
        : "";
        headers << "\"" + headerText + "\"";
    }
    stream << headers.join(";") << "\n";

    // Guardar el contenido de la tabla
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            QString cellText = item ? item->text() : "";
            rowData << "\"" + cellText + "\"";
        }
        stream << rowData.join(";") << "\n";
    }

    file.close();
    qDebug() << "Tabla guardada correctamente en el archivo:" << fileName;
}

void ventanaCaptura::on_pushButton_5_clicked()
{
    guardarTablaCSV();
}


//Generacion de graficas
void ventanaCaptura::generarGrafica()
{
    QString seleccion = ui->comboBoxGrafica->currentText();
    QString tipoGrafica = ui->comboBoxTipoGrafica->currentText();
    QMap<QString, int> frecuencia;

    // Calcular frecuencias
    if (seleccion == "Protocolo") {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
            frecuencia[ui->tableWidget->item(i, 4)->text()]++;
    } else if (seleccion == "IP Fuente") {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
            frecuencia[ui->tableWidget->item(i, 2)->text()]++;
    } else if (seleccion == "IP Destino") {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
            frecuencia[ui->tableWidget->item(i, 3)->text()]++;
    } else if (seleccion == "Longitud") {
        for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
            int longitud = ui->tableWidget->item(i, 5)->text().toInt();
            if (longitud >= 0 && longitud <= 512) frecuencia["<= 512"]++;
            else if (longitud > 512 && longitud <= 1024) frecuencia["513-1024"]++;
            else if (longitud > 1024) frecuencia["> 1024"]++;
        }
    }

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);



    if (tipoGrafica == "Pastel") {
        QPieSeries *series = new QPieSeries();

        for (auto it = frecuencia.begin(); it != frecuencia.end(); ++it) {
            QPieSlice *slice = series->append(it.key(), it.value());
            slice->setLabel(QString("%1 (%2%)")
                                .arg(it.key())
                                .arg(100.0 * it.value() / ui->tableWidget->rowCount(), 0, 'f', 1));
            slice->setLabelVisible(true);
            slice->setExploded(true);
            slice->setExplodeDistanceFactor(0.1);
            slice->setLabelPosition(QPieSlice::LabelOutside);
        }

        series->setLabelsVisible(true);
        chart->addSeries(series);
        chart->setTitle("Distribución de " + seleccion);
    } else if (tipoGrafica == "Barras") {
        QBarSeries *barSeries = new QBarSeries();
        QBarSet *set = new QBarSet("");

        QStringList categories;
        for (auto it = frecuencia.begin(); it != frecuencia.end(); ++it) {
            *set << it.value();
            categories << it.key();
        }

        barSeries->append(set);
        chart->addSeries(barSeries);

        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        barSeries->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Frecuencia");
        chart->addAxis(axisY, Qt::AlignLeft);
        barSeries->attachAxis(axisY);

        set->setColor(QColor::fromRgb(100, 150, 240));
        chart->setTitle("Grafico de Barras de " + seleccion);

    }

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow *graficaVentana = new QMainWindow();
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *centralWidget = new QWidget();


    //Boton para guardar img
    QPushButton *btnGuardar = new QPushButton("Guardar Imagen");
    connect(btnGuardar, &QPushButton::clicked, [chartView]() {
        QString filePath = QFileDialog::getSaveFileName(nullptr, "Guardar Grafica", "", "PNG Files (*.png)");
        if (!filePath.isEmpty()) {
            QPixmap pixmap(chartView->size());
            QPainter painter(&pixmap);
            chartView->render(&painter);
            pixmap.save(filePath);
            painter.end();
        }
    });
    //agregamos el layout
    layout->addWidget(chartView);
    layout->addWidget(btnGuardar);
    centralWidget->setLayout(layout);

    graficaVentana->setCentralWidget(centralWidget);
    graficaVentana->resize(900, 700);
    graficaVentana->setWindowTitle("Graficos");
    graficaVentana->show();
}
