#ifndef HABCAMLOG_H
#define HABCAMLOG_H

#include <QtGui>
#include <QtNetwork>

class HabcamLog: public QObject
{
	Q_OBJECT

public:
	HabcamLog();
	void				timeStampAndLogThisString(QString label, QString inData);
	void				logThisString(QString inData);
	void				closeLogFile();
   void           setLoggingEnabled(bool yesNo);
   void           setNetworkDistribution(bool yesNo);
   void           setNetworkParameters(QHostAddress address, unsigned short thePort, QUdpSocket *theSocket);

private slots:
	void				timerExpiration();
private:
	
	QString				logFileName;
   QFile             *logFile;
	QTextStream			loggingOut;
	QTimer				*fileTimer;
   bool              loggingEnabled;
   QHostAddress      destinationAddress;
   unsigned short    destinationSocket;
   QUdpSocket        *socketToWriteTo;
   bool              writeToNetwork;
   bool              networkParametersSet;
	

};
	

#endif
