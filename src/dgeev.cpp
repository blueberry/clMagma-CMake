/*
    -- clMAGMA (version 1.1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date January 2014

       @precisions normal d -> s

*/
#include <stdio.h>
#include "common_magma.h"

#define PRECISION_d

/*
 * VERSION1 - LAPACK
 * VERSION2 - MAGMA whithout T
 * VERSION3 - MAGMA with T
 */
#define VERSION3

extern "C" magma_int_t
magma_dgeev(magma_vec_t jobvl, magma_vec_t jobvr, magma_int_t n,
            double *a, magma_int_t lda,
            double *WR, double *WI,
            double *vl, magma_int_t ldvl,
            double *vr, magma_int_t ldvr,
            double *work, magma_int_t lwork,
            magma_int_t *info, magma_queue_t queue)
{
/*  -- clMAGMA (version 1.1.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       @date January 2014

    Purpose
    =======
    DGEEV computes for an N-by-N real nonsymmetric matrix A, the
    eigenvalues and, optionally, the left and/or right eigenvectors.

    The right eigenvector v(j) of A satisfies
                     A * v(j) = lambda(j) * v(j)
    where lambda(j) is its eigenvalue.
    The left eigenvector u(j) of A satisfies
                  u(j)**T * A = lambda(j) * u(j)**T
    where u(j)**T denotes the transpose of u(j).

    The computed eigenvectors are normalized to have Euclidean norm
    equal to 1 and largest component real.

    Arguments
    =========
    JOBVL   (input) CHARACTER*1
            = 'N': left eigenvectors of A are not computed;
            = 'V': left eigenvectors of are computed.

    JOBVR   (input) CHARACTER*1
            = 'N': right eigenvectors of A are not computed;
            = 'V': right eigenvectors of A are computed.

    N       (input) INTEGER
            The order of the matrix A. N >= 0.

    A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
            On entry, the N-by-N matrix A.
            On exit, A has been overwritten.

    LDA     (input) INTEGER
            The leading dimension of the array A.  LDA >= max(1,N).

    WR      (output) DOUBLE PRECISION array, dimension (N)
    WI      (output) DOUBLE PRECISION array, dimension (N)
            WR and WI contain the real and imaginary parts,
            respectively, of the computed eigenvalues.  Complex
            conjugate pairs of eigenvalues appear consecutively
            with the eigenvalue having the positive imaginary part
            first.

    VL      (output) DOUBLE PRECISION array, dimension (LDVL,N)
            If JOBVL = 'V', the left eigenvectors u(j) are stored one
            after another in the columns of VL, in the same order
            as their eigenvalues.
            If JOBVL = 'N', VL is not referenced.
            u(j) = VL(:,j), the j-th column of VL.

    LDVL    (input) INTEGER
            The leading dimension of the array VL.  LDVL >= 1; if
            JOBVL = 'V', LDVL >= N.

    VR      (output) DOUBLE PRECISION array, dimension (LDVR,N)
            If JOBVR = 'V', the right eigenvectors v(j) are stored one
            after another in the columns of VR, in the same order
            as their eigenvalues.
            If JOBVR = 'N', VR is not referenced.
            v(j) = VR(:,j), the j-th column of VR.

    LDVR    (input) INTEGER
            The leading dimension of the array VR.  LDVR >= 1; if
            JOBVR = 'V', LDVR >= N.

    WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK))
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) INTEGER
            The dimension of the array WORK.  LWORK >= (1+nb)*N.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = i, the QR algorithm failed to compute all the
                  eigenvalues, and no eigenvectors have been computed;
                  elements and i+1:N of W contain eigenvalues which have
                  converged.
    =====================================================================    */

    magma_int_t c__1 = 1;
    magma_int_t c__0 = 0;
    magma_int_t c_n1 = -1;
    
    magma_int_t a_dim1, a_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
            i__2, i__3;
    double d__1, d__2;

    magma_int_t i__, k, ihi, ilo;
    double      r__, cs, sn, scl;
    double dum[1], eps;
    magma_int_t ibal;
    double anrm;
    magma_int_t ierr, itau, iwrk, nout;
    magma_int_t scalea;
    double cscale;
    double bignum;
    magma_int_t minwrk;
    magma_int_t wantvl;
    double smlnum;
    magma_int_t lquery, wantvr, select[1];

    magma_int_t nb = 0;
    magmaDouble_ptr dT;
    //magma_timestr_t start, end;

    char side[2] = {0, 0};
    magma_vec_t jobvl_ = jobvl;
    magma_vec_t jobvr_ = jobvr;

    *info = 0;
    lquery = lwork == -1;
    wantvl = lapackf77_lsame(lapack_const(jobvl_), "V");
    wantvr = lapackf77_lsame(lapack_const(jobvr_), "V");
    if (! wantvl && ! lapackf77_lsame(lapack_const(jobvl_), "N")) {
        *info = -1;
    } else if (! wantvr && ! lapackf77_lsame(lapack_const(jobvr_), "N")) {
        *info = -2;
    } else if (n < 0) {
        *info = -3;
    } else if (lda < max(1,n)) {
        *info = -5;
    } else if ( (ldvl < 1) || (wantvl && (ldvl < n))) {
        *info = -9;
    } else if ( (ldvr < 1) || (wantvr && (ldvr < n))) {
        *info = -11;
    }

    /*  Compute workspace   */
    if (*info == 0) {

        nb = magma_get_dgehrd_nb(n);
        minwrk = (2+nb)*n;
        work[0] = (double) minwrk;
        
        if (lwork < minwrk && ! lquery) {
            *info = -13;
        }

    }

    if (*info != 0) {
        magma_xerbla( __func__, -(*info) );
        return *info;
    }
    else if (lquery) {
        return *info;
    }

    /* Quick return if possible */
    if (n == 0) {
        return *info;
    }
   
    // if eigenvectors are needed
#if defined(VERSION3)
    if (MAGMA_SUCCESS != magma_dmalloc( &dT, nb*n )) {
        *info = MAGMA_ERR_DEVICE_ALLOC;
        return *info;
    }
#endif

    // subtract row and col for 1-based indexing
    a_dim1   = lda;
    a_offset = 1 + a_dim1;
    a       -= a_offset;
    vl_dim1   = ldvl;
    vl_offset = 1 + vl_dim1;
    vl       -= vl_offset;
    vr_dim1   = ldvr;
    vr_offset = 1 + vr_dim1;
    vr       -= vr_offset;
    --work;

    /* Get machine constants */
    eps    = lapackf77_dlamch("P");
    smlnum = lapackf77_dlamch("S");
    bignum = 1. / smlnum;
    lapackf77_dlabad(&smlnum, &bignum);
    smlnum = magma_dsqrt(smlnum) / eps;
    bignum = 1. / smlnum;

    /* Scale A if max element outside range [SMLNUM,BIGNUM] */
    anrm = lapackf77_dlange("M", &n, &n, &a[a_offset], &lda, dum);
    scalea = 0;
    if (anrm > 0. && anrm < smlnum) {
        scalea = 1;
        cscale = smlnum;
    } else if (anrm > bignum) {
        scalea = 1;
        cscale = bignum;
    }
    if (scalea) {
        lapackf77_dlascl("G", &c__0, &c__0, &anrm, &cscale, &n, &n,
                &a[a_offset], &lda, &ierr);
    }

    /* Balance the matrix
       (Workspace: need N) */
    ibal = 1;
    lapackf77_dgebal("B", &n, &a[a_offset], &lda, &ilo, &ihi, &work[ibal], &ierr);

    /* Reduce to upper Hessenberg form
       (Workspace: need 3*N, prefer 2*N+N*NB) */
    itau = ibal + n;
    iwrk = itau + n;
    i__1 = lwork - iwrk + 1;

    //start = get_current_time();
#if defined(VERSION1)
    /*
     * Version 1 - LAPACK
     */
    lapackf77_dgehrd(&n, &ilo, &ihi, &a[a_offset], &lda,
                     &work[itau], &work[iwrk], &i__1, &ierr);
#elif defined(VERSION2)
    /*
     *  Version 2 - LAPACK consistent HRD
     */
    magma_dgehrd2(n, ilo, ihi, &a[a_offset], lda,
                  &work[itau], &work[iwrk], &i__1, &ierr);
#elif defined(VERSION3)
    /*
     * Version 3 - LAPACK consistent MAGMA HRD + matrices T stored,
     */
    magma_dgehrd(n, ilo, ihi, &a[a_offset], lda,
                 &work[itau], &work[iwrk], i__1, dT, 0, &ierr, queue);
#endif
    //end = get_current_time();
    //printf("    Time for dgehrd = %5.2f sec\n", GetTimerValue(start,end)/1000.);

    if (wantvl) {
      /*        Want left eigenvectors
                Copy Householder vectors to VL */
        side[0] = 'L';
        lapackf77_dlacpy(MagmaLowerStr, &n, &n,
                         &a[a_offset], &lda, &vl[vl_offset], &ldvl);

        /*
         * Generate orthogonal matrix in VL
         *   (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB)
         */
        i__1 = lwork - iwrk + 1;

        //start = get_current_time();
#if defined(VERSION1) || defined(VERSION2)
        /*
         * Version 1 & 2 - LAPACK
         */
        lapackf77_dorghr(&n, &ilo, &ihi, &vl[vl_offset], &ldvl,
                         &work[itau], &work[iwrk], &i__1, &ierr);
#elif defined(VERSION3)
        /*
         * Version 3 - LAPACK consistent MAGMA HRD + matrices T stored
         */
        magma_dorghr(n, ilo, ihi, &vl[vl_offset], ldvl, &work[itau],
                     dT, 0, nb, &ierr, queue);
#endif
        //end = get_current_time();
        //printf("    Time for dorghr = %5.2f sec\n", GetTimerValue(start,end)/1000.);

        /*
         * Perform QR iteration, accumulating Schur vectors in VL
         *   (Workspace: need N+1, prefer N+HSWORK (see comments) )
         */
        iwrk = itau;
        i__1 = lwork - iwrk + 1;
        lapackf77_dhseqr("S", "V", &n, &ilo, &ihi, &a[a_offset], &lda, WR, WI,
                         &vl[vl_offset], &ldvl, &work[iwrk], &i__1, info);

        if (wantvr) {
          /* Want left and right eigenvectors
             Copy Schur vectors to VR */
            side[0] = 'B';
            lapackf77_dlacpy("F", &n, &n, &vl[vl_offset], &ldvl, &vr[vr_offset], &ldvr);
        }

    } else if (wantvr) {
        /*  Want right eigenvectors
            Copy Householder vectors to VR */
        side[0] = 'R';
        lapackf77_dlacpy("L", &n, &n, &a[a_offset], &lda, &vr[vr_offset], &ldvr);

        /*
         * Generate orthogonal matrix in VR
         *   (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB)
         */
        i__1 = lwork - iwrk + 1;
        //start = get_current_time();
#if defined(VERSION1) || defined(VERSION2)
        /*
         * Version 1 & 2 - LAPACK
         */
        lapackf77_dorghr(&n, &ilo, &ihi, &vr[vr_offset], &ldvr,
                         &work[itau], &work[iwrk], &i__1, &ierr);
#elif defined(VERSION3)
        /*
         * Version 3 - LAPACK consistent MAGMA HRD + matrices T stored
         */
        magma_dorghr(n, ilo, ihi, &vr[vr_offset], ldvr,
                     &work[itau], dT, 0, nb, &ierr, queue);
#endif
        //end = get_current_time();
        //printf("    Time for dorghr = %5.2f sec\n", GetTimerValue(start,end)/1000.);

        /*
         * Perform QR iteration, accumulating Schur vectors in VR
         *   (Workspace: need N+1, prefer N+HSWORK (see comments) )
         */
        iwrk = itau;
        i__1 = lwork - iwrk + 1;
        lapackf77_dhseqr("S", "V", &n, &ilo, &ihi, &a[a_offset], &lda, WR, WI,
                &vr[vr_offset], &ldvr, &work[iwrk], &i__1, info);
    } else {
        /*
         * Compute eigenvalues only
         *   (Workspace: need N+1, prefer N+HSWORK (see comments) )
         */
        iwrk = itau;
        i__1 = lwork - iwrk + 1;
        lapackf77_dhseqr("E", "N", &n, &ilo, &ihi, &a[a_offset], &lda, WR, WI,
                &vr[vr_offset], &ldvr, &work[iwrk], &i__1, info);
    }

    /* If INFO > 0 from DHSEQR, then quit */
    if (*info > 0) {
        fprintf(stderr, "DHSEQR returned with info = %d\n", (int) *info);
        goto L50;
    }

    if (wantvl || wantvr) {
        /*
         * Compute left and/or right eigenvectors
         *   (Workspace: need 4*N)
         */
        lapackf77_dtrevc(side, "B", select, &n, &a[a_offset], &lda, &vl[vl_offset], &ldvl,
                &vr[vr_offset], &ldvr, &n, &nout, &work[iwrk], &ierr);
    }

    if (wantvl) {
        /*
         * Undo balancing of left eigenvectors
         *   (Workspace: need N)
         */
        lapackf77_dgebak("B", "L", &n, &ilo, &ihi,
                         &work[ibal], &n, &vl[vl_offset], &ldvl, &ierr);

        /* Normalize left eigenvectors and make largest component real */
        for (i__ = 1; i__ <= n; ++i__) {
            if ( WI[i__-1] == 0.) {
                scl = cblas_dnrm2(n, &vl[i__ * vl_dim1 + 1], 1);
                scl = 1. / scl;
                cblas_dscal(n, (scl), &vl[i__ * vl_dim1 + 1], 1);
            } else if (WI[i__-1] > 0.) {
                d__1 = cblas_dnrm2(n, &vl[ i__      * vl_dim1 + 1], 1);
                d__2 = cblas_dnrm2(n, &vl[(i__ + 1) * vl_dim1 + 1], 1);
                scl = lapackf77_dlapy2(&d__1, &d__2);
                scl = 1. / scl;
                cblas_dscal(n, (scl), &vl[ i__      * vl_dim1 + 1], 1);
                cblas_dscal(n, (scl), &vl[(i__ + 1) * vl_dim1 + 1], 1);
                i__2 = n;
                for (k = 1; k <= i__2; ++k) {
                    /* Computing 2nd power */
                    d__1 = vl[k + i__ * vl_dim1];
                    /* Computing 2nd power */
                    d__2 = vl[k + (i__ + 1) * vl_dim1];
                    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
                }
                /* Comment:
                   Fortran BLAS does not have to add 1
                   C       BLAS must add one to cblas_idamax */
                k = cblas_idamax(n, &work[iwrk], 1)+1;
                lapackf77_dlartg(&vl[k +  i__      * vl_dim1],
                                 &vl[k + (i__ + 1) * vl_dim1], &cs, &sn, &r__);
                cblas_drot(n, &vl[ i__      * vl_dim1 + 1], 1,
                           &vl[(i__ + 1) * vl_dim1 + 1], 1, cs, (sn));
                vl[k + (i__ + 1) * vl_dim1] = 0.;
            }
        }
    }

    if (wantvr) {
        /*
         * Undo balancing of right eigenvectors
         *   (Workspace: need N)
         */
        lapackf77_dgebak("B", "R", &n, &ilo, &ihi, &work[ibal], &n,
                         &vr[vr_offset], &ldvr, &ierr);

        /* Normalize right eigenvectors and make largest component real */
        for (i__ = 1; i__ <= n; ++i__) {
            if (WI[i__-1] == 0.) {
                scl = 1. / cblas_dnrm2(n, &vr[i__ * vr_dim1 + 1], 1);
                cblas_dscal(n, (scl), &vr[i__ * vr_dim1 + 1], 1);
            } else if (WI[i__-1] > 0.) {
                d__1 = cblas_dnrm2(n, &vr[ i__      * vr_dim1 + 1], 1);
                d__2 = cblas_dnrm2(n, &vr[(i__ + 1) * vr_dim1 + 1], 1);
                scl = lapackf77_dlapy2(&d__1, &d__2);
                scl = 1. / scl;
                cblas_dscal(n, (scl), &vr[ i__      * vr_dim1 + 1], 1);
                cblas_dscal(n, (scl), &vr[(i__ + 1) * vr_dim1 + 1], 1);
                i__2 = n;
                for (k = 1; k <= i__2; ++k) {
                    /* Computing 2nd power */
                    d__1 = vr[k + i__ * vr_dim1];
                    /* Computing 2nd power */
                    d__2 = vr[k + (i__ + 1) * vr_dim1];
                    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
                }
                /* Comment:
                   Fortran BLAS does not have to add 1
                   C       BLAS must add one to cblas_idamax */
                k = cblas_idamax(n, &work[iwrk], 1)+1;
                lapackf77_dlartg(&vr[k + i__ * vr_dim1], &vr[k + (i__ + 1) * vr_dim1],
                        &cs, &sn, &r__);
                cblas_drot(n, &vr[ i__      * vr_dim1 + 1], 1,
                              &vr[(i__ + 1) * vr_dim1 + 1], 1, cs, (sn));
                vr[k + (i__ + 1) * vr_dim1] = 0.;
            }
        }
    }

    /*  Undo scaling if necessary */
L50:
    if (scalea) {
        i__1 = n - *info;
        /* Computing MAX */
        i__3 = n - *info;
        i__2 = max(i__3,1);
        lapackf77_dlascl("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1,
                         WR + (*info), &i__2, &ierr);
        i__1 = n - *info;
        /* Computing MAX */
        i__3 = n - *info;
        i__2 = max(i__3,1);
        lapackf77_dlascl("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1,
                WI + (*info), &i__2, &ierr);
        if (*info > 0) {
            i__1 = ilo - 1;
            lapackf77_dlascl("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1,
                    WR, &n, &ierr);
            i__1 = ilo - 1;
            lapackf77_dlascl("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1,
                    WI, &n, &ierr);
        }
    }

#if defined(VERSION3)
    magma_free( dT );
#endif
    return *info;
} /* magma_dgeev */
