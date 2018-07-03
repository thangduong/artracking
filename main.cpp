#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

#define	SURF_MIN_HESSIAN					250
#define	PERCENT_GOOD_MATCH					20

int main(int argc, char** argv)
{
	string objFile = "reference.jpg";
	string sceneFile = "sequence\\seq_00000060.jpg";
	string replaceFile = "replacement.jpg";
	string outputFile = "seq_00000060_.jpg";;

	if (argc >= 4) {
		objFile = argv[1];
		sceneFile = argv[2];
		replaceFile = argv[3];
		if (argc > 4)
			outputFile = argv[4];
		else {
			outputFile = sceneFile.substr(0, sceneFile.find_last_of('.')) + "_.jpg";
		}
	}
	else {
		cout << "Usage: " << argv[0] << " <obj-file> <scene-file> <replace-file> <output-file>" << endl;
	}

	// load data
	Mat objImg = imread(objFile);
	Mat sceneImg = imread(sceneFile);
	Mat replacementImg = imread(replaceFile);

	if (objImg.data == nullptr || sceneImg.data == nullptr || replacementImg.data == nullptr) {
		cout << "Can't load all input files" << endl;
		return -1;
	}

	// compute keypoints and descriptors
	SurfFeatureDetector detector(SURF_MIN_HESSIAN);
	SurfDescriptorExtractor extractor;
	FlannBasedMatcher matcher;

	std::vector<KeyPoint> objKeyPts, sceneKeyPts;
	Mat objDescriptors, sceneDescriptors;

	detector.detect(objImg, objKeyPts);
	detector.detect(sceneImg, sceneKeyPts);
	extractor.compute(objImg, objKeyPts, objDescriptors);
	extractor.compute(sceneImg, sceneKeyPts, sceneDescriptors);

	std::vector<DMatch> allMatches;
	std::vector<Point2f> objMatchPts;
	std::vector<Point2f> sceneMatchPts;

	// match descriptors from object with that in the scene
	matcher.match(objDescriptors, sceneDescriptors, allMatches);

	// take only the best PERCENT_GOOD_MATCH percent
	sort(allMatches.begin(), allMatches.end(), [](DMatch a, DMatch b) { return a.distance < b.distance; });

	for (auto i = 0; i < allMatches.size()*PERCENT_GOOD_MATCH/100; i++)
	{
		objMatchPts.push_back(objKeyPts[allMatches[i].queryIdx].pt);
		sceneMatchPts.push_back(sceneKeyPts[allMatches[i].trainIdx].pt);
	}

	// TODO: 
	// 1 - check that we have enough # of good match points
	// 2 - check that that the distance in the good match points are sufficiently good

	// get homography and blit the replacement into the scene
	Mat H = findHomography(objMatchPts, sceneMatchPts, CV_RANSAC);

	warpPerspective(replacementImg, sceneImg, H, sceneImg.size(), INTER_LINEAR, BORDER_TRANSPARENT);

	imwrite(outputFile, sceneImg);

	// for testing
	// imshow("Output", sceneImg);	waitKey(0);
	return 0;
}
