#ifndef TOWCAMSOCKETTHREAD_H
#define TOWCAMSOCKETTHREAD_H

#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>
#include "HCLog.h"



typedef struct {
   QUdpSocket				*depthSocket;
   unsigned short			depthInSocketNumber;

   QUdpSocket				*altitudeSocket;
   unsigned short			altitudeInSocketNumber;

   QUdpSocket				*switchSocket;
   unsigned short			switchInSocketNumber;
   QHostAddress             switchHostAddress;
   unsigned short			switchOutSocketNumber;
} networkStructureT;

class TowcamSocketThread : public QObject
{
   Q_OBJECT
public:
   explicit TowcamSocketThread(networkStructureT theNetworkStructure);
    void        setLatitude(double inLatitude);

private:
   networkStructureT myNetworkStructure;
   double					altimeterTemperature;
   double					altimeterRange;
   QDateTime            lastAltimeter;

   QDateTime            lastDepth;
   double                  depthMeasurement;
   double               pressureMeasurement;

   HabcamLog				*theLog;
   int                        loggingEnabled;
   int						secondsSinceSwitchUpdate;
   QTimer                  *oneHzTimer;
   int                  parseValeport(char *dataString);
   double               latitude;

signals:
   void  newAltitude(QString theAltitude);
   void  newDepth(QString theDepth);
   void  newSwitches(int sw1, int sw2, int sw3, int sw4);
   void  noSwitchContact();
   void  updateAltPlot(double);
   void  oneHzData(QString, QString, QString);
   void                finished();


public slots:

   void	sledSocketDataPending();
   void  sendSwitchCommand(QString);
   void  startup();

    void    oneHzTimeout();

};

#endif // TOWCAMSOCKETTHREAD_H
