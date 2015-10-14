


void CalMeanAndVariance(double* arry, int nArry, double& mean, double& var, double& minv, double& maxv)
{
	minv = FLT_MAX;
	maxv = -FLT_MAX;
	mean = 0;
	var = 0;
	int j;
	for( j = 0; j < nArry; j++ ) {
		mean += arry[j];
		var += arry[j]*arry[j];

		if (arry[j] < minv) {
			minv = arry[j];
		}
		if (arry[j] > maxv) {
			maxv = arry[j];
		}	 
	}
	mean /= nArry;
	var /= nArry;
	var = sqrt(var - (mean*mean));
}


double CalChiSquareDistance( double* arr1, double* arr2, int nArr )
{
	double val, dist;
	dist = 0;
	for ( int i = 0; i < nArr; i++ ) {
		val = arr1[i] + arr2[i];
		if ( val != 0 ) {
			double tmp = (arr1[i]-arr2[i])*(arr1[i]-arr2[i])/val;
			tmp = tmp/2.0;
			dist += tmp;
		}
	}
	
	return dist;
}


double CalDSCcost(CVoxel* v1, CVoxel* v2)
{
	int x, y, z, nX, nY, nZ, iValue1, iValue2 ;
	int nA1 = 0, nA2 = 0, nA1A2 = 0 ;
	double fDscA1A2 ;
	nX = v1->m_nX ;
	nY = v1->m_nY ;
	nZ = v1->m_nZ ;

	for( z = 0 ; z < nZ ; z++ ){
		for( y = 0 ; y < nY ; y++ ){
			for( x = 0 ; x < nX ; x++ ){
				iValue1 = v1->GetAt( x, y, z ) ;
				iValue2 = v2->GetAt( x, y, z ) ;
				if( iValue1 > 0 && iValue2 > 0 ){
					nA1++ ;
					nA2++ ;
					nA1A2++ ;
				}
				else if( iValue1 > 0 && iValue2 == 0 )	nA1++ ;
				else if( iValue1 == 0 && iValue2 > 0 )	nA2++ ;
				else{
				}
			}
		}
	}

	if ( nA1A2 == 0 ) {
		fDscA1A2 = 0;
	}

	else {
		fDscA1A2 = 2.0f * (float)nA1A2 / ( nA1 + nA2 ) ;
	}

	return fDscA1A2;
}

double CalSubRegionDSCcost(CVoxel* v1, CVoxel* v2, cv::Point3i stpt, cv::Point3i enpt)
{
	int x, y, z, i, nX, nY, nZ, iValue1, iValue2 ;
	int nA1 = 0, nA2 = 0, nA1A2 = 0 ;
	double fDscA1A2 ;
	nX = v1->m_nX ;
	nY = v1->m_nY ;
	nZ = v1->m_nZ ;

	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >=0 && x < v2->m_nX && y >=0 && y < v2->m_nY && z >=0 && z < v2->m_nZ ) {
					iValue1 = v1->m_pData[i];
					iValue2 = v2->GetAt( x, y, z ) ;
					if( iValue1 > 0 && iValue2 > 0 ){
						nA1++ ;
						nA2++ ;
						nA1A2++ ;
					}
					else if( iValue1 > 0 && iValue2 == 0 )	nA1++ ;
					else if( iValue1 == 0 && iValue2 > 0 )	nA2++ ;
					else{
					}
				}
			}
		}
	}

	if ( nA1A2 == 0 ) {
		fDscA1A2 = 0;
	}

	else {
		fDscA1A2 = 2.0f * (float)nA1A2 / ( nA1 + nA2 ) ;
	}

	return fDscA1A2;
}


