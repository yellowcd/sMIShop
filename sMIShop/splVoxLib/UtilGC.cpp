
#include "UtilGC.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Graph Cut Segmentation related methods
//////////////////////////////////////////////////////////////////////////////////////////

void Prob2Potential(float *f, int N)
{
	for ( int i = 0; i < N; i++ ) {
		f[i] = ( f[i]>0 ? std::min(-(float)log(f[i]), (float)UPOT_MAX) : (float)UPOT_MAX);
		//b[i] = ( b[i]>0 ? min(-(float)log(f[i]), UPOT_MAX) : UPOT_MAX);
	}
}

void ComputeUnaryPairwiseSums(CFVoxel *fgU, CFVoxel *bgU, CTypedPtrArray<CPtrArray, CFVoxel*> &pw, cvPoint3iArray &pwnc, 
							  CVoxel *segm, double &usum, double &pwsum)
{
	int imX = segm->m_nX;
	int imY = segm->m_nY;
	int imZ = segm->m_nZ;
	usum = 0;
	pwsum = 0;
	gtype fgusum, bgusum;
	fgusum = bgusum = 0;
	int x, y, z, i, n, l;
	int nn = (int) pwnc.GetSize();
	cv::Point3i dpt(imX, imY, imZ);
	for ( z = 0, i = 0; z < imZ; z++ ) {
		for ( y = 0; y < imY; y++ ) {
			for ( x = 0; x < imX; x++, i++ ) {
				// get label of current pixel
				l = segm->m_pData[i];
				//////////////////////////////////////////////////////////////////////////
				// sum unary and pairwise energy from labels and potentials
				// UNARY energy
				// if voxel is assigned as background, add bgUnaries
				if ( l == 0 )
					bgusum += bgU->m_pfData[i];
				// else, add fgUnaries
				else 
					fgusum += fgU->m_pfData[i];

				// PAIRWISE energy
				// for all neighbors 
				cv::Point3i cpt(x,y,z);
				for ( n = 0; n < nn; n++ ) {
					cv::Point3i pt = cpt+pwnc[n];
					if ( ZERO_3DPT <= pt && pt < dpt  && 
						l != segm->m_pData[pt.m_z*imX*imY+pt.m_y*imX+pt.m_x] )
						pwsum += pw[n]->GetAt(x,y,z);
				}
			} // end for x
		} // end for y
	} 
	usum = fgusum+bgusum;
}


void ComputeUnaryPairwiseSums(CFVoxel *fgU, CFVoxel *bgU, 
							  CTypedPtrArray<CPtrArray, CFVoxel*> &pw, cvPoint3iArray &pwnc, int idx,
							  CVoxel *segm, double &usum, double &pwsum)
{
	int imX = segm->m_nX;
	int imY = segm->m_nY;
	int imZ = segm->m_nZ;
	usum = 0;
	pwsum = 0;
	gtype fgusum, bgusum;
	fgusum = bgusum = 0;
	int x, y, z, i, n, l;
	int nn = (int) pwnc.GetSize();
	cv::Point3i dpt(imX, imY, imZ);
	for ( z = 0, i = 0; z < imZ; z++ ) {
		for ( y = 0; y < imY; y++ ) {
			for ( x = 0; x < imX; x++, i++ ) {
				// get label of current pixel
				l = segm->m_pData[i];
				//////////////////////////////////////////////////////////////////////////
				// sum unary and pairwise energy from labels and potentials
				// UNARY energy
				// if voxel is assigned as background, add bgUnaries
				if ( l == 0 )
					bgusum += bgU->m_pfData[i];
				// else, add fgUnaries
				else 
					fgusum += fgU->m_pfData[i];

				// PAIRWISE energy
				// for all neighbors 
				cv::Point3i cpt(x,y,z);
				for ( n = 0; n < nn; n++ ) {
					cv::Point3i pt = cpt+pwnc[n];
					if ( ZERO_3DPT <= pt && pt < dpt  && 
						l != segm->m_pData[pt.m_z*imX*imY+pt.m_y*imX+pt.m_x] ) {
						//CFVoxel* tmpPW = pw[idx*nn + n];
						pwsum += pw[idx*nn + n]->GetAt(x,y,z);
					}
						
				}
			} // end for x
		} // end for y
	} 
	usum = fgusum+bgusum;
}


