#include <projector_tracker/ProjectorTracker.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace cv;

#define diffMinBetweenFrames 3
char imgFile[200];
int captureCount = 0;
ProjectorTracker::ProjectorTracker ()
{

}

bool ProjectorTracker::loadSetting(std::string matrix_file , std::string tag_patternType, std::string tag_knownObj, std::string tag_W, std::string tag_H, std::string tag_squareSize, std::string tag_x, std::string tag_y,
                                   std::string tag_CheckerPatternW, std::string tag_CheckerPatternH, std::string tag_CheckerSquare,
                                   std::string tag_in_reproj_err, std::string tag_ex_reproj_err, std::string tag_num_before_clean, std::string tag_num_before_calib,
                                   std::string cam_intrinsic, std::string tag_cam_k, std::string tag_cam_d,  std::string tag_cam_w, std::string tag_cam_h, 
                                   std::string pro_intrinsic, std::string tag_pro_k, std::string tag_pro_d ,std::string tag_pro_w, std::string tag_pro_h
                                  )
{
    cv::FileStorage fs( matrix_file, cv::FileStorage::READ );
    if( !fs.isOpened() )
    {
      std::cout << "Failed to open Setting File." << std::endl;
      return false;
    }
    // Loading calibration parameters
    int iKnownObj =0;
    // Loading calibration parameters
    int type = -1;
    fs[tag_patternType] >> type;
    switch(type ) {
        /*case 0:
                patternType = CHESSBOARD;
                break;
        case 1:
                patternType = CIRCLES_GRID;
                break;*/
        case 2:
                patternType = ASYMMETRIC_CIRCLES_GRID;
                break;
        case 3:
                patternType = ARUCO;
                break;
        case 4:
                patternType = GRAYCODE;
                break;
    }

    std::cout << "patternType : "<< patternType << std::endl;

    fs[tag_knownObj] >> iKnownObj;
    std::cout << "knownObj : "<< iKnownObj << std::endl;
    known3DObj = iKnownObj != 0;

    int patternW, patternH;
    fs[tag_W] >> patternW;
    std::cout << "pattern width : "<< patternW << std::endl;
    fs[tag_H] >> patternH;
    std::cout << "pattern height : "<< patternH << std::endl;
    patternSize = cv::Size(patternW,patternH);
    
    fs[tag_squareSize] >> squareSize;
    std::cout << "squareSize : "<< squareSize << std::endl;
    
    float corner_x=0, corner_y=0;
    fs[tag_x] >> corner_x;
    std::cout << "corner_x : "<< corner_x << std::endl;
    fs[tag_y] >> corner_y;
    std::cout << "corner_y : "<< corner_y << std::endl;
    patternPosition = Point2f(corner_x,corner_y);
        
        
    int checkerPatternW, checkerPatternH;
    fs[tag_CheckerPatternW] >> checkerPatternW;
    std::cout << "checker patternW : "<< checkerPatternW << std::endl;
    fs[tag_CheckerPatternH] >> checkerPatternH;
    std::cout << "checker patternH : "<< checkerPatternH << std::endl;
    checkerBoardSize = cv::Size(checkerPatternW, checkerPatternH);
    
    fs[tag_CheckerSquare] >> checkerSquareSize;
    std::cout << "checker squareSize : "<< checkerSquareSize << std::endl;
    
    fs[tag_in_reproj_err] >> maxIntrinsicReprojectionError;
    std::cout << "max intrinsic reprojection error : "<< maxIntrinsicReprojectionError << std::endl;
    
    fs[tag_ex_reproj_err] >> maxExtrinsicReprojectionError;
    std::cout << "max extrinsic reprojection error : "<< maxExtrinsicReprojectionError << std::endl;
    
    fs[tag_num_before_clean] >> numBoardsBeforeCleaning;
    std::cout << "numBoardsBeforeCleaning : "<< numBoardsBeforeCleaning << std::endl;
    fs[tag_num_before_calib] >> numBoardsFinalCamera;
    std::cout << "numBoardsFinalCamera : "<< numBoardsFinalCamera << std::endl;
        
    loadIntrinsic(cam_intrinsic, tag_cam_k, tag_cam_d, tag_cam_w, tag_cam_h, false);
    loadIntrinsic(pro_intrinsic, tag_pro_k, tag_pro_d, tag_pro_w, tag_pro_h, true);
    return true;
}
void ProjectorTracker::loadIntrinsic(std::string camcalib, std::string tag_K, std::string tag_D, std::string tag_w, std::string tag_h, bool isProjector){
    cv::FileStorage fs( camcalib, cv::FileStorage::READ );
    if( !fs.isOpened() )
    {
      std::cout << "Failed to open Camera Calibration Data File." << std::endl;
      return ;
    }
    // Loading calibration parameters
    if(isProjector)
    {
        cout << "loading camera intrinsic" << endl;
        fs[tag_K] >> cameraMatrix;
        cout << cameraMatrix.size() << endl;
        fs[tag_D] >> cameraDistCoeffs;
        int w, h;
        fs[tag_w] >> w;
        fs[tag_h] >> h;
        ImagerSize = cv::Size(w,h);
    }
    else
    {
        cout << "loading projector intrinsic" << endl;
        fs[tag_K] >> projectorMatrix;
        cout << projectorMatrix.size() << endl; 
        fs[tag_D] >> projectorDistCoeffs;
        int w, h;
        fs[tag_w] >> w;
        fs[tag_h] >> h;
        addedImageSize = cv::Size(w,h);
    }
}
// Generates the images needed for shadowMasks computation
void getImagesForShadowMasks (int width, int height, Mat &blackImage, Mat &whiteImage) {
    blackImage = Mat (height, width, CV_8UC3, Scalar (0, 0, 0));
    whiteImage = Mat (height, width, CV_8UC3, Scalar (255, 255, 255));
}

// Computes the required number of pattern images
void computeNumberOfImages (size_t width, size_t height, size_t &numOfColImgs, size_t &numOfRowImgs, size_t &numOfPatternImages) {
    numOfColImgs = (size_t) ceil (log (double (width)) / log (2.0));
    numOfRowImgs = (size_t) ceil (log (double (height)) / log (2.0));
    numOfPatternImages = 2 * numOfColImgs + 2 * numOfRowImgs;
}