double* CalMultilabelDSCcost(CVoxel* v1, CVoxel* v2, int nLabel)
{
	int x, y, z, nX, nY, nZ, iValue1, iValue2 ;
	nX = v1->m_nX ;
	nY = v1->m_nY ;
	nZ = v1->m_nZ ;

	double* dscArr = new double[nLabel];
	int i;
	for( i = 0 ; i < nLabel ; i++ ){
		int nA1 = 0, nA2 = 0, nA1A2 = 0 ;
		double fDscA1A2 ;
		for( z = 0 ; z < nZ ; z++ ){
			for( y = 0 ; y < nY ; y++ ){
				for( x = 0 ; x < nX ; x++ ){
					iValue1 = v1->GetAt( x, y, z ) ;
					iValue2 = v2->GetAt( x, y, z ) ;
					if( iValue1 == i && iValue2 == i ){
						nA1++ ;
						nA2++ ;
						nA1A2++ ;
					}
					else if( iValue1 == i && iValue2 != i )	nA1++ ;
					else if( iValue1 != i && iValue2 == i )	nA2++ ;
					else{
					}
				}
			}
		}

		if ( nA1A2 == 0 ) {
			fDscA1A2 = 0;
		}

		else {
			fDscA1A2 = 2.0f * (float)nA1A2 / ( nA1 + nA2 ) ;
		}

		dscArr[i] = fDscA1A2;
	}

	return dscArr;
}


double CalOverlaycost(CVoxel* v1, CVoxel* v2)
{
	int x, y, z, nX, nY, nZ, iValue1, iValue2 ;
	int nA1 = 0, nA2 = 0, nA1A2 = 0 ;
	double fDscA1A2 ;
	nX = v1->m_nX ;
	nY = v1->m_nY ;
	nZ = v1->m_nZ ;

	for( z = 0 ; z < nZ ; z++ ){
		for( y = 0 ; y < nY ; y++ ){
			for( x = 0 ; x < nX ; x++ ){
				iValue1 = v1->GetAt( x, y, z ) ;
				iValue2 = v2->GetAt( x, y, z ) ;
				if( iValue1 > 0 && iValue2 > 0 ){
					nA1A2++;
				}
				else if( iValue1 > 0 && iValue2 == 0 )	nA1++ ;
				else if( iValue1 == 0 && iValue2 > 0 )	nA2++ ;
				else{
				}
			}
		}
	}

	if ( nA1A2 == 0 ) {
		fDscA1A2 = 0;
	}

	else {
		fDscA1A2 = (float)nA1A2 / (nA1A2 + nA1 + nA2 ) ;
	}

	return fDscA1A2;
}

void LinearNormalizeOfMRImage( CVoxel *inputvoxel, CPoint range )
{
	int nX = inputvoxel->m_nX;
	int nY = inputvoxel->m_nY;
	int nZ = inputvoxel->m_nZ;
	int nPix = nX*nY*nZ;
	int ratio = (int)Round(nPix * 0.85);
	//int ratio = nPix - 300;
	int val,newval;
	
	
	int minval = 0;
	int maxval = 500;
	/*int minval = 1000000 ;
	int maxval = 0;

	// Find min value
	for (int z = 0; z<nZ ; z++){
		for (int y = 0; y < nY; y++) {
			for (int x = 0; x < nX; x++) {
				val = inputvoxel->GetAt(x,y,z);
				if (val < minval)		minval = val;
				if (val > maxval)		maxval = val;
			}
		}
	}
	
	// Find max value
	int search = maxval;
	int count = nPix;
	while( count > ratio ) {
		for (int z = 0; z< nZ ; z++){
			for (int y = 0; y < nY; y++) {
				for (int x = 0; x < nX; x++) {
					val = inputvoxel->GetAt(x,y,z);
					if( val == search ){
						count--;
					}
				}
			}
		}
		search--;
	}
	maxval = search;*/

	/*
	int search = minval;
	while( count < ratio ) {
		for (int z = 0; z< nZ ; z++){
			for (int y = 0; y < nY; y++) {
				for (int x = 0; x < nX; x++) {
					val = inputvoxel->GetAt(x,y,z);
					if( val == search ){
						count++;
					}
				}
			}
		}
		search++;
	}
	maxval = search;*/

	//linear normalization
	double minnewval = range.x;
	double maxnewval = range.y;
	for (int z = 0; z<nZ ; z++){
		for (int y = 0; y < nY; y++) {
			for (int x = 0; x < nX; x++) {
				val = inputvoxel->GetAt(x,y,z);
				if (val > maxval) {
					inputvoxel->SetAt(x,y,z,maxnewval);
				}
				else{
					newval = (int)Round(((double)(val - minval) / (maxval - minval)) * maxnewval);
					inputvoxel->SetAt(x,y,z,newval);
				}
			}
		}
	}
}