void ComputeTRWSUnaryPairwiseSums( float** unaryCost, int * nPair, int ** idxArray, float** pCostArray, int nNode, int* nlabel, int *label, double &usum, double &pwsum )
{
	usum = 0;
	pwsum = 0;
	int i, j, l, m;
	int label1, label2;

	for ( i = 0; i < nNode; i++ ) {
		label1 = label[i];
		usum += (double)unaryCost[i][label1];

		int idx = 0;
		for ( j = 0; j < nPair[i]; j++ ) {
			label2 = label[idxArray[i][j]];

			//int beforeIdx = 0;
			//for ( k = 0; k < m; k++ ) {
			//	beforeIdx += nlabel[i] * nlabel[idxArray[i][k]];
			//}

			for ( l = 0; l < nlabel[i]; l++ ) {
				for ( m = 0; m < nlabel[idxArray[i][j]]; m++ ) {
					if ( label1 == l && label2 == m ) {
						pwsum += pCostArray[i][idx];
					}
					
					idx++;
				}
			}
			//pwsum += (double)pCostArray[i][beforeIdx + l+m*nlabel[i]];
			//pairwiseEnergy[l + m*nlabel[i]] = pCostArray[i][idx];
		}
	}
}


void ComputeGradientPairwiseTerms(IplImage *img, gtype *pairwise, gtype lambda)
{
	int w = img->width; 
	int h = img->height;
	int ws = img->widthStep;
	int n = w*h*4;
	int nghb[4];
	int nidx, imidx;
	int diff, diff_square;
	int neighbor_count = 0;
	long int sumDiff = 0;
	BYTE *pImg = (BYTE *)img->imageData;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			imidx = y*ws+x;
		// neighbors
			nghb[0] = (y == 0)? -1 : (imidx  - ws);
			nghb[1] = (y == 0 || x == w - 1)? -1 : (imidx  - ws + 1);
			nghb[2] = (x == w - 1)? -1 : (imidx  + 1);
			nghb[3] = (y == h - 1 || x == w - 1)? -1 : (imidx  + ws + 1);
			for (int j = 0; j < 4; j++) {
				nidx = nghb[j];
				if ( nidx < 0 ) {	continue; }
				diff = pImg[imidx] - pImg[nidx];
				diff_square = diff*diff;
				sumDiff += diff_square;
				neighbor_count++;
			}
		}
	}
	double avgDiff = (double)sumDiff / (double)neighbor_count ;

	// construct neighbor info...
	double one_over_dist ;
	double one_over_sqrt2 = 1.0f / (float)sqrt(2.0) ;
	double beta;
	if( avgDiff != 0 )
		beta = -1.0f / avgDiff * 0.5f;

	//float gamma = 5.0 ;
	//i = 0;
	//pV = m_pV;
	int pIdx = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			imidx = y*ws+x;
			// neighbors
			nghb[0] = (y == 0)? -1 : (imidx  - ws);
			nghb[1] = (y == 0 || x == w - 1)? -1 : (imidx  - ws + 1);
			nghb[2] = (x == w - 1)? -1 : (imidx  + 1);
			nghb[3] = (y == h - 1 || x == w - 1)? -1 : (imidx  + ws + 1);
			for (int j = 0; j < 4; j++) {
				nidx = nghb[j];
				if ( nidx < 0 ) {	
					pairwise[pIdx++] = 0; 
					continue;
				}
				if( j == 0 || j == 2 )
					one_over_dist = 1.0f ;
				else 
					one_over_dist = one_over_sqrt2 ;
				diff = pImg[imidx] - pImg[nidx];
				diff_square = diff*diff;
				pairwise[pIdx++] = (int)Round((double)lambda*exp(beta *(double)diff_square)*one_over_dist);
			}
		}
	}
	//SavePairwise(w, h, lambda, pairwise);
}