int getPatternImageNum (int width, int height) {
    size_t numOfPatterns = -1;
    size_t nCols, nRows;
    computeNumberOfImages (width, height, nRows, nCols, numOfPatterns);
    return (int) numOfPatterns;
}
vector<Point2f> ProjectorTracker::getPatternPoints(){
    std::vector<cv::Point2f> ret;
    Point2f p;
    if(patternType == ASYMMETRIC_CIRCLES_GRID ){
        for(int i = 0; i < patternSize.height; i++) {
            for(int j = 0; j < patternSize.width; j++) {
                    p.x = patternPosition.x + float(((2 * j) + (i % 2)) * squareSize);
                    p.y = patternPosition.y + float(i * squareSize);
                    ret.push_back(p);
            }
        }
    }
    else if(patternType == ARUCO || patternType == CIRCLES_GRID || patternType == CHESSBOARD ){
    	for(int i = 0; i < patternSize.height; i++) {
            for(int j = 0; j < patternSize.width; j++) {
                    p.x = patternPosition.x + float(j * squareSize);
                    p.y = patternPosition.y + float(i * squareSize);
                    ret.push_back(p);
            }
        }
    }
    return ret;
    
}
vector<Mat> ProjectorTracker::getPatternImages () {
    int width = ImagerSize.width;
    int height = ImagerSize.height;
    if(patternType == ARUCO){//USE ARUCO MARKERS
        cout << "generating pattern for projector width and height : " << width << "," << height << endl;
        dictionary= cv::aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
        //board = cv::aruco::CharucoBoard::create(patternSize.width, patternSize.height, 0.04f, 0.02f, dictionary);
        //cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
        //cv::Ptr<cv::aruco::CharucoBoard> board = cv::aruco::CharucoBoard::create(arucoW, arucoH, 0.04f, 0.02f, dictionary);
        cv::Mat boardImage = cv::Mat::zeros(ImagerSize, CV_8UC1);
        boardImage = cv::Scalar(255);
        vector<Point2f> pattern_pts = getPatternPoints();
        Mat markerImage;
        int markerSize = squareSize -10;
        for(int p = 0; p < pattern_pts.size() ; p++){
            Point2f pt = pattern_pts[p];
            if(pt.x < width - markerSize/2 && pt.y < height - markerSize/2 && pt.x >  markerSize/2 && pt.y > markerSize/2) {
                cv::aruco::drawMarker(dictionary, p , markerSize, markerImage, 1); //marker id = p
                cv::Rect roi( pt, cv::Size( markerSize , markerSize));
                cv::Mat destinationROI = boardImage( roi );
                int x_corner = pt.x - markerSize/2;
                int y_corner = pt.y - markerSize/2;
                markerImage.copyTo(boardImage.rowRange(y_corner, y_corner + markerSize).colRange(x_corner, x_corner + markerSize));
            }
        }
        //board.draw( cv::Size(width, height), boardImage, 10, 1 );
        vector<Mat> ret;
        ret.push_back(boardImage);
        return ret;
    }
    else if(patternType == ASYMMETRIC_CIRCLES_GRID ){
        vector<Point2f> pattern_pts = getPatternPoints();
        cout << "generating pattern for projector width and height : " << width << "," << height << endl;	
        Mat boardImage = Mat::zeros(ImagerSize,CV_8UC3);
	for(const auto & p : pattern_pts) {
		circle(boardImage, p , 20, Scalar( 255, 0, 0), CV_FILLED); //blue circles
	}
	vector<Mat> ret;
	ret.push_back(boardImage);
        return ret;
    }
    else if(patternType == GRAYCODE)
    {//USE GRAY CODE
        size_t numOfPatternImages ;
        size_t numOfRowImgs;
        size_t numOfColImgs;
        computeNumberOfImages (width, height, numOfColImgs, numOfRowImgs, numOfPatternImages);

        vector<Mat> ret;
        ret.resize (numOfPatternImages);

        for (size_t i = 0; i < numOfPatternImages; i++) {
            ret[i] = Mat (height, width, CV_8U);
        }

        uchar flag = 0;

        for (int j = 0; j < width; j++) { // rows loop
            int rem = 0, num = j, prevRem = j % 2;

            for (size_t k = 0; k < numOfColImgs; k++) { // images loop
                num = num / 2;
                rem = num % 2;

                if ( (rem == 0 && prevRem == 1) || (rem == 1 && prevRem == 0)) {
                    flag = 1;
                } else {
                    flag = 0;
                }

                for (int i = 0; i < height; i++) { // rows loop

                    uchar pixel_color = (uchar) flag * 255;

                    ret[2 * numOfColImgs - 2 * k - 2].at<uchar> (i, j) = pixel_color;

                    if (pixel_color > 0)
                        pixel_color = (uchar) 0;
                    else
                        pixel_color = (uchar) 255;

                    ret[2 * numOfColImgs - 2 * k - 1].at<uchar> (i, j) = pixel_color;   // inverse
                }

                prevRem = rem;
            }
        }

        for (int i = 0; i < height; i++) { // rows loop
            int rem = 0, num = i, prevRem = i % 2;

            for (size_t k = 0; k < numOfRowImgs; k++) {
                num = num / 2;
                rem = num % 2;

                if ( (rem == 0 && prevRem == 1) || (rem == 1 && prevRem == 0)) {
                    flag = 1;
                } else {
                    flag = 0;
                }

                for (int j = 0; j < width; j++) {
                    uchar pixel_color = (uchar) flag * 255;
                    ret[2 * numOfRowImgs - 2 * k + 2 * numOfColImgs - 2].at<uchar> (i, j) = pixel_color;

                    if (pixel_color > 0)
                        pixel_color = (uchar) 0;
                    else
                        pixel_color = (uchar) 255;

                    ret[2 * numOfRowImgs - 2 * k + 2 * numOfColImgs - 1].at<uchar> (i, j) = pixel_color;
                }

                prevRem = rem;
            }
        }

        // Generate the all-white and all-black images needed for shadows mask computation
        Mat white;
        Mat black;
        getImagesForShadowMasks (width, height, black, white);
        ret.push_back (black);
        ret.push_back (white);
        return ret;
    }
}

// Computes the shadows occlusion where we cannot reconstruct the model
void computeShadowMask (const Mat blackImage, const Mat whiteImage, double blackThreshold, Mat &shadowMask) {
    int cam_width = whiteImage.cols;
    int cam_height = blackImage.rows;
    shadowMask = Mat (cam_height, cam_width, CV_8U);
    for (int i = 0; i < cam_height; i++) {
        for (int j = 0; j < cam_width; j++) {
            double white = whiteImage.at<uchar> (i, j) ;
            double black = blackImage.at<uchar> (i, j) ;
            double diff = abs (white - black) ;

            //cout << diff << ", ";
            if (diff > blackThreshold) {
                shadowMask.at<uchar> (i, j)  = (uchar) 1;
            } else {
                shadowMask.at<uchar> (i, j) = (uchar) 0;
            }
        }
    }
}

// Converts a gray code sequence (~ binary number) to a decimal number
int grayToDec (const vector<uchar> &gray) {
    int dec = 0;

    uchar tmp = gray[0];

    if (tmp)
        dec += (int) pow ( (float) 2, int (gray.size() - 1));

    for (int i = 1; i < (int) gray.size(); i++) {
        // XOR operation
        tmp = tmp ^ gray[i];

        if (tmp)
            dec += (int) pow ( (float) 2, int (gray.size() - i - 1));
    }

    return dec;
}

