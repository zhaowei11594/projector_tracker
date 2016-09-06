
#ifndef PROJECTORTRACKER_H
#define PROJECTORTRACKER_H

#include <projector_tracker/CameraProjectorInterface.h>
#include <opencv2/core.hpp>

const unsigned int DEFAULT_BLACK_THRESHOLD = 40;  // 3D_underworld default value 40
const unsigned int DEFAULT_WHITE_THRESHOLD = 5;   // 3D_underworld default value  5
using namespace std;
using namespace cv;
enum CalibrationPattern {CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID};
/**
 * @brief Camera-Projector relative position tracker
 * 
 */
class ProjectorTracker {
public:
    using CameraProjectorImagePair = CameraProjectorInterface::CameraProjectorImagePair;
    
    /**
    * @brief Creates a ProjectorTracker 
    */
    ProjectorTracker();
    /**
    * @brief Load Setting
    *
    * @param std::string matrix_file, std::string tag,...
    */
    bool loadSetting(std::string matrix_file , std::string tag_aruco, std::string tag_known, std::string tag_arucoW, std::string arucoH, std::string tag_arucoNu, 
                     std::string tag_PatternW, std::string tag_PatternH, std::string tag_Square,
                     std::string cam_intrinsic, std::string tag_cam_k, std::string tag_cam_d,  std::string tag_cam_w, std::string tag_cam_h, 
                     std::string pro_intrinsic, std::string tag_pro_k, std::string tag_pro_d ,std::string tag_pro_w, std::string tag_pro_h
    );
    /**
    * @brief Creates a Gray code pattern/Aruco board to be projected by the projector
    * 
    * @return std::vector<cv::Mat>
    */
    std::vector<cv::Mat> getPatternImages();
    /**
    * @brief find the 2D points of calibration board on camera image
    * @param const Mat& img, bool refine
    * @return bool
    */
    bool findBoard(const cv::Mat& img, std::vector<cv::Point2f>& pointBuf, bool refine);
    /**
    * @brief detect charuco board on projector and camera image to establish correspondence
    * @param const cv::Mat& projected, const cv::Mat& acquired
    * @return int
    */
    //void findCorrespondence_Aruco(const std::vector<CameraProjectorImagePair>& cp_images, std::vector<std::vector<cv::Point2f> >& camPixels, std::vector<std::vector<cv::Point2f> >& projPixels);
    //int findCorrespondence_GrayCode(const std::vector<CameraProjectorImagePair>& cp_images, std::vector<cv::Point2f>& camPixels, std::vector<cv::Point2f>& projPixels);
    /**
    * @brief compute Board rotation and translation according to camera plane
    * @param const vector<cv::Point2f> & img3DPts,
    *        const vector<cv::Point2f> & imgPts,
             cv::Mat& K, cv::Mat distCoeffs
    * @return void
    */
    void computeCandidateBoardPose(const std::vector<cv::Point2f> & imgPts,const std::vector<cv::Point3f> & img3DPts, 
                                                     const cv::Mat& K, const cv::Mat& distCoeffs, cv::Mat& boardRot, cv::Mat& boardTrans);
    /**
    * @brief backproject the 2D points on camera image to 3D object points giving Extrinsic and Intrinsic Matrices
    *
    * @param const cv::Mat& K,cv::Mat& boardRot64,const cv::Mat& boardTrans64, const vector<cv::Point2f>& imgPt
    * @return bool
    */
    bool backProject(const cv::Mat& K, const cv::Mat& boardRot64,
                                        const cv::Mat& boardTrans64,
                                        const std::vector<cv::Point2f>& imgPt,
                                        std::vector<cv::Point3f>& worldPt);
    /**
    * @brief Given the current frame grabbed by the camera, compute the relative position of the projector.
    * 
    * @param camera_image Current frame as grabbed by the RGB camera.
    * @return cv::Mat
    */
    bool addProjected(const cv::Mat& patternImg, const cv::Mat& projectedImg) ;
    bool stereoCalibrate();
    static std::vector<Point3f> createObjectPoints(cv::Size patternSize, float squareSize, CalibrationPattern patternType);
    //cv::Mat computeRelativePosition(const std::vector<CameraProjectorImagePair>& camera_image);
    //void computeRt_knownObjectPoints(const std::vector<CameraProjectorImagePair>& cp_images, std::vector<std::vector<cv::Point2f> > camPxls, std::vector<std::vector<cv::Point2f> > projPxls, cv::Mat& transCamToProj );

    //bool computeRt_unknownObjectPoints( std::vector<std::vector<cv::Point2f> >& camPixels, std::vector<std::vector<cv::Point2f> >& projPixels, cv::Mat& transCamToProj);
    
    void drawAruco(const cv::Mat& img, std::vector<cv::Point2f> arucoPts, std::vector<int> arucoIds);
    bool findAruco(const cv::Mat& img, vector<cv::Point2f>& pointBuf);
    int size() const;
    bool clean();
    float getReprojectionError() const;
    float getReprojectionError(int i) const;
    void updateReprojectionError();
    float maxReprojectionError;
    
    float reprojectionError;
    std::vector<float> perViewErrors;
    
    int numBoardsFinalCamera;
    int numBoardsBeforeCleaning;
    
    std::vector<std::vector<cv::Point3f> > objectPoints;
    std::vector<std::vector<cv::Point2f> > cam_imgPoints;
    std::vector<std::vector<cv::Point2f> > pro_imgPoints;
    cv::Size addedImageSize;
    cv::Size ImagerSize;
    
    cv::Mat projectorMatrix     ;
    cv::Mat projectorDistCoeffs ;
    cv::Mat cameraMatrix        ;
    cv::Mat cameraDistCoeffs    ;
    
    //extrinsic cam to projector
    cv::Mat rotCamToProj;
    cv::Mat transCamToProj;
    cv::Mat fundamentalMatrix, essentialMatrix;
    
    void loadIntrinsic(string camcalib, string tag_k, string tag_d,  std::string tag_w, std::string tag_h, bool forProjector);
    
//protected:
//    std::shared_ptr<CameraProjectorInterface> cp_interface;  // Gives synchronized access to camera and projector

private:
    cv::Mat pattern;  /// cached pattern
    void saveExtrinsics(cv::Mat R, cv::Mat t, std::string filename) ;
    bool known3DObj;
    bool useAruco;
    int arucoNu;
    int arucoW;
    int arucoH;
    
    cv::Size checkerBoardSize;
    double checkerSquareSize;

};

#endif // PROJECTORTRACKER_H
