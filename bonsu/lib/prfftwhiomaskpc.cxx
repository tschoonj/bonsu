/*
#############################################
##   Filename: prfftwhiomaskpc.cxx
##
##    Copyright (C) 2013 Marcus C. Newton
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
## Contact: Bonsu.Devel@gmail.com
#############################################
*/

#include <Python.h>
#include "prfftwmodule.h"

void mask_gamma(double* gamma, int32_t* nn, int32_t* maskdim)
{
	int ii, i, j, k;
	if( nn[0] ==1 )  maskdim[0] = 1;
	if( nn[1] ==1 )  maskdim[1] = 1;
	if( nn[2] ==1 )  maskdim[2] = 1;
	int32_t nns[3] = {0, 0, 0};
	int32_t nne[3] = {0, 0, 0};
	nns[0] = (nn[0] - maskdim[0])/2;
	nne[0] = nns[0] + maskdim[0];
	nns[1] = (nn[1] - maskdim[1])/2;
	nne[1] = nns[1] + maskdim[1];
	nns[2] = (nn[2] - maskdim[2])/2;
	nne[2] = nns[2] + maskdim[2];
	
	for(i=0;i<nn[0]; i++)
	{
		for(j=0;j<nn[1]; j++)
		{
			for(k=0;k<nn[2]; k++)
			{
				if( i >= nns[0] && i  < nne[0] &&
					j >= nns[1] && j < nne[1] &&
					k >= nns[2] && k < nne[2] )
				{
					ii = (k+nn[2]*(j+nn[1]*i));
					gamma[2*ii] = 0.0;
					gamma[2*ii+1] = 0.0;
				}
			}
		}
	}
}

void make_Id_iter( double* rho, double* rhom1, double* pca_Id_iter, int32_t* nn)
{
	int len = nn[0] * nn[1] * nn[2];
	int i;
	double itnsty, itnstym1;
	for(i=0; i<len; i++)
	{
		itnsty = rho[2*i]*rho[2*i] + rho[2*i+1]*rho[2*i+1];
		itnstym1 = rhom1[2*i]*rhom1[2*i] + rhom1[2*i+1]*rhom1[2*i+1];
		pca_Id_iter[2*i] = (2.0*itnsty - itnstym1);
		pca_Id_iter[2*i+1] = 0.0;
	}
}

void CopySquare( double* rho, double* itnsty, int32_t* nn)
{
	int len = nn[0] * nn[1] * nn[2];
	int i;
	for(i=0; i<len; i++)
	{
		itnsty[2*i] = (rho[2*i]*rho[2*i] + rho[2*i+1]*rho[2*i+1]);
		itnsty[2*i+1] = 0.0;
	}
}

void divide_I_Id_iter( double* expdata, double* pca_Idm_iter, double* mask, double* pca_Idmdiv_iter, int32_t* nn)
{
	int len = nn[0] * nn[1] * nn[2];
	int i;
	double val1[2] = {0.0,0.0};
	double val2[2] = {0.0,0.0};
	double divis = 0;
	for(i=0; i<len; i++)
	{
		
		val1[0] = (expdata[2*i]*expdata[2*i] + expdata[2*i+1]*expdata[2*i+1]);
		val1[1] = 0.0;
		val2[0] = pca_Idm_iter[2*i];
		val2[1] = pca_Idm_iter[2*i+1];
		divis = val2[0]*val2[0] + val2[1]*val2[1];
		if(divis >1e-150)
		{
			pca_Idmdiv_iter[2*i] = (val1[0]*val2[0] + val1[1]*val2[1])/divis;
			pca_Idmdiv_iter[2*i+1] =(val1[1]*val2[0] - val1[0]*val2[1])/divis;
		}
		else
		{
			pca_Idmdiv_iter[2*i] = 0.0;
			pca_Idmdiv_iter[2*i+1] = 0.0;
		}
	}
}

