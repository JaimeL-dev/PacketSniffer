#include "hiloCaptura.h"
#include <QDateTime>

hiloCaptura::hiloCaptura(const QString &deviceName, QObject *parent)
    : QThread(parent), deviceName(deviceName), running(true) {}

void hiloCaptura::run() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(deviceName.toStdString().c_str(), 65536, 1, 1000, errbuf);

    if (!handle) {
        qDebug("Error: %s", errbuf);
        return;
    }

    while (running) {
        struct pcap_pkthdr *header;
        const u_char *data;
        int res = pcap_next_ex(handle, &header, &data);

        if (res == 1 && running) {
            // Inicializar variables
            QString time = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            QString source = "";
            QString destination = "";
            QString protocol = "";
            QString length = QString::number(header->len);
            QString info = "Datos capturados del paquete";
            QString rawData;

            // Convertir raw data a hexadecimal
            for (uint i = 0; i < header->caplen; ++i) {
                rawData += QString("%1 ").arg(data[i], 2, 16, QChar('0')).toUpper();
            }

            // Parsear cabecera Ethernet
            QString macSource = QString("%1:%2:%3:%4:%5:%6")
                                    .arg(data[6], 2, 16, QChar('0'))
                                    .arg(data[7], 2, 16, QChar('0'))
                                    .arg(data[8], 2, 16, QChar('0'))
                                    .arg(data[9], 2, 16, QChar('0'))
                                    .arg(data[10], 2, 16, QChar('0'))
                                    .arg(data[11], 2, 16, QChar('0')).toUpper();
            QString macDestination = QString("%1:%2:%3:%4:%5:%6")
                                         .arg(data[0], 2, 16, QChar('0'))
                                         .arg(data[1], 2, 16, QChar('0'))
                                         .arg(data[2], 2, 16, QChar('0'))
                                         .arg(data[3], 2, 16, QChar('0'))
                                         .arg(data[4], 2, 16, QChar('0'))
                                         .arg(data[5], 2, 16, QChar('0')).toUpper();

            // Verificar si es IPv4
            if (data[12] == 0x08 && data[13] == 0x00) {
                const u_char *ipHeader = data + 14;
                quint8 protocolType = ipHeader[9];
                QString ttl = QString::number(ipHeader[8]);

                // Direcciones IP
                source = QString("%1.%2.%3.%4").arg(ipHeader[12]).arg(ipHeader[13]).arg(ipHeader[14]).arg(ipHeader[15]);
                destination = QString("%1.%2.%3.%4").arg(ipHeader[16]).arg(ipHeader[17]).arg(ipHeader[18]).arg(ipHeader[19]);

                // Determinar protocolo
                if (protocolType == 1) { // ICMP
                    protocol = "ICMP";
                    const u_char *icmpHeader = ipHeader + 20;

                    // Tipo y código ICMP
                    quint8 type = icmpHeader[0];
                    quint8 code = icmpHeader[1];

                    info = QString("ICMP - Type: %1, Code: %2").arg(type).arg(code);

                    emit packetCaptured(time, source, destination, protocol, length, info, rawData,
                                        macSource, macDestination, ttl, "", "", "");
                } else if (protocolType == 6) { // TCP
                    protocol = "TCP";
                    const u_char *tcpHeader = ipHeader + 20;
                    quint16 portSrc = (tcpHeader[0] << 8) | tcpHeader[1];
                    quint16 portDst = (tcpHeader[2] << 8) | tcpHeader[3];
                    QString flags = "";

                    if (tcpHeader[13] & 0x02) flags += "SYN ";
                    if (tcpHeader[13] & 0x10) flags += "ACK ";
                    if (tcpHeader[13] & 0x01) flags += "FIN ";
                    if (tcpHeader[13] & 0x04) flags += "RST ";

                    emit packetCaptured(time, source, destination, protocol, length, info, rawData,
                                        macSource, macDestination, ttl, QString::number(portSrc),
                                        QString::number(portDst), flags.trimmed());
                } else if (protocolType == 17) { // UDP
                    protocol = "UDP";
                    const u_char *udpHeader = ipHeader + 20;
                    quint16 portSrc = (udpHeader[0] << 8) | udpHeader[1];
                    quint16 portDst = (udpHeader[2] << 8) | udpHeader[3];

                    emit packetCaptured(time, source, destination, protocol, length, info, rawData,
                                        macSource, macDestination, ttl, QString::number(portSrc),
                                        QString::number(portDst), "");
                }
            }
        }
    }
    pcap_close(handle);
}



void hiloCaptura::stop() {
    running = false;
}

void hiloCaptura::restart() {
    if (!isRunning()) { // Si el hilo no está corriendo
        running = true;
        start();
    }
}