// For a (x,y) pixel of the camera returns the corresponding projector's pixel
bool getProjPixel (const vector<Mat> &patternImages, size_t width, size_t height, int x, int y, Point &projPix) {
    vector<uchar> grayCol;
    vector<uchar> grayRow;
    bool error = false;
    int xDec, yDec;
    size_t numOfColImgs, numOfRowImgs, numOfPatternImages;
    computeNumberOfImages (width, height, numOfColImgs, numOfRowImgs, numOfPatternImages);
    // process column images
    for (size_t count = 0; count < numOfColImgs; count++) {
        // get pixel intensity for regular pattern projection and its inverse
        double val1 = patternImages[count * 2].at<uchar> (Point (x, y));
        double val2 = patternImages[count * 2 + 1].at<uchar> (Point (x, y));
        double diff = abs (val1 - val2) ;

        // check if the intensity difference between the values of the normal and its inverse projection image is in a valid range
        if (diff < DEFAULT_WHITE_THRESHOLD)
            error = true;

        // determine if projection pixel is on or off
        if (val1 > val2)
            grayCol.push_back (1);
        else
            grayCol.push_back (0);
    }

    xDec = grayToDec (grayCol);

    // process row images
    for (size_t count = 0; count < numOfRowImgs; count++) {
        // get pixel intensity for regular pattern projection and its inverse
        double val1 = patternImages[count * 2 + numOfColImgs * 2].at<uchar> (Point (x, y));
        double val2 = patternImages[count * 2 + numOfColImgs * 2 + 1].at<uchar> (Point (x, y));

        // check if the intensity difference between the values of the normal and its inverse projection image is in a valid range
        if (abs (val1 - val2) < DEFAULT_WHITE_THRESHOLD)
            error = true;

        // determine if projection pixel is on or off
        if (val1 > val2)
            grayRow.push_back (1);
        else
            grayRow.push_back (0);
    }

    yDec = grayToDec (grayRow);

    if ( (yDec >= height || xDec >= width)) {
        error = true;
    }
    projPix.x = xDec;
    projPix.y = yDec;
    return error;
}