void LinearNormalizeOfMRImage( CVoxel *inputvoxel, CPoint range, int maxval )
{
	int nX = inputvoxel->m_nX;
	int nY = inputvoxel->m_nY;
	int nZ = inputvoxel->m_nZ;
	int nPix = nX*nY*nZ;
	//int ratio = (int)Round(nPix * 0.99);
	int ratio = nPix - 1000;
	int count=0;

	int val,newval;
	int minval = 0;

	//linear normalization
	double minnewval = range.x;
	double maxnewval = range.y;
	for (int z = 0; z<nZ ; z++){
		for (int y = 0; y < nY; y++) {
			for (int x = 0; x < nX; x++) {
				val = inputvoxel->GetAt(x,y,z);
				if (val > maxval) {
					inputvoxel->SetAt(x,y,z,maxnewval);
				}
				else{
					newval = (int)Round(((double)(val - minval) / (maxval - minval)) * maxnewval);
					inputvoxel->SetAt(x,y,z,newval);
				}
			}
		}
	}
}


void RearrangeFileArrayWithDoubleFileName(CStringArray &fileArr)
{
	// rearrange of file array( follow the order of number )
	int nArr = fileArr.GetSize();
	CString curFileDir;
	CStringArray newfileArr;
	newfileArr.Copy(fileArr);
	int nLength1 = 0;

	// The number of file length 1
	for ( int i = 0; i < nArr; i++ ) {	
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));

		if ( curFileDir.GetLength() == 1 )
			nLength1++;
	}

	// Set file length 1 path to real fileArr
	int q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		if ( curFileDir.GetLength() == 1 )
		{
			fileArr.SetAt(q,newfileArr[i]);
			q++;
		}
	}

	// Set file length 2 path to real fileArr
	q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		if ( curFileDir.GetLength() != 1  )
		{
			fileArr.SetAt( nLength1+q, newfileArr[i]);
			q++;
		}
	}
}

void RearrangeFileArrayWithFileName(CStringArray &fileArr)
{
	// rearrange of file array( follow the order of number )
	int nArr = fileArr.GetSize();
	CString curFileDir;
	CStringArray newfileArr;
	newfileArr.Copy(fileArr);
	int nLength1 = 0;

	// The number of file length 1
	for ( int i = 0; i < nArr; i++ ) {	
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));

		if ( curFileDir.GetLength() == 1 )
			nLength1++;
	}

	// Set file length 1 path to real fileArr
	int q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		if ( curFileDir.GetLength() == 1 )
		{
			fileArr.SetAt(q,newfileArr[i]);
			q++;
		}
	}

	// Set file length 2 path to real fileArr
	q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileName(newfileArr[i]);
		curFileDir = curFileDir.Left(curFileDir.ReverseFind('-'));
		if ( curFileDir.GetLength() != 1  )
		{
			fileArr.SetAt( nLength1+q, newfileArr[i]);
			q++;
		}
	}
}


void RearrangeFileArray(CStringArray &fileArr)
{
	// rearrange of file array( follow the order of number )
	int nArr = fileArr.GetSize();
	CString curFileDir;
	CStringArray newfileArr;
	newfileArr.Copy(fileArr);
	int nLength1 = 0;

	// The number of file length 1
	for ( int i = 0; i < nArr; i++ ) {	
		curFileDir = GetFileDirName(newfileArr[i]);
		if ( curFileDir.GetLength() == 1 )
			nLength1++;
	}

	// Set file length 1 path to real fileArr
	int q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileDirName(newfileArr[i]);
		if ( curFileDir.GetLength() == 1 )
		{
			fileArr.SetAt(q,newfileArr[i]);
			q++;
		}
	}

	// Set file length 2 path to real fileArr
	q=0;
	for ( int i = 0; i < nArr; i++ ) {
		curFileDir = GetFileDirName(newfileArr[i]);
		if ( curFileDir.GetLength() != 1  )
		{
			fileArr.SetAt( nLength1+q, newfileArr[i]);
			q++;
		}
	}
}

