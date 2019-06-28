#include "qcvdetectfilter.h"

cv::CascadeClassifier classifier;

QVideoFilterRunnable* QCvDetectFilter::createFilterRunnable()
{
    return new QCvDetectFilterRunnable(this);
}

QVideoFrame QCvDetectFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags)
{
    Q_UNUSED(flags);
    //    qDebug() << flags;
    input->map(QAbstractVideoBuffer::ReadOnly);

    if(surfaceFormat.handleType() == QAbstractVideoBuffer::NoHandle)
    {
        int input_w = input->width();
        int input_h = input->height();
        cv::Mat yuyv(input_h, input_w,CV_8UC2, input->bits());
        cv::Mat mat (input_h, input_w,CV_8UC3);
        cvtColor(yuyv, mat, CV_YUV2BGR_YUYV);
        //        cvtColor(tmp, mat, input->pixelFormat() == QVideoFrame::Format_YUV420P ? CV_YUV2BGR_YV12 : CV_YUV2BGR_NV12);
        //                cv::flip(mat, mat, 0);//ham xoay hinh anh
        /*
             * Resize in not mandatory but it can speed up things quite a lot!
             */
        //        QSize resized = image.size().scaled(320, 240, Qt::KeepAspectRatio);
        //        cv::resize(mat, mat, cv::Size(resized.width(), resized.height()));
        ///--------------------------------------------------------------------------------------------------------------
        vector<decodedObject> decodedObjects;
        decode(mat, decodedObjects);
        // Loop over all decoded objects
        for(int i = 0; i < decodedObjects.size(); i++)
        {
            vector<Point> points = decodedObjects[i].location;
            vector<Point> hull;
            // If the points do not form a quad, find convex hull
            if(points.size() > 4)
                convexHull(points, hull);
            else
                hull = points;
            // Number of points in the convex hull
            int n = hull.size();
            if(n==4){
                emit filter->objectDetected(float(hull[0].x) / float(mat.cols),
                                        float(hull[0].y) / float(mat.rows),
                                        tinhKhoangCach(hull[0],hull[2]) / float(mat.cols),
                                        tinhKhoangCach(hull[0],hull[1])/ float(mat.rows));
            }

        }
        ///--------------------------------------------------------------------------------------------------------------
    }
    input->unmap();
    return *input;
}

void QCvDetectFilterRunnable::decode(Mat &im, vector<decodedObject> &decodedObjects)
{
    // Create zbar scanner
    ImageScanner scanner;

    // Configure scanner
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

    // Convert image to grayscale
    Mat imGray;
    cvtColor(im, imGray,COLOR_BGR2GRAY);

    // Wrap image data in a zbar image
    Image image(im.cols, im.rows, "Y800", (uchar *)imGray.data, im.cols * im.rows);

    // Scan the image for barcodes and QRCodes
    int n = scanner.scan(image);

    // Print results
    for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
    {
        decodedObject obj;

        obj.type = symbol->get_type_name();
        obj.data = symbol->get_data();

        // Print type and data
        cout << "Type : " << obj.type << endl;
        cout << "Data : " << obj.data << endl << endl;

        // Obtain location
        for(int i = 0; i< symbol->get_location_size(); i++)
        {
            obj.location.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));
        }
        decodedObjects.push_back(obj);
    }
}

double QCvDetectFilterRunnable::tinhKhoangCach(Point X, Point Y)
{
    double kc;
    kc = sqrt(pow((X.x - Y.x), 2) + pow((X.y - Y.y), 2));
    return kc;
}

