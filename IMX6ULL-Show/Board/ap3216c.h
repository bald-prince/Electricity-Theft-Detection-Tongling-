#ifndef AP3216C_H
#define AP3216C_H

#include <QObject>
#include <QTimer>

class Ap3216c : public QObject
{
    Q_OBJECT
public:
    explicit Ap3216c(QObject *parent = nullptr);
    ~Ap3216c();

    Q_INVOKABLE void setCapture(bool str);

    QString alsData();
    QString psData();
    QString irData();

private:
    QTimer *timer;
    QString alsdata;
    QString psdata;
    QString irdata;

    QString readAlsData();
    QString readPsData();
    QString readIrData();

    Q_PROPERTY(QString alsData READ alsData NOTIFY ap3216cDataChanged)
    Q_PROPERTY(QString psData READ psData NOTIFY ap3216cDataChanged)
    Q_PROPERTY(QString irData READ irData NOTIFY ap3216cDataChanged)

public slots:
    void timer_timeout();

signals:
    void ap3216cDataChanged();
};

#endif // AP3216C_H