void SaveArrayColorProfile( CString str, float* colorProfile, int m_nDist )
{
	float VIEW_HIST_HEIGHT = 500;
	float VIEW_BIN_WIDTH = 5;

	IplImage *colorimg = cvCreateImage(cvSize(m_nDist*VIEW_BIN_WIDTH, VIEW_HIST_HEIGHT), IPL_DEPTH_8U, 3);
	cvSet(colorimg, cvScalar(255,255,255));

	int i;
	for ( i = 0; i < m_nDist; i++ ) {
		int colorH = (int)(VIEW_HIST_HEIGHT-colorProfile[i]);

		if ( colorH < VIEW_HIST_HEIGHT )
			cvRectangle( colorimg, cvPoint(VIEW_BIN_WIDTH*(i), colorH), cvPoint(VIEW_BIN_WIDTH*(i+1)-1, VIEW_HIST_HEIGHT), cvScalar(0,255,0), CV_FILLED);
	}

	cvSaveImage(CStringA(str), colorimg);
	cvReleaseImage(&colorimg);
}


void MakePairwiseIdx( cvPoint3iArray& nPtArr, int nPt )
{
	int nNpt = nPt;
	for ( int i = 0; i < nNpt; i++ ) 
		nPtArr.Add(cv::Point3i());

	if ( nNpt == 3 ) {
		nPtArr[0].Set(1, 0, 0);	
		nPtArr[1].Set(0, 1, 0);	
		nPtArr[2].Set(0, 0, 1);
	}

	if ( nNpt == 13 ) {
		nPtArr[0].Set(1, 0, -1);	nPtArr[1].Set(1, 1, -1);	nPtArr[2].Set(0, 1, -1);	nPtArr[3].Set(-1, 1, -1);
		nPtArr[4].Set(1, 0, 0);	nPtArr[5].Set(1, 1, 0);	nPtArr[6].Set(0, 1, 0);	nPtArr[7].Set(-1, 1, 0);
		nPtArr[8].Set(1, 0, 1);	nPtArr[9].Set(1, 1, 1);	nPtArr[10].Set(0, 1, 1);	nPtArr[11].Set(-1, 1, 1);
		nPtArr[12].Set(0, 0, 1);
	}
}

void SortMinDistances( double * data, int* dataidx, int p, int r )
{
	if ( p < r ) {
		int q = minPartition(data,dataidx, p, r);
		SortMinDistances(data, dataidx, p, q-1);
		SortMinDistances(data, dataidx, q+1, r);
	}
}

int minPartition( double * data,int* dataidx , int p, int r )
{
	double x = data[r];
	int i = p-1;
	for ( int j = p; j < r; j++ ) {
		if ( data[j] <= x )	{
			i++;
			double temp = data[i];
			data[i] = data[j];
			data[j] = temp;

			int temp2 = dataidx[i];
			dataidx[i] = dataidx[j];
			dataidx[j] = temp2;
		}
	}
	double temp = data[r];
	data[r] = data[i+1];
	data[i+1] = temp;

	double temp2 = dataidx[r];
	dataidx[r] = dataidx[i+1];
	dataidx[i+1] = (int)temp2;

	return i+1;
}

void SortMaxDistances( double * data, int* dataidx, int p, int r )
{
	if ( p < r ) {
		int q = maxPartition(data,dataidx, p, r);
		SortMaxDistances(data, dataidx, p, q-1);
		SortMaxDistances(data, dataidx, q+1, r);
	}
}

int maxPartition( double * data,int* dataidx , int p, int r )
{
	double x = data[r];
	int i = p-1;
	for ( int j = p; j < r; j++ ) {
		if ( data[j] >= x )	{
			i++;
			double temp = data[i];
			data[i] = data[j];
			data[j] = temp;

			int temp2 = dataidx[i];
			dataidx[i] = dataidx[j];
			dataidx[j] = temp2;
		}
	}
	double temp = data[r];
	data[r] = data[i+1];
	data[i+1] = temp;

	double temp2 = dataidx[r];
	dataidx[r] = dataidx[i+1];
	dataidx[i+1] = (int)temp2;

	return i+1;
}

float ComputeNCC(CVoxel* dVxl, float patMean, float patVar, CVoxel* img, float cMean, float cVar)
{
	// compute ncc
	int nX = img->m_nX;
	int nY = img->m_nY;
	int nZ = img->m_nZ;

	int x, y, z, i;
	float sum = 0;
	int count = 0;
	for ( z = 0, i = 0; z < nZ; z++ ) {
		for ( y = 0; y < nY; y++ ) {
			for ( x = 0; x < nX; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					sum += dVxl->m_pData[i]*(img->GetAt(x,y,z)-cMean);
					count++;
				}
				else {
					sum += dVxl->m_pData[i]*(-cMean);
					count++;
				}
			}
		}
	}
	return sum/(sqrt(patVar*cVar)*(float)count);
}

