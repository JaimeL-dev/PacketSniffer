#ifndef HILOCAPTURA_H
#define HILOCAPTURA_H

#include <QThread>
#include <QString>
#include <pcap.h>

class hiloCaptura : public QThread {
    Q_OBJECT

public:
    hiloCaptura(const QString &deviceName, QObject *parent = nullptr);
    void run() override;
    void stop();
    void restart();

signals:
signals:
    void packetCaptured(const QString &time,
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



private:
    QString deviceName;
    volatile bool running;
};

#endif // HILOCAPTURA_H