bool ProjectorTracker::interpolate3D(const cv::Mat& img, const vector<cv::Point2f> checkercorners, const vector<Point2f> markers, vector<Point3f>& marker_objectPts){
    
}
bool ProjectorTracker::backProject(const cv::Mat& boardRot64,
                                    const cv::Mat& boardTrans64,
                                    const vector<cv::Point2f>& imgPt,
                                    vector<cv::Point3f>& worldPt) {
    //http://stackoverflow.com/questions/14514357/converting-a-2d-image-point-to-a-3d-world-point
    if( imgPt.size() == 0 ) {
        return false;
    }
    else
    {
        cv::Mat imgPt_h = cv::Mat::zeros(3, imgPt.size(), CV_32F);
        for( int h=0; h<imgPt.size(); ++h ) {
            imgPt_h.at<float>(0,h) = imgPt[h].x;
            imgPt_h.at<float>(1,h) = imgPt[h].y;
            imgPt_h.at<float>(2,h) = 1.0f;
        }
        Mat Kinv64 = cameraMatrix.inv();
        Mat Kinv,boardRot,boardTrans;
        Kinv64.convertTo(Kinv, CV_32F);
        boardRot64.convertTo(boardRot, CV_32F);
        boardTrans64.convertTo(boardTrans, CV_32F);
        // Transform all image points to world points in camera reference frame
        // and then into the plane reference frame
        Mat worldImgPt = Mat::zeros( 3, imgPt.size(), CV_32F );
        Mat rot3x3;
        Rodrigues(boardRot, rot3x3);
        Mat transPlaneToCam = rot3x3.inv()*boardTrans;
        for( int i=0; i<imgPt.size(); ++i ) {
            Mat col = imgPt_h.col(i);
            Mat worldPtcam = Kinv*col;
            //Mat d = rot3x3.inv()*worldPtcam;
            Mat worldPtPlane = rot3x3.inv()*(worldPtcam);
            
            float scale = transPlaneToCam.at<float>(2)/worldPtPlane.at<float>(2);
            Mat worldPtPlaneReproject = scale*worldPtPlane-transPlaneToCam;

            cv::Point3f pt, pt1;
            pt.x = worldPtPlaneReproject.at<float>(0);
            pt.y = worldPtPlaneReproject.at<float>(1);
            pt.z = 0;
            //cout << "pt " << pt.x << "," << pt.y << endl;
            worldPt.push_back(pt);
        }
        
    }
    return true;
}
void ProjectorTracker::drawCheckerBoard(const cv::Mat& img,std::vector<cv::Point2f> checkerCorners, cv::Mat& out){
    // Draw the corners.
    out = img.clone();
    drawChessboardCorners( out, checkerBoardSize, checkerCorners, true );
} 
void ProjectorTracker::drawAruco_board(const cv::Mat& image, std::vector<vector<cv::Point2f> >  markerCorners, std::vector<int> markerIds, std::vector<cv::Point2f> interpolated_corners,std::vector<int> interpolated_ids, cv::Mat& out){
    out = image.clone();
    aruco::drawDetectedMarkers(out, markerCorners, markerIds);
    //aruco::drawDetectedCornersCharuco(out, interpolated_corners, interpolated_ids, cv::Scalar(255, 0, 0));

    imwrite("detected_aruco.jpg", out);
    //show for debug
    cv::namedWindow( "aruco", cv:: WINDOW_NORMAL );// Create a window for display.
    resizeWindow( "aruco", 640, 480 );
    // Moving window of circlegrid to see the image at first screen
    moveWindow( "aruco", 0, 0 );
    imshow( "aruco", out );                   // Show our image inside it.
    waitKey(5);
}
void ProjectorTracker::drawAruco(const cv::Mat& image, std::vector<vector<cv::Point2f> >  markerCorners, std::vector<int> markerIds, cv::Mat& out){
    out = image.clone();
    //aruco::drawDetectedMarkers(out, markerCorners, markerIds);
    //drawing the centers of each marker
    for(const auto & p : markerCorners) {
        Point2f center = (p[0] + p[2]) /2;
        circle(out, center, 5, Scalar( 0, 0, 255), CV_FILLED); 
    }
    //imwrite("detected_aruco.jpg", out);
    //show for debug
    cv::namedWindow( "aruco", cv:: WINDOW_NORMAL );// Create a window for display.
    resizeWindow( "aruco", 640, 480 );
    // Moving window of circlegrid to see the image at first screen
    moveWindow( "aruco", 0, 0 );
    imshow( "aruco", out );                   // Show our image inside it.

}
bool ProjectorTracker::findAruco(const cv::Mat& image, vector<cv::Point2f>& markerPts, vector<vector<cv::Point2f> > & markerCorners, std::vector<int>& markerIds){
    //Ptr<aruco::Board> board = charucoboard.staticCast<aruco::Board>();
    
    cv::Mat display = image.clone();
    
    //detect Aruco corners on camera image
    vector< vector< Point2f > > rejectedMarkers;
    Vec3d rvec, tvec;
    cv::aruco::DetectorParameters  detectorParams;// = cv::aruco::DetectorParameters::create();
    detectorParams.doCornerRefinement = false;
    // detect markers
    aruco::detectMarkers(image, dictionary, markerCorners, markerIds, detectorParams,
                            rejectedMarkers);
    //cout << "markerIds.size() " << markerIds.size() << endl;
    // refind strategy to detect more markers
    //if(refindStrategy)
    aruco::refineDetectedMarkers(image, board, markerCorners, markerIds, rejectedMarkers,
                                        cameraMatrix, cameraDistCoeffs);

    markerPts.resize(markerIds.size());
    for(int i=0; i < markerIds.size(); i++){
        markerPts[i] = (markerCorners[i][0] + markerCorners[i][2] ) / 2;//take the center of the marker 
    }
    return markerIds.size() > 0;
}
bool ProjectorTracker::findAruco_board(const cv::Mat& image, std::vector<vector<cv::Point2f> > & markerCorners, std::vector<int>& markerIds,  std::vector<cv::Point2f> & interpolated_charucoCorners,  std::vector<int>& interpolated_charucoIds){
    //Ptr<aruco::Board> board = charucoboard.staticCast<aruco::Board>();
    cv::Mat display = image.clone();
    //detect Aruco corners on camera image
    //vector< int > markerIds;//, charucoIds;
    vector< vector< Point2f > > rejectedMarkers;
    //vector< Point2f > charucoCorners;
    Vec3d rvec, tvec;
    cv::aruco::DetectorParameters  detectorParams;// = cv::aruco::DetectorParameters::create();
    detectorParams.doCornerRefinement = false;
    // detect markers
    aruco::detectMarkers(image, dictionary, markerCorners, markerIds, detectorParams,
                            rejectedMarkers);

    // refind strategy to detect more markers
    //if(refindStrategy)
    aruco::refineDetectedMarkers(image, board, markerCorners, markerIds, rejectedMarkers,
                                        cameraMatrix, cameraDistCoeffs);

    // interpolate charuco corners
    int interpolatedCorners = 0;
    if(markerIds.size() > 0){
        //cout << "interpolating charuco corners .." << endl;
        interpolatedCorners =
            aruco::interpolateCornersCharuco(markerCorners, markerIds, image, board,
                                                 interpolated_charucoCorners, interpolated_charucoIds);//, cameraMatrix, cameraDistCoeffs
    }
    // if at least one marker detected
    if (interpolated_charucoIds.size() > 0) {
        return true;
    }
    return false;
}
/*bool ProjectorTracker::findBoard(const cv::Mat& img, vector<cv::Point2f>& pointBuf, bool refine) {
    bool found=false;
    // no CV_CALIB_CB_FAST_CHECK, because it breaks on dark images (e.g., dark IR images from kinect)
    int chessFlags = CV_CALIB_CB_ADAPTIVE_THRESH;// | CV_CALIB_CB_NORMALIZE_IMAGE;
    found = findChessboardCorners(img, checkerBoardSize , pointBuf, chessFlags);

    // improve corner accuracy
    if(found) {
        cv::Mat grayMat;
        if(img.type() != CV_8UC1) {
            cv::cvtColor(img, grayMat, CV_RGB2GRAY);
        } else {
            grayMat = img;
        }

        if(refine) {
            // the 11x11 dictates the smallest image space square size allowed
            // in other words, if your smallest square is 11x11 pixels, then set this to 11x11
            cornerSubPix(grayMat, pointBuf, cv::Size(1,1),  cv::Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));
        }
        // Draw the corners.
        Mat drawImg = img.clone();
        drawChessboardCorners( drawImg, checkerBoardSize, pointBuf, true );
	cv::imwrite("detected_checker.jpg", drawImg);
	cv::namedWindow( "checkerboard", cv:: WINDOW_NORMAL );// Create a window for display.
	resizeWindow( "checkerboard", 640, 480 );
        moveWindow( "checkerboard", 0, 0 );
	imshow( "checkerboard", drawImg );                   // Show our image inside it.
	waitKey(5);
    }

    return found;
}*/
void ProjectorTracker::computeCandidateBoardPose(const vector<cv::Point2f> & imgPts, const vector<cv::Point3f> & img3DPts,
                                                 const cv::Mat& K, const cv::Mat& distCoeffs, cv::Mat& boardRot, cv::Mat& boardTrans){
    cv::solvePnP(img3DPts, imgPts,
                 K,
                 distCoeffs,
                 boardRot, boardTrans);
   // cout << boardTrans.at<float> (0) << "," << boardTrans.at<float> (1) << "," << boardTrans.at<float> (2) << endl;
   // cout << boardRot.at<float> (0,1) << "," << boardRot.at<float> (0,1) << "," << boardRot.at<float> (0,2) << endl;
}
/*

void ProjectorTracker::findCorrespondence_Aruco(const std::vector<CameraProjectorImagePair>& cp_images ,std::vector<std::vector<cv::Point2f> >& camPixels_boards, std::vector<std::vector<cv::Point2f> >& projPixels_boards)
{
    //int ret = 0;
    for(int j=0; j < arucoNu; j++){
        Mat projected = cp_images[j].projected;
        Mat copyProjected;
        projected.copyTo(copyProjected);
        Mat acquired = cp_images[j].acquired;
        Mat copyAcquired;
        acquired.copyTo(copyAcquired);
        std::vector<cv::Point2f> camPixels;
        std::vector<cv::Point2f> projPixels;
        //define Aruco Board
        cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);//cv::Ptr<>
        cv::aruco::CharucoBoard  board = cv::aruco::CharucoBoard::create(5, 7, 0.04f, 0.02f, dictionary);
        cv::aruco::DetectorParameters  params;// = cv::aruco::DetectorParameters::create();
        params.doCornerRefinement = false;
        //detect Aruco corners on projector image
        std::vector<int> proj_ids;
        std::vector<std::vector<cv::Point2f> > proj_corners;
        cv::aruco::detectMarkers(projected, dictionary, proj_corners, proj_ids, params);
        std::vector<cv::Point2f> proj_charucoCorners;
        std::vector<int> proj_charucoIds;
        // if at least one marker detected
        if (proj_ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(copyProjected, proj_corners, proj_ids);
            cv::aruco::interpolateCornersCharuco(proj_corners, proj_ids, projected, board, proj_charucoCorners, proj_charucoIds);
            // if at least one charuco corner detected
            if(proj_charucoIds.size() > 0)
                cv::aruco::drawDetectedCornersCharuco(copyProjected, proj_charucoCorners, proj_charucoIds, cv::Scalar(255, 0, 0));
        }

        //cv::imshow("aruco projected", copyProjected);
        //cv::waitKey(1000);
        //cout << "saving arucoprojected.jpeg" << endl;
        //imwrite("arucoprojected.jpeg",copyProjected);

        //detect Aruco corners on camera image
        std::vector<int> cam_ids;
        std::vector<std::vector<cv::Point2f> > cam_corners;
        cv::aruco::detectMarkers(acquired, dictionary, cam_corners, cam_ids, params);
        std::vector<cv::Point2f> cam_charucoCorners;
        std::vector<int> cam_charucoIds;
        // if at least one marker detected
        if (cam_ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(copyAcquired, cam_corners, cam_ids);
            cv::aruco::interpolateCornersCharuco(cam_corners, cam_ids, acquired, board, cam_charucoCorners, cam_charucoIds);
            // if at least one charuco corner detected
            if(cam_charucoIds.size() > 0)
                cv::aruco::drawDetectedCornersCharuco(copyAcquired, cam_charucoCorners, cam_charucoIds, cv::Scalar(255, 0, 0));
        }

        //cv::imshow("aruco aquired", copyAcquired);
        //cv::waitKey(1000);
        //cout << "saving arucoacquired.jpeg" << endl;
        //imwrite("arucoacquired.jpeg",copyAcquired);
        //matching project pixels and cam pixels based on aruco id:
        for(int i = 0; i < cam_charucoIds.size(); i++){
            for(int j = 0; j < proj_charucoIds.size(); j++){
                if(cam_charucoIds[i] == proj_charucoIds[j]){
                    camPixels.push_back(cam_charucoCorners[i]);
                    projPixels.push_back(proj_charucoCorners[j]);
                }
            }
        }
        //ret = camPixels.size();
        //cout << "matching list of size " << ret << endl;
        camPixels_boards.push_back(camPixels);
        projPixels_boards.push_back(projPixels);
    }
}
int ProjectorTracker::findCorrespondence_GrayCode(const std::vector<CameraProjectorImagePair>& cp_images, std::vector<cv::Point2f>& camPixels, std::vector<cv::Point2f>& projPixels){
    //use graycode to find correspondence
    Mat blackImage, whiteImage;
    int seq_length = cp_images.size();
    blackImage = cp_images[seq_length - 2].acquired;
    whiteImage = cp_images[seq_length - 1].acquired;
    vector<Mat> camera_images;
    for(int i=0;i< seq_length; i++){
        camera_images.push_back(cp_images[i].acquired);
    }
    // Computing shadows mask
    Mat shadowMask;
    imwrite("black.jpeg",blackImage);
    imwrite("white.jpeg",whiteImage);
    computeShadowMask (blackImage, whiteImage, DEFAULT_BLACK_THRESHOLD, shadowMask);
    imwrite("shadow.jpeg",shadowMask);

    //dim identify:
    int cam_width = cp_interface->getCameraCalibration().width;
    int cam_height = cp_interface->getCameraCalibration().height;
    int proj_width = cp_interface->getProjectorCalibration().width;
    int proj_height = cp_interface->getProjectorCalibration().height;

    Point projPixel;
    for (int i = 0; i < cam_width; i++) {
        for (int j = 0; j < cam_height; j++) {
            //if the pixel is not shadowed, reconstruct
            if (shadowMask.at<uchar> (j, i)) {
                //for a (x,y) pixel of the camera returns the corresponding projector pixel by calculating the decimal number
                bool error = getProjPixel (camera_images, proj_width, proj_height, i, j, projPixel);
                if (error)
                {
                    continue;
                }
                else
                {
                    camPixels.push_back (Point (i, j));
                    projPixels.push_back (projPixel);
                    //visualize projector correspondence on camera image
                    circle (whiteImage, Point (i , j), 2, (255, 0, 0), 0);
                }
            }
        }
    }
    imwrite("correspondence.jpeg",whiteImage);

}
*/