float ComputeNCC(CVoxel* dVxl, float patMean, float patVar, CVoxel* img, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar)
{
	// compute ncc
	int nX = img->m_nX;
	int nY = img->m_nY;
	int nZ = img->m_nZ;

	int x, y, z, i;
	float sum = 0;
	int count = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					sum += dVxl->m_pData[i]*(img->GetAt(x,y,z)-cMean);
					count++;
				}
				else {
					sum += dVxl->m_pData[i]*(-cMean);
					count++;
				}
			}
		}
	}
	return sum/(sqrt(patVar*cVar)*(float)count);
}

float ComputeWeightedSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar, CFVoxel* w)
{
	// compute ncc
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	float patMean = 0; 
	float patVar = 0;
	float wsum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					wsum += w->m_pfData[i];
					float val = img->m_pData[i]*w->m_pfData[i];
					patMean += val;
				}
			}
		}
	}
	patMean /= wsum;

	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					float val = w->m_pfData[i]*(img->m_pData[i]-patMean)*(img->m_pData[i]-patMean);
					patVar += val;
				}
			}
		}
	}

	patVar /= wsum;


	float sum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					float val = w->m_pfData[i]*(img->m_pData[i]-patMean)*(cimg->GetAt(x,y,z)-cMean);
					sum += val;
				}
			}
		}
	}

	return sum/(sqrt(patVar*cVar)*wsum);
}

float ComputeWeightedSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar, CVoxel* roi, CFVoxel* w)
{
	// compute ncc
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	float patMean = 0; 
	float patVar = 0;
	float wsum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if (roi->m_pData[i]) {
						wsum += w->m_pfData[i];
						float val = img->m_pData[i]*w->m_pfData[i];
						patMean += val;
					}
				}
			}
		}
	}
	patMean /= wsum;

	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if (roi->m_pData[i]) {
						float val = w->m_pfData[i]*(img->m_pData[i]-patMean)*(img->m_pData[i]-patMean);
						patVar += val;
					}
				}
			}
		}
	}

	patVar /= wsum;


	float sum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if (roi->m_pData[i]) {
						float val = w->m_pfData[i]*(img->m_pData[i]-patMean)*(cimg->GetAt(x,y,z)-cMean);
						sum += val;
					}
				}
			}
		}
	}

	return sum/(sqrt(patVar*cVar)*wsum);
}

float ComputeSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar)
{
	// compute ncc
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	float patMean = 0; 
	float patVar = 0;
	int count = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					//if ( cimg->GetAt(x,y,z) > 120 ) {
						patMean += img->m_pData[i];
						count++;
					//}
				}
			}
		}
	}
	patMean /= (float)count;

	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					//if ( cimg->GetAt(x,y,z) > 120 ) {
						patVar += (img->m_pData[i]-patMean)*(img->m_pData[i]-patMean);
					//}
				}
			}
		}
	}
	patVar /= (float)count;


	float sum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					//if ( cimg->GetAt(x,y,z) > 120 ) {
						sum += (img->m_pData[i]-patMean)*(cimg->GetAt(x,y,z)-cMean);
					//}
				}
			}
		}
	}

	return sum/(sqrt(patVar*cVar)*(float)count);
}

