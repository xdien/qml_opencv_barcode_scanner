#ifndef QCVDETECTFILTER_H
#define QCVDETECTFILTER_H

#include <QAbstractVideoFilter>
#include <QDebug>
#include <QTemporaryFile>
#include <QMutex>
#include "opencv2/opencv.hpp"
#include <zbar.h>

using namespace cv;
using namespace std;
using namespace zbar;
typedef struct
{
  string type;
  string data;
  vector <Point> location;
} decodedObject;



class QCvDetectFilter : public QAbstractVideoFilter
{
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable();

signals:
    void objectDetected(float x, float y, float w, float h);
    void barcodeDetected(const QPoint &topLeft, const QPoint &bottomRight);

public slots:

};

class QCvDetectFilterRunnable : public QVideoFilterRunnable
{
public:
    QCvDetectFilterRunnable(QCvDetectFilter *creator) {filter = creator;}
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

private:
    void dft(cv::InputArray input, cv::OutputArray output);
    QCvDetectFilter *filter;
    ///
    /// \brief decode Find and decode barcodes and QR codes
    /// \param im
    /// \param decodedObjects
    ///
    void decode(Mat &im, vector<decodedObject>&decodedObjects);
    double tinhKhoangCach(cv::Point X, cv::Point Y);
};

#endif // QCVDETECTFILTER_H