int ProjectorTracker::size() const {
    return cam_imgPoints.size();
}
bool ProjectorTracker::addProjected(const cv::Mat& patternImg, const cv::Mat& projectedImg){
    if(patternType == ASYMMETRIC_CIRCLES_GRID)
        return addProjected_circlegrid(patternImg, projectedImg);
    else if(patternType == ARUCO)
        return addProjected_aruco(patternImg, projectedImg);
    return false;
}
void ProjectorTracker::processImageForCircleDetection(const Mat& img, Mat& processedImg){
    Mat hsv;
    cvtColor(img,hsv,CV_BGR2HSV);
    Scalar hsv_l(100,50,50);
    Scalar hsv_h(150,255,255);

    Mat bw;
    inRange(hsv,hsv_l,hsv_h,bw);
    processedImg =  cv::Scalar::all(255) - bw;
    imwrite("circle_detect.jpeg", processedImg);
}
void ProjectorTracker::drawCircleGrid(const Mat& img, vector<Point2f> circlePointBuf){
	// Draw the corners.
        Mat drawImg = img.clone();
	for(const auto & p : circlePointBuf) {
            circle(drawImg, p, 5, Scalar( 0, 0, 255), CV_FILLED); 
	}
	cv::imwrite("detected_circle.jpg", drawImg);
	cv::namedWindow( "circlegrid", cv:: WINDOW_NORMAL );// Create a window for display.
	resizeWindow( "circlegrid", 640, 480 );
	// Moving window of circlegrid to see the image at first screen
	moveWindow( "circlegrid", 0, 0 );
	imshow( "circlegrid", drawImg );                   // Show our image inside it.
	waitKey(5);
}
bool ProjectorTracker::addProjected_circlegrid(const cv::Mat& patternImg, const cv::Mat& projectedImg){
    if(addedImageSize != projectedImg.size()){
        std::cout << "Captured image size is not consistent with calibrated camera resolution" << std::endl;
        return false;
    }
    if(ImagerSize != patternImg.size()){
        std::cout << "Pattern image size is not consistent with calibrated projector resolution" << std::endl;
        return false;
    }

    // find corners
    vector<Point2f> chessImgPts;
    int chessFlags = CV_CALIB_CB_ADAPTIVE_THRESH;// | CV_CALIB_CB_NORMALIZE_IMAGE;
    bool foundCheckerBoard = findChessboardCorners(projectedImg, checkerBoardSize, chessImgPts, chessFlags);
    if(foundCheckerBoard){
        std::cout << "found checker board" << std::endl;
        Mat displayImg;
        drawCheckerBoard(projectedImg, chessImgPts, displayImg);
        vector<cv::Point2f> circlesImgPts;
        SimpleBlobDetector::Params params;
        params.maxArea = 10e4;
        params.minArea = 10;
        params.minDistBetweenBlobs = 5;
        Ptr<FeatureDetector> blobDetector = SimpleBlobDetector::create(params);
        Mat processedImg;
        processImageForCircleDetection(projectedImg, processedImg);
        bool bProjectedPatternFound = cv::findCirclesGrid(processedImg, patternSize, circlesImgPts, patternType, blobDetector);
        imwrite("preprocessed.jpg", processedImg);
        if(bProjectedPatternFound && circlesImgPts.size() >= 4){
            cout << "found circle grid" << endl;
            drawCircleGrid(displayImg, circlesImgPts);
            vector<cv::Point3f> circlesObjectPts;
            cv::Mat boardRot;
            cv::Mat boardTrans;
            vector<cv::Point3f> checkerObjectPts = createObjectPoints(checkerBoardSize, checkerSquareSize, CHESSBOARD);
            computeCandidateBoardPose(chessImgPts, checkerObjectPts ,cameraMatrix, cameraDistCoeffs,  boardRot, boardTrans);
            backProject(boardRot, boardTrans, circlesImgPts, circlesObjectPts);
            //camera points:
            cam_imgPoints.push_back(circlesImgPts);
            
            //projector points
            vector<Point2f> pro_circlesImgPts = getPatternPoints();
            pro_imgPoints.push_back(pro_circlesImgPts);
            objectPoints.push_back(circlesObjectPts);
            return true;
        }
    }
    else
        std::cout << "ProjectorTracker::addProjected_circlegrid() failed, maybe your patternSize is wrong or the image has poor lighting?" << std::endl;;
    return false;
}
bool ProjectorTracker::addProjected_aruco(const cv::Mat& patternImg, const cv::Mat& projectedImg){
    if(addedImageSize != projectedImg.size()){
        std::cout << "Captured image size is not consistent with calibrated camera resolution" << std::endl;
        return false;
    }
    if(ImagerSize != patternImg.size()){
        std::cout << "Pattern image size is not consistent with calibrated projector resolution" << std::endl;
        return false;
    }
    // find corners
    vector<Point2f> chessImgPts;
    int chessFlags = CV_CALIB_CB_ADAPTIVE_THRESH;
    bool foundCheckerBoard = findChessboardCorners(projectedImg, checkerBoardSize, chessImgPts, chessFlags);
    if(foundCheckerBoard){
       // std::cout << "found checker board" << std::endl;
        Mat displayImg;
        drawCheckerBoard(projectedImg, chessImgPts, displayImg);
        vector<vector<cv::Point2f> > aruco_corners;
        vector<cv::Point2f>  aruco_pts;
        vector<int> arucoIds;

        bool bProjectedPatternFound = findAruco(projectedImg, aruco_pts, aruco_corners, arucoIds) ;
        if(bProjectedPatternFound && aruco_pts.size() >= 4){
            //std::cout << "found aruco board with " << aruco_pts.size() << " corners " << std::endl;
            drawAruco(displayImg, aruco_corners, arucoIds, displayImg); 
            captureCount ++;
            sprintf(imgFile, "../data/cam_proj/cam_proj_%02d.jpg",  captureCount);
            cout << "saving captured image to " << imgFile << endl;
            imwrite(imgFile, projectedImg);
            sprintf(imgFile, "../data/cam_proj/projected_pattern%02d.jpg",  captureCount);
            imwrite(imgFile, patternImg);
            sprintf(imgFile, "../data/cam_proj/detected_pattern_%02d.jpg",  captureCount);
            imwrite(imgFile, displayImg);
            vector<cv::Point3f> arucoObjectPts;
            cv::Mat boardRot;
            cv::Mat boardTrans;
            vector<cv::Point3f> checkerObjectPts = createObjectPoints(checkerBoardSize, checkerSquareSize, CHESSBOARD);
            //draw 3D points on the checker board for debugging
            for(int i = 0; i < checkerObjectPts.size(); i++){
                Point3f p = checkerObjectPts[i];
                string coordinate = "(";
                coordinate.append(to_string((int) (p.x * 1000)));
                coordinate.append(",");
                coordinate.append(to_string((int) (p.y * 1000)));
                coordinate.append(")");
                putText(displayImg, coordinate, chessImgPts[i], 
    FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(200,200,250), 1, CV_AA);
            }

            computeCandidateBoardPose(chessImgPts, checkerObjectPts ,cameraMatrix, cameraDistCoeffs,  boardRot, boardTrans);
            backProject(boardRot, boardTrans, aruco_pts, arucoObjectPts);
            for(int i = 0; i < arucoObjectPts.size(); i+=3){
                Point3f p = arucoObjectPts[i];
                string coordinate = "(";
                coordinate.append(to_string((int) (p.x * 1000)));
                coordinate.append(",");
                coordinate.append(to_string((int) (p.y * 1000)));
                coordinate.append(")");
                putText(displayImg, coordinate, aruco_pts[i], 
    FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(255,0,0), 1, CV_AA);
            }
            imwrite("3d.jpg", displayImg);
            //camera points:
            cam_imgPoints.push_back(aruco_pts);
            vector<vector<cv::Point2f> > pro_aruco_corners;
            vector<cv::Point2f> pro_aruco_pts_all;
            vector<cv::Point2f> pro_aruco_pts;
            vector<int> pro_arucoIds;
   
            bool found = findAruco(patternImg, pro_aruco_pts_all, pro_aruco_corners, pro_arucoIds);
            
            pro_aruco_pts.resize(aruco_pts.size());//take only those that are detected in projected image
            for(int i = 0; i < aruco_pts.size(); i++){
                for(int j = 0; j < pro_aruco_pts_all.size(); j++){
                    if(pro_arucoIds[j] == arucoIds[i]) {
                        pro_aruco_pts[i] = pro_aruco_pts_all[j];
                        continue;
                    }
                }
            }
            
            //projector points
            pro_imgPoints.push_back(pro_aruco_pts);
            objectPoints.push_back(arucoObjectPts);
            //store camera pose
            cam_board_rot.push_back(boardRot);
            cam_board_trans.push_back(boardTrans);
            
            return true;
        }
    }
    else
        std::cout << "ProjectorTracker::addProjected_aruco() failed, maybe your patternSize is wrong or the image has poor lighting?" << std::endl;;
    return false;
}
vector<Point3f> ProjectorTracker::createObjectPoints(cv::Size _patternSize, float _squareSize, CalibrationPattern _patternType) {
    vector<Point3f> corners;
    switch(_patternType) {
        case CHESSBOARD:
        case CIRCLES_GRID:
            for(int i = 0; i < _patternSize.height; i++)
                for(int j = 0; j < _patternSize.width; j++)
                    corners.push_back(Point3f(float(j * _squareSize), float(i * _squareSize), 0));
            break;
        case ASYMMETRIC_CIRCLES_GRID:
            for(int i = 0; i < _patternSize.height; i++)
                for(int j = 0; j < _patternSize.width; j++)
                    corners.push_back(Point3f(float(((2 * j) + (i % 2)) * _squareSize), float(i * _squareSize), 0));
    }
    return corners;
}
void ProjectorTracker::updateReprojectionError() {
    vector<Point2f> imagePoints2;
    int totalPoints = 0;
    double totalErr = 0;

    perViewErrors.clear();
    perViewErrors.resize(objectPoints.size());
    
    for(int i = 0; i < (int)objectPoints.size(); i++) {
        projectPoints(Mat(objectPoints[i]), boardRotations[i], boardTranslations[i], projectorMatrix, projectorDistCoeffs, imagePoints2);
        double err = norm(Mat(pro_imgPoints[i]), Mat(imagePoints2), CV_L2);//imagePoints2

        int n = objectPoints[i].size();
        perViewErrors[i] = sqrt(err * err / n);
        totalErr += err * err;
        totalPoints += n;
        cout <<  "view " << i << " has error of " << perViewErrors[i] << endl;
    }

    reprojectionError = sqrt(totalErr / totalPoints);

    cout << "all views have error of " << reprojectionError << endl;
}
int ProjectorTracker::cleanStereo(){
    int removed = 0;
    for(int i = size() - 1; i >= 0; i--) {
        if(getReprojectionError(i) > maxIntrinsicReprojectionError) {
            objectPoints.erase(objectPoints.begin() + i);
            pro_imgPoints.erase(pro_imgPoints.begin() + i);
            cam_imgPoints.erase(cam_imgPoints.begin() + i);
            perViewErrors.erase(perViewErrors.begin() + i);
            removed++;
        }
    }
    cout << "ProjectorTracker::clean() removed the last object/image point pair" << endl;
    return removed;
}
float ProjectorTracker::getReprojectionError() const {
    return reprojectionError;
}
float ProjectorTracker::getReprojectionError(int i) const {
    return perViewErrors[i];
}
bool ProjectorTracker::calibrateProjector(){
    if(size() < 1) {
            cout << "ProjectorTracker::calibrateProjector() doesn't have any image data to calibrate from." << endl;
            return false;
    }
    cout << "pose #"<< size() << endl;
    int calibFlags = 0;
    float rms = calibrateCamera(objectPoints, pro_imgPoints, ImagerSize, projectorMatrix, projectorDistCoeffs, boardRotations, boardTranslations, calibFlags |CV_CALIB_FIX_PRINCIPAL_POINT | CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);

    bool ok = checkRange(projectorMatrix) && checkRange(projectorDistCoeffs);
    if(!ok) {
        cout <<  "ProjectorTracker::calibrateProjector() failed to calibrate the projector" << endl;
    }
    else 
    {
        updateReprojectionError();
        cout <<  "ProjectorTracker::calibrateProjector() succeeded to calibrate the projector" << endl;
    }
    return ok;
    
}
void ProjectorTracker::saveExtrinsic(string filename) const{
    if(!extrinsic_ready){
        cout << "ProjectorTracker::saveExtrinsic() failed, because your calibration isn't ready yet!" << endl;
    }
    else{
        cout << "saving camera to projector transformation to " << filename << endl;
    }
    FileStorage fs(filename, FileStorage::WRITE);

    fs << "projectorWidth" << ImagerSize.width;
    fs << "projectorHeight" << ImagerSize.height;

    fs << "cameraWidth" << addedImageSize.width;
    fs << "cameraHeight" << addedImageSize.height;
    
    fs << "rotation" << rotCamToProj;
    fs << "translation" << transCamToProj;
    fs.release();
}
bool ProjectorTracker::getCamToProjRotation(Mat& rot){
    rot = rotCamToProj;
    return extrinsic_ready;
}
bool ProjectorTracker::getCamToProjTranslation(Mat& t){
    t = transCamToProj;
    return extrinsic_ready;
}
void ProjectorTracker::saveProjectorIntrinsic(string filename) const {
    if(!intrinsic_ready){
        cout << "ProjectorTracker::saveProjectorIntrinsic() failed, because your calibration isn't ready yet!" << endl;
    }
    FileStorage fs(filename, FileStorage::WRITE);

    fs << "projectorMatrix" << projectorMatrix;
    fs << "projectorWidth" << ImagerSize.width;
    fs << "projectorHeight" << ImagerSize.height;

    fs << "distCoeffs" << projectorDistCoeffs;
    fs << "reprojectionError" << reprojectionError;
    fs.release();
}
bool ProjectorTracker::stereoCalibrate(){
    if(size() < 4) {
        cout << "ProjectorTracker::stereoCalibrate() doesn't have enough image data to calibrate from." << endl;
        return false;
    }
    cout << "pose #"<< size() -1 << endl;

    vector<vector<cv::Point2f> > auxImagePointsCamera;
    for (int i=0; i<objectPoints.size() ; i++ ) {
        vector<cv::Point2f> auxImagePoints;
        projectPoints(cv::Mat(objectPoints[i]),
                        cam_board_rot[i],
                        cam_board_trans[i],
                        cameraMatrix,
                        cameraDistCoeffs,
                        auxImagePoints);
        
        auxImagePointsCamera.push_back(auxImagePoints);
        //compare auxImagePoints with cam_imgPoints
        /*for(int j=0; j<auxImagePoints.size(); j++){
            cout << "auxImgPts : " << auxImagePoints[j].x << "," <<   auxImagePoints[j].y << endl;
            cout << "cam_imgPoints : " << cam_imgPoints[i][j].x << "," <<   cam_imgPoints[i][j].y << endl;
        }*/
    }
        
    //may use auxImagePointsCamera instead of cam_imgPoints
    double reprojErr = cv::stereoCalibrate(objectPoints,
                        cam_imgPoints,
                        pro_imgPoints,
                        cameraMatrix, cameraDistCoeffs,
                        projectorMatrix, projectorDistCoeffs,
                        addedImageSize,
                        rotCamToProj, transCamToProj,
                        essentialMatrix, fundamentalMatrix, CV_CALIB_USE_INTRINSIC_GUESS);// CV_CALIB_FIX_INTRINSIC 
    cout << "Stereo calib reprojection error : " << reprojErr << endl;
    extrinsic_ready = reprojErr < maxExtrinsicReprojectionError;
    saveExtrinsic("../data/cam_proj_trans.yml");
    return extrinsic_ready;
}
void ProjectorTracker::clear(){
    objectPoints.clear();
    pro_imgPoints.clear();
    cam_imgPoints.clear();
    perViewErrors.clear();
}
bool ProjectorTracker::known3DObj_calib(const Mat& patternImg, const Mat& captured)
{
    //Mat undistorted;
    //undistort(captured, undistorted, cameraMatrix, cameraDistCoeffs);
    bool addOK = addProjected(patternImg, captured );//undistorted
    if(addOK ) {
        if(intrinsic_ready ) 
            stereoCalibrate();
        else{
            calibrateProjector();
        }
    }
    //display for debug
    cv::namedWindow( "captured", cv:: WINDOW_NORMAL );// Create a window for display.
    resizeWindow( "captured", 640, 480 );
    // Moving window of captured to see the image at first screen
    moveWindow( "captured", 640, 0 );
    imshow( "captured", captured );                   // Show our image inside it.
    char key = waitKey(50);
    
    
    if(size() >= numBoardsBeforeCleaning) {
        cout << "Cleaning" << endl;
        cleanStereo();
        if(getReprojectionError(size()-1) > maxIntrinsicReprojectionError) {
                cout << "Board found, but reproj. error is too high, skipping" << endl;
        }
    }

    if (size()>= numBoardsFinalCamera) {
        cout << "enough to calibrate ..." << endl;
        if(getReprojectionError() < maxIntrinsicReprojectionError)
        {
            intrinsic_ready = true;
            cout << "saving projector intrinsic" << endl;
            saveProjectorIntrinsic("../data/calibrationProjector.yml");
        }
    }
    return extrinsic_ready;
}