int update_gamma
(
	double* expdata,
	double* rho,
	double* rhom1,
	double* gamma,
	double* mask,
	int32_t ndim,
	int32_t* nn,
	int32_t* nnh,
	int numiter,
	double* tmpdata1,
	double* tmpdata2,
	fftw_plan* torecip,
	fftw_plan* toreal
)
{
	int i;
	int len = nn[0] * nn[1] * nn[2];
	double* pca_Id_iter = (double*) fftw_malloc( 2*len * sizeof(double));
	double* pca_Idm_iter;
	double* pca_Idmdiv_iter = (double*) fftw_malloc( 2*len * sizeof(double));
	double* pca_IdmdivId_iter = (double*) fftw_malloc( 2*len * sizeof(double));
	if (!pca_Id_iter || !pca_Idmdiv_iter || !pca_IdmdivId_iter)
	{
		free(pca_Id_iter);
		free(pca_Idmdiv_iter);
		free(pca_IdmdivId_iter);
		return 1;
	}
	pca_Idm_iter = pca_Id_iter;

	double itnsty_sum = 0.0;
	
	
	for(i=0; i<numiter; i++)
	{
		ZeroArray(pca_Idmdiv_iter, nn);
		make_Id_iter(rho, rhom1, pca_Id_iter, nn);
		SumArray(pca_Id_iter, nn, &itnsty_sum);
		
		
		
		CopyArray(pca_Id_iter, pca_IdmdivId_iter, nn);
		conj_reflect(pca_IdmdivId_iter, nn);
		
		wrap_array(pca_Id_iter, nn, -1);
		wrap_array(gamma, nn, -1);
		convolve_nomem2(pca_Id_iter, gamma, ndim, nn, tmpdata1, tmpdata2, torecip, toreal);
		wrap_array(pca_Id_iter, nn, 1);
		wrap_array(gamma, nn, 1);
		
		divide_I_Id_iter(expdata, pca_Idm_iter, mask, pca_Idmdiv_iter, nn);
		
		wrap_array(pca_IdmdivId_iter, nn, -1);
		wrap_array(pca_Idmdiv_iter, nn, -1);
		convolve_nomem2(pca_IdmdivId_iter, pca_Idmdiv_iter, ndim, nn, tmpdata1, tmpdata2, torecip, toreal);
		wrap_array(pca_IdmdivId_iter, nn, 1);
		wrap_array(pca_Idmdiv_iter, nn, 1);
		
		ScaleArray(pca_IdmdivId_iter, nn, (1.0/itnsty_sum));
		MultiplyArray(gamma, pca_IdmdivId_iter, gamma, nn);
		
		mask_gamma(gamma, nn, nnh); 
		
		
		
		
	}
	
	free(pca_Id_iter);
	free(pca_Idmdiv_iter);
	free(pca_IdmdivId_iter);
	return 0;
}

void SumArray
(
	double* data,
	int32_t* nn,
	double* sum
)
{
	int len = nn[0] * nn[1] * nn[2];
	int i;
	*sum = 0.0;
	for(i=0; i<len; i++)
	{
		*sum += sqrt( data[2*i]*data[2*i] + data[2*i+1]*data[2*i+1] );
	}
}

void lorentz_ft_fill
(
	double* data,
	int32_t* nn,
	double gammaHWHM
)
{
	int i,j,k,ii;
	for(i=0;i<nn[0]; i++)
	{
		for(j=0;j<nn[1]; j++)
		{
			for(k=0;k<nn[2]; k++)
			{
				ii = (k+nn[2]*(j+nn[1]*i));
				data[2*ii] = exp(- fabs(gammaHWHM)*
									sqrt((double) (i-nn[0]/2)*(i-nn[0]/2)
									+(j-nn[1]/2)*(j-nn[1]/2)
									+(k-nn[2]/2)*(k-nn[2]/2))
									);
				data[2*ii+1] = 0.0;
			}
		}
	}
}

void MaskedSetPCAmplitudes
(
	double* seqdata,
	double* expdata,
	double* itnsty,
	double* mask,
	int32_t* nn
)
{
	int64_t len = (int64_t) nn[0] * nn[1] * nn[2];
	int64_t i;
	double expamp, pcamp, amp, phase;
	for(i=0; i<len; i++)
	{
		if (mask[2*i] > 1e-6)
		{
			expamp = sqrt( expdata[2*i]*expdata[2*i] +
						expdata[2*i+1]*expdata[2*i+1]);
			amp = sqrt( seqdata[2*i]*seqdata[2*i] +
						seqdata[2*i+1]*seqdata[2*i+1]);
			pcamp = sqrt(sqrt( itnsty[2*i]*itnsty[2*i] + itnsty[2*i+1]*itnsty[2*i+1] ));
			phase = atan2(seqdata[2*i+1], seqdata[2*i]);
			seqdata[2*i] = (expamp*amp/pcamp)*cos(phase);
			seqdata[2*i+1] = (expamp*amp/pcamp)*sin(phase);
		}
	}
}