float ComputeSubRegionNCC(CVoxel* img, cv::Point3i istpt, CVoxel* cimg, cv::Point3i cstpt, cv::Point3i size, float cMean, float cVar)
{
	// compute ncc
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	float patMean = 0; 
	float patVar = 0;
	int count = 0;
	int cx,cy,cz,tx,ty,tz;
	for ( z = 0, i = 0; z < size.m_z; z++ ) {
		for ( y = 0; y < size.m_y; y++ ) {
			for ( x = 0; x < size.m_x; x++, i++ ) {
				cx = x+cstpt.m_x;
				cy = y+cstpt.m_y;
				cz = z+cstpt.m_z;
				if ( cx >= 0 && cx < nX && cy >= 0 && cy < nY && cz >= 0 && cz < nZ ) {
					tx = x+istpt.m_x;
					ty = y+istpt.m_y;
					tz = z+istpt.m_z;
					patMean += img->GetAt(tx,ty,tz);
					count++;
				}
			}
		}
	}
	patMean /= (float)count;

	for ( z = 0, i = 0; z < size.m_z; z++ ) {
		for ( y = 0; y < size.m_y; y++ ) {
			for ( x = 0; x < size.m_x; x++, i++ ) {
				cx = x+cstpt.m_x;
				cy = y+cstpt.m_y;
				cz = z+cstpt.m_z;
				if ( cx >= 0 && cx < nX && cy >= 0 && cy < nY && cz >= 0 && cz < nZ ) {
					tx = x+istpt.m_x;
					ty = y+istpt.m_y;
					tz = z+istpt.m_z;
					patVar += (img->GetAt(tx,ty,tz)-patMean)*(img->GetAt(tx,ty,tz)-patMean);
				}
			}
		}
	}
	patVar /= (float)count;


	float sum = 0;
	for ( z = 0, i = 0; z < size.m_z; z++ ) {
		for ( y = 0; y < size.m_y; y++ ) {
			for ( x = 0; x < size.m_x; x++, i++ ) {
				cx = x+cstpt.m_x;
				cy = y+cstpt.m_y;
				cz = z+cstpt.m_z;
				if ( cx >= 0 && cx < nX && cy >= 0 && cy < nY && cz >= 0 && cz < nZ ) {
					tx = x+istpt.m_x;
					ty = y+istpt.m_y;
					tz = z+istpt.m_z;
					sum += (img->GetAt(tx,ty,tz)-patMean)*(cimg->GetAt(cx,cy,cz)-cMean);
				}
			}
		}
	}

	return sum/(sqrt(patVar*cVar)*(float)count);
}


float ComputeSubRegionNCC(CVoxel* img, float iMean, float iVar, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, float cMean, float cVar)
{
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	int count = 0;

	float sum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				sum += (img->m_pData[i]-iMean)*(cimg->GetAt(x,y,z)-cMean);
				count++;
			}
		}
	}

	return sum/(sqrt(iVar*cVar)*(float)count);
}

float ComputeSubRegionNCC(CVoxel* img, cv::Point3i istpt, float iMean, float iVar, CVoxel* cimg, cv::Point3i cstpt, cv::Point3i size, float cMean, float cVar)
{
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	int count = 0;

	float sum = 0;
	//for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
	//	for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
	//		for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
	for ( z = 0, i = 0; z < size.m_z; z++ ) {
		for ( y = 0; y < size.m_y; y++ ) {
			for ( x = 0; x < size.m_x; x++, i++ ) {
				sum += (img->GetAt(x+istpt.m_x,y+istpt.m_y,z+istpt.m_z)-iMean)*(cimg->GetAt(x+cstpt.m_x,y+cstpt.m_y,z+cstpt.m_z)-cMean);
				count++;
			}
		}
	}

	return sum/(sqrt(iVar*cVar)*(float)count);
}



float ComputeSubRegionNCC(CVoxel* img, CVoxel* cimg, cv::Point3i stpt, cv::Point3i enpt, CVoxel* label, float cMean, float cVar)
{
	// compute ncc
	int nX = cimg->m_nX;
	int nY = cimg->m_nY;
	int nZ = cimg->m_nZ;

	int x, y, z, i;
	float patMean = 0; 
	float patVar = 0;
	int count = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if ( label->m_pData[i] ) {
						patMean += img->m_pData[i];
						count++;
					}
				}
			}
		}
	}
	patMean /= (float)count;

	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if ( label->m_pData[i] ) { 
						patVar += (img->m_pData[i]-patMean)*(img->m_pData[i]-patMean);
					}
				}
			}
		}
	}

	patVar /= (float)count;


	float sum = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				if ( x >= 0 && x < nX && y >= 0 && y < nY && z >= 0 && z < nZ ) {
					if ( label->m_pData[i] ) { 
						sum += (img->m_pData[i]-patMean)*(cimg->GetAt(x,y,z)-cMean);
					}
				}
			}
		}
	}

	return sum/(sqrt(patVar*cVar)*(float)count);
}