void ComputeColorGradientPairwiseTerms(IplImage *img, gtype *pairwise, gtype lambda)
{
	int w = img->width; 
	int h = img->height;
	int ws = img->widthStep;
	int n = w*h*4;
	int nghb[4];
	int nidx, imidx, imRGBidx, nRGBidx;
	int diff;
	int neighbor_count = 0;
	long int sumDiff = 0;
	BYTE *pImg = (BYTE *)img->imageData;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			imidx = y*w+x;
			imRGBidx = y*ws+x*3;
			// neighbors
			nghb[0] = (y == 0)? -1 : (imidx  - w);
			nghb[1] = (y == 0 || x == w - 1)? -1 : (imidx  - w + 1);
			nghb[2] = (x == w - 1)? -1 : (imidx  + 1);
			nghb[3] = (y == h - 1 || x == w - 1)? -1 : (imidx  + w + 1);
			for (int j = 0; j < 4; j++) {
				nidx = nghb[j];
				nRGBidx = nidx*3;
				if ( nidx < 0 ) {	continue; }
				diff = (pImg[imRGBidx    ] - pImg[nRGBidx    ])*(pImg[imRGBidx    ] - pImg[nRGBidx   ])+
						 (pImg[imRGBidx+1] - pImg[nRGBidx+1])*(pImg[imRGBidx+1] - pImg[nRGBidx+1])+
						 (pImg[imRGBidx+2] - pImg[nRGBidx+2])*(pImg[imRGBidx+2] - pImg[nRGBidx+2]);
				sumDiff += diff;
				neighbor_count++;
			}
		}
	}
	double avgDiff = (double)sumDiff / (double)neighbor_count ;

	// construct neighbor info...
	double one_over_dist ;
	double one_over_sqrt2 = 1.0f / (float)sqrt(2.0) ;
	double beta;
	if( avgDiff != 0 )
		beta = -1.0f / avgDiff * 0.5f;

	//float gamma = 5.0 ;
	//i = 0;
	//pV = m_pV;
	int pIdx = 0;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			imidx = y*w+x;
			imRGBidx = y*ws+x*3;
			// neighbors
			nghb[0] = (y == 0)? -1 : (imidx  - w);
			nghb[1] = (y == 0 || x == w - 1)? -1 : (imidx  - w + 1);
			nghb[2] = (x == w - 1)? -1 : (imidx  + 1);
			nghb[3] = (y == h - 1 || x == w - 1)? -1 : (imidx  + w + 1);
			for (int j = 0; j < 4; j++) {
				nidx = nghb[j];
				nRGBidx = nidx*3;
				if ( nidx < 0 ) {	
					pairwise[pIdx++] = 0; 
					continue;
				}
				else {
					if( j == 0 || j == 2 )
						one_over_dist = 1.0f ;
					else 
						one_over_dist = one_over_sqrt2 ;
					diff = (pImg[imRGBidx    ] - pImg[nRGBidx    ])*(pImg[imRGBidx    ] - pImg[nRGBidx   ])+
						(pImg[imRGBidx+1] - pImg[nRGBidx+1])*(pImg[imRGBidx+1] - pImg[nRGBidx+1])+
						(pImg[imRGBidx+2] - pImg[nRGBidx+2])*(pImg[imRGBidx+2] - pImg[nRGBidx+2]);
					pairwise[pIdx++] = (int)Round((double)lambda*exp(beta *(double)diff)*one_over_dist);
				}
			}
		}
	}	
}

void ConstantPairwiseTerms(CVoxel *vox, gtype *pairwise, gtype constVal)
{
	int nx = vox->m_nX; 
	int ny = vox->m_nY;
	int nz = vox->m_nZ;
	int n = nx*ny*nz*3;
	int nghb[3];
	int neighbor_count = 0;

	short int *pvox = vox->m_pData;
	int x, y, z, i;
	int nidx;
	int nxy = nx*ny;

	int pIdx = 0;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	
						pairwise[pIdx++] = 0; 
						continue;
					}

					if ( pvox[i] == pvox[nidx]) {
						pairwise[pIdx++] = constVal;
					}
					else	pairwise[pIdx++] = 0;
				}
			}
		}
	}
}

void ComputeVoxelPairwiseTerms(CVoxel *vox, CFVoxelPtrArray& pairwise, gtype lambda)
{
	int nx = vox->m_nX; 
	int ny = vox->m_nY;
	int nz = vox->m_nZ;
	int n = nx*ny*nz*3;
	int nghb[3];
	int diff, diff_square;
	int neighbor_count = 0;
	double sumDiff = 0;
	short int *pvox = vox->m_pData;
	int x, y, z, i;
	int nidx;
	int nxy = nx*ny;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) { continue; }
					diff = pvox[i] - pvox[nidx];
					diff_square = diff*diff;
					sumDiff += diff_square;
					neighbor_count++;
				}
			}
		}
	}
	double avgDiff = sumDiff / (double)neighbor_count ;

	// construct neighbor info...
	double beta;
	if( avgDiff != 0 )
		beta = -1.0f / avgDiff * 0.5f;

	//float gamma = 5.0 ;
	int pIdx = 0;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	
						CFVoxel* tmp = pairwise.GetAt(j);
						tmp->SetAt(x,y,z,0);
						//pairwise[pIdx++] = 0; 
						continue;
					}
					diff = pvox[i] - pvox[nidx];
					diff_square = diff*diff;
					//pairwise[pIdx++] = (int)Round((double)lambda*exp(beta *(double)diff_square));
					double val = lambda*exp(beta *(double)diff_square);
					CFVoxel* tmp = pairwise.GetAt(j);
					tmp->SetAt(x,y,z,val);
				}
			}
		}
	}
}