bool ProjectorTracker::addProjected2D(const Mat& patternImg, const Mat& projectedImg){
    //find camera projector pixel correspondence using aruco
    vector<vector<cv::Point2f> > aruco_corners;
    vector<cv::Point2f>  aruco_pts;
    vector<int> arucoIds;

    bool bProjectedPatternFound = findAruco(projectedImg,aruco_pts, aruco_corners, arucoIds) ;
    if(bProjectedPatternFound && aruco_pts.size() >= 4){
        //std::cout << "found aruco board with " << aruco_pts.size() << " corners " << std::endl;
        Mat displayImg;
        drawAruco(projectedImg, aruco_corners, arucoIds, displayImg);
        
        //camera points:
        cam_imgPoints.push_back(aruco_pts);
        
         //detect 2D aruco on projector pattern image
        vector<vector<cv::Point2f> > pro_aruco_corners;
        vector<cv::Point2f> pro_aruco_pts ;
        vector<cv::Point2f> detected_aruco_pts ;
        vector<int> pro_arucoIds;
        bool found = findAruco(patternImg, pro_aruco_pts, pro_aruco_corners, pro_arucoIds);
        
        detected_aruco_pts.resize(aruco_pts.size());//take only those that are detected in projected image
        for(int i = 0; i < aruco_pts.size(); i++){
            for(int j = 0; j < pro_aruco_pts.size(); j++){
                if(pro_arucoIds[j] == arucoIds[i]) 
                    detected_aruco_pts[i] = pro_aruco_pts[j];
            }
        }
        
        //projector points
        pro_imgPoints.push_back(detected_aruco_pts);
        
        return true;
    }
    return false;
}
bool ProjectorTracker::run(const Mat& patternImg, const Mat& projectedImg){
    if(!updateCamDiff(projectedImg)){
        cout << "move the board around ..." << endl;
        return false;
    }

    if(known3DObj) {
        known3DObj_calib(patternImg, projectedImg);
    }
    else{
        unknown3DObj_calib(patternImg, projectedImg);
        
    }

}
bool ProjectorTracker::updateCamDiff(cv::Mat camMat) {
    if(prevMat.size() != Size(0,0)){
        Mat diffMat;
        absdiff(prevMat, camMat, diffMat);
        float diffMean = mean(Mat(mean(diffMat)))[0];
        camMat.copyTo(prevMat);
        //cout << "diff mean : " << diffMean << endl;
        return diffMinBetweenFrames < diffMean;
    }
    else 
        camMat.copyTo(prevMat);
}
bool ProjectorTracker::unknown3DObj_calib(const Mat& patternImg, const Mat& projectedImg){
    bool finished = false;
    bool addOK = addProjected2D(patternImg, projectedImg);
    if(addOK){
        std::vector<cv::Point2f> camPixels;
        std::vector<cv::Point2f> projPixels;
        //merge correspondence into same vectors
        for(int board= 0; board < cam_imgPoints.size(); board++ ){
            for(int i = 0; i< cam_imgPoints[board].size(); i ++){
                camPixels.push_back(cam_imgPoints[board][i]);
                projPixels.push_back(pro_imgPoints[board][i]);
            }
        }
        if(camPixels.size() >= 100){
            cout << camPixels.size() << endl;
            cout << "find fundamental matrix ..." << endl;
            //Mat F = findFundamentalMat (camPixels, projPixels, CV_FM_8POINT);
            Mat F = findFundamentalMat (camPixels, projPixels, CV_FM_RANSAC);
            Mat F_64;
            F.convertTo(F_64, CV_64F); 
            if(F_64.size() != Size(0,0))
            {
                cout << "find essential matrix ..." << endl;
                cout << projectorMatrix.type() << ", " << F_64.type() << ", " << cameraMatrix.type() << endl;
                cout << F.size() << endl;
                Mat E = projectorMatrix.t() * F_64 * cameraMatrix;
                cout << "perform SVD on Essential matrix" << endl;
                //Perform SVD on E
                SVD decomp = SVD (E);

                //U
                Mat U = decomp.u;

                //S
                Mat S (3, 3, CV_64F, Scalar (0));
                S.at<double> (0, 0) = decomp.w.at<double> (0, 0);
                S.at<double> (1, 1) = decomp.w.at<double> (0, 1);
                S.at<double> (2, 2) = decomp.w.at<double> (0, 2);

                //Vt
                Mat Vt = decomp.vt;

                //W
                Mat W (3, 3, CV_64F, Scalar (0));
                W.at<double> (0, 1) = -1;
                W.at<double> (1, 0) = 1;
                W.at<double> (2, 2) = 1;

                Mat Wt (3, 3, CV_64F, Scalar (0));
                Wt.at<double> (0, 1) = 1;
                Wt.at<double> (1, 0) = -1;
                Wt.at<double> (2, 2) = 1;

                Mat R1 = U * W * Vt;
                //Mat R2 = U * Wt * Vt;
                Mat u1 = U.col (2);
                //Mat u2 = -U.col (2);
                //4 candidates
                //cout << "computed rotation, translation: " << endl;
                //cout << R1 << "," << u1 << endl;
                //check if R1 u1 are in correct range
                extrinsic_ready = true;
                rotCamToProj = R1;
                transCamToProj = u1;
                cout << "saving cam to projector extrinsic ..." << endl;
                //save R1, u1 to "cam_proj_trans.yaml"
                saveExtrinsic("../data/cam_proj_trans_nochecker.yml");
                
                /*ret.at<double> (3, 3) = 1;
                for(int i = 0; i < 3; i++){
                    for(int j =0 ; j < 3; j++){
                        ret.at<double> (j,i) = R1.at<double> (j,i);
                    }
                }
                for(int i=0;i<3;i++)
                    ret.at<double> (i,0) = u1.at<double> (0,i);
                ret.at<double> (3,3) = 1;
                for(int i=0;i<3;i++)
                    ret.at<double> (3,i) = 0;*/
                
                return true;
            }
        }
        else
        {
            cerr << "correspodence lists size mismatched or not enough correspondences" << endl;
            return false;
        }
    }
}