float ComputeSubRegionOverlayCount(CFVoxel* prob, cv::Point3i stpt, cv::Point3i enpt, CVoxel* uimg, cv::Point3i uStpt)
{
	int nX = uimg->m_nX;
	int nY = uimg->m_nY;
	int nZ = uimg->m_nZ;

	int x, y, z, i;
	int cx, cy, cz;

	float val = 0;
	for ( z = stpt.m_z, i = 0; z < enpt.m_z; z++ ) {
		for ( y = stpt.m_y; y < enpt.m_y; y++ ) {
			for ( x = stpt.m_x; x < enpt.m_x; x++, i++ ) {
				cx = uStpt.m_x+x-stpt.m_x;
				cy = uStpt.m_y+y-stpt.m_y;
				cz = uStpt.m_z+z-stpt.m_z;

				if ( cx >= 0 && cx < nX && cy >= 0 && cy < nY && cz >= 0 && cz < nZ ) {
					if ( uimg->GetAt(cx,cy,cz) == 1 && prob->GetAt(x,y,z) == 0) {
						val = val+1;
					}

					if ( uimg->GetAt(cx,cy,cz) == 3 && prob->GetAt(x,y,z) > 0.5) {
						val = val+prob->GetAt(x,y,z);
					}

					//if ( uimg->GetAt(x,y,z) == 1 || uimg->GetAt(x,y,z) == 3) {
					//	int a = 0;
					//}
				}
			}
		}
	}

	return val;
}

float ComputeSubRegionOverlayCount2(CFVoxel* prob, cv::Point3i stpt, int userInputType, CVoxel* cimg, CVoxel* uimg, cv::Point3i uStpt, cv::Point3i size)
{
	int nX = uimg->m_nX;
	int nY = uimg->m_nY;
	int nZ = uimg->m_nZ;

	int x, y, z, i;
	int cx,cy,cz,ux,uy,uz;

	int labelU, labelC;
	if ( userInputType == 0 ) {
		labelU = 2;
		labelC = 1;
	}
	else {
		labelU = 1;
		labelC = 2;
	}

	float val = 0;
	for ( z = 0, i = 0; z < size.m_z; z++ ) {
		for ( y = 0; y < size.m_y; y++ ) {
			for ( x = 0; x < size.m_x; x++, i++ ) {
				cx = stpt.m_x+x;
				cy = stpt.m_y+y;
				cz = stpt.m_z+z;
				ux = uStpt.m_x+x;
				uy = uStpt.m_y+y;
				uz = uStpt.m_z+z;

				//int count = 0;
				if ( ux >= 0 && ux < nX && uy >= 0 && uy < nY && uz >= 0 && uz < nZ ) {
					if ( uimg->GetAt(ux,uy,uz) == BYTE_BG && prob->GetAt(cx,cy,cz) < 0.5 ) {
						val = val+1;
					}
					else if ( uimg->GetAt(ux,uy,uz) == labelU && prob->GetAt(cx,cy,cz) > 0.5 ) {
						val = val+prob->GetAt(cx,cy,cz);
					}
					else if ( uimg->GetAt(ux,uy,uz) == BYTE_BG && prob->GetAt(cx,cy,cz) > 0.5 ) {
						val = val-prob->GetAt(cx,cy,cz);
					}
					else if ( uimg->GetAt(ux,uy,uz) == labelU && prob->GetAt(cx,cy,cz) < 0.5 ) {
						val = val-1;
					}

					else  if ( uimg->GetAt(ux,uy,uz) == 0 && cimg->GetAt(ux,uy,uz) == labelC && prob->GetAt(cx,cy,cz) > 0.5 ) {
						val = val+1;
					}
					else  if ( uimg->GetAt(ux,uy,uz) == 0 && cimg->GetAt(ux,uy,uz) == labelC && prob->GetAt(cx,cy,cz) < 0.5 ) {
						val = val-1;
					}
					else  if ( uimg->GetAt(ux,uy,uz) == 0 && cimg->GetAt(ux,uy,uz) != labelC && prob->GetAt(cx,cy,cz) > 0.5 ) {
						val = val-1;
					}
					else  if ( uimg->GetAt(ux,uy,uz) == 0 && cimg->GetAt(ux,uy,uz) != labelC && prob->GetAt(cx,cy,cz) < 0.5 ) {
						val = val+1;
					}

					else {

					}

					if ( prob->GetAt(cx,cy,cz) > 0.5 ) {
						int a = 0;
					}

					if ( uimg->GetAt(ux,uy,uz) == labelC ) {
						int a = 0;
					}


					/*if ( uimg->GetAt(ux,uy,uz) == 1 && m_label->m_pData[k] != 2 ) {
						count--;
					}
					else  if ( label->GetAt(l,m,n) == 3 && m_label->m_pData[k] == 2 ) {
						count--;
					}
					else  if ( label->GetAt(l,m,n) == 1 && m_label->m_pData[k] == 2 ) {
						count++;
					}
					else  if ( label->GetAt(l,m,n) == 3 && m_label->m_pData[k] != 2 ) {
						count++;
					}
					else  if ( label->GetAt(l,m,n) == 0 && clabel->GetAt(l,m,n) && m_label->m_pData[k] == 2 ) {
						count++;
					}
					else  if ( label->GetAt(l,m,n) == 0 && clabel->GetAt(l,m,n) == 0 && m_label->m_pData[k] == 2 ) {
						count--;
					}
					else {
					}*/
				}
			}
		}
	}

	return val;
}