void ComputeVoxelPairwiseTerms(CVoxel *vox, gtype *pairwise, gtype lambda)
{
	int nx = vox->m_nX; 
	int ny = vox->m_nY;
	int nz = vox->m_nZ;
	int n = nx*ny*nz*3;
	int nghb[3];
	int diff, diff_square;
	int neighbor_count = 0;
	double sumDiff = 0;
	short int *pvox = vox->m_pData;
	int x, y, z, i;
	int nidx;
	int nxy = nx*ny;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	continue; }
					diff = pvox[i] - pvox[nidx];
					diff_square = diff*diff;
					sumDiff += diff_square;
					neighbor_count++;
				}
			}
		}
	}
	double avgDiff = sumDiff / (double)neighbor_count ;

	// construct neighbor info...
	double beta;
	if( avgDiff != 0 )
		//beta = -0.01f / avgDiff * 0.5f;
		beta = -0.1f / avgDiff * 0.5f;
		//beta = -1.0f / avgDiff * 0.5f;

	//float gamma = 5.0 ;
	int pIdx = 0;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	
						pairwise[pIdx++] = 0; 
						continue;
					}
					diff = pvox[i] - pvox[nidx];
					diff_square = diff*diff;
					pairwise[pIdx++] = (double)lambda*exp(beta *(double)diff_square);
				}
			}
		}
	}
}

void ComputeBCIPairwiseTerms(CVoxel *vox, int *pairwise, int lambda)
{
	int nx = vox->m_nX; 
	int ny = vox->m_nY;
	int nz = vox->m_nZ;
	int n = nx*ny*nz*3;
	int nghb[3];
	int diff;
	//int diff_square;
	double sumDiff = 0;
	short int *pvox = vox->m_pData;
	int x, y, z, i;
	int nidx;
	int nxy = nx*ny;
/*	
	int neighbor_count = 0;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	continue; }
					diff = pvox[i] - pvox[nidx];
					diff_square = diff*diff;
					sumDiff += diff_square;
					neighbor_count++;
				}
			}
		}
	}
	double avgDiff = sumDiff / (double)neighbor_count ;

	// construct neighbor info...
	double beta;
	if( avgDiff != 0 )
		beta = -1.0f / avgDiff * 0.5f;*/

	//float gamma = 5.0 ;
	int pIdx = 0;
	for (z = 0, i = 0; z < nz; z++) {
		for (y = 0; y < ny; y++) {
			for (x = 0; x < nx; x++, i++) {
				// neighbors
				nghb[0] = (z == nz-1)? -1 : (i + nxy);
				nghb[1] = (y == ny-1)? -1 : (i + nx);
				nghb[2] = (x == nx-1)? -1 : (i + 1);
				for (int j = 0; j < 3; j++) {
					nidx = nghb[j];
					if ( nidx < 0 ) {	
						pairwise[pIdx++] = 0; 
						continue;
					}
					diff = pvox[i] - pvox[nidx];
					if(diff!=0)
					{
						pairwise[pIdx++] = 1*lambda;
					}
					else
					{
						pairwise[pIdx++] = 0;
					}
				}
			}
		}
	}
}

void SavePairwise(int w, int h, int lambda, gtype *pairwise)
{
	IplImage* img1 = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U , 1 );
	IplImage* img2 = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U , 1 );
	IplImage* img3 = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U , 1 );
	IplImage* img4 = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U , 1 );
	int i, j, k;
	for(j=0, i=0;  j<h; j++){
		for(k=0; k<w; k++, i++ ){
			img1->imageData[j*img1->widthStep+k] = (BYTE)((double)pairwise[4*i]/(double)lambda*255);
			img2->imageData[j*img2->widthStep+k] = (BYTE)((double)pairwise[4*i+1]/(double)lambda*255);
			img3->imageData[j*img3->widthStep+k] = (BYTE)((double)pairwise[4*i+2]/(double)lambda*255);
			img4->imageData[j*img4->widthStep+k] = (BYTE)((double)pairwise[4*i+3]/(double)lambda*255);
		}
	}
	cvSaveImage("F:\\2008_Fall_NewResearch\\_3D-DB\\pair1.bmp",img1);
	cvSaveImage("F:\\2008_Fall_NewResearch\\_3D-DB\\pair2.bmp",img2);
	cvSaveImage("F:\\2008_Fall_NewResearch\\_3D-DB\\pair3.bmp",img3);
	cvSaveImage("F:\\2008_Fall_NewResearch\\_3D-DB\\pair4.bmp",img4);
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img3);
	cvReleaseImage(&img4);
}
