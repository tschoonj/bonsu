/*
#############################################
##  Filename: prfftwmodule.h
##
##  Copyright (C) 2012 Marcus C. Newton
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program. If not, see <http://www.gnu.org/licenses/>.
##
## Contact: Bonsu.Devel@gmail.com
#############################################
*/

#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_10_API_VERSION
#include <numpy/arrayobject.h> 
#include <stdint.h>
#include "fftw3.h"
#include "math.h"

#ifdef _MSC_VER 
	#ifndef __cplusplus
		#define inline __inline
	#endif
	#include <windows.h>
	#define sleep Sleep
	#define PRFFTW_PSLEEP 1000
#else
	#define PRFFTW_PSLEEP 1
#endif 

#if PY_MAJOR_VERSION >= 3
	#define PyInt_FromLong PyLong_FromLong
	#define PyString_FromString PyUnicode_FromFormat
#endif

PyObject* prfftw_hiomask(PyObject *self, PyObject *args);
PyObject* prfftw_hio(PyObject *self, PyObject *args);
PyObject* prfftw_hioplus(PyObject *self, PyObject *args);
PyObject* prfftw_pchio(PyObject *self, PyObject *args);
PyObject* prfftw_pgchio(PyObject *self, PyObject *args);
PyObject* prfftw_er(PyObject *self, PyObject *args);
PyObject* prfftw_ermask(PyObject *self, PyObject *args);
PyObject* prfftw_poermask(PyObject *self, PyObject *args);
PyObject* prfftw_raar(PyObject *self, PyObject *args);
PyObject* prfftw_hpr(PyObject *self, PyObject *args);
PyObject* prfftw_hiomaskpc(PyObject *self, PyObject *args);
PyObject* prfftw_ermaskpc(PyObject *self, PyObject *args);
PyObject* prfftw_hprmaskpc(PyObject *self, PyObject *args);
PyObject* prfftw_raarmaskpc(PyObject *self, PyObject *args);

PyObject* prfftw_fft(PyObject *self, PyObject *args);
PyObject* prfftw_threshold(PyObject *self, PyObject *args);
PyObject* prfftw_rangereplace(PyObject *self, PyObject *args);
PyObject* prfftw_gaussian_fill(PyArrayObject *self, PyObject *args);
PyObject* prfftw_gaussian_filter(PyObject *self, PyObject *args);
PyObject* prfftw_convolve(PyObject *self, PyObject *args);
PyObject* prfftw_convolve2(PyObject *self, PyObject *args);
PyObject* prfftw_wrap(PyObject *self, PyObject *args);
PyObject* prfftw_medianfilter(PyObject *self, PyObject *args);
PyObject* prfftw_conj_reflect(PyObject *self, PyObject *args);


typedef struct _SeqObjects
{
	double* residual;
	int32_t* citer_flow;
	double* visual_amp_real;
	double* visual_phase_real;
	double* visual_amp_recip;
	double* visual_phase_recip;
	PyObject* updatereal;
	PyObject* updaterecip;
	PyObject* updatelog;
	int startiter;
	int numiter;
	int maxiter;
	double alpha;
	double beta;
	double gamma;
	double delta;
} SeqObjects;

typedef struct _SeqArrayObjects
{
	int arraytype;
	int ndim;
	npy_intp *dims;
    int32_t nn[3];
	int32_t* citer_flow;
	int startiter;
	int numiter;
    double* seqdata;
    double* expdata;
    double* support;
    double* mask;
    double* rho_m1;
    double* rho_m2;
    double* epsilon;
    double* tmparray1;
    double* tmparray2;
    double* tmparray3;
    double* tmparray4;
} SeqArrayObjects;


void conj_reflect(double* data, int32_t* nn);

int convolve2(double* indata1, double* indata2, int32_t ndim, int32_t* dims);

int convolve_nomem2(double* indata1, double* indata2, int32_t ndim, int32_t* dims, double* data1, double* data2, fftw_plan* torecip, fftw_plan* toreal);