float ComputeGradientSum(CVoxel* gradientMap, CFVoxel* probPat)
{
	int nX = probPat->m_nX;
	int nY = probPat->m_nY;
	int nZ = probPat->m_nZ;

	int x,y,z,i;
	float sum = 0;
	int count = 0;
	for ( z = 0, i = 0; z < nZ; z++ ) {
		for ( y = 0; y < nY; y++ ) {
			for ( x = 0; x < nX; x++, i++ ) {
				if ( probPat->GetAt(x,y,z) > 0.1 && probPat->GetAt(x,y,z) < 0.9 ) {
					sum += gradientMap->GetAt(x,y,z);
					count++;
				}
			}
		}
	}

	sum /= count;
	return sum;
}


CVoxel* VTKImageToCVoxel(vtkImageData* vtkIamge, int nX, int nY, int nZ )
{
	double spacing[3];
	vtkIamge->GetSpacing(spacing);
	
	float fXSp = (float)spacing[0];
	float fYSp = (float)spacing[1];
	float fZSp = (float)spacing[2];
	CVoxel* vxl = new CVoxel(nX,nY,nZ);

	short* imgdata = ( short* )vtkIamge->GetScalarPointer();
	int x,y,z;
	short int val;
	for( z = 0 ; z < nZ ; z++ ) {
		for( y = 0 ; y < nY ; y++ ) {
			for( x = 0 ; x < nX ; x++ ) {
				val = (short int)imgdata[nX*nY*z+nX*y+x];
				vxl->SetAt(x,y,z,val);
			}
		}
	}

	return vxl;
}


CVoxel* VTKImageToCVoxel(vtkImageData* vtkIamge)
{
	int dims[3];
	double spacing[3];
	vtkIamge->GetDimensions(dims);
	vtkIamge->GetSpacing(spacing);
	//vtkIamge->GetExtent(dims);

	int nX = dims[0];
	int nY = dims[1];
	int nZ = dims[2];
	float fXSp = (float)spacing[0];
	float fYSp = (float)spacing[1];
	float fZSp = (float)spacing[2];
	CVoxel* vxl = new CVoxel(nX,nY,nZ);

	short* imgdata = ( short* )vtkIamge->GetScalarPointer();
	int x,y,z;
	short int val;
	for( z = 0 ; z < nZ ; z++ ) {
		for( y = 0 ; y < nY ; y++ ) {
			for( x = 0 ; x < nX ; x++ ) {
				val = (short int)imgdata[nX*nY*z+nX*y+x];
				vxl->SetAt(x,y,z,val);
			}
		}
	}

	return vxl;
}

double** Create2DDoubleMat( int nRows, int nCols )
{
	int i, j;
	double **mat = new double*[nRows];
	for ( i = 0; i < nRows; i++ ) {
		mat[i] = new double[nCols];
		for ( j = 0; j < nCols; j++ ) {
			mat[i][j] = 0;
		}
	}
	return mat;
}