void HIOMaskPC
(
	double* seqdata,
	double* expdata,
	double* support,
	double* mask,
	double gammaHWHM,
	int gammaRS,
	int numiterRL,
	int startiterRL,
	int waititerRL,
	int zex,
	int zey,
	int zez,
	double beta,
	int startiter,
	int numiter,
	int ndim,
	double* rho_m1,
	int32_t* nn,
	double* residual,
	int32_t* citer_flow,
	double* visual_amp_real,
	double* visual_phase_real,
	double* visual_amp_recip,
	double* visual_phase_recip,
	PyObject* updatereal,
	PyObject* updaterecip,
	PyObject* updatelog
)
{
	Py_BEGIN_ALLOW_THREADS;

	fftw_init_threads();
	fftw_plan_with_nthreads(citer_flow[7]);

	fftw_plan torecip;
	fftw_plan toreal;
	int32_t iter;
	double sos = 0.0;
	double sos1 = 0.0;
	double sos2 = 0.0;
	double res = 0.0;
	double norm = 0.0;
	int32_t update_count_real = 0;
	int32_t update_count_recip = 0;
	int32_t gamma_count = (int32_t) waititerRL +1;
	
	
	int len = ((int) nn[0]) * ((int) nn[1]) * ((int) nn[2]);
	double* pca_gamma_ft = (double*) fftw_malloc( 2*len * sizeof(double));
	double* pca_inten = (double*) fftw_malloc( 2*len * sizeof(double));
	
	double* pca_rho_m1_ft = (double*) fftw_malloc( 2*len * sizeof(double));

	if (!pca_gamma_ft || !pca_inten || !pca_rho_m1_ft)
	{
		free(pca_gamma_ft);
		free(pca_inten);
		free(pca_rho_m1_ft);
		return;
	}
	double gamma_sum;
	
	int32_t nnh[3] = {(nn[0] - zex), (nn[1] - zey), (nn[2] - zez)};
	if( nnh[0] < 1)
	{
		nnh[0] = 1;
	}
	if( nnh[1] < 1)
	{
		nnh[1] = 1;
	}
	if( nnh[2] < 1)
	{
		nnh[2] = 1;
	}
	
	
	fftw_plan torecip_tmp;
	fftw_plan toreal_tmp;
	int32_t nn2[3] = {0, 0, 0};
	nn2[0] = nn[0] + 2*(nn[0]/2);
	nn2[1] = nn[1] + 2*(nn[1]/2);
	nn2[2] = nn[2] + 2*(nn[2]/2);
	if( nn[0] == 1)
	{
		nn2[0] = nn[0];
	}
	if( nn[1] == 1)
	{
		nn2[1] = nn[1];
	}
	if( nn[2] == 1)
	{
		nn2[2] = nn[2];
	}
	int len2 = ((int) nn2[0]) * ((int) nn2[1]) * ((int) nn2[2]);
	double* tmpdata1 = (double*) fftw_malloc( 2*len2 * sizeof(double));
	double* tmpdata2 = (double*) fftw_malloc( 2*len2 * sizeof(double));
	if (!tmpdata1 || !tmpdata2)
	{
		free(tmpdata1);
		free(tmpdata2);
		return;
	}
	FFTPlan( &torecip_tmp, &toreal_tmp, tmpdata1, nn2, ndim );
	
	
	CopyArray(seqdata, rho_m1, nn); 
	FFTPlan( &torecip, &toreal, seqdata, nn, ndim );
	CopyArray(rho_m1, seqdata, nn); 

	MaskedSumOfSquares( expdata, mask, nn, &sos );

	for( iter=startiter; iter < (numiter+startiter); iter++)
	{
		while( citer_flow[1] == 1 ) sleep(PRFFTW_PSLEEP);
		if( citer_flow[1] == 2 ) break; 
		CopyArray( seqdata, rho_m1, nn );
		
		FFTStride(seqdata, nn, &torecip);
		if( (iter - startiter) == startiterRL )
		{
			CopyArray( seqdata, pca_rho_m1_ft, nn );
		}
		if( gamma_count > waititerRL &&  (iter - startiter) > startiterRL)
		{
			
			if( (iter - startiter) == (startiterRL+1) || gammaRS > 0)
			{
				lorentz_ft_fill(pca_gamma_ft, nn, gammaHWHM);
				SumArray(pca_gamma_ft, nn, &gamma_sum);
				ScaleArray(pca_gamma_ft, nn, (1.0/gamma_sum));
				wrap_array(pca_gamma_ft, nn, 1);
			}
			
			update_gamma(expdata, seqdata, pca_rho_m1_ft, pca_gamma_ft, mask, ndim, nn, nnh, numiterRL, tmpdata1, tmpdata2, &torecip_tmp, &toreal_tmp); 
			
			SumArray(pca_gamma_ft, nn, &gamma_sum);
			ScaleArray(pca_gamma_ft, nn, (1.0/gamma_sum));
			gamma_count = 1;
			
			CopyArray( seqdata, pca_rho_m1_ft, nn );
		}
		
		if( citer_flow[5] > 0 && update_count_recip == citer_flow[5] ) 
		{
			CopyAmp( seqdata, visual_amp_recip, nn );
			if( citer_flow[6] > 0 ) CopyPhase( seqdata, visual_phase_recip, nn );
			update_count_recip = 0;
			Py_BLOCK_THREADS;
			PyObject_CallObject(updaterecip, NULL);
			Py_UNBLOCK_THREADS;
		}
		else
		{
			update_count_recip ++;
		}
		
		MaskedCalculateResiduals(seqdata, expdata, mask, nn, &res);
		
		if( (iter - startiter) > startiterRL)
		{
			CopySquare(seqdata, pca_inten, nn);
			wrap_array(pca_inten, nn, -1);
			wrap_array(pca_gamma_ft, nn, -1);
			convolve_nomem2(pca_inten, pca_gamma_ft, ndim, nn, tmpdata1, tmpdata2, &torecip_tmp, &toreal_tmp);
			wrap_array(pca_inten, nn, 1);
			wrap_array(pca_gamma_ft, nn, 1);
			MaskedSetPCAmplitudes(seqdata, expdata, pca_inten, mask, nn);
		}
		else
		{
			MaskedSetAmplitudes(seqdata, expdata, mask, nn);
		}
		FFTStride(seqdata, nn, &toreal);
		
		residual[iter] = (double) ( (double) res/sos);
		
		SumOfSquares( seqdata, nn, &sos1);
		RS_HIO(seqdata, rho_m1, support, nn, beta );
		SumOfSquares( seqdata, nn, &sos2 );
		norm = sqrt( (double) sos1/sos2 );
		ScaleArray( seqdata, nn, norm );
		
		if( citer_flow[3] > 0 && update_count_real == citer_flow[3] ) 
		{
			CopyAmp( seqdata, visual_amp_real, nn );
			if( citer_flow[6] > 0 ) CopyPhase( seqdata, visual_phase_real, nn );
			update_count_real = 0;
			Py_BLOCK_THREADS;
			PyObject_CallObject(updatereal, NULL);
			Py_UNBLOCK_THREADS;
		}
		else
		{
			update_count_real ++;
		}
		
		Py_BLOCK_THREADS;
		PyObject_CallObject(updatelog, NULL);
		Py_UNBLOCK_THREADS;
		
		citer_flow[0] += 1;
		gamma_count += 1;
	}
	
	free(tmpdata1);
	free(tmpdata2);
	free(pca_gamma_ft);
	free(pca_inten);
	
	fftw_destroy_plan( torecip_tmp );
	fftw_destroy_plan( toreal_tmp );

	fftw_destroy_plan( torecip );
	fftw_destroy_plan( toreal );

	fftw_cleanup_threads();

	Py_END_ALLOW_THREADS;
}