#ifndef TOWCAM_H
#define TOWCAM_H

#include <QtWidgets>
#include <QtNetwork>
#include <QDebug>

#include "qcustomplot.h"
#include "compilation.h"

#include "ini_file.h"
#include "HCLog.h"

#define	DEFAULT_FLYING_HISTORY	60
#define   MAX_FLYING_HISTORY      300
#define	MAX_N_OF_FLYING_POINTS	MAX_FLYING_HISTORY+1

#define  DEFAULT_IN_SOCKET    99999


#define PROGRAM_VERSION    "1.1"
#define DEFAULT_LATITUDE   30.0

typedef enum {
   BAILEY,
   ADAM_DIO,
   ADAM_AIO,
   UNKNOWN_SWITCH_TYPE
} eSwitchTypeT;

#define ERRONEOUS_SWITCH_CHANNEL    10

class SwitchWidget : public QWidget
{
	Q_OBJECT

private:

	QGroupBox				*switchGroupBox;
	QRadioButton			*switchOnRadioButton;
	QRadioButton			*switchOffRadioButton;
	QLabel					*switchStatusLabel;
	QString					*switchName;
	int						switchNumber;

	QPalette					offPalette;
	QPalette					onPalette;
	QPalette					errorPalette;
	QFont						onFont;
	QFont						offFont;

private slots:

   void    switchStatusChange();


public:
	SwitchWidget(int mySwitchNumber,QString mySwitchLabel);
	void	changeSwitchLabels(char status);
	void	setErrorPalette();

};

class TowCam : public QWidget
{
   Q_OBJECT
   
private:

	QUdpSocket				*depthSocket;
	unsigned short			depthInSocketNumber;

	QUdpSocket				*altitudeSocket;
	unsigned short			altitudeInSocketNumber;

    QUdpSocket				*flSocket;
    unsigned short			flInSocketNumber;


	QUdpSocket				*switchSocket;
	unsigned short			switchInSocketNumber;
	QHostAddress			switchHostAddress;
	unsigned short			switchOutSocketNumber;




   SwitchWidget            *switches[4];
   eSwitchTypeT            switchType[4];
   int                     switchChannel[4];

    double                  latitude;



   QLabel                  *timeLabel;
   QLabel                  *depthLabel;
   //QLabel                  *depthValueLabel;

   QLabel                  *altitudeLabel;
   QLabel                  *flRangeLabel;
   //QLabel                  *altitudeValueLabel;

	int		nOfFlyingPlotPoints;
	int      flyingHistory;

    int		nOfFLPlotPoints;
    int      flHistory;


    QCustomPlot		*flyingPlot;
    QCustomPlot		*flPlot;
    //BottomCurve	*bottomCurve;


	QLineEdit				*yAxisMinEdit;
	QLineEdit				*yAxisMaxEdit;
	QLabel					*minLabel;
	QLabel					*maxLabel;

    QLineEdit				*flyAxisMinEdit;
    QLineEdit				*flyAxisMaxEdit;
    QLabel					*flminLabel;
    QLabel					*flmaxLabel;

	double					yMinBound;
    double					yMaxBound;

    double					flyMinBound;
    double					flyMaxBound;

	double	depth[MAX_N_OF_FLYING_POINTS];
	double	altitude[MAX_N_OF_FLYING_POINTS];
	double	time[MAX_N_OF_FLYING_POINTS];
    double  xData[MAX_N_OF_FLYING_POINTS];
    double  range[MAX_N_OF_FLYING_POINTS];
    double	fltime[MAX_N_OF_FLYING_POINTS];
    double  flxData[MAX_N_OF_FLYING_POINTS];




   QTextEdit            *altimeterDataWindow;
   QTextEdit            *depthDataWindow;
   QTextEdit            *flDataWindow;

   QPushButton          *pumpPushButton;
   bool                 pumpIsOn;

   QFont						offFont;

private slots:
   void	processDepthBounds();
   void	processRangeBounds();

   void  gotANewDepth(QString);
   void  gotANewAltitude(QString);
   void  gotANewRange(QString);
   void  updateAltPlot(double);
   void  updateFLPlot(double);
   void newSwitches(int sw1,int sw2,int sw3,int sw4);
   void noSwitchContact();

   void    oneHzTimeout(QString, QString, QString,QString);


signals:
   void switchCommand(QString);



public:
   TowCam(IniFile  iniFile);
   void  switchAction(int swID, bool switchState);
   ~TowCam();
};

#endif // TOWCAM_H