int convolve(double* indata1, double* indata2, int32_t ndim, int32_t* dims);

void convolve_nomem(double* data1, double* data2, int32_t ndim, int32_t* nn, fftw_plan* torecip, fftw_plan* toreal);

void gaussian_fill(PyObject* arg1, double sigma);

int wrap_array(double* indata, int32_t* nn, int drctn);

void MedianReplaceVoxel(double* data1, double* data2, int32_t* nn,int32_t k_x, int32_t k_y, int32_t k_z, double maxerr);
int Compare (const void *X, const void *Y);
void MedianFilter( double* data1, double* data2, int32_t* nn, int32_t k_x,	int32_t k_y, int32_t k_z );

void BlankLineReplace( double* data1,  double* data2,  int32_t* nn, int32_t k_x, int32_t k_y, int32_t k_z, int32_t x1, int32_t x2, int32_t y1, int32_t y2, int32_t z1, int32_t z2 );


void HIO(double* seqdata,	double* expdata, double* support,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);

void HIOMask(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, int numiter_relax);

void HIOPlus(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);
						
void PCHIO(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim, double phasemax, double phasemin,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);

void PGCHIO(double* seqdata,	double* expdata, double* support, double* mask, double* tmpdata,
						double beta, int startiter, int numiter, int ndim, double phasemax, double phasemin,
						double qx, double qy, double qz,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);

void ER(double* seqdata,	double* expdata, double* support,
						int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);

void ERMask(double* seqdata,	double* expdata, double* support, double* mask,
						int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, int numiter_relax);

void POERMask(double* seqdata,	double* expdata, double* support, double* mask,
						int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);

void RAAR(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, int numiter_relax);

void HPR(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, int numiter_relax);

void CSHIO(double* seqdata,	double* expdata, double* support, double* mask,
						double beta, int startiter, int numiter, int ndim, double cs_p, double* epsilon, double cs_d, double cs_eta, int32_t relax,
						double* rho_m1, double* rho_m2, double* elp, int32_t* nn, double* residual, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog);
						
void HIOMaskPC(double* seqdata,	double* expdata, double* support, double* mask,
						double gammaHWHM, int gammaRS, int numiterRL, int startiterRL, int waititerRL, int zex, int zey, int zez,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, double* pca_gamma_ft, int32_t* nn, double* residual, double* residualRL, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, PyObject* updatelog2, int accel);

void ERMaskPC(double* seqdata,	double* expdata, double* support, double* mask,
						double gammaHWHM, int gammaRS, int numiterRL, int startiterRL, int waititerRL, int zex, int zey, int zez,
						int startiter, int numiter, int ndim,
						double* rho_m1, double* pca_gamma_ft, int32_t* nn, double* residual, double* residualRL, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, PyObject* updatelog2, int accel);

void HPRMaskPC(double* seqdata,	double* expdata, double* support, double* mask,
						double gammaHWHM, int gammaRS, int numiterRL, int startiterRL, int waititerRL, int zex, int zey, int zez,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, double* pca_gamma_ft, int32_t* nn, double* residual, double* residualRL, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, PyObject* updatelog2, int accel);

void RAARMaskPC(double* seqdata,	double* expdata, double* support, double* mask,
						double gammaHWHM, int gammaRS, int numiterRL, int startiterRL, int waititerRL, int zex, int zey, int zez,
						double beta, int startiter, int numiter, int ndim,
						double* rho_m1, double* pca_gamma_ft, int32_t* nn, double* residual, double* residualRL, int32_t* citer_flow,
						double* visual_amp_real, double* visual_phase_real, double* visual_amp_recip, double* visual_phase_recip,
						PyObject* updatereal, PyObject* updaterecip, PyObject* updatelog, PyObject* updatelog2, int accel);


void MaskedSetPCAmplitudes( double* seqdata, double* expdata, double* itnsty, double* mask, int32_t* nn );
void lorentz_ft_fill( double* data, int32_t* nn, double gammaHWHM );
void SumArray( double* data, int32_t* nn, double* sum );

void divide_I_Id_iter( double* expdata, double* pca_Idm_iter, double* mask, double* pca_Idmdiv_iter, int32_t* nn);
void CopySquare( double* rho, double* itnsty, int32_t* nn);
void make_Id_iter( double* rho, double* rhom1, double* pca_Id_iter, int32_t* nn);
void mask_gamma(double* gamma, int32_t* nn, int32_t* maskdim);

void RS_HIO(double* seqdata, double* rho_m1, double* support, int32_t* nn, double beta);

void RS_HIO_P(double* seqdata, double* rho_m1, double* support, int32_t* nn, double beta);

void RS_PCHIO(double* seqdata, double* rho_m1, double* support, int32_t* nn, double beta, double phasemax, double phasemin);

void RS_PGCHIO(double* seqdata, double* rho_m1, double* support, double* tmpdata, int32_t* nn, double beta, double phasemax, double phasemin, double qx, double qy, double qz);

void RS_POER(double* seqdata, double* rho_m1, double* support, int32_t* nn);

void RS_ER(double* seqdata, double* rho_m1, double* support, int32_t* nn);

void RS_RAAR(double* seqdata, double* rho_m1, double* support, int32_t* nn, double beta);

void RS_HPR(double* seqdata, double* rho_m1, double* support, int32_t* nn, double beta);

void SqrtArray(double* data, int32_t* nn);
 
void CopyArray(double* data1,	double* data2, int32_t* nn);

void CopyRealArray(double* data1,	double* data2, int32_t* nn);
 
void SubtractArray(double* data0, double* data1, double* data2, int32_t* nn);
 
void AddArray(double* data0, double* data1, double* data2, int32_t* nn);

void MultiplyArray(double* data0, double* data1, double* data2, int32_t* nn);

void CopyAmp(double* data1, double* data2, int32_t* nn);
 
void CopyPhase(double* data1, double* data2, int32_t* nn);

void ZeroArray(double* data, int32_t* nn);

void ScaleArray(double* data,  int32_t* nn, double factor);

void ExponentArray(double* data, int32_t* nn, int factor);
 
void ConstantArray(double* data, int32_t* nn, double real, double imag);
 
void Norm2array(double* data, int32_t* nn, double* norm2);
 
void FFTPlan(fftw_plan* torecip, fftw_plan* toreal, double* data, int32_t* nn, int32_t ndim);
 
void FFTStride(double* data, int32_t* nn, fftw_plan* plan);
 
void SumOfSquares(double* data, int32_t* nn, double* sos);
 
void MaskedSumOfSquares(double* data, double* mask, int32_t* nn, double* sos);
 
void CalculateResiduals(double* seqdata, double* expdata, int32_t* nn, double* sos);
 
void MaskedCalculateResiduals(double* seqdata, double* expdata, double* mask, int32_t* nn, double* sos);
 
void MaskedSetAmplitudes(double* seqdata, double* expdata, double* mask, int32_t* nn);
 
void SetAmplitudes(double* seqdata, double* expdata, int32_t* nn);
 
void Calculate_Delp(double* rho_m1, double* rho_m2, double* elp, int32_t* nn, double p, double epsilon);
 
void MaskedSetAmplitudesRelaxed(double* seqdata, double* expdata, double* mask, double res, int32_t relax, int32_t* nn);

void MaskedSetAmplitudesIterRelaxed(	double* seqdata,	double* expdata,	double* mask,	int32_t* nn,	int niter, int iter);
void MaskedSetPCAmplitudesIterRelaxed(	double* seqdata,	double* expdata, double* itnsty, double* mask, int32_t* nn, int niter, int iter);
 
inline int32_t modclip(int32_t idx, int32_t idx_max); 

inline int32_t modclip(int32_t idx, int32_t idx_max)
{
	if (idx < 0 ) 
	{
		return 0;
	}
	else if (idx > idx_max )
	{
		return idx_max;
	}
	else
	{
		return idx;
	}
}
